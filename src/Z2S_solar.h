#pragma once

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>

#ifndef Z2S_SOLAR_LOCATION
#define Z2S_SOLAR_LOCATION "z2s_solar"
#endif

// 2024-01-01 00:00:00 — timestamps older than this mean clock is not synced yet
static constexpr time_t Z2S_SOLAR_MIN_VALID_TIME = 1704067200;

struct Z2S_SolarConfig {
  float lat    = 0.0f;
  float lon    = 0.0f;
  float tz_h   = 1.0f;   // Standard (winter) UTC offset in hours (e.g. CET = +1)
  bool  eu_dst = true;   // Auto +1h during EU summer time (last Sun Mar -> Oct)
  bool  valid  = false;
};

struct Z2S_SunTimes {
  uint32_t day_key      = 0;     // YYYYMMDD
  uint16_t rise_min     = 0;     // Local minutes from midnight (0..1439)
  uint16_t set_min      = 0;     // Local minutes from midnight (0..1439)
  bool     has_rise_set = false; // False during polar day/night
  bool     polar_day    = false; // True when sun stays above horizon all day
};

class Z2S_Solar {
 public:
  static bool isValidCoords(float lat, float lon, float tz_h) {
    if (std::isnan(lat) || std::isnan(lon) || std::isnan(tz_h)) return false;
    if (std::isinf(lat) || std::isinf(lon) || std::isinf(tz_h)) return false;
    return (lat >= -89.9f && lat <= 89.9f) &&
           (lon >= -180.0f && lon <= 180.0f) &&
           (tz_h >= -12.0f && tz_h <= 14.0f);
  }

  static void setGatewayConfig(float lat, float lon, float tz_h, bool eu_dst) {
    if (!isValidCoords(lat, lon, tz_h)) return;
    State& s = state();
    taskENTER_CRITICAL(&s.mux);
    s.gw.lat        = lat;
    s.gw.lon        = lon;
    s.gw.tz_h       = tz_h;
    s.gw.eu_dst     = eu_dst;
    s.gw.valid      = true;
    s.gw_cache_day  = 0;
    taskEXIT_CRITICAL(&s.mux);
  }

  static void clearGatewayConfig() {
    State& s = state();
    taskENTER_CRITICAL(&s.mux);
    s.gw            = {};
    s.gw_cache_day  = 0;
    taskEXIT_CRITICAL(&s.mux);
  }

  static bool getGatewayConfig(Z2S_SolarConfig& out) {
    State& s = state();
    taskENTER_CRITICAL(&s.mux);
    out = s.gw;
    taskEXIT_CRITICAL(&s.mux);
    return out.valid;
  }

  static bool isConfigured() {
    State& s = state();
    taskENTER_CRITICAL(&s.mux);
    bool ok = s.gw.valid;
    taskEXIT_CRITICAL(&s.mux);
    return ok;
  }

  // Optional runtime-only Lua override (cleared automatically on every VM reset)
  static bool setLuaOverride(float lat, float lon, float tz_h, bool eu_dst) {
    if (!isValidCoords(lat, lon, tz_h)) return false;
    State& s = state();
    taskENTER_CRITICAL(&s.mux);
    s.lua.lat       = lat;
    s.lua.lon       = lon;
    s.lua.tz_h      = tz_h;
    s.lua.eu_dst    = eu_dst;
    s.lua.valid     = true;
    s.lua_cache_day = 0;
    taskEXIT_CRITICAL(&s.mux);
    return true;
  }

  static void clearLuaOverride() {
    State& s = state();
    taskENTER_CRITICAL(&s.mux);
    s.lua           = {};
    s.lua_cache_day = 0;
    taskEXIT_CRITICAL(&s.mux);
  }

