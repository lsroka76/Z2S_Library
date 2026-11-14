#include "soc/usb_serial_jtag_struct.h"
#include "driver/gpio.h"

#include "GeminiProtocol.h"

#if defined(USE_GEMINI_PROTOCOL)

#include "ZigbeeCore.h"
#include "ZigbeeGateway.h"

#if defined(USE_GEMINI_PROTOCOL_NCP)
#include <Update.h>
#include <esp_partition.h>

#define ZB_STORAGE_SIZE_16K  0x4000UL  // 16,384 bytes (legacy backup)
#define ZB_STORAGE_SIZE_64K  0x10000UL // 65,536 bytes (64 KB backup)
#define ZB_PAGE_OFFSET_16K   0x2000UL  // 8 KB  (legacy Page 1 offset)
#define ZB_PAGE_OFFSET_64K   0x8000UL  // 32 KB (new 64 KB Page 1 offset)

static const esp_partition_t *s_zb_stage_part   = nullptr;
static uint32_t               s_zb_stage_offset = 0;
static bool                   s_zb_stage_error  = false;

static bool copyStageToZbStorage(
  const esp_partition_t *stage_part, const esp_partition_t *zb_part,
  uint32_t src_off, uint32_t dst_off, uint32_t len)
{
  uint8_t buf[256];
  for (uint32_t off = 0; off < len; off += sizeof(buf)) {
    if (esp_partition_read(stage_part, src_off + off, buf, sizeof(buf)) != ESP_OK ||
        esp_partition_write(zb_part, dst_off + off, buf, sizeof(buf)) != ESP_OK) {
      return false;
    }
  }
  return true;
}
#endif


static char s_ncp_version[24] = "NCP-1.0.0";

SemaphoreHandle_t GeminiBridge::_tx_mutex          = nullptr;
SemaphoreHandle_t GeminiBridge::_sync_cmd_mutex    = nullptr;
SemaphoreHandle_t GeminiBridge::_sync_rsp_sem      = nullptr;
volatile uint8_t  GeminiBridge::_expected_rsp_type = 0;
volatile uint8_t  GeminiBridge::_active_seq        = 0;
void             *GeminiBridge::_sync_rsp_dest     = nullptr;
uint16_t          GeminiBridge::_sync_rsp_max_len  = 0;
volatile bool     GeminiBridge::_sync_rsp_ok       = false;

static uint8_t           s_coord_ieee[8]       = {};
static uint8_t           s_ext_pan_id[8]       = {};
static uint8_t           s_current_channel     = 20;
static int8_t            s_tx_power            = 20;
static uint32_t          s_host_utc_time       = 0;
static uint32_t          s_host_local_time     = 0;
static uint8_t           s_host_time_status    = 0x02;
static esp_zb_zcl_attr_t s_dummy_zcl_attr      = {};

struct QueuedCommand {
  GeminiFrameHeader hdr;
  uint8_t           payload[GEMINI_MAX_PAYLOAD];
};

static QueueHandle_t s_cmd_queue = nullptr;

bool GeminiBridge::initUart() {
  if (_tx_mutex != nullptr) return true; // Already initialized

  _tx_mutex       = xSemaphoreCreateMutex();
  _sync_cmd_mutex = xSemaphoreCreateMutex();
  _sync_rsp_sem   = xSemaphoreCreateBinary();

  #if defined(USE_GEMINI_PROTOCOL_HOST)
  // Optional hardware reset of C6 if EN/BOOT pins are wired (>= 0)
  if (GEMINI_PIN_C6_BOOT >= 0) {
    pinMode(GEMINI_PIN_C6_BOOT, OUTPUT);
    digitalWrite(GEMINI_PIN_C6_BOOT, HIGH);
  }
  if (GEMINI_PIN_C6_EN >= 0) {
    pinMode(GEMINI_PIN_C6_EN, OUTPUT);
    digitalWrite(GEMINI_PIN_C6_EN, LOW);
    vTaskDelay(pdMS_TO_TICKS(50));
    digitalWrite(GEMINI_PIN_C6_EN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(200));
  }

  // If using S3 Native USB port pins (GPIO19 D- / GPIO20 D+), disconnect internal USB PHY & pull-up
  if (GEMINI_PIN_TX == 19 || GEMINI_PIN_RX == 20 || GEMINI_PIN_TX == 20 || GEMINI_PIN_RX == 19) {
    USB_SERIAL_JTAG.conf0.usb_pad_enable = 0;
    USB_SERIAL_JTAG.conf0.dp_pullup      = 0;
    gpio_reset_pin((gpio_num_t)GEMINI_PIN_TX);
    gpio_reset_pin((gpio_num_t)GEMINI_PIN_RX);
  }
  #elif defined(USE_GEMINI_PROTOCOL_NCP)
  // If using C6 Native USB port pins (GPIO12 D- / GPIO13 D+), disconnect internal USB PHY & pull-up
  if (GEMINI_PIN_TX == 12 || GEMINI_PIN_RX == 13 || GEMINI_PIN_TX == 13 || GEMINI_PIN_RX == 12) {
    USB_SERIAL_JTAG.conf0.usb_pad_enable = 0;
    USB_SERIAL_JTAG.conf0.dp_pullup      = 0;
    gpio_reset_pin((gpio_num_t)GEMINI_PIN_TX);
    gpio_reset_pin((gpio_num_t)GEMINI_PIN_RX);
  }
  #endif

  bool use_hw_flow = (GEMINI_PIN_RTS >= 0 && GEMINI_PIN_CTS >= 0);

  const uart_config_t uart_cfg = {
    .baud_rate           = GEMINI_UART_BAUD,
    .data_bits           = UART_DATA_8_BITS,
    .parity              = UART_PARITY_DISABLE,
    .stop_bits           = UART_STOP_BITS_1,
    .flow_ctrl           = use_hw_flow ? UART_HW_FLOWCTRL_CTS_RTS : UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = (uint8_t)(use_hw_flow ? 96 : 0),
    .source_clk          = UART_SCLK_DEFAULT,
  };

  int rts_pin = (GEMINI_PIN_RTS >= 0) ? GEMINI_PIN_RTS : UART_PIN_NO_CHANGE;
  int cts_pin = (GEMINI_PIN_CTS >= 0) ? GEMINI_PIN_CTS : UART_PIN_NO_CHANGE;

  ESP_ERROR_CHECK(uart_driver_install(GEMINI_UART_NUM, 4096, 4096, 0, NULL, 0));
  ESP_ERROR_CHECK(uart_param_config(GEMINI_UART_NUM, &uart_cfg));
  ESP_ERROR_CHECK(uart_set_pin(GEMINI_UART_NUM, GEMINI_PIN_TX, GEMINI_PIN_RX, rts_pin, cts_pin));

  // Low-latency FIFO tuning: trigger interrupt after 32 bytes or 2 idle byte-times (~10 us)
  uart_set_rx_full_threshold(GEMINI_UART_NUM, 32);
  uart_set_rx_timeout(GEMINI_UART_NUM, 2);

  s_cmd_queue = xQueueCreate(12, sizeof(QueuedCommand));
  xTaskCreate(cmdWorkerTask, "gemini_wrk", 8192, nullptr, 12, nullptr);
  xTaskCreate(rxTask,        "gemini_rx",  6144, nullptr, 18, nullptr);

  return true;
}

