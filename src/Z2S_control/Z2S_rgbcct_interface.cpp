/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "ColorFormat.h"
#include "Z2S_rgbcct_interface.h"
#include "rgbhsv.h"

#include <supla/log_wrapper.h>
#include <supla/storage/storage.h>

/*****************************************************************************/

static const uint8_t SCENE_ICE_LAND_BLUE[]    = {0x01, 0x15, 0x0a, 0x52, 0x52, 0xe0, 0x00, 0x00, 0x64, 0x00, 0xc1, 0x61, 0x00, 0xb4, 0x30, 0x00, 0xb5, 0x52, 0x00, 0xc4, 0x63};
static const uint8_t SCENE_GLACIER_EXPRESS[]  = {0x01, 0x16, 0x0a, 0x64, 0x64, 0x60, 0x00, 0x00, 0x64, 0x00, 0x92, 0x5f, 0x00, 0xc6, 0x60};
static const uint8_t SCENE_SEA_OF_CLOUDS[]    = {0x01, 0x17, 0x03, 0x5e, 0x5e, 0x60, 0x00, 0x00, 0x64, 0x00, 0x38, 0x2f, 0x00, 0x1e, 0x5c, 0x00, 0xd5, 0x45, 0x01, 0x1a, 0x64};
static const uint8_t SCENE_FIREWORKS_AT_SEA[] = {0x01, 0x18, 0x02, 0x64, 0x64, 0xe0, 0x00, 0x00, 0x64, 0x00, 0xb2, 0x39, 0x01, 0x0a, 0x64, 0x01, 0x2d, 0x64, 0x01, 0x3f, 0x64};
static const uint8_t SCENE_FIREFLY_NIGHT[]    = {0x01, 0x1a, 0x03, 0x4b, 0x4b, 0xe0, 0x00, 0x00, 0x64, 0x00, 0xe0, 0x39, 0x01, 0x09, 0x53};
static const uint8_t SCENE_GRASS_LAND[]       = {0x01, 0x1c, 0x0a, 0x5a, 0x5a, 0xe0, 0x00, 0x00, 0x52, 0x00, 0x9d, 0x64, 0x00, 0x8e, 0x64};
static const uint8_t SCENE_NORTHERN_LIGHTS[]  = {0x01, 0x1d, 0x03, 0x52, 0x52, 0xe0, 0x00, 0x00, 0x64, 0x00, 0xae, 0x64, 0x00, 0xa6, 0x64, 0x00, 0xc1, 0x64, 0x00, 0xcc, 0x64};
static const uint8_t SCENE_LATE_AUTUMN[]      = {0x01, 0x1e, 0x0a, 0x52, 0x52, 0xe0, 0x00, 0x00, 0x64, 0x00, 0x19, 0x64, 0x00, 0x22, 0x5e, 0x00, 0x2c, 0x5b, 0x00, 0x14, 0x64, 0x00, 0x0c, 0x64};
static const uint8_t SCENE_GAME[]             = {0x01, 0x1f, 0x02, 0x5f, 0x5f, 0x60, 0x00, 0x00, 0x64, 0x01, 0x10, 0x64, 0x00, 0xd2, 0x64, 0x00, 0xad, 0x64, 0x00, 0x8b, 0x64};
static const uint8_t SCENE_HOLIDAY[]          = {0x01, 0x20, 0x0a, 0x55, 0x55, 0x60, 0x00, 0x00, 0x64, 0x00, 0xc2, 0x58, 0x01, 0x3e, 0x33, 0x00, 0xff, 0x46, 0x01, 0x1d, 0x64};
static const uint8_t SCENE_PARTY[]            = {0x01, 0x22, 0x04, 0x64, 0x64, 0x60, 0x00, 0x00, 0x64, 0x00, 0xd7, 0x5c, 0x00, 0xbc, 0x53, 0x00, 0x37, 0x1e, 0x00, 0x2c, 0x3f, 0x01, 0x61, 0x3f};
static const uint8_t SCENE_TREND[]            = {0x01, 0x23, 0x02, 0x64, 0x64, 0x60, 0x00, 0x00, 0x64, 0x01, 0x08, 0x4b, 0x00, 0xb1, 0x2f, 0x00, 0xcd, 0x57};
static const uint8_t SCENE_MEDITATION[]       = {0x01, 0x25, 0x03, 0x43, 0x43, 0x60, 0x00, 0x00, 0x64, 0x00, 0xb7, 0x35, 0x00, 0x9b, 0x54, 0x00, 0xcd, 0x61};
static const uint8_t SCENE_DATING[]           = {0x01, 0x26, 0x01, 0x59, 0x59, 0xe0, 0x00, 0x00, 0x64, 0x01, 0x19, 0x47, 0x01, 0x49, 0x3d, 0x00, 0xcd, 0x61, 0x00, 0x26, 0x64};
static const uint8_t SCENE_VALENTINES_DAY[]   = {0x01, 0x2a, 0x01, 0x64, 0x64, 0x60, 0x00, 0x00, 0x64, 0x01, 0x15, 0x64, 0x01, 0x05, 0x64, 0x01, 0x45, 0x64, 0x01, 0x2f, 0x64};
static const uint8_t SCENE_NEON_WORLD[]       = {0x01, 0x37, 0x0a, 0x5a, 0x5a, 0x60, 0x00, 0x00, 0x64, 0x00, 0x33, 0x58, 0x00, 0x18, 0x64, 0x01, 0x00, 0x45, 0x00, 0xe3, 0x5e, 0x00, 0xac, 0x30};