  // Parses "Lat,Lon[,TzH[,EuDst]]" (e.g. "52.41,16.93,1,1").
  // Empty/whitespace input clears gateway config and returns true.
  static bool parseCsv(const char* csv, bool* out_cleared = nullptr) {
    if (out_cleared) *out_cleared = false;
    if (!csv) {
      clearGatewayConfig();
      if (out_cleared) *out_cleared = true;
      return true;
    }

    while (*csv == ' ' || *csv == '\t' || *csv == '\r' || *csv == '\n') {
      ++csv;
    }
    if (*csv == '\0') {
      clearGatewayConfig();
      if (out_cleared) *out_cleared = true;
      return true;
    }

    float vals[4] = {};
    size_t count  = 0;
    const char* p = csv;
    while (*p != '\0' && count < 4) {
      char* end = nullptr;
      float v   = strtof(p, &end);
      if (end == p) break;
      vals[count++] = v;
      p = end;
      while (*p == ' ' || *p == '\t') ++p;
      if (*p == ',' || *p == ';') ++p;
    }

    if (count < 2) return false;

    float lat   = vals[0];
    float lon   = vals[1];
    float tz_h  = (count >= 3) ? vals[2] : 1.0f;
    bool eu_dst = (count >= 4) ? (vals[3] != 0.0f) : (count == 2);

    if (!isValidCoords(lat, lon, tz_h)) return false;

    setGatewayConfig(lat, lon, tz_h, eu_dst);
    return true;
  }

  static size_t formatCsv(char* dst, size_t dst_cap) {
    if (!dst || dst_cap == 0) return 0;
    Z2S_SolarConfig cfg{};
    if (!getGatewayConfig(cfg)) {
      dst[0] = '\0';
      return 0;
    }
    int n = snprintf(dst, dst_cap, "%.4f,%.4f,%.1f,%u",
                     static_cast<double>(cfg.lat),
                     static_cast<double>(cfg.lon),
                     static_cast<double>(cfg.tz_h),
                     cfg.eu_dst ? 1U : 0U);
    return (n > 0) ? ((static_cast<size_t>(n) < dst_cap) ? static_cast<size_t>(n) : (dst_cap - 1)) : 0;
  }

  // Returns cached daily sunrise/sunset for tm_local (computes outside spinlock)
  static bool getToday(
      const struct tm& tm_local, Z2S_SunTimes& out, bool use_lua_override = false) {
    const uint32_t day_key = static_cast<uint32_t>(
        (tm_local.tm_year + 1900) * 10000 +
        (tm_local.tm_mon + 1) * 100 +
        tm_local.tm_mday);

    State& s = state();
    Z2S_SolarConfig cfg{};
    bool use_lua = false;

    taskENTER_CRITICAL(&s.mux);
    if (use_lua_override && s.lua.valid) {
      use_lua = true;
      if (s.lua_cache_day == day_key) {
        out = s.lua_cache;
        taskEXIT_CRITICAL(&s.mux);
        return true;
      }
      cfg = s.lua;
    } else if (s.gw.valid) {
      if (s.gw_cache_day == day_key) {
        out = s.gw_cache;
        taskEXIT_CRITICAL(&s.mux);
        return true;
      }
      cfg = s.gw;
    }
    taskEXIT_CRITICAL(&s.mux);

    if (!cfg.valid) return false;

    Z2S_SunTimes computed{};
    computeNoaa(cfg, tm_local, day_key, computed);

    taskENTER_CRITICAL(&s.mux);
    if (use_lua) {
      if (s.lua.valid && s.lua.lat == cfg.lat && s.lua.lon == cfg.lon &&
          s.lua.tz_h == cfg.tz_h && s.lua.eu_dst == cfg.eu_dst) {
        s.lua_cache     = computed;
        s.lua_cache_day = day_key;
      }
    } else {
      if (s.gw.valid && s.gw.lat == cfg.lat && s.gw.lon == cfg.lon &&
          s.gw.tz_h == cfg.tz_h && s.gw.eu_dst == cfg.eu_dst) {
        s.gw_cache     = computed;
        s.gw_cache_day = day_key;
      }
    }
    taskEXIT_CRITICAL(&s.mux);

    out = computed;
    return true;
  }