bool GeminiBridge::sendFrame(uint8_t msg_type, const void *payload, uint16_t len, uint8_t seq) {
  if (!_tx_mutex || len > GEMINI_MAX_PAYLOAD) return false;

  GeminiFrameHeader hdr = {
    .sync1       = GEMINI_SYNC_1,
    .sync2       = GEMINI_SYNC_2,
    .msg_type    = msg_type,
    .seq_num     = seq,
    .payload_len = len
  };

  uint16_t crc = geminiCrc16(reinterpret_cast<const uint8_t *>(&hdr), sizeof(hdr));
  if (len > 0 && payload != nullptr) {
    crc = geminiCrc16(static_cast<const uint8_t *>(payload), len, crc);
  }

  xSemaphoreTake(_tx_mutex, portMAX_DELAY);
  uart_write_bytes(GEMINI_UART_NUM, &hdr, sizeof(hdr));
  if (len > 0 && payload != nullptr) {
    uart_write_bytes(GEMINI_UART_NUM, payload, len);
  }
  uart_write_bytes(GEMINI_UART_NUM, &crc, sizeof(crc));
  xSemaphoreGive(_tx_mutex);

  return true;
}

bool GeminiBridge::sendCommandWaitReply(
  uint8_t cmd_type, const void *cmd_payload, uint16_t cmd_len,
  uint8_t expected_rsp_type, void *rsp_buf, uint16_t max_rsp_len,
  uint32_t timeout_ms)
{
  if (!_sync_cmd_mutex) return false;

  xSemaphoreTake(_sync_cmd_mutex, portMAX_DELAY);
  xSemaphoreTake(_sync_rsp_sem, 0);

  static uint8_t s_seq = 1;
  _active_seq        = ++s_seq;
  _expected_rsp_type = expected_rsp_type;
  _sync_rsp_dest     = rsp_buf;
  _sync_rsp_max_len  = max_rsp_len;
  _sync_rsp_ok       = false;

  sendFrame(cmd_type, cmd_payload, cmd_len, _active_seq);

  bool got_reply = (xSemaphoreTake(_sync_rsp_sem, pdMS_TO_TICKS(timeout_ms)) == pdTRUE) && _sync_rsp_ok;
  _expected_rsp_type = 0;
  _sync_rsp_dest     = nullptr;

  xSemaphoreGive(_sync_cmd_mutex);
  return got_reply;
}

void GeminiBridge::setNcpVersion(const char *ver) {
  if (ver) {
    strncpy(s_ncp_version, ver, sizeof(s_ncp_version) - 1);
    s_ncp_version[sizeof(s_ncp_version) - 1] = '\0';
  }
}

const char *GeminiBridge::getNcpVersion() {
  return s_ncp_version;
}

void GeminiBridge::sendStackStartedEvt() {
#if defined(USE_GEMINI_PROTOCOL_NCP)
  G_PktEvtStackStarted evt = {};
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_get_long_address(evt.coordinator_ieee);
  esp_zb_get_extended_pan_id(evt.extended_pan_id);
  evt.current_channel = esp_zb_get_current_channel();
  esp_zb_get_tx_power(&evt.tx_power);
  esp_zb_lock_release();
  evt.is_factory_new = Zigbee.isFactoryStack() ? 1 : 0;
  strncpy(evt.ncp_version, s_ncp_version, sizeof(evt.ncp_version) - 1);
  sendFrame(G_EVT_STACK_STARTED, &evt, sizeof(evt));
#endif
}

bool GeminiBridge::hostNcpOtaBegin() {
  uint8_t rsp[2] = {};
  if (!sendCommandWaitReply(G_CMD_NCP_OTA_BEGIN, nullptr, 0, G_RSP_STATUS, rsp, sizeof(rsp), 5000)) {
    return false;
  }
  return (rsp[0] != 0);
}

bool GeminiBridge::hostNcpOtaWrite(const uint8_t *data, size_t len) {
  size_t offset = 0;
  while (offset < len) {
    uint16_t chunk = ((len - offset) > 256) ? 256 : (uint16_t)(len - offset);
    G_PktNcpOtaWrite pkt = {};
    pkt.len = chunk;
    memcpy(pkt.data, data + offset, chunk);

    uint8_t rsp[2] = {};
    if (!sendCommandWaitReply(
          G_CMD_NCP_OTA_WRITE, &pkt, sizeof(uint16_t) + chunk, G_RSP_STATUS, rsp, sizeof(rsp), 3000) ||
        rsp[0] == 0) {
      return false;
    }
    offset += chunk;
  }
  return true;
}

bool GeminiBridge::hostNcpOtaEnd() {
  uint8_t rsp[2] = {};
  if (!sendCommandWaitReply(G_CMD_NCP_OTA_END, nullptr, 0, G_RSP_STATUS, rsp, sizeof(rsp), 5000)) {
    return false;
  }
  return (rsp[0] != 0);
}

#if defined(USE_GEMINI_PROTOCOL_HOST)
extern "C" bool z2s_ncp_ota_begin() { return GeminiBridge::hostNcpOtaBegin(); }
extern "C" bool z2s_ncp_ota_write(const uint8_t *data, size_t len) { return GeminiBridge::hostNcpOtaWrite(data, len); }
extern "C" bool z2s_ncp_ota_end() { return GeminiBridge::hostNcpOtaEnd(); }
#endif

bool GeminiBridge::hostZbStorageBegin() {
  uint8_t rsp[2] = {};
  if (!sendCommandWaitReply(G_CMD_ZB_STORAGE_BEGIN, nullptr, 0, G_RSP_STATUS, rsp, sizeof(rsp), 5000)) {
    return false;
  }
  return (rsp[0] != 0);
}

bool GeminiBridge::hostZbStorageWrite(const uint8_t *data, size_t len) {
  size_t offset = 0;
  while (offset < len) {
    uint16_t chunk = ((len - offset) > 256) ? 256 : (uint16_t)(len - offset);
    G_PktNcpOtaWrite pkt = {};
    pkt.len = chunk;
    memcpy(pkt.data, data + offset, chunk);

    uint8_t rsp[2] = {};
    if (!sendCommandWaitReply(
          G_CMD_ZB_STORAGE_WRITE, &pkt, sizeof(uint16_t) + chunk, G_RSP_STATUS, rsp, sizeof(rsp), 3000) ||
        rsp[0] == 0) {
      return false;
    }
    offset += chunk;
  }
  return true;
}

bool GeminiBridge::hostZbStorageEnd() {
  uint8_t rsp[2] = {};
  if (!sendCommandWaitReply(G_CMD_ZB_STORAGE_END, nullptr, 0, G_RSP_STATUS, rsp, sizeof(rsp), 10000)) {
    return false;
  }
  return (rsp[0] != 0);
}

#if defined(USE_GEMINI_PROTOCOL_HOST)
extern "C" bool z2s_zb_storage_begin() { return GeminiBridge::hostZbStorageBegin(); }
extern "C" bool z2s_zb_storage_write(const uint8_t *data, size_t len) { return GeminiBridge::hostZbStorageWrite(data, len); }
extern "C" bool z2s_zb_storage_end() { return GeminiBridge::hostZbStorageEnd(); }
#endif

uint32_t GeminiBridge::hostZbStorageSize() {
  uint32_t part_size = 0;
  if (!sendCommandWaitReply(
        G_CMD_ZB_STORAGE_INFO, nullptr, 0, G_RSP_ZB_STORAGE_INFO, &part_size, sizeof(part_size), 2000)) {
    return 0;
  }
  return part_size;
}

bool GeminiBridge::hostZbStorageRead(uint32_t offset, uint8_t *dst, size_t len) {
  if (!dst) return false;
  size_t copied = 0;
  while (copied < len) {
    uint16_t chunk = ((len - copied) > 256) ? 256 : (uint16_t)(len - copied);
    G_PktZbStorageReadReq req = {
      .offset = offset + (uint32_t)copied,
      .len    = chunk
    };
    G_PktZbStorageReadRsp rsp = {};

    if (!sendCommandWaitReply(
          G_CMD_ZB_STORAGE_READ, &req, sizeof(req),
          G_RSP_ZB_STORAGE_READ, &rsp, sizeof(rsp), 2000) ||
        !rsp.success || rsp.len != chunk) {
      return false;
    }
    memcpy(dst + copied, rsp.data, chunk);
    copied += chunk;
  }
  return true;
}