struct GlSpiSceneEntry {
  const uint8_t *data;
  uint8_t len;
};

static constexpr GlSpiSceneEntry GL_SPI_SCENES[16] = {
  {SCENE_ICE_LAND_BLUE,    sizeof(SCENE_ICE_LAND_BLUE)},    // 0  (ID: 0x15)
  {SCENE_GLACIER_EXPRESS,  sizeof(SCENE_GLACIER_EXPRESS)},  // 1  (ID: 0x16)
  {SCENE_SEA_OF_CLOUDS,    sizeof(SCENE_SEA_OF_CLOUDS)},    // 2  (ID: 0x17)
  {SCENE_FIREWORKS_AT_SEA, sizeof(SCENE_FIREWORKS_AT_SEA)}, // 3  (ID: 0x18)
  {SCENE_FIREFLY_NIGHT,    sizeof(SCENE_FIREFLY_NIGHT)},    // 4  (ID: 0x1A)
  {SCENE_GRASS_LAND,       sizeof(SCENE_GRASS_LAND)},       // 5  (ID: 0x1C)
  {SCENE_NORTHERN_LIGHTS,  sizeof(SCENE_NORTHERN_LIGHTS)},  // 6  (ID: 0x1D)
  {SCENE_LATE_AUTUMN,      sizeof(SCENE_LATE_AUTUMN)},      // 7  (ID: 0x1E)
  {SCENE_GAME,             sizeof(SCENE_GAME)},             // 8  (ID: 0x1F)
  {SCENE_HOLIDAY,          sizeof(SCENE_HOLIDAY)},          // 9  (ID: 0x20)
  {SCENE_PARTY,            sizeof(SCENE_PARTY)},            // 10 (ID: 0x22)
  {SCENE_TREND,            sizeof(SCENE_TREND)},            // 11 (ID: 0x23)
  {SCENE_MEDITATION,       sizeof(SCENE_MEDITATION)},       // 12 (ID: 0x25)
  {SCENE_DATING,           sizeof(SCENE_DATING)},           // 13 (ID: 0x26)
  {SCENE_VALENTINES_DAY,   sizeof(SCENE_VALENTINES_DAY)},   // 14 (ID: 0x2A)
  {SCENE_NEON_WORLD,       sizeof(SCENE_NEON_WORLD)}        // 15 (ID: 0x37)
};

static constexpr uint8_t GL_SPI_MUSIC_BASE[31] = {
  0x01, 0x01, 0x00, 0x03, 0x64, 0x32, 0x01, 0x00,
  0x00, 0x64, 0x00, 0x00, 0x64, 0x00, 0x78, 0x64,
  0x00, 0xF0, 0x64, 0x00, 0x3C, 0x64, 0x00, 0xB4,
  0x64, 0x01, 0x2C, 0x64, 0x00, 0x00, 0x00
};

struct GlSpiMusicParams {
  uint8_t byte2;
  uint8_t byte3;
  uint8_t color_val; // 0x64 (100) for most modes, 0x50 (80) for jazz
};

// Mode IDs: 0=rock, 1=jazz, 2=classic, 3=rolling, 4=energy, 5=spectrum
static constexpr GlSpiMusicParams GL_SPI_MUSIC_MODES[6] = {
  {0x00, 0x03, 0x64}, // 0: rock
  {0x00, 0x02, 0x50}, // 1: jazz
  {0x00, 0x12, 0x64}, // 2: classic
  {0x01, 0x12, 0x64}, // 3: rolling
  {0x02, 0x12, 0x64}, // 4: energy
  {0x03, 0x12, 0x64}  // 5: spectrum
};

/*****************************************************************************/

Supla::Control::Z2S_RGBCCTInterface::Z2S_RGBCCTInterface() 
  : Z2S_Core(this) {

  channel.setType(SUPLA_CHANNELTYPE_DIMMERANDRGBLED);
  channel.setFlag(SUPLA_CHANNEL_FLAG_RGBW_COMMANDS_SUPPORTED);
  channel.setFlag(SUPLA_CHANNEL_FLAG_RUNTIME_CHANNEL_CONFIG_UPDATE);
  channel.setFuncList(
      SUPLA_RGBW_BIT_FUNC_DIMMER | SUPLA_RGBW_BIT_FUNC_RGB_LIGHTING |
      SUPLA_RGBW_BIT_FUNC_DIMMER_AND_RGB_LIGHTING |
      SUPLA_RGBW_BIT_FUNC_DIMMER_CCT | SUPLA_RGBW_BIT_FUNC_DIMMER_CCT_AND_RGB);
  channel.setDefault(SUPLA_CHANNELFNC_DIMMER_CCT_AND_RGB);
  usedConfigTypes.set(SUPLA_CONFIG_TYPE_DEFAULT);
}

/*****************************************************************************/