  static bool isEuDstDay(int month, int day, int iso_wday) {
    if (month > 3 && month < 10) return true;
    if (month < 3 || month > 10) return false;
    const int prev_sun = day - (iso_wday % 7);
    return (month == 3) ? (prev_sun >= 25) : (prev_sun < 25);
  }

  // Double-precision NOAA solar equation (~1 min accuracy)
  static void computeNoaa(
      const Z2S_SolarConfig& cfg, const struct tm& tm_local,
      uint32_t day_key, Z2S_SunTimes& out) {
    static constexpr double kPi     = 3.14159265358979323846;
    static constexpr double kDeg2Rad = kPi / 180.0;
    static constexpr double kRad2Deg = 180.0 / kPi;
    static constexpr uint16_t kMdays[12] = {
      0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
    };

    const int year     = tm_local.tm_year + 1900;
    const int month    = tm_local.tm_mon + 1;
    const int day      = tm_local.tm_mday;
    const int iso_wday = (tm_local.tm_wday == 0) ? 7 : tm_local.tm_wday;

    int doy = kMdays[(month >= 1 && month <= 12) ? (month - 1) : 0] + day;
    if (month > 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
      doy += 1;
    }

    const double g = (2.0 * kPi / 365.0) * (static_cast<double>(doy) - 0.5);
    const double eqtime = 229.18 * (
        0.000075
      + 0.001868 * std::cos(g) - 0.032077 * std::sin(g)
      - 0.014615 * std::cos(2.0 * g) - 0.040849 * std::sin(2.0 * g));
    const double decl = 0.006918
      - 0.399912 * std::cos(g) + 0.070257 * std::sin(g)
      - 0.006758 * std::cos(2.0 * g) + 0.000907 * std::sin(2.0 * g)
      - 0.002697 * std::cos(3.0 * g) + 0.001480 * std::sin(3.0 * g);

    const double lat_r  = static_cast<double>(cfg.lat) * kDeg2Rad;
    const double cos_ha =
        (std::cos(90.833 * kDeg2Rad) / (std::cos(lat_r) * std::cos(decl))) -
        std::tan(lat_r) * std::tan(decl);

    out.day_key = day_key;
    if (cos_ha < -1.0) {
      out.has_rise_set = false;
      out.polar_day    = true;
      out.rise_min     = 0;
      out.set_min      = 1439;
      return;
    }
    if (cos_ha > 1.0) {
      out.has_rise_set = false;
      out.polar_day    = false;
      out.rise_min     = 0;
      out.set_min      = 0;
      return;
    }

    const double ha_min = std::acos(cos_ha) * kRad2Deg * 4.0;
    const double tz_h   = static_cast<double>(cfg.tz_h) +
                          ((cfg.eu_dst && isEuDstDay(month, day, iso_wday)) ? 1.0 : 0.0);
    const double noon   = 720.0 - 4.0 * static_cast<double>(cfg.lon) - eqtime + tz_h * 60.0;

    auto clampMin = [](double v) -> uint16_t {
      long m = std::lround(v);
      if (m < 0) m = 0;
      if (m > 1439) m = 1439;
      return static_cast<uint16_t>(m);
    };

    out.has_rise_set = true;
    out.polar_day    = false;
    out.rise_min     = clampMin(noon - ha_min);
    out.set_min      = clampMin(noon + ha_min);
  }

 private:
  struct State {
    portMUX_TYPE    mux           = portMUX_INITIALIZER_UNLOCKED;
    Z2S_SolarConfig gw            = {};
    Z2S_SolarConfig lua           = {};
    Z2S_SunTimes    gw_cache      = {};
    Z2S_SunTimes    lua_cache     = {};
    uint32_t        gw_cache_day  = 0;
    uint32_t        lua_cache_day = 0;
  };

  static State& state() {
    static State inst;
    return inst;
  }
};