#if defined(USE_GEMINI_PROTOCOL_HOST)
extern "C" uint32_t z2s_zb_storage_size() { return GeminiBridge::hostZbStorageSize(); }
extern "C" bool z2s_zb_storage_read(uint32_t offset, uint8_t *dst, size_t len) {
  return GeminiBridge::hostZbStorageRead(offset, dst, len);
}
#endif


void GeminiBridge::getCoordinatorIeee(uint8_t *ieee) {
  if (ieee) memcpy(ieee, s_coord_ieee, 8);
}

void GeminiBridge::getExtendedPanId(uint8_t *ext_pan_id) {
  if (ext_pan_id) memcpy(ext_pan_id, s_ext_pan_id, 8);
}

uint8_t GeminiBridge::getCurrentChannel() {
  return s_current_channel;
}

void GeminiBridge::hostGetTxPower(int8_t *power) {
  if (!power) return;
  int8_t rx_pwr = s_tx_power;
  if (sendCommandWaitReply(G_CMD_GET_TX_POWER, nullptr, 0, G_RSP_TX_POWER, &rx_pwr, sizeof(rx_pwr), 500)) {
    s_tx_power = rx_pwr;
  }
  *power = s_tx_power;
}

void GeminiBridge::hostSetTxPower(int8_t power) {
  s_tx_power = power;
  sendFrame(G_CMD_SET_TX_POWER, &power, sizeof(power));
}

esp_err_t GeminiBridge::hostIcAdd(uint8_t *ieee, uint8_t ic_type, uint8_t *ic_data) {
  if (!ieee || !ic_data) return ESP_ERR_INVALID_ARG;
  G_PktIcAdd pkt = {};
  memcpy(pkt.ieee_addr, ieee, 8);
  pkt.ic_type = ic_type;
  memcpy(pkt.ic_data, ic_data, 18);
  return sendFrame(G_CMD_IC_ADD, &pkt, sizeof(pkt)) ? ESP_OK : ESP_FAIL;
}

esp_err_t GeminiBridge::hostIcRemoveAll() {
  return sendFrame(G_CMD_IC_REMOVE_ALL, nullptr, 0) ? ESP_OK : ESP_FAIL;
}

esp_zb_zcl_status_t GeminiBridge::hostSetTimeAttr(uint16_t attr_id, void *value_p) {
  if (!value_p) return ESP_ZB_ZCL_STATUS_FAIL;
  if (attr_id == ESP_ZB_ZCL_ATTR_TIME_TIME_ID) {
    memcpy(&s_host_utc_time, value_p, 4);
  } else if (attr_id == ESP_ZB_ZCL_ATTR_TIME_TIME_STATUS_ID) {
    s_host_time_status = *(uint8_t *)value_p;
  } else if (attr_id == ESP_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID) {
    memcpy(&s_host_local_time, value_p, 4);
    G_PktTimeClusterSync pkt = {
      .utc_time    = s_host_utc_time,
      .local_time  = s_host_local_time,
      .time_status = s_host_time_status
    };
    sendFrame(G_CMD_SET_TIME_CLUSTER, &pkt, sizeof(pkt));
  }
  return ESP_ZB_ZCL_STATUS_SUCCESS;
}

esp_zb_zcl_attr_t *GeminiBridge::hostGetTimeAttr(uint16_t attr_id) {
  if (attr_id == ESP_ZB_ZCL_ATTR_TIME_TIME_ID)
    s_dummy_zcl_attr.data_p = &s_host_utc_time;
  else if (attr_id == ESP_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID)
    s_dummy_zcl_attr.data_p = &s_host_local_time;
  else
    s_dummy_zcl_attr.data_p = &s_host_time_status;
  return &s_dummy_zcl_attr;
}

size_t GeminiBridge::requestOtaChunkFromHost(uint8_t *dest, uint32_t offset, uint8_t size) {
  G_PktOtaChunkReq req = { .offset = offset, .size = size };
  G_PktOtaChunkRsp rsp = {};
  if (sendCommandWaitReply(
        G_EVT_OTA_CHUNK_REQ, &req, sizeof(req), G_RSP_OTA_CHUNK, &rsp, sizeof(rsp), 500)) {
    uint8_t copy_len = (rsp.bytes_read <= size) ? rsp.bytes_read : size;
    if (copy_len > 0 && dest) {
      memcpy(dest, rsp.data, copy_len);
    }
    return copy_len;
  }
  return 0;
}

void GeminiBridge::rxTask(void *arg) {
  enum State { WAIT_S1, WAIT_S2, READ_HDR, READ_PAYLOAD, READ_CRC };
  State state = WAIT_S1;

  GeminiFrameHeader hdr = {};
  uint8_t  hdr_bytes[sizeof(GeminiFrameHeader)];
  uint8_t  payload[GEMINI_MAX_PAYLOAD];
  uint8_t  crc_bytes[2];
  uint16_t pos = 0;
  uint8_t  rx_chunk[64];

  for (;;) {
    int read_len = uart_read_bytes(GEMINI_UART_NUM, rx_chunk, sizeof(rx_chunk), pdMS_TO_TICKS(20));
    for (int i = 0; i < read_len; i++) {
      uint8_t b = rx_chunk[i];
      switch (state) {
        case WAIT_S1:
          if (b == GEMINI_SYNC_1) { hdr_bytes[0] = b; state = WAIT_S2; }
          break;
        case WAIT_S2:
          if (b == GEMINI_SYNC_2) { hdr_bytes[1] = b; pos = 2; state = READ_HDR; }
          else if (b == GEMINI_SYNC_1) { hdr_bytes[0] = b; state = WAIT_S2; }
          else { state = WAIT_S1; }
          break;
        case READ_HDR:
          hdr_bytes[pos++] = b;
          if (pos == sizeof(GeminiFrameHeader)) {
            memcpy(&hdr, hdr_bytes, sizeof(hdr));
            if (hdr.payload_len > GEMINI_MAX_PAYLOAD) {
              state = WAIT_S1;
            } else if (hdr.payload_len == 0) {
              pos = 0;
              state = READ_CRC;
            } else {
              pos = 0;
              state = READ_PAYLOAD;
            }
          }
          break;
        case READ_PAYLOAD:
          payload[pos++] = b;
          if (pos == hdr.payload_len) {
            pos = 0;
            state = READ_CRC;
          }
          break;
        case READ_CRC:
          crc_bytes[pos++] = b;
          if (pos == 2) {
            uint16_t rx_crc = 0;
            memcpy(&rx_crc, crc_bytes, 2);
            uint16_t calc_crc = geminiCrc16(hdr_bytes, sizeof(hdr));
            if (hdr.payload_len > 0) {
              calc_crc = geminiCrc16(payload, hdr.payload_len, calc_crc);
            }
            if (rx_crc == calc_crc) {
              handleFrame(hdr, payload);
            } else {
              log_e("GeminiBridge CRC mismatch on msg 0x%02X", hdr.msg_type);
            }
            state = WAIT_S1;
          }
          break;
      }
    }
  }
}