void Supla::Control::Z2S_RGBCCTInterface::onLoadState() {

    Supla::Storage::ReadState(
      (unsigned char *)&_brightness, sizeof(_brightness));
    Supla::Storage::ReadState(
      (unsigned char *)&_last_brightness, sizeof(_last_brightness));

    if ((_brightness == 0) && (_last_brightness == 0))
      _last_brightness = 100;

    Supla::Storage::ReadState(
      (unsigned char *)&_colorBrightness, sizeof(_colorBrightness));
    Supla::Storage::ReadState(
      (unsigned char *)&_last_colorBrightness, sizeof(_last_colorBrightness));

    Supla::Storage::ReadState(
      (unsigned char *)&_whiteTemperature, sizeof(_whiteTemperature));
    Supla::Storage::ReadState(
      (unsigned char *)&_last_whiteTemperature, 
      sizeof(_last_whiteTemperature));

    Supla::Storage::ReadState((unsigned char *)&_red, sizeof(_red));
    Supla::Storage::ReadState((unsigned char *)&_last_red, sizeof(_last_red));

    Supla::Storage::ReadState((unsigned char *)&_green, sizeof(_green));
    Supla::Storage::ReadState(
      (unsigned char *)&_last_green, sizeof(_last_green));

    Supla::Storage::ReadState((unsigned char *)&_blue, sizeof(_blue));
    Supla::Storage::ReadState(
      (unsigned char *)&_last_blue, sizeof(_last_blue));
}

/*****************************************************************************/

void Supla::Control::Z2S_RGBCCTInterface::onSaveState() {

  Supla::Storage::WriteState(
    (unsigned char *)&_brightness, sizeof(_brightness));
  Supla::Storage::WriteState(
    (unsigned char *)&_last_brightness, sizeof(_last_brightness));

  Supla::Storage::WriteState(
      (unsigned char *)&_colorBrightness, sizeof(_colorBrightness));
    Supla::Storage::WriteState(
      (unsigned char *)&_last_colorBrightness, sizeof(_last_colorBrightness));

    Supla::Storage::WriteState(
      (unsigned char *)&_whiteTemperature, sizeof(_whiteTemperature));
    Supla::Storage::WriteState(
      (unsigned char *)&_last_whiteTemperature, 
      sizeof(_last_whiteTemperature));

    Supla::Storage::WriteState((unsigned char *)&_red, sizeof(_red));
    Supla::Storage::WriteState(
      (unsigned char *)&_last_red, sizeof(_last_red));

    Supla::Storage::WriteState((unsigned char *)&_green, sizeof(_green));
    Supla::Storage::WriteState(
      (unsigned char *)&_last_green, sizeof(_last_green));

    Supla::Storage::WriteState((unsigned char *)&_blue, sizeof(_blue));
    Supla::Storage::WriteState(
      (unsigned char *)&_last_blue, sizeof(_last_blue));
}

/*****************************************************************************/

void Supla::Control::Z2S_RGBCCTInterface::onInit() {

  //at this stage ZigBee is still not active
  if ((_brightness == 0) && (_last_brightness == 0))
    _last_brightness = 100;
}

/*****************************************************************************/

int32_t Supla::Control::Z2S_RGBCCTInterface::handleNewValueFromServer(
  TSD_SuplaChannelNewValue *newValue) {

  uint8_t whiteTemperature = static_cast<uint8_t>(newValue->value[7]);
  uint8_t command = static_cast<uint8_t>(newValue->value[6]);
  uint8_t toggleOnOff = static_cast<uint8_t>(newValue->value[5]);
  uint8_t red = static_cast<uint8_t>(newValue->value[4]);
  uint8_t green = static_cast<uint8_t>(newValue->value[3]);
  uint8_t blue = static_cast<uint8_t>(newValue->value[2]);
  uint8_t colorBrightness = static_cast<uint8_t>(newValue->value[1]);
  uint8_t brightness = static_cast<uint8_t>(newValue->value[0]);

  SUPLA_LOG_DEBUG(
      "[%d]: red=%d, green=%d, blue=%d, colorBrightness=%d, brightness=%d, "
      "command=%d, toggleOnOff=%d, whiteTemperature=%d", getChannelNumber(), 
      red, green, blue, colorBrightness, brightness, command, toggleOnOff, 
      whiteTemperature);

  if (colorBrightness > 100) {
    colorBrightness = 100;
  }
  if (brightness > 100) {
    brightness = 100;
  }

  bool modeSwitched = false;

  bool wasOn = (_colorBrightness > 0) || (_brightness > 0);

  if ((_colorBrightness == 0) && (colorBrightness > 0)) {

    log_i("RGB ON");

    brightness = 0;       
    whiteTemperature = 0;
    modeSwitched = true;
  } 
  
  else if ((_brightness == 0) && (brightness > 0)) {

    log_i("DIMMER ON");

    colorBrightness = 0;  
    modeSwitched = true;

    if (whiteTemperature == 0)
      whiteTemperature = _last_whiteTemperature;

    if (brightness == 100 && _last_brightness > 0)
      brightness = _last_brightness;
  }

  if ((_colorBrightness > 0) && (colorBrightness == 0)) {

    log_i("RGB OFF");
    
    _last_red = _red;
    _last_green = _green;
    _last_blue = _blue;
    _last_colorBrightness = _colorBrightness;
  }

  if ((_brightness > 0) && (brightness == 0)) {

    log_i("DIMMER OFF");
    
    _last_brightness = _brightness;
    _last_whiteTemperature = _whiteTemperature;
  }

  bool isNowOn = (colorBrightness > 0) || (brightness > 0);

  if (!isNowOn) {
    
    log_i("OFF");
    
    turnOnOff = 0;
  } 
  else if (!wasOn) {
    
    turnOnOff = 1; 
  } 
  else {
  
    turnOnOff = 2; 
  }

  _red = red;
  _green = green;
  _blue = blue;
  _colorBrightness = colorBrightness;
  _brightness = brightness;
  _whiteTemperature = whiteTemperature;

  if (turnOnOff == 2 && !modeSwitched)
    _lastServerMsgReceivedMs = millis() | 1;
  else 
    _lastServerMsgReceivedMs = 1;
  
  Supla::Storage::ScheduleSave(5000, 2000);

  return -1;
}

