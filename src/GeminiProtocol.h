#pragma once

#include <Arduino.h>
#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <freertos/queue.h>

// Auto-enable Gemini Protocol when building for ESP32-S3 or C6 Co-Processor
#if CONFIG_IDF_TARGET_ESP32S3 || defined(Z2S_C6_COPROCESSOR_FIRMWARE)
  #ifndef USE_GEMINI_PROTOCOL
    #define USE_GEMINI_PROTOCOL 1
  #endif
#endif

#if defined(USE_GEMINI_PROTOCOL)
  #if CONFIG_IDF_TARGET_ESP32S3
    #define USE_GEMINI_PROTOCOL_HOST 1
  #elif defined(Z2S_C6_COPROCESSOR_FIRMWARE)
    #define USE_GEMINI_PROTOCOL_NCP  1
  #else
    #undef USE_GEMINI_PROTOCOL
  #endif
#endif

#include "ZigbeeGateway.h"

#ifndef GEMINI_UART_NUM
  #define GEMINI_UART_NUM      UART_NUM_1
#endif
#ifndef GEMINI_UART_BAUD
  #define GEMINI_UART_BAUD     2000000 // 2 Mbps
#endif

#if defined(USE_GEMINI_PROTOCOL_HOST) // ESP32-S3 Host Pins
  #ifndef GEMINI_PIN_TX
    #define GEMINI_PIN_TX      17
    #define GEMINI_PIN_RX      18
    #define GEMINI_PIN_RTS     15
    #define GEMINI_PIN_CTS     16
    #define GEMINI_PIN_C6_EN   38
    #define GEMINI_PIN_C6_BOOT 39
  #endif
#elif defined(USE_GEMINI_PROTOCOL_NCP) // ESP32-C6 Co-Processor Pins
  #ifndef GEMINI_PIN_TX
    #define GEMINI_PIN_TX      4
    #define GEMINI_PIN_RX      5
    #define GEMINI_PIN_RTS     7
    #define GEMINI_PIN_CTS     6
  #endif
#endif

#define GEMINI_SYNC_1          0x5A // 'Z'
#define GEMINI_SYNC_2          0x32 // '2'
#define GEMINI_MAX_PAYLOAD     320

enum GeminiMsgType : uint8_t {
  // --- S3 Host -> C6 NCP Commands ---
  G_CMD_ZB_BEGIN             = 0x01,
  G_CMD_OPEN_NETWORK         = 0x02,
  G_CMD_FACTORY_RESET        = 0x03,
  G_CMD_SET_CHANNEL_MASK     = 0x04,
  G_CMD_FIND_ENDPOINT        = 0x05,
  G_CMD_ACTIVE_EP_DISC       = 0x06,
  G_CMD_SIMPLE_DESC_REQ      = 0x07,
  G_CMD_IEEE_ADDR_REQ        = 0x08,
  G_CMD_CLEAR_LOCAL_BINDINGS = 0x09,
  G_CMD_SET_TIME_CLUSTER     = 0x0A,
  G_CMD_SET_TX_POWER         = 0x0B,
  G_CMD_GET_TX_POWER         = 0x0C,
  G_CMD_IC_ADD               = 0x0D,
  G_CMD_IC_REMOVE_ALL        = 0x0E,

  G_CMD_QUERY_BASIC          = 0x10,
  G_CMD_READ_ATTR            = 0x11,
  G_CMD_READ_ATTR_MULT       = 0x12,
  G_CMD_WRITE_ATTR           = 0x13,
  G_CMD_CONFIG_REPORT        = 0x14,
  G_CMD_READ_REPORT_CFG      = 0x15,
  G_CMD_READ_REPORT_CMD      = 0x16,
  G_CMD_CUSTOM_CLUSTER_REQ   = 0x17,
  G_CMD_APSDE_RAW_REQ        = 0x18,

  G_CMD_ON_OFF               = 0x20,
  G_CMD_LEVEL_MOVE           = 0x21,
  G_CMD_COLOR_MOVE           = 0x22,
  G_CMD_WINDOW_COVERING      = 0x23,
  G_CMD_IAS_ENROLL_RSP       = 0x24,
  G_CMD_ADD_GROUP            = 0x25,

  G_CMD_BIND_CLUSTER         = 0x30,
  G_CMD_BIND_DIRECT          = 0x31,
  G_CMD_LEAVE_REQ            = 0x32,
  G_CMD_DEV_FACTORY_RESET    = 0x33,
  G_CMD_OTA_NOTIFY_START     = 0x40,