void GeminiBridge::handleFrame(const GeminiFrameHeader &hdr, const uint8_t *payload) {
  if (hdr.msg_type >= 0x80 && hdr.msg_type < 0xA0) {
    if (hdr.msg_type == _expected_rsp_type && hdr.seq_num == _active_seq) {
      if (_sync_rsp_dest && hdr.payload_len <= _sync_rsp_max_len) {
        memcpy(_sync_rsp_dest, payload, hdr.payload_len);
        _sync_rsp_ok = true;
      }
      xSemaphoreGive(_sync_rsp_sem);
    }
    return;
  }

  QueuedCommand item;
  item.hdr = hdr;
  if (hdr.payload_len > 0) {
    memcpy(item.payload, payload, hdr.payload_len);
  }
  xQueueSend(s_cmd_queue, &item, 0);
}

void GeminiBridge::forwardAttrReport(
  uint16_t short_addr, uint16_t src_endpoint, uint16_t cluster_id,
  const esp_zb_zcl_attribute_t *attribute, bool reporting,
  bool is_basic, int8_t rssi)
{
  if (!attribute) return;
  uint16_t data_size = (attribute->data.value && attribute->data.size <= 256)
                       ? attribute->data.size : 0;

  uint8_t buf[sizeof(G_PktEvtAttrReport) + 256];
  G_PktEvtAttrReport *pkt = reinterpret_cast<G_PktEvtAttrReport *>(buf);
  pkt->short_addr       = short_addr;
  pkt->src_endpoint     = src_endpoint;
  pkt->cluster_id       = cluster_id;
  pkt->attr_id          = attribute->id;
  pkt->attr_type        = attribute->data.type;
  pkt->is_reporting     = reporting ? 1 : 0;
  pkt->is_basic_cluster = is_basic ? 1 : 0;
  pkt->rssi             = rssi;
  pkt->data_size        = data_size;

  if (data_size > 0) {
    memcpy(pkt->data, attribute->data.value, data_size);
  }
  sendFrame(G_EVT_ATTR_REPORT, buf, sizeof(G_PktEvtAttrReport) + data_size);
}