/*****************************************************************************/

void Supla::Control::Z2S_RGBCCTInterface::setStateOnServer(bool state) {

  log_i("state %u", state);

  if (state)
    _device_state = RGBCCT_STATE_ON;
  else
    _device_state = RGBCCT_STATE_OFF;
}

/*****************************************************************************/

void Supla::Control::Z2S_RGBCCTInterface::setValueOnServer(
  uint8_t red, uint8_t green, uint8_t blue, uint8_t colorBrightness,
  uint8_t brightness, uint8_t whiteTemperature) {

  if (_colorBrightness > 0) {
    
    _last_red = _red;
    _last_green = _green;
    _last_blue = _blue;
    _last_colorBrightness = _colorBrightness;
  }
  if (_brightness > 0) {
    
    _last_brightness = _brightness;
    _last_whiteTemperature = _whiteTemperature;
  }

  _red = red;
  _green = green;
  _blue = blue;
  _colorBrightness = colorBrightness;
  _brightness = brightness;
  _whiteTemperature = whiteTemperature;
  
  _lastDeviceMsgReceivedMs = millis();
}

/*****************************************************************************/

void Supla::Control::Z2S_RGBCCTInterface::sendOnOffCmd(bool state) {

  switch (getRGBColorMode()) {


    case Z2S_TUYA_DP_COLOR_HS_RGB: {

      if (!state)
        sendTuyaRequestCmdBool(_short_addr, _endpoint, 0x01, false);
    } break;


    default:

      zbGateway.sendOnOffCmd(_short_addr, _endpoint, state);
  }  
}

/*****************************************************************************/