  // --- Synchronous Replies (0x80..0x9F) ---
  G_RSP_STATUS               = 0x80,
  G_RSP_QUERY_BASIC          = 0x81,
  G_RSP_READ_ATTR            = 0x82,
  G_RSP_WRITE_ATTR           = 0x83,
  G_RSP_READ_REPORT_CFG      = 0x84,
  G_RSP_CUSTOM_CMD           = 0x85,
  G_RSP_SIMPLE_DESC          = 0x86,
  G_RSP_OTA_CHUNK            = 0x87,
  G_RSP_TX_POWER             = 0x88,

  // --- C6 NCP -> S3 Host Asynchronous Events ---
  G_EVT_STACK_STARTED        = 0xA0,
  G_EVT_OPEN_NETWORK         = 0xA1,
  G_EVT_DEVICE_UPDATE        = 0xA2,
  G_EVT_DEVICE_ANNCE         = 0xA3,
  G_EVT_DEVICE_LEAVE         = 0xA4,
  G_EVT_NEW_DEVICE_JOINED    = 0xA5,
  G_EVT_ATTR_REPORT          = 0xB0,
  G_EVT_IAS_STATUS_CHANGE    = 0xB1,
  G_EVT_RAW_CUSTOM_CMD       = 0xB2,
  G_EVT_CUSTOM_CLUSTER_CMD   = 0xB3,
  G_EVT_UPDATE_RSSI          = 0xB4,
  G_EVT_OTA_CHUNK_REQ        = 0xC0,
  G_EVT_OTA_STATUS           = 0xC1
};

enum GeminiColorSubCmd : uint8_t {
  G_COLOR_MOVE_HUE             = 1,
  G_COLOR_MOVE_SAT             = 2,
  G_COLOR_MOVE_HUE_SAT         = 3,
  G_COLOR_ENH_MOVE_HUE_SAT     = 4,
  G_COLOR_MOVE_XY              = 5,
  G_COLOR_MOVE_CT              = 6
};

struct __attribute__((packed)) GeminiFrameHeader {
  uint8_t  sync1;
  uint8_t  sync2;
  uint8_t  msg_type;
  uint8_t  seq_num;
  uint16_t payload_len;
};

struct __attribute__((packed)) G_PktZbBegin {
  uint32_t channel_mask;
  uint8_t  role;
  uint8_t  erase_nvs;
  uint8_t  open_network_sec;
};

struct __attribute__((packed)) G_PktEvtStackStarted {
  uint8_t  coordinator_ieee[8];
  uint8_t  extended_pan_id[8];
  uint8_t  current_channel;
  int8_t   tx_power;
};

struct __attribute__((packed)) G_PktTimeClusterSync {
  uint32_t utc_time;
  uint32_t local_time;
  uint8_t  time_status;
};

struct __attribute__((packed)) G_PktIcAdd {
  uint8_t  ieee_addr[8];
  uint8_t  ic_type;
  uint8_t  ic_data[18];
};

struct __attribute__((packed)) G_PktQueryBasicReq {
  zbg_device_params_t device;
  uint8_t             single_attribute;
  uint16_t            attribute_id;
};

struct __attribute__((packed)) G_PktQueryBasicRsp {
  uint8_t                    success;
  query_basic_cluster_data_t data;
};

struct __attribute__((packed)) G_PktReadAttrReq {
  uint16_t short_addr;
  uint8_t  ieee_addr[8];
  uint8_t  src_endpoint;
  uint8_t  dst_endpoint;
  uint16_t cluster_id;
  uint16_t attribute_id;
  uint8_t  ack;
  uint8_t  direction;
  uint8_t  dis_default_resp;
  uint8_t  manuf_specific;
  uint16_t manuf_code;
};

struct __attribute__((packed)) G_PktReadAttrMultReq {
  uint16_t short_addr;
  uint8_t  ieee_addr[8];
  uint8_t  dst_endpoint;
  uint16_t cluster_id;
  uint8_t  attr_number;
  uint16_t attr_ids[32];
};

struct __attribute__((packed)) G_PktReadAttrRsp {
  uint8_t  success;
  uint8_t  status;
  uint16_t attr_id;
  uint8_t  attr_type;
  uint16_t data_size;
  uint8_t  data[256];
};

struct __attribute__((packed)) G_PktWriteAttrReq {
  uint16_t short_addr;
  uint8_t  ieee_addr[8];
  uint8_t  src_endpoint;
  uint8_t  dst_endpoint;
  uint16_t cluster_id;
  uint16_t attribute_id;
  uint8_t  attribute_type;
  uint16_t attribute_size;
  uint8_t  ack;
  uint8_t  manuf_specific;
  uint16_t manuf_code;
  uint8_t  dis_default_resp;
  uint8_t  is_ext;
  uint16_t profile_id;
  uint8_t  value[248];
};