void GeminiBridge::cmdWorkerTask(void *arg) {
  QueuedCommand item;
  for (;;) {
    if (xQueueReceive(s_cmd_queue, &item, portMAX_DELAY) != pdTRUE) continue;

    const uint8_t *p = item.payload;

    #if defined(USE_GEMINI_PROTOCOL_HOST)
    // =========================================================================
    // S3 HOST WORKER: Process incoming events from C6 NCP
    // =========================================================================
    switch (item.hdr.msg_type) {
      
      case G_EVT_STACK_STARTED: {
        if (item.hdr.payload_len >= 18) {
          const auto *evt = reinterpret_cast<const G_PktEvtStackStarted *>(p);
          memcpy(s_coord_ieee, evt->coordinator_ieee, 8);
          memcpy(s_ext_pan_id, evt->extended_pan_id, 8);
          s_current_channel = evt->current_channel;
          s_tx_power        = evt->tx_power;
          if (item.hdr.payload_len >= 19) {
            Zigbee._is_factory_new = (evt->is_factory_new != 0);
          }
          if (item.hdr.payload_len >= sizeof(G_PktEvtStackStarted) && evt->ncp_version[0] != '\0') {
            strncpy(s_ncp_version, evt->ncp_version, sizeof(s_ncp_version) - 1);
            s_ncp_version[sizeof(s_ncp_version) - 1] = '\0';
          }
        }
        Zigbee._started   = true;
        Zigbee._connected = true;
        xSemaphoreGive(Zigbee.lock);
      } break;

      case G_EVT_OPEN_NETWORK: {
        uint8_t open_sec = *p;
        Zigbee.setNetworkOpen(open_sec > 0);
        zbGateway.zbOpenNetwork(open_sec);
      } break;

      case G_EVT_DEVICE_UPDATE: {
        const auto *evt = reinterpret_cast<const G_PktEvtDeviceLifecycle *>(p);
        esp_zb_ieee_addr_t ieee;
        memcpy(ieee, evt->ieee_addr, 8);
        zbGateway.zbDeviceUpdate(evt->short_addr, ieee, evt->param);
      } break;

      case G_EVT_DEVICE_ANNCE: {
        const auto *evt = reinterpret_cast<const G_PktEvtDeviceLifecycle *>(p);
        esp_zb_ieee_addr_t ieee;
        memcpy(ieee, evt->ieee_addr, 8);
        uint8_t res = zbGateway.zbDeviceAnnce(evt->short_addr, ieee);
        if (res == DEVICE_ANNCE_RESULT_JOIN) {
          if (Zigbee.isNetworkOpen()) {
            sendFrame(G_CMD_FIND_ENDPOINT, &evt->short_addr, sizeof(uint16_t));
          }
        } else if (res == DEVICE_ANNCE_RESULT_REJOIN) {
          zbGateway.zbDeviceRejoin(evt->short_addr, ieee);
        }
      } break;

      case G_EVT_DEVICE_LEAVE: {
        const auto *evt = reinterpret_cast<const G_PktEvtDeviceLifecycle *>(p);
        esp_zb_ieee_addr_t ieee;
        memcpy(ieee, evt->ieee_addr, 8);
        zbGateway.zbDeviceLeave(evt->short_addr, ieee, evt->param);
      } break;

      case G_EVT_NEW_DEVICE_JOINED: {
        zbg_device_params_t *dev = (zbg_device_params_t *)malloc(sizeof(zbg_device_params_t));
        if (dev) {
          memcpy(dev, p, sizeof(zbg_device_params_t));
          zbGateway._joined_devices.push_back(dev);
          zbGateway._new_device_joined = true;
        }
      } break;

      case G_EVT_UPDATE_RSSI: {
        uint16_t short_addr = 0;
        memcpy(&short_addr, p, 2);
        int8_t rssi = (int8_t)p[2];
        if (zbGateway._on_update_device_last_rssi) {
          zbGateway._on_update_device_last_rssi(short_addr, rssi);
        }
      } break;

      case G_EVT_ATTR_REPORT: {
        const auto *evt = reinterpret_cast<const G_PktEvtAttrReport *>(p);
        if (evt->rssi != 0 && zbGateway._on_update_device_last_rssi) {
          zbGateway._on_update_device_last_rssi(evt->short_addr, evt->rssi);
        }
        esp_zb_zcl_attribute_t attr = {};
        attr.id         = evt->attr_id;
        attr.data.type  = (esp_zb_zcl_attr_type_t)evt->attr_type;
        attr.data.size  = evt->data_size;
        attr.data.value = (evt->data_size > 0) ? (void *)evt->data : nullptr;

        esp_zb_zcl_addr_t addr = {};
        addr.addr_type    = ESP_ZB_ZCL_ADDR_TYPE_SHORT;
        addr.u.short_addr = evt->short_addr;

        if (evt->is_basic_cluster) {
          if (zbGateway._on_basic_receive) {
            zbGateway._on_basic_receive(evt->short_addr, evt->src_endpoint, evt->cluster_id, &attr);
          }
        } else {
          zbGateway.zbProcessAttributeReporting(
            addr, evt->src_endpoint, evt->cluster_id, &attr, evt->is_reporting != 0);
        }
      } break;

      case G_EVT_IAS_STATUS_CHANGE: {
        const auto *evt = reinterpret_cast<const G_PktEvtIasStatus *>(p);
        if (evt->rssi != 0 && zbGateway._on_update_device_last_rssi) {
          zbGateway._on_update_device_last_rssi(evt->short_addr, evt->rssi);
        }
        esp_zb_zcl_ias_zone_status_change_notification_message_t msg = {};
        msg.info.src_address.u.short_addr = evt->short_addr;
        msg.info.src_endpoint             = evt->src_endpoint;
        msg.info.cluster                  = evt->cluster_id;
        msg.zone_status                   = evt->zone_status;
        msg.extended_status               = evt->extended_status;
        msg.zone_id                       = evt->zone_id;
        msg.delay                         = evt->delay;
        zbGateway.zbIASZoneStatusChangeNotification(&msg);
      } break;

      case G_EVT_RAW_CUSTOM_CMD: {
        const auto *evt = reinterpret_cast<const G_PktEvtRawCustomCmd *>(p);
        if (evt->rssi != 0 && zbGateway._on_update_device_last_rssi) {
          zbGateway._on_update_device_last_rssi(evt->short_addr, evt->rssi);
        }
        bool handled = false;
        if (zbGateway._on_custom_cmd_receive) {
          handled = zbGateway._on_custom_cmd_receive(
            evt->short_addr, evt->endpoint_id, evt->cluster_id,
            evt->cmd_id, evt->group_id, evt->buffer_size, (uint8_t *)evt->buffer);
        }
        if (!handled && zbGateway._on_cmd_custom_cluster_receive) {
          esp_zb_ieee_addr_t dummy_ieee = {};
          zbGateway._on_cmd_custom_cluster_receive(
            0, dummy_ieee, evt->short_addr, evt->endpoint_id,
            evt->cluster_id, evt->cmd_id, evt->buffer_size, (uint8_t *)evt->buffer);
        }
      } break;

      case G_EVT_CUSTOM_CLUSTER_CMD: {
        const auto *evt = reinterpret_cast<const G_PktEvtCustomClusterCmd *>(p);
        if (zbGateway._on_cmd_custom_cluster_receive) {
          esp_zb_ieee_addr_t ieee;
          memcpy(ieee, evt->ieee_addr, 8);
          zbGateway._on_cmd_custom_cluster_receive(
            evt->tsn, ieee, evt->short_addr, evt->endpoint_id,
            evt->cluster_id, evt->command_id, evt->payload_size, (uint8_t *)evt->payload);
        }
      } break;

      case G_EVT_OTA_CHUNK_REQ: {
        const auto *req = reinterpret_cast<const G_PktOtaChunkReq *>(p);
        G_PktOtaChunkRsp rsp = {};
        uint8_t max_sz = (req->size <= sizeof(rsp.data)) ? req->size : sizeof(rsp.data);
        if (zbGateway._on_fill_ota_buffer) {
          rsp.bytes_read = (uint8_t)zbGateway._on_fill_ota_buffer(rsp.data, req->offset, max_sz);
        }
        sendFrame(G_RSP_OTA_CHUNK, &rsp, sizeof(rsp), item.hdr.seq_num);
      } break;

      case G_EVT_OTA_STATUS: {
        uint32_t upgrade_time = 0;
        memcpy(&upgrade_time, p, 4);
        uint8_t server_status = p[4];
        if (zbGateway._on_fill_ota_buffer) {
          zbGateway._on_fill_ota_buffer(nullptr, upgrade_time, server_status);
        }
      } break;
    }

    #elif defined(USE_GEMINI_PROTOCOL_NCP)
    // =========================================================================
    // C6 NCP WORKER: Execute Zigbee commands requested by S3 Host
    // =========================================================================
    switch (item.hdr.msg_type) {
      
      case G_CMD_ZB_BEGIN: {
        const auto *req = reinterpret_cast<const G_PktZbBegin *>(p);
        if (!Zigbee.started()) {
        
          Zigbee.setPrimaryChannelMask(req->channel_mask);
          Zigbee.setRebootOpenNetwork(req->open_network_sec);
          Zigbee.begin((zigbee_role_t)req->role, req->erase_nvs != 0);
        }
        if (Zigbee.started()) {
          sendStackStartedEvt();
        }
      } break;

      case G_CMD_OPEN_NETWORK: {
        uint8_t open_sec = *p;
        Zigbee.openNetwork(open_sec);
        // Always echo the state back so S3 GUI/LED stays in sync even if C6 was already open!
        sendFrame(G_EVT_OPEN_NETWORK, &open_sec, sizeof(uint8_t));
      } break;

      case G_CMD_FACTORY_RESET: {
        Zigbee.factoryReset();
      } break;

      case G_CMD_SET_CHANNEL_MASK: {
        uint32_t mask = 0;
        memcpy(&mask, p, sizeof(uint32_t));
        Zigbee.setNVRAMChannelMask(mask);
      } break;

      case G_CMD_SET_TIME_CLUSTER: {
        const auto *req = reinterpret_cast<const G_PktTimeClusterSync *>(p);
        uint32_t utc_t = req->utc_time;
        uint32_t loc_t = req->local_time;
        uint8_t  stat  = req->time_status;
        esp_zb_lock_acquire(portMAX_DELAY);
        esp_zb_zcl_set_attribute_val(
          1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
          ESP_ZB_ZCL_ATTR_TIME_TIME_ID, &utc_t, false);
        esp_zb_zcl_set_attribute_val(
          1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
          ESP_ZB_ZCL_ATTR_TIME_TIME_STATUS_ID, &stat, false);
        esp_zb_zcl_set_attribute_val(
          1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
          ESP_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID, &loc_t, false);
        esp_zb_lock_release();
      } break;

      case G_CMD_SET_TX_POWER: {
        int8_t pwr = (int8_t)(*p);
        esp_zb_lock_acquire(portMAX_DELAY);
        esp_zb_set_tx_power(pwr);
        esp_zb_lock_release();
      } break;

      case G_CMD_GET_TX_POWER: {
        int8_t pwr = 20;
        esp_zb_lock_acquire(portMAX_DELAY);
        esp_zb_get_tx_power(&pwr);
        esp_zb_lock_release();
        sendFrame(G_RSP_TX_POWER, &pwr, sizeof(pwr), item.hdr.seq_num);
      } break;

      case G_CMD_IC_ADD: {
        const auto *req = reinterpret_cast<const G_PktIcAdd *>(p);
        esp_zb_ieee_addr_t ieee;
        uint8_t ic[18];
        memcpy(ieee, req->ieee_addr, 8);
        memcpy(ic, req->ic_data, 18);
        esp_zb_lock_acquire(portMAX_DELAY);
        esp_zb_secur_ic_add(ieee, (esp_zb_secur_ic_type_t)req->ic_type, ic);
        esp_zb_lock_release();
      } break;

      case G_CMD_IC_REMOVE_ALL: {
        esp_zb_lock_acquire(portMAX_DELAY);
        esp_zb_secur_ic_remove_all_req();
        esp_zb_lock_release();
      } break;

      case G_CMD_FIND_ENDPOINT: {
        uint16_t short_addr = 0;
        memcpy(&short_addr, p, sizeof(uint16_t));
        esp_zb_zdo_match_desc_req_param_t cmd_req = {};
        cmd_req.dst_nwk_addr     = short_addr;
        cmd_req.addr_of_interest = short_addr;
        zbGateway.findEndpoint(&cmd_req);
      } break;

      case G_CMD_ACTIVE_EP_DISC: {
        uint16_t short_addr = 0;
        memcpy(&short_addr, p, sizeof(uint16_t));
        zbg_device_params_t dev = {};
        dev.short_addr = short_addr;
        zbGateway.zbPrintDeviceDiscovery(&dev);
      } break;

      case G_CMD_SIMPLE_DESC_REQ: {
        const auto *req = reinterpret_cast<const G_PktSimpleCmd *>(p);
        uint32_t app_dev_id = 0;
        zbGateway.sendSimpleDescriptorRequestCmd(req->short_addr, req->dst_endpoint, &app_dev_id);
        sendFrame(G_RSP_SIMPLE_DESC, &app_dev_id, sizeof(app_dev_id), item.hdr.seq_num);
      } break;

      case G_CMD_IEEE_ADDR_REQ: {
        const auto *req = reinterpret_cast<const G_PktSimpleCmd *>(p);
        zbg_device_params_t dev = {};
        dev.short_addr = req->short_addr;
        zbGateway.sendIEEEAddrReqCmd(&dev, req->flag1 != 0);
      } break;

      case G_CMD_CLEAR_LOCAL_BINDINGS: {
        zbGateway.clearLocalBindings();
      } break;

      case G_CMD_QUERY_BASIC: {
        const auto *req = reinterpret_cast<const G_PktQueryBasicReq *>(p);
        zbg_device_params_t dev = req->device;
        zbGateway.clearQueryBasicClusterData();
        bool ok = zbGateway.zbQueryDeviceBasicCluster(&dev, req->single_attribute != 0, req->attribute_id);
        G_PktQueryBasicRsp rsp = {};
        rsp.success = ok ? 1 : 0;
        memcpy(&rsp.data, zbGateway.getQueryBasicClusterData(), sizeof(query_basic_cluster_data_t));
        sendFrame(G_RSP_QUERY_BASIC, &rsp, sizeof(rsp), item.hdr.seq_num);
      } break;

      case G_CMD_READ_ATTR: {
        const auto *req = reinterpret_cast<const G_PktReadAttrReq *>(p);
        zbg_device_params_t dev = {};
        dev.short_addr = req->short_addr;
        dev.endpoint   = req->dst_endpoint;
        memcpy(dev.ieee_addr, req->ieee_addr, 8);

        bool ok = zbGateway.sendAttributeRead(
          &dev, req->cluster_id, req->attribute_id, req->ack != 0,
          req->direction, req->dis_default_resp, req->manuf_specific,
          req->manuf_code, req->src_endpoint);

        if (req->ack) {
          G_PktReadAttrRsp rsp = {};
          rsp.success   = ok ? 1 : 0;
          rsp.status    = *zbGateway.getReadAttrStatusLastResult();
          auto *attr    = zbGateway.getReadAttrLastResult();
          rsp.attr_id   = attr->id;
          rsp.attr_type = attr->data.type;
          rsp.data_size = (attr->data.size <= 256) ? attr->data.size : 256;
          if (attr->data.value && rsp.data_size > 0) {
            memcpy(rsp.data, attr->data.value, rsp.data_size);
          }
          sendFrame(G_RSP_READ_ATTR, &rsp, sizeof(rsp), item.hdr.seq_num);
        }
      } break;

      case G_CMD_READ_ATTR_MULT: {
        const auto *req = reinterpret_cast<const G_PktReadAttrMultReq *>(p);
        zbg_device_params_t dev = {};
        dev.short_addr = req->short_addr;
        dev.endpoint   = req->dst_endpoint;
        memcpy(dev.ieee_addr, req->ieee_addr, 8);
        uint8_t count = (req->attr_number <= 32) ? req->attr_number : 32;
        zbGateway.sendAttributesRead(&dev, req->cluster_id, count, (uint16_t *)req->attr_ids);
      } break;

      case G_CMD_WRITE_ATTR: {
        const auto *req = reinterpret_cast<const G_PktWriteAttrReq *>(p);
        zbg_device_params_t dev = {};
        dev.short_addr = req->short_addr;
        dev.endpoint   = req->dst_endpoint;
        memcpy(dev.ieee_addr, req->ieee_addr, 8);

        bool ok = false;
        if (req->is_ext) {
          ok = zbGateway.sendAttributeWriteExt(
            &dev, req->cluster_id, req->attribute_id,
            (esp_zb_zcl_attr_type_t)req->attribute_type, req->attribute_size,
            (void *)req->value, req->ack != 0, req->manuf_specific,
            req->manuf_code, req->dis_default_resp != 0, req->profile_id);
        } else {
          ok = zbGateway.sendAttributeWrite(
            &dev, req->cluster_id, req->attribute_id,
            (esp_zb_zcl_attr_type_t)req->attribute_type, req->attribute_size,
            (void *)req->value, req->ack != 0, req->manuf_specific,
            req->manuf_code, req->dis_default_resp != 0, req->src_endpoint);
        }
        if (req->ack) {
          G_PktWriteAttrRsp rsp = {
            .success = (uint8_t)(ok ? 1 : 0),
            .status  = (uint8_t)(*zbGateway.getWriteAttrStatusLastResult()),
            .attr_id = *zbGateway.getWriteAttrAttributeIdLastResult()
          };
          sendFrame(G_RSP_WRITE_ATTR, &rsp, sizeof(rsp), item.hdr.seq_num);
        }
      } break;

      case G_CMD_CONFIG_REPORT: {
        const auto *req = reinterpret_cast<const G_PktConfigReportReq *>(p);
        zbg_device_params_t dev = req->device;
        bool ok = zbGateway.setClusterReportingExt(
          &dev, req->cluster_id, req->attribute_id, req->attribute_type,
          req->min_interval, req->max_interval, (void *)req->delta_bytes,
          req->ack != 0, req->direction, req->dis_default_resp,
          req->manuf_specific, req->manuf_code);
        if (req->ack) {
          uint8_t rsp[2] = { (uint8_t)(ok ? 1 : 0), (uint8_t)(*zbGateway.getConfigReportStatusLastResult()) };
          sendFrame(G_RSP_STATUS, rsp, sizeof(rsp), item.hdr.seq_num);
        }
      } break;

      case G_CMD_READ_REPORT_CFG: {
        const auto *req = reinterpret_cast<const G_PktConfigReportReq *>(p);
        zbg_device_params_t dev = req->device;
        bool ok = zbGateway.readClusterReportCfgCmd(
          &dev, req->cluster_id, req->attribute_id, req->ack != 0,
          req->direction, req->dis_default_resp, req->manuf_specific, req->manuf_code);
        if (req->ack) {
          G_PktReadReportCfgRsp rsp = {};
          rsp.success  = ok ? 1 : 0;
          rsp.variable = *zbGateway.getReportConfigRespVariableLastResult();
          sendFrame(G_RSP_READ_REPORT_CFG, &rsp, sizeof(rsp), item.hdr.seq_num);
        }
      } break;

      case G_CMD_READ_REPORT_CMD: {
        const auto *req = reinterpret_cast<const G_PktConfigReportReq *>(p);
        zbg_device_params_t dev = req->device;
        zbGateway.readClusterReportCmd(
          &dev, req->cluster_id, req->attribute_id, req->ack != 0,
          req->direction, req->dis_default_resp, req->manuf_specific, req->manuf_code);
        if (req->ack) {
          uint8_t rsp[2] = { 1, 0 };
          sendFrame(G_RSP_STATUS, rsp, sizeof(rsp), item.hdr.seq_num);
        }
      } break;

      case G_CMD_CUSTOM_CLUSTER_REQ: {
        const auto *req = reinterpret_cast<const G_PktCustomClusterReq *>(p);
        zbg_device_params_t dev = {};
        dev.short_addr = req->short_addr;
        dev.endpoint   = req->dst_endpoint;
        memcpy(dev.ieee_addr, req->ieee_addr, 8);

        bool ok = zbGateway.sendCustomClusterCmd(
          &dev, req->custom_cluster_id, req->custom_command_id,
          (esp_zb_zcl_attr_type_t)req->data_type, req->data_size,
          (uint8_t *)req->data, req->ack != 0, req->direction,
          req->dis_default_resp, req->manuf_specific, req->manuf_code);

        if (req->ack) {
          uint8_t rsp[3] = {
            (uint8_t)(ok ? 1 : 0),
            (uint8_t)(*zbGateway.getCustomCmdStatusLastResult()),
            *zbGateway.getCustomCmdRespToCmdLastResult()
          };
          sendFrame(G_RSP_CUSTOM_CMD, rsp, sizeof(rsp), item.hdr.seq_num);
        }
      } break;

      case G_CMD_APSDE_RAW_REQ: {
        const auto *req = reinterpret_cast<const G_PktApsdeRawReq *>(p);
        zbGateway.sendAPSDEDataRequestCmd(
          req->short_addr, req->tsn, req->dst_endpoint, req->cluster_id,
          req->command_id, req->payload_size, (uint8_t *)req->payload,
          req->direction, req->dis_default_resp, req->manuf_specific,
          req->manuf_code, req->is_global != 0, req->profile_id);
      } break;

      case G_CMD_ON_OFF: {
        const auto *req = reinterpret_cast<const G_PktSimpleCmd *>(p);
        zbg_device_params_t dev = {};
        dev.short_addr = req->short_addr;
        dev.endpoint   = req->dst_endpoint;
        memcpy(dev.ieee_addr, req->ieee_addr, 8);
        zbGateway.sendOnOffCmd(&dev, req->flag1 != 0);
      } break;

      case G_CMD_LEVEL_MOVE: {
        const auto *req = reinterpret_cast<const G_PktSimpleCmd *>(p);
        zbg_device_params_t dev = {};
        dev.short_addr = req->short_addr;
        dev.endpoint   = req->dst_endpoint;
        memcpy(dev.ieee_addr, req->ieee_addr, 8);
        zbGateway.sendLevelMoveToLevelCmd(&dev, (uint8_t)req->param1, req->param2, req->flag1 != 0);
      } break;

      case G_CMD_COLOR_MOVE: {
        const auto *req = reinterpret_cast<const G_PktSimpleCmd *>(p);
        zbg_device_params_t dev = {};
        dev.short_addr = req->short_addr;
        dev.endpoint   = req->dst_endpoint;
        memcpy(dev.ieee_addr, req->ieee_addr, 8);
        switch (req->cmd_sub_type) {
          case G_COLOR_MOVE_HUE:
            zbGateway.sendColorMoveToHueCmd(&dev, (uint8_t)req->param1, req->flag1, req->param2);
            break;
          case G_COLOR_MOVE_SAT:
            zbGateway.sendColorMoveToSaturationCmd(&dev, (uint8_t)req->param1, req->param2);
            break;
          case G_COLOR_MOVE_HUE_SAT:
            zbGateway.sendColorMoveToHueAndSaturationCmd(&dev, (uint8_t)req->param1, (uint8_t)req->param2, req->param3);
            break;
          case G_COLOR_ENH_MOVE_HUE_SAT:
            zbGateway.sendColorEnhancedMoveToHueAndSaturationCmd(&dev, req->param1, (uint8_t)req->param2, req->param3);
            break;
          case G_COLOR_MOVE_XY:
            zbGateway.sendColorMoveToColorCmd(&dev, req->param1, req->param2, req->param3);
            break;
          case G_COLOR_MOVE_CT:
            zbGateway.sendColorMoveToColorTemperatureCmd(&dev, req->param1, req->param2);
            break;
        }
      } break;

      case G_CMD_WINDOW_COVERING: {
        const auto *req = reinterpret_cast<const G_PktSimpleCmd *>(p);
        zbg_device_params_t dev = {};
        dev.short_addr = req->short_addr;
        dev.endpoint   = req->dst_endpoint;
        memcpy(dev.ieee_addr, req->ieee_addr, 8);
        uint16_t val = req->param1;
        zbGateway.sendWindowCoveringCmd(&dev, req->cmd_sub_type, (req->flag1 != 0) ? (void *)&val : nullptr);
      } break;

      case G_CMD_IAS_ENROLL_RSP: {
        const auto *req = reinterpret_cast<const G_PktSimpleCmd *>(p);
        zbg_device_params_t dev = {};
        dev.short_addr = req->short_addr;
        dev.endpoint   = req->dst_endpoint;
        memcpy(dev.ieee_addr, req->ieee_addr, 8);
        zbGateway.sendIASzoneEnrollResponseCmd(&dev, (uint8_t)req->param1, (uint8_t)req->param2);
      } break;

      case G_CMD_ADD_GROUP: {
        const auto *req = reinterpret_cast<const G_PktSimpleCmd *>(p);
        zbg_device_params_t dev = {};
        dev.short_addr = req->short_addr;
        dev.endpoint   = req->dst_endpoint;
        memcpy(dev.ieee_addr, req->ieee_addr, 8);
        zbGateway.sendAddGroupRequestCmd(&dev, req->param1, req->flag1 != 0);
      } break;

      case G_CMD_BIND_CLUSTER: {
        const auto *req = reinterpret_cast<const G_PktBindClusterReq *>(p);
        zbg_device_params_t dev = req->device;
        if (req->mode == 0)      zbGateway.bindDeviceCluster(&dev, req->cluster_id, req->groupcast_flag);
        else if (req->mode == 1) zbGateway.bindDeviceCluster2(&dev, req->cluster_id);
        else if (req->mode == 2) zbGateway.unbindLocalDeviceCluster(&dev, req->cluster_id);
        uint8_t rsp[2] = { 1, 0 };
        sendFrame(G_RSP_STATUS, rsp, sizeof(rsp), item.hdr.seq_num);
      } break;

      case G_CMD_BIND_DIRECT: {
        const auto *req = reinterpret_cast<const G_PktBindDirectReq *>(p);
        esp_zb_ieee_addr_t src_ieee, dst_ieee;
        memcpy(src_ieee, req->src_ieee, 8);
        memcpy(dst_ieee, req->dst_ieee, 8);
        zbGateway.bindDevices(
          src_ieee, dst_ieee, req->dst_short_addr, req->src_ep, req->dst_ep,
          req->cluster_id, req->unbind != 0);
      } break;

      case G_CMD_LEAVE_REQ: {
        const auto *req = reinterpret_cast<const G_PktSimpleCmd *>(p);
        esp_zb_ieee_addr_t ieee;
        memcpy(ieee, req->ieee_addr, 8);
        zbGateway.sendDeviceLeaveRequest(ieee, req->short_addr, req->param1 != 0, req->flag1 != 0);
      } break;

      case G_CMD_DEV_FACTORY_RESET: {
        const auto *req = reinterpret_cast<const G_PktSimpleCmd *>(p);
        zbg_device_params_t dev = {};
        dev.short_addr = req->short_addr;
        dev.endpoint   = req->dst_endpoint;
        memcpy(dev.ieee_addr, req->ieee_addr, 8);
        zbGateway.sendDeviceFactoryReset(&dev, req->flag1 != 0);
      } break;

      case G_CMD_OTA_NOTIFY_START: {
        const auto *req = reinterpret_cast<const G_PktOtaNotifyStart *>(p);
        esp_zb_ieee_addr_t ieee;
        memcpy(ieee, req->ieee_addr, 8);
        esp_err_t err = zbGateway.sendOTAUpgradeServerNotifyRequest(
          ieee, (uint8_t *)req->header_raw, nullptr);
        uint8_t rsp[2] = { (uint8_t)(err == ESP_OK ? 1 : 0), (uint8_t)err };
        sendFrame(G_RSP_STATUS, rsp, sizeof(rsp), item.hdr.seq_num);
      } break;
      
      case G_CMD_NCP_OTA_BEGIN: {
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        bool ok = Update.begin(maxSketchSpace, U_FLASH);
        log_i("C6 NCP OTA Begin (max=%lu): %s", maxSketchSpace, ok ? "OK" : "FAIL");
        uint8_t rsp[2] = { (uint8_t)(ok ? 1 : 0), (uint8_t)Update.getError() };
        sendFrame(G_RSP_STATUS, rsp, sizeof(rsp), item.hdr.seq_num);
      } break;

      case G_CMD_NCP_OTA_WRITE: {
        const auto *req = reinterpret_cast<const G_PktNcpOtaWrite *>(p);
        uint16_t chunk_len = (req->len <= 256) ? req->len : 256;
        size_t written = Update.write((uint8_t *)req->data, chunk_len);
        bool ok = (written == chunk_len);
        uint8_t rsp[2] = { (uint8_t)(ok ? 1 : 0), (uint8_t)Update.getError() };
        sendFrame(G_RSP_STATUS, rsp, sizeof(rsp), item.hdr.seq_num);
      } break;

      case G_CMD_NCP_OTA_END: {
        bool ok = Update.end(true);
        log_i("C6 NCP OTA End: %s", ok ? "SUCCESS! Rebooting C6..." : "FAIL");
        uint8_t rsp[2] = { (uint8_t)(ok ? 1 : 0), (uint8_t)Update.getError() };
        sendFrame(G_RSP_STATUS, rsp, sizeof(rsp), item.hdr.seq_num);
        if (ok) {
          uart_wait_tx_done(GEMINI_UART_NUM, pdMS_TO_TICKS(100));
          vTaskDelay(pdMS_TO_TICKS(250));
          esp_restart();
        }
      } break;

      case G_CMD_ZB_STORAGE_BEGIN: {
        s_zb_stage_offset = 0;
        s_zb_stage_error  = true;
        s_zb_stage_part   = esp_partition_find_first(
          ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "reserved");

        esp_err_t err = ESP_ERR_NOT_FOUND;
        if (s_zb_stage_part && s_zb_stage_part->size >= ZB_STORAGE_SIZE_64K) {
          err = esp_partition_erase_range(s_zb_stage_part, 0, ZB_STORAGE_SIZE_64K);
          if (err == ESP_OK) {
            s_zb_stage_error = false;
          }
        }
        log_i("C6 zb_storage Stage Begin: %s", !s_zb_stage_error ? "OK" : "FAIL");
        uint8_t rsp[2] = { (uint8_t)(!s_zb_stage_error ? 1 : 0), (uint8_t)err };
        sendFrame(G_RSP_STATUS, rsp, sizeof(rsp), item.hdr.seq_num);
      } break;

      case G_CMD_ZB_STORAGE_WRITE: {
        const auto *req = reinterpret_cast<const G_PktNcpOtaWrite *>(p);
        uint16_t chunk_len = (req->len <= 256) ? req->len : 256;
        esp_err_t err = ESP_FAIL;

        if (!s_zb_stage_error && s_zb_stage_part &&
            (s_zb_stage_offset + chunk_len <= ZB_STORAGE_SIZE_64K)) {
          err = esp_partition_write(s_zb_stage_part, s_zb_stage_offset, req->data, chunk_len);
          if (err == ESP_OK) {
            s_zb_stage_offset += chunk_len;
          } else {
            s_zb_stage_error = true;
          }
        } else {
          s_zb_stage_error = true;
        }

        uint8_t rsp[2] = { (uint8_t)(!s_zb_stage_error ? 1 : 0), (uint8_t)err };
        sendFrame(G_RSP_STATUS, rsp, sizeof(rsp), item.hdr.seq_num);
      } break;

      case G_CMD_ZB_STORAGE_END: {
        bool ok = false;
        uint8_t err_code = 0xFF;

        const esp_partition_t *zb_part = esp_partition_find_first(
          ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "zb_storage");

        // Validate staged upload size is exactly 16 KB (16384) or 64 KB (65536)
        if (!s_zb_stage_error && s_zb_stage_part && zb_part &&
            (s_zb_stage_offset == ZB_STORAGE_SIZE_16K || s_zb_stage_offset == ZB_STORAGE_SIZE_64K) &&
            s_zb_stage_offset <= zb_part->size) {

          // Freeze ZBOSS task so it cannot write to zb_storage during flash update
          if (Zigbee.started()) {
            esp_zb_lock_acquire(portMAX_DELAY);
          }

          if (esp_partition_erase_range(zb_part, 0, zb_part->size) == ESP_OK) {
            if (s_zb_stage_offset == ZB_STORAGE_SIZE_16K && zb_part->size == ZB_STORAGE_SIZE_64K) {
              log_i("C6 zb_storage: Auto-upgrading 16KB image to 64KB layout...");
              // Page 0: 0x0000..0x1FFF -> 0x0000..0x1FFF (0x2000..0x7FFF stays 0xFF)
              // Page 1: 0x2000..0x3FFF -> 0x8000..0x9FFF (0xA000..0xFFFF stays 0xFF)
              ok = copyStageToZbStorage(s_zb_stage_part, zb_part, 0x0000, 0x0000, ZB_PAGE_OFFSET_16K) &&
                   copyStageToZbStorage(s_zb_stage_part, zb_part, ZB_PAGE_OFFSET_16K, ZB_PAGE_OFFSET_64K, ZB_PAGE_OFFSET_16K);
            } else {
              log_i("C6 zb_storage: Restoring %lu-byte image 1:1...", (unsigned long)s_zb_stage_offset);
              ok = copyStageToZbStorage(s_zb_stage_part, zb_part, 0x0000, 0x0000, s_zb_stage_offset);
            }
          }
          err_code = ok ? 0 : 1;
        } else {
          log_e("C6 zb_storage End rejected: written=%lu, err=%d",
                (unsigned long)s_zb_stage_offset, (int)s_zb_stage_error);
        }

        uint8_t rsp[2] = { (uint8_t)(ok ? 1 : 0), err_code };
        sendFrame(G_RSP_STATUS, rsp, sizeof(rsp), item.hdr.seq_num);

        if (ok || Zigbee.started()) {
          // If ZBOSS was locked (or update succeeded), flush UART and reboot C6
          uart_wait_tx_done(GEMINI_UART_NUM, pdMS_TO_TICKS(100));
          vTaskDelay(pdMS_TO_TICKS(250));
          esp_restart();
        }
      } break;

      case G_CMD_ZB_STORAGE_INFO: {
        const esp_partition_t *zb_part = esp_partition_find_first(
          ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "zb_storage");
        uint32_t part_size = zb_part ? (uint32_t)zb_part->size : 0;
        sendFrame(G_RSP_ZB_STORAGE_INFO, &part_size, sizeof(part_size), item.hdr.seq_num);
      } break;

      case G_CMD_ZB_STORAGE_READ: {
        const auto *req = reinterpret_cast<const G_PktZbStorageReadReq *>(p);
        G_PktZbStorageReadRsp rsp = {};
        uint16_t rlen = (req->len <= 256) ? req->len : 256;

        const esp_partition_t *zb_part = esp_partition_find_first(
          ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "zb_storage");

        if (zb_part && (req->offset + rlen <= zb_part->size)) {
          if (Zigbee.started()) esp_zb_lock_acquire(portMAX_DELAY);
          esp_err_t err = esp_partition_read(zb_part, req->offset, rsp.data, rlen);
          if (Zigbee.started()) esp_zb_lock_release();

          if (err == ESP_OK) {
            rsp.success = 1;
            rsp.len     = rlen;
          }
        }
        // Only send the active header + requested bytes (3 + rlen) to minimize UART airtime
        sendFrame(G_RSP_ZB_STORAGE_READ, &rsp, 3 + rsp.len, item.hdr.seq_num);
      } break;
    }
    #endif
  }
}

#endif // USE_GEMINI_PROTOCOL