void Supla::Control::Z2S_RGBCCTInterface::sendValueToDevice(
  uint8_t red, uint8_t green, uint8_t blue, uint8_t colorBrightness,
  uint8_t brightness, uint8_t whiteTemperature) {

  if (Zigbee.started()) {

    log_i("turnOnOff %u", turnOnOff);

    switch (turnOnOff) {


      case 0: {

        sendOnOffCmd(false);
        return;
      } break;

      
      case 1: {

        sendOnOffCmd(true);
        //return;
      } break;


      case 3: {

        sendOnOffCmd(true);
        turnOnOff = 2;
      } break;


      default: 
      break;
    }
    if (colorBrightness > 0) {

      log_i(
        "red %d, green %d, blue %d, color brightness %d", red, green, blue, 
        colorBrightness);
      
      ColorHSV hsv;
      ColorRGB rgb;

      uint8_t red_cb = red; //(red*colorBrightness) / 100;
      uint8_t green_cb = green; //(green*colorBrightness) / 100;
      uint8_t blue_cb = blue; //(blue*colorBrightness) / 100;

      rgb.m_r = (float)red_cb / 255;
      rgb.m_g = (float)green_cb / 255;
      rgb.m_b = (float)blue_cb / 255;

      log_i(
        "rgb.m_r = %.4f, m_g = %.4f, m_b = %.4f", rgb.m_r, rgb.m_g, rgb.m_b);

      hsv = rgbToHsv(rgb);

      log_i(
        "hsv.m_h = %.4f, m_s = %.4f, m_v = %.4f", hsv.m_h, hsv.m_s, hsv.m_v);

      uint16_t _hue = hsv.m_h*360;
      uint16_t _hue_360 = _hue;
      uint8_t  _saturation = hsv.m_s * 100; //colorBrightness;
      uint8_t _value = hsv.m_v * 100;
      uint16_t  _saturation_1000 = hsv.m_s*1000;

      log_i("hue %d, saturation %d (before mapping)", _hue, _saturation);
      _hue = mapFloat(_hue, 0, 360, 0, 254);
      _saturation = mapFloat(_saturation, 0, 100, 0, 254);
      log_i("hue %d, saturation %d (after mapping)", _hue, _saturation);
      //_value = mapFloat(_value, 0, 100, 1, 254);

      _value = mapFloat(colorBrightness, 1, 100, 1, 254);

      uint16_t rev_hue = mapFloat(_hue, 0, 254, 0, 360);
      uint8_t rev_saturation = mapFloat(_saturation, 0, 254, 0, 100);
      log_i("hue %d, saturation %d (reversed)", rev_hue, rev_saturation);

      hsv.m_h = (float)rev_hue / 360;
      hsv.m_s = (float)rev_saturation / 100;

      log_i(
        "hsv.m_h = %.4f, m_s = %.4f, m_v = %.4f", hsv.m_h, hsv.m_s, hsv.m_v);
      
      rgb = hsvToRgb(hsv);

      log_i(
        "rgb.m_r = %.4f, m_g = %.4f, m_b = %.4f", rgb.m_r, rgb.m_g, rgb.m_b);

      log_i(
        "rgb.m_r = %.4f, m_g = %.4f, m_b = %.4f", rgb.m_r * 255, 
        rgb.m_g * 255, rgb.m_b * 255);

      uint8_t light_mode = 0x01;

      switch (getRGBColorMode()) {

        case Z2S_COLOR_HS_RGB:
        case Z2S_PHILIPS_COLOR_HS_RGB:

          zbGateway.sendColorMoveToHueAndSaturationCmd(
            _short_addr, _endpoint, _hue, _saturation, 1); 
          zbGateway.sendLevelMoveToLevelCmd(_short_addr, _endpoint, _value, 1);
        break;


        case Z2S_COLOR_XY_RGB:
        case Z2S_PHILIPS_COLOR_XY_RGB: {

          espXyColor_t xy_color = espRgbToXYColor(red_cb, green_cb, blue_cb);
          xy_color.x = map(xy_color.x, 0 ,0xFFFF, 0, 0xFEFF);
          xy_color.y = map(xy_color.y, 0 ,0xFFFF, 0, 0xFEFF);

          _value = mapFloat(colorBrightness, 1, 100, 1, 254);

          log_i("XY color mode x:0x%x, y:0x%x", xy_color.x, xy_color.y);
          
          zbGateway.sendColorMoveToColorCmd(
            _short_addr, _endpoint, xy_color.x, xy_color.y, 1);
          zbGateway.sendLevelMoveToLevelCmd(_short_addr, _endpoint, _value, 1);
        } break;


        case Z2S_TUYA_COLOR_HS_RGB: {
          
          zbGateway.sendCustomClusterCmd(
            _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF0, 
            ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &light_mode ,false);

          zbGateway.sendColorMoveToHueAndSaturationCmd(
            _short_addr, _endpoint, _hue, _saturation, 1);
        } break;


        case Z2S_TUYA_COLOR_XY_RGB: {
          
          zbGateway.sendCustomClusterCmd(
            _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF0, 
            ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &light_mode, false);

          espXyColor_t xy_color = espRgbToXYColor(red_cb, green_cb, blue_cb);
          xy_color.x = map(xy_color.x, 0 ,0xFFFF, 0, 0xFEFF);
          xy_color.y = map(xy_color.y, 0 ,0xFFFF, 0, 0xFEFF);

          log_i("Tuya XY color mode x:0x%x, y:0x%x", xy_color.x, xy_color.y);
          
          zbGateway.sendColorMoveToColorCmd(
            _short_addr, _endpoint, xy_color.x, xy_color.y, 1);
        } break;


        case Z2S_TUYA_DP_COLOR_HS_RGB:{

          rgb.m_r = (float)red / 255;
          rgb.m_g = (float)green / 255;
          rgb.m_b = (float)blue / 255;

          hsv = rgbToHsv(rgb);

          _hue_360 = hsv.m_h*360;
      
          _saturation_1000 = hsv.m_s*1000;

          uint16_t _color_brightness_1000 = map(
            colorBrightness, 0, 100, 0, 1000);

          uint8_t test_buffer[27];
          uint16_t seq_num = random() & 0xFFFF;

          test_buffer[0] = (seq_num >> 8) & 0x00FF;
          test_buffer[1] = seq_num & 0x00FF;
        
          test_buffer[2] = 0x01;              
          test_buffer[3] = TUYA_DP_TYPE_BOOL;
          test_buffer[4] = 0x00;
          test_buffer[5] = 0x01;            
          test_buffer[6] = 0x01;          
        
          test_buffer[7]  = 0x02;              
          test_buffer[8]  = TUYA_DP_TYPE_ENUM;
          test_buffer[9]  = 0x00;
          test_buffer[10] = 0x01;            
          test_buffer[11] = 0x01;           

          test_buffer[12] = TUYA_RGBWCT_LED_EF00_COLOR_DP;
          test_buffer[13] = TUYA_DP_TYPE_RAW;              
          test_buffer[14] = 0x00;
          test_buffer[15] = 11;                           
          test_buffer[16] = 0x00;
          test_buffer[17] = 0x01; // COLOR
          test_buffer[18] = 0x00; // GRADIENT (0x00 = instant, 0x01 = smooth transition)
          test_buffer[19] = 0x14;
          test_buffer[20] = 0x00; // SEGMENT
          test_buffer[21] = (_hue_360 >> 8) & 0x00FF;
          test_buffer[22] = _hue_360 & 0x00FF; // COLOR (Hue 0-360)
          test_buffer[23] = (_saturation_1000 >> 8) & 0x00FF;
          test_buffer[24] = _saturation_1000 & 0x00FF; // SATURATION (0-1000)
          test_buffer[25] = (_color_brightness_1000 >> 8) & 0x00FF;
          test_buffer[26] = _color_brightness_1000 & 0x00FF; // BRIGHTNESS (10-1000)

          zbGateway.sendCustomClusterCmd(
            _short_addr, _endpoint, TUYA_PRIVATE_CLUSTER_EF00, 
            TUYA_REQUEST_CMD, ESP_ZB_ZCL_ATTR_TYPE_SET, 27, test_buffer, 
            false);
        } break;
      }
    }
    
    if (brightness > 0) {

      switch (getRGBColorMode()) {


        case Z2S_TUYA_DP_COLOR_HS_RGB: {

          uint8_t test_buffer[28];
          uint16_t seq_num = random() & 0xFFFF;

          test_buffer[0] = (seq_num >> 8) & 0x00FF;
          test_buffer[1] = seq_num & 0x00FF;

          test_buffer[2] = 0x01;              
          test_buffer[3] = TUYA_DP_TYPE_BOOL; 
          test_buffer[4] = 0x00;
          test_buffer[5] = 0x01;            
          test_buffer[6] = 0x01;              

          test_buffer[7]  = 0x02;             
          test_buffer[8]  = TUYA_DP_TYPE_ENUM; 
          test_buffer[9]  = 0x00;
          test_buffer[10] = 0x01;            
          test_buffer[11] = 0x00;         

          // --- DP 4: Color Temperature (8 bytes) ---
          // Range: 0 - 1000 (0 = warm / 370 mireds, 1000 = cold / 153 mireds)

          uint16_t color_temperature = mapFloat(
            whiteTemperature, 0, 100, 0, 1000);
          test_buffer[12] = 0x04;               
          test_buffer[13] = TUYA_DP_TYPE_VALUE; 
          test_buffer[14] = 0x00;
          test_buffer[15] = 0x04;               
          test_buffer[16] = 0x00;
          test_buffer[17] = 0x00;
          test_buffer[18] = (color_temperature >> 8) & 0x00FF;
          test_buffer[19] = color_temperature & 0x00FF;

          // --- DP 3: White Brightness (8 bytes) ---
          // Range: 10 - 1000

          uint16_t _white_brightness_1000 = mapFloat(
            brightness, 1, 100, 10, 1000);
          
          test_buffer[20] = 0x03;             
          test_buffer[21] = TUYA_DP_TYPE_VALUE; 
          test_buffer[22] = 0x00;
          test_buffer[23] = 0x04;              
          test_buffer[24] = 0x00;
          test_buffer[25] = 0x00;
          test_buffer[26] = (_white_brightness_1000 >> 8) & 0x00FF;
          test_buffer[27] = _white_brightness_1000 & 0x00FF;

          zbGateway.sendCustomClusterCmd(
            _short_addr, _endpoint, TUYA_PRIVATE_CLUSTER_EF00, TUYA_REQUEST_CMD, 
            ESP_ZB_ZCL_ATTR_TYPE_SET, 28, test_buffer, false);
        } break;


        default: {
          
          uint8_t level = mapFloat(brightness, 1, 100, 1, 254);
          zbGateway.sendLevelMoveToLevelCmd(_short_addr, _endpoint, level, 1);
  
          log_i(
            "SUPLA_CHANNELFNC_DIMMER_CCT_AND_RGB - sending white "
            "temperature!");

          uint16_t color_temperature = mapFloat(
            whiteTemperature, 0, 100, _max_warm_cct, _min_cool_cct);
        
	          zbGateway.sendColorMoveToColorTemperatureCmd(
              _short_addr, _endpoint, color_temperature, 1);
        } break;
      }
    }
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RGBCCTInterface::syncDevice() {

  if (Zigbee.started()) {

    uint8_t sync_counter = 0;

    if (_device_state == RGBCCT_STATE_UNKNOWN) {

      log_i("syncing RGBCCT device state...");

      zbGateway.sendAttributeRead(
        _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
        ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, false);
    }
    else
      sync_counter++;

    if (_device_color_mode == RGBCCT_COLOR_MODE_UNKNOWN) {

      log_i("syncing RGBCCT device color mode...");

      zbGateway.sendAttributeRead(
        _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 
        ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_MODE_ID, false);
    }
    else
      sync_counter++;
    if (sync_counter == 2) {

      _fresh_start = false;
      //if ((_device_state == RGBCCT_STATE_ON))
      log_i("sync done...");
      if ((_brightness > 0) || (_colorBrightness > 0))
        turnOnOff = 1;
      else
        turnOnOff = 0;
      _lastServerMsgReceivedMs = 1;
    }
  }
}

/*****************************************************************************/


void Supla::Control::Z2S_RGBCCTInterface::ping() {

  if (Zigbee.started()) {

    _fresh_start = false;
    
    zbGateway.sendAttributeRead(
      _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
      ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, false);
  }
}

/*****************************************************************************/


void Supla::Control::Z2S_RGBCCTInterface::turnOn() {

  if (turnOnOff == 0) {

    turnOnOff = 1;

    if (_last_colorBrightness > 0)
      _colorBrightness = _last_colorBrightness;
    else {

      _last_colorBrightness = _colorBrightness;
      _colorBrightness = 100;
    }

    if (_red == 0 && _green == 0 && _blue == 0) {

      if (_last_red > 0 || _last_green > 0 || _last_blue > 0) {
        
        _red = _last_red;
        _green = _last_green;
        _blue = _last_blue;
      } 
      else {
        _green = 255; 
      }
    }

    _lastServerMsgReceivedMs = 1;
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RGBCCTInterface::turnOnDimmer() {

  log_i("turnOnOff %u", turnOnOff);

  if ((turnOnOff > 0) && (_brightness > 0))
    return; 

  if (turnOnOff == 0) {
    
    turnOnOff = 1;
  } 
  else {
    
    _last_colorBrightness = _colorBrightness;
    _last_red = _red;
    _last_green = _green;
    _last_blue = _blue;
    _colorBrightness = 0;
    turnOnOff = 2;
  }

  if (_last_brightness > 0) {
    
    _brightness = _last_brightness;
  } 
  else {
    
    _last_brightness = _brightness;
    _brightness = 100;
  }

  if (_whiteTemperature == 0) {
    
    _whiteTemperature = _last_whiteTemperature;
  }

  _lastServerMsgReceivedMs = 1;
}

/*****************************************************************************/

void Supla::Control::Z2S_RGBCCTInterface::turnOff() {

  if (turnOnOff > 0)
    turnOnOff = 0;

  if (_colorBrightness > 0) {

    _last_red = _red;
    _last_green = _green;
    _last_blue = _blue;
    _last_colorBrightness = _colorBrightness;
    _colorBrightness = 0;
  }

  if (_brightness > 0) {
    
    _last_brightness = _brightness;
    _last_whiteTemperature = _whiteTemperature;
    _brightness = 0;
  }

  _lastServerMsgReceivedMs = 1;
}

/*****************************************************************************/

void Supla::Control::Z2S_RGBCCTInterface::iterateAlways() {

  if (getRGBColorMode() == Z2S_TUYA_DP_COLOR_HS_RGB)
    _fresh_start = false;
  
  if (_fresh_start && ((millis() - _last_sync_ms) > 5000)) {

    _last_sync_ms = millis();
    syncDevice();
    return;
  }

  if ((_lastServerMsgReceivedMs != 0) && 
      (millis() - _lastServerMsgReceivedMs >= 400)) {
    
    _lastServerMsgReceivedMs = 0;

    log_i(
      "[S] red %d, green %d, blue %d, color brightness %d, brightness %d," 
      "whiteTemperature %d", _red, _green, _blue, _colorBrightness, 
      _brightness, _whiteTemperature);
      

    channel.setNewValue(
      _red, _green, _blue, _colorBrightness, _brightness, _whiteTemperature);

    Supla::Storage::ScheduleSave(5000, 2000);
    
    sendValueToDevice(
      _red, _green, _blue, _colorBrightness, _brightness, _whiteTemperature);
  }

  if ((_lastDeviceMsgReceivedMs != 0) && 
      (millis() - _lastDeviceMsgReceivedMs >= 400)) {
  
    _lastDeviceMsgReceivedMs = 0;

    log_i(
      "[D] red %d, green %d, blue %d, color brightness %d, brightness %d" 
      "whiteTemperature %d", _red, _green, _blue, _colorBrightness, 
      _brightness, _whiteTemperature);

    channel.setNewValue(
      _red, _green, _blue, _colorBrightness, _brightness, _whiteTemperature);
      
    Supla::Storage::ScheduleSave(5000, 2000);
  }

  //if (_fresh_start && ((millis() - _last_ping_ms) > 5000))
  //  ping();

  if (getKeepAliveMs() && ((millis() - _last_ping_ms) > getKeepAliveMs())) {
    
    if (true) {
      
      
      _last_seen_ms = getZbDeviceLastSeenMs();

      if ((millis() - _last_seen_ms) > getKeepAliveMs()) {
      	ping();
        _last_ping_ms = millis();
      } else {
        _last_ping_ms = _last_seen_ms;
        if (!channel.isStateOnline()) 
	        channel.setStateOnline();
      }
    }
  }
  if (getTimeoutMs() && channel.isStateOnline() && 
      ((millis() - _last_seen_ms) > getTimeoutMs())) {

	  log_i("current_millis %u, _last_seen_ms %u", millis(), _last_seen_ms);

    
    _last_seen_ms = getZbDeviceLastSeenMs();

    log_i("current_millis %u, _last_seen_ms(updated) %u", millis(), _last_seen_ms);

    if ((millis() - _last_seen_ms) > getTimeoutMs())
      channel.setStateOffline();
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RGBCCTInterface::increaseBrightness(
  int8_t add_to_brightness) {

  bool wasOn = isOn();

  int16_t new_brightness = _brightness + add_to_brightness;

  if (new_brightness < 0) new_brightness = 0;
  if (new_brightness > 100) new_brightness = 100;

  if (_brightness > 0) {
    _last_brightness = _brightness;
  }

  if (new_brightness > 0 && _colorBrightness > 0) {
    _last_red = _red;
    _last_green = _green;
    _last_blue = _blue;
    _last_colorBrightness = _colorBrightness;
    _colorBrightness = 0;
  }

  if (_brightness == 0 && new_brightness > 0 && _whiteTemperature == 0) {
    
    _whiteTemperature = _last_whiteTemperature;
  }


  if (_brightness > 0 && new_brightness == 0) {
    
    _last_whiteTemperature = _whiteTemperature;
  }

  _brightness = new_brightness;

  if (!isOn()) {
    turnOnOff = 0;
  } 
  else if (!wasOn) {
    turnOnOff = 1;
  } 
  else {
    turnOnOff = 2;
  }

  _lastServerMsgReceivedMs = 1; // Or: (turnOnOff == 2) ? (millis() | 1) : 1;
  Supla::Storage::ScheduleSave(5000, 2000);
}

/*****************************************************************************/

void Supla::Control::Z2S_RGBCCTInterface::handleAction(
  int event, int action) {

  (void)(event);
  
  switch (action) {


    case TURN_ON:

      if (!isOn())
        turnOn();
     break;


    case TURN_OFF:

      if (isOn())
        turnOff();
     break;


    case DIM_W:

      increaseBrightness(-10);
    break;


    case BRIGHTEN_W: 
      
      increaseBrightness(10);
    break;


    case TURN_ON_W:

      turnOnDimmer();
    break;
  }

  if ((action >= Z2S_SUPLA_ACTION_BRIGHTEN_W_WITH_STEP_FIRST_ACTION) &&
      (action <= Z2S_SUPLA_ACTION_BRIGHTEN_W_WITH_STEP_LAST_ACTION)) {

    int16_t add_to_brightness = action - 
      Z2S_SUPLA_ACTION_BRIGHTEN_W_WITH_STEP_FIRST_ACTION;

    increaseBrightness(add_to_brightness);
  }

  if ((action >= Z2S_SUPLA_ACTION_DIM_W_WITH_STEP_FIRST_ACTION) &&
      (action <= Z2S_SUPLA_ACTION_DIM_W_WITH_STEP_LAST_ACTION)) {

    int16_t add_to_brightness = -1 * (action - 
      Z2S_SUPLA_ACTION_DIM_W_WITH_STEP_FIRST_ACTION);
      
    increaseBrightness(add_to_brightness);
  }
}

/*****************************************************************************/

bool void Supla::Control::Z2S_RGBCCTInterface::sendSceneById(
  uint8_t scene_id) {
  const uint8_t *scene_data = nullptr;
  uint8_t scene_len = 0;

  if (scene_id < 16) {

    scene_data = GL_SPI_SCENES[scene_id].data;
    scene_len  = GL_SPI_SCENES[scene_id].len;
  } 
  else {
    
    for (uint8_t i = 0; i < 16; i++) {
      
      if (GL_SPI_SCENES[i].data[1] == scene_id) {
        scene_data = GL_SPI_SCENES[i].data;
        scene_len  = GL_SPI_SCENES[i].len;
        break;
      }
    }
  }

  if (!scene_data || scene_len == 0) {
    return false; 
  }

  uint8_t test_buffer[40];
  uint16_t seq_num = random() & 0xFFFF;

  test_buffer[0] = (seq_num >> 8) & 0x00FF;
  test_buffer[1] = seq_num & 0x00FF;


  test_buffer[2] = 0x01;              
  test_buffer[3] = TUYA_DP_TYPE_BOOL; 
  test_buffer[4] = 0x00;
  test_buffer[5] = 0x01;              
  test_buffer[6] = 0x01;              

  test_buffer[7]  = 0x02;              
  test_buffer[8]  = TUYA_DP_TYPE_ENUM; 
  test_buffer[9]  = 0x00;
  test_buffer[10] = 0x01;              
  test_buffer[11] = 0x02;              

  test_buffer[12] = 51;                
  test_buffer[13] = TUYA_DP_TYPE_RAW;  
  test_buffer[14] = 0x00;
  test_buffer[15] = scene_len;        

  memcpy(&test_buffer[16], scene_data, scene_len);

  uint16_t total_len = 16 + scene_len;

  zbGateway.sendCustomClusterCmd(
    _short_addr, _endpoint, TUYA_PRIVATE_CLUSTER_EF00, TUYA_REQUEST_CMD, 
    ESP_ZB_ZCL_ATTR_TYPE_SET, total_len, test_buffer, false);

  return true;
}

/*****************************************************************************/

bool void Supla::Control::Z2S_RGBCCTInterface::sendMusicMode(
  uint8_t mode_id, uint8_t sensitivity) {

  if (mode_id >= 6) {
    return false; 
  }

  
  if (sensitivity < 1)   sensitivity = 1;
  if (sensitivity > 100) sensitivity = 100;

  uint8_t test_buffer[47];
  uint16_t seq_num = random() & 0xFFFF;

  test_buffer[0] = (seq_num >> 8) & 0x00FF;
  test_buffer[1] = seq_num & 0x00FF;

  test_buffer[2] = 0x01;              
  test_buffer[3] = TUYA_DP_TYPE_BOOL; 
  test_buffer[4] = 0x00;
  test_buffer[5] = 0x01;             
  test_buffer[6] = 0x01;              

  test_buffer[7]  = 0x02;             
  test_buffer[8]  = TUYA_DP_TYPE_ENUM; 
  test_buffer[9]  = 0x00;
  test_buffer[10] = 0x01;              
  test_buffer[11] = 0x03;              

  test_buffer[12] = 52;                
  test_buffer[13] = TUYA_DP_TYPE_RAW; 
  test_buffer[14] = 0x00;
  test_buffer[15] = 31;           

  uint8_t *dp52_payload = &test_buffer[16];
  memcpy(dp52_payload, GL_SPI_MUSIC_BASE, 31);

  dp52_payload[2] = GL_SPI_MUSIC_MODES[mode_id].byte2;
  dp52_payload[3] = GL_SPI_MUSIC_MODES[mode_id].byte3;
  dp52_payload[5] = sensitivity;

  uint8_t c_val = GL_SPI_MUSIC_MODES[mode_id].color_val;
  if (c_val != 0x64) {
    dp52_payload[12] = c_val;
    dp52_payload[15] = c_val;
    dp52_payload[18] = c_val;
    dp52_payload[21] = c_val;
    dp52_payload[24] = c_val;
    dp52_payload[27] = c_val;
  }

  zbGateway.sendCustomClusterCmd(
    _short_addr, _endpoint, TUYA_PRIVATE_CLUSTER_EF00, TUYA_REQUEST_CMD, 
    ESP_ZB_ZCL_ATTR_TYPE_SET, sizeof(test_buffer), test_buffer, false);

  return true;
}

/*****************************************************************************/