struct __attribute__((packed)) G_PktWriteAttrRsp {
  uint8_t  success;
  uint8_t  status;
  uint16_t attr_id;
};

struct __attribute__((packed)) G_PktConfigReportReq {
  zbg_device_params_t device;
  uint16_t            cluster_id;
  uint16_t            attribute_id;
  uint8_t             attribute_type;
  uint16_t            min_interval;
  uint16_t            max_interval;
  uint8_t             delta_bytes[8];
  uint8_t             ack;
  uint8_t             direction;
  uint8_t             dis_default_resp;
  uint8_t             manuf_specific;
  uint16_t            manuf_code;
};

struct __attribute__((packed)) G_PktReadReportCfgRsp {
  uint8_t                                        success;
  esp_zb_zcl_read_report_config_resp_variable_t variable;
};

struct __attribute__((packed)) G_PktCustomClusterReq {
  uint16_t short_addr;
  uint8_t  ieee_addr[8];
  uint8_t  dst_endpoint;
  int16_t  custom_cluster_id;
  uint16_t custom_command_id;
  uint8_t  data_type;
  uint16_t data_size;
  uint8_t  ack;
  uint8_t  direction;
  uint8_t  dis_default_resp;
  uint8_t  manuf_specific;
  uint16_t manuf_code;
  uint8_t  data[255];
};

struct __attribute__((packed)) G_PktApsdeRawReq {
  uint16_t short_addr;
  uint8_t  tsn;
  uint8_t  dst_endpoint;
  uint16_t cluster_id;
  uint8_t  command_id;
  uint8_t  direction;
  uint8_t  dis_default_resp;
  uint8_t  manuf_specific;
  uint16_t manuf_code;
  uint8_t  is_global;
  uint16_t profile_id;
  uint8_t  payload_size;
  uint8_t  payload[250];
};

struct __attribute__((packed)) G_PktSimpleCmd {
  uint16_t short_addr;
  uint8_t  ieee_addr[8];
  uint8_t  dst_endpoint;
  uint8_t  cmd_sub_type;
  uint16_t param1;
  uint16_t param2;
  uint16_t param3;
  uint8_t  flag1;
};

struct __attribute__((packed)) G_PktBindClusterReq {
  zbg_device_params_t device;
  uint16_t            cluster_id;
  uint8_t             groupcast_flag;
  uint8_t             mode;
};

struct __attribute__((packed)) G_PktBindDirectReq {
  uint8_t  src_ieee[8];
  uint8_t  dst_ieee[8];
  uint16_t dst_short_addr;
  uint8_t  src_ep;
  uint8_t  dst_ep;
  uint16_t cluster_id;
  uint8_t  unbind;
};

struct __attribute__((packed)) G_PktOtaNotifyStart {
  uint8_t  ieee_addr[8];
  uint8_t  header_raw[64];
};

struct __attribute__((packed)) G_PktOtaChunkReq {
  uint32_t offset;
  uint8_t  size;
};

struct __attribute__((packed)) G_PktOtaChunkRsp {
  uint8_t  bytes_read;
  uint8_t  data[128];
};

struct __attribute__((packed)) G_PktEvtDeviceLifecycle {
  uint16_t short_addr;
  uint8_t  ieee_addr[8];
  uint8_t  param;
};

struct __attribute__((packed)) G_PktEvtAttrReport {
  uint16_t short_addr;
  uint16_t src_endpoint;
  uint16_t cluster_id;
  uint16_t attr_id;
  uint8_t  attr_type;
  uint8_t  is_reporting;
  uint8_t  is_basic_cluster;
  int8_t   rssi;
  uint16_t data_size;
  uint8_t  data[];
};

struct __attribute__((packed)) G_PktEvtIasStatus {
  uint16_t short_addr;
  uint16_t src_endpoint;
  uint16_t cluster_id;
  uint16_t zone_status;
  uint8_t  extended_status;
  uint8_t  zone_id;
  uint16_t delay;
  int8_t   rssi;
};

struct __attribute__((packed)) G_PktEvtRawCustomCmd {
  uint16_t short_addr;
  uint16_t endpoint_id;
  uint16_t cluster_id;
  uint8_t  cmd_id;
  uint16_t group_id;
  int8_t   rssi;
  uint8_t  buffer_size;
  uint8_t  buffer[];
};

struct __attribute__((packed)) G_PktEvtCustomClusterCmd {
  uint8_t  tsn;
  uint8_t  ieee_addr[8];
  uint16_t short_addr;
  uint16_t endpoint_id;
  uint16_t cluster_id;
  uint8_t  command_id;
  uint16_t payload_size;
  uint8_t  payload[];
};

static inline uint16_t geminiCrc16(const uint8_t *data, size_t len, uint16_t crc = 0xFFFF) {
  while (len--) {
    crc ^= (uint16_t)(*data++) << 8;
    for (uint8_t i = 0; i < 8; i++) {
      crc = (crc & 0x8000) ? ((crc << 1) ^ 0x1021) : (crc << 1);
    }
  }
  return crc;
}

#if defined(USE_GEMINI_PROTOCOL)
class GeminiBridge {
public:
  static bool initUart();
  static bool sendFrame(uint8_t msg_type, const void *payload, uint16_t len, uint8_t seq = 0);
  static bool sendCommandWaitReply(
    uint8_t cmd_type, const void *cmd_payload, uint16_t cmd_len,
    uint8_t expected_rsp_type, void *rsp_buf, uint16_t max_rsp_len,
    uint32_t timeout_ms = 3000);

  static void forwardAttrReport(
    uint16_t short_addr, uint16_t src_endpoint, uint16_t cluster_id,
    const esp_zb_zcl_attribute_t *attribute, bool reporting,
    bool is_basic = false, int8_t rssi = 0);

  static size_t requestOtaChunkFromHost(uint8_t *dest, uint32_t offset, uint8_t size);
  static void sendStackStartedEvt();

  // S3 Host shims for direct ZBOSS SDK calls in Z2S_Gateway.ino & z2s_web_gui.cpp
  static void getCoordinatorIeee(uint8_t *ieee);
  static void getExtendedPanId(uint8_t *ext_pan_id);
  static uint8_t getCurrentChannel();
  static void hostGetTxPower(int8_t *power);
  static void hostSetTxPower(int8_t power);
  static esp_err_t hostIcAdd(uint8_t *ieee, uint8_t ic_type, uint8_t *ic_data);
  static esp_err_t hostIcRemoveAll();
  static esp_zb_zcl_status_t hostSetTimeAttr(uint16_t attr_id, void *value_p);
  static esp_zb_zcl_attr_t *hostGetTimeAttr(uint16_t attr_id);

private:
  static void rxTask(void *arg);
  static void handleFrame(const GeminiFrameHeader &hdr, const uint8_t *payload);
  static void cmdWorkerTask(void *arg);

  static SemaphoreHandle_t _tx_mutex;
  static SemaphoreHandle_t _sync_cmd_mutex;
  static SemaphoreHandle_t _sync_rsp_sem;
  static volatile uint8_t  _expected_rsp_type;
  static volatile uint8_t  _active_seq;
  static void             *_sync_rsp_dest;
  static uint16_t          _sync_rsp_max_len;
  static volatile bool     _sync_rsp_ok;
};
#endif

#if defined(USE_GEMINI_PROTOCOL_HOST)
  #if !SOC_IEEE802154_SUPPORTED
    static inline void esp_coex_wifi_i154_enable() {}
  #endif

  #define esp_zb_lock_acquire(t)                true
  #define esp_zb_lock_release()                 ((void)0)
  #define esp_zb_get_long_address(a)            GeminiBridge::getCoordinatorIeee(a)
  #define esp_zb_get_extended_pan_id(a)         GeminiBridge::getExtendedPanId(a)
  #define esp_zb_get_current_channel()          GeminiBridge::getCurrentChannel()
  #define esp_zb_get_tx_power(p)                GeminiBridge::hostGetTxPower(p)
  #define esp_zb_set_tx_power(p)                GeminiBridge::hostSetTxPower(p)
  #define esp_zb_secur_ic_add(ieee, t, ic)      GeminiBridge::hostIcAdd(ieee, t, ic)
  #define esp_zb_secur_ic_remove_all_req()      GeminiBridge::hostIcRemoveAll()
  #define esp_zb_zcl_set_attribute_val(ep, cl, role, attr_id, val_p, chk) \
    GeminiBridge::hostSetTimeAttr(attr_id, val_p)
  #define esp_zb_zcl_get_attribute(ep, cl, role, attr_id) \
    GeminiBridge::hostGetTimeAttr(attr_id)
  #define esp_zb_get_primary_network_channel_set() (1UL << GeminiBridge::getCurrentChannel())
  #define esp_zb_nwk_get_next_neighbor(it, nb)     ESP_ERR_NOT_FOUND
  #define esp_zb_zdo_mgmt_lqi_req(req, cb, ctx)    ((void)0)
#endif