#ifndef USE_SUPLA_WEB_SERVER

#include <cstdarg>

#include <esp_partition.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "multi_heap.h"

#include <ZigbeeGateway.h>

#include "z2s_web_gui.h"
#include "z2s_devices_database.h"
#include "z2s_devices_table.h"
#include "z2s_device_tuya_custom_cluster.h"
#include "z2s_device_virtual_relay.h"
#include "z2s_device_local_action_handler.h"
#include "TuyaDatapoints.h"

#include "z2s_version_info.h"
#include "z2s_little_fs.h"
#include "task_info.h"

#include <SuplaDevice.h>
#include <supla/storage/littlefs_config.h>
#include <supla/storage/storage.h>

#include <supla/device/register_device.h>
#include <supla/version.h>

#include "web_gui_templates.h"

ESPAsyncHTTPUpdateServer updateServer;

extern multi_heap_handle_t _z2s_custom_heap;

extern bool force_leave_global_flag;

extern int8_t _enable_gui_on_start;
extern int32_t _gui_build_flags;
extern int32_t _gui_start_delay;

extern uint8_t _force_config_on_start;

extern uint8_t _rebuild_Supla_channels_on_start;
extern uint8_t _use_new_at_model;
extern uint32_t _auto_connection_reset_timeout;

extern uint8_t _z2s_security_level;

static volatile bool GUIstarted = false;
static volatile bool GUIbuilt = false;

//UI handles

//uint16_t gui_cmd_hidden_label;
uint16_t gui_cmd_hidden_response_number;

uint16_t gateway_general_info;
uint16_t gateway_memory_info;
uint16_t enable_gui_switcher;
uint16_t force_config_switcher;
uint16_t gui_start_delay_number;
uint16_t auto_connection_reset_timeout_number;
uint16_t gateway_mdns_name_text;
//uint16_t rebuild_Supla_channels_switcher;
uint16_t use_new_at_model_switcher;
uint16_t github_ota_button;
uint16_t github_ota_info_label;

uint16_t wifi_ssid_text;
uint16_t wifi_pass_text;
uint16_t Supla_server;
uint16_t Supla_email;
uint16_t Supla_skip_certificate_switcher;
uint16_t save_button;
uint16_t save_label;

uint16_t pairing_mode_switcher = 0xFFFF;
uint16_t force_leave_switcher = 0xFFFF;
uint16_t zigbee_tx_power_number;
uint16_t zigbee_primary_channel_text;
uint16_t zigbee_primary_channel_label;
uint16_t zigbee_last_binding_result_label;
uint16_t zigbee_installation_code_str;
uint16_t zigbee_installation_code_ieee_address;
uint16_t factory_reset_switcher;
uint16_t factory_reset_label;

uint16_t devicestab;
uint16_t device_selector = 0xFFFF;
uint16_t zb_device_info_label;
uint16_t device_status_label;
uint16_t device_name_text;
uint16_t device_name_save_button;

uint16_t battery_panel_label;
uint16_t battery_voltage_min_number;
uint16_t battery_voltage_max_number;
uint16_t battery_voltage_min_save_button;
uint16_t battery_voltage_max_save_button;
uint16_t disable_battery_percentage_msg_switcher;
uint16_t disable_battery_voltage_msg_switcher;

uint16_t getswbuild_button;
uint16_t getrssi_button;
uint16_t swbuildlabel;
uint16_t rssilabel;

enum clusters_attributes_controls {

	clusters_attributes_tab,
	clusters_attributes_device_selector,
	clusters_attributes_device_info_label,
	device_endpoint_number,
	device_cluster_selector,
	device_attribute_id_text,
	device_read_attribute_label,
	device_attribute_id_selector,
	device_attribute_type_selector,
	device_attribute_value_selector,
	device_attribute_size_number,
	device_config_min_number,
	device_config_max_number,
	device_config_delta_number,
	device_attribute_value_text,
	device_async_switcher,
	device_read_attribute_button,
	device_write_attribute_button,
	device_read_config_button,
	device_write_config_button,
	device_custom_command_button,
	manufacturer_code_switcher,
	manufacturer_code_selector,
	device_Tuya_payload_label,
	device_bind_button,
	device_bind_2_button,
	device_last_enum_position
};

uint16_t clusters_attributes_table[device_last_enum_position];

uint16_t remove_device_and_channels_button;
uint16_t remove_all_devices_button;
uint16_t start_zigbee_ota_button;
uint16_t zigbee_ota_status_label;

uint16_t channel_selector = 0xFFFF;
uint16_t channel_status_label;
uint16_t zb_channel_info_label;
uint16_t zb_channel_timings_label;
uint16_t zb_channel_flags_label;
uint16_t zb_channel_params_label;
uint16_t channel_name_text;
uint16_t channel_name_save_button;
uint16_t local_channel_function_selector = 0xFFFF;
uint16_t disable_channel_notifications_switcher;
uint16_t trv_auto_to_schedule_switcher;
uint16_t trv_auto_to_schedule_manual_switcher;
uint16_t trv_fixed_calibration_switcher;
uint16_t trv_cooperative_childlock_switcher;
uint16_t set_sorwns_on_start_switcher;
uint16_t enable_resend_temperature_switcher;
uint16_t skip_subdevice_registation_switcher;
uint16_t param_1_text;
uint16_t param_1_desc_label;
uint16_t param_1_save_button;
uint16_t param_2_number;
uint16_t param_2_desc_label;
uint16_t param_2_save_button;
uint16_t param_3_number;
uint16_t param_3_desc_label;
uint16_t param_3_save_button;
uint16_t keepalive_number;
uint16_t timeout_number;
uint16_t refresh_number;
uint16_t keepalive_save_button;
uint16_t timeout_save_button;
uint16_t refresh_save_button;
uint16_t remove_channel_button;
uint16_t lah_status_label;
static uint16_t lah_panel;

uint16_t advanced_device_selector = 0xFFFF;
uint16_t advanced_device_info_label;

uint16_t valve_program_selector;
uint16_t valve_cycles_number;
uint16_t valve_worktime_number;
uint16_t valve_volume_number;
uint16_t valve_pause_number;
uint16_t save_program_button;
uint16_t load_program_button;
uint16_t start_program_button;
uint16_t stop_program_button;
uint16_t send_program_button;
uint16_t send_program_2_button;
uint16_t valve_info_label;

uint16_t gas_alarm_ringtone_selector;
uint16_t gas_alarm_time_number;
uint16_t gas_alarm_ringtone_button;
uint16_t gas_alarm_time_button;
uint16_t gas_alarm_self_test_button;
uint16_t gas_alarm_silence_button;
uint16_t gas_alarm_info_label;
uint16_t gas_alarm_Tuya_payload_label;

uint16_t moes_alarm_melody_selector;
uint16_t moes_alarm_duration_number;
uint16_t moes_alarm_volume_number;
uint16_t moes_alarm_melody_button;
uint16_t moes_alarm_duration_button;
uint16_t moes_alarm_volume_button;
uint16_t moes_alarm_info_label;
uint16_t moes_alarm_Tuya_payload_label;

enum Tuya_devices_tab_controls {

	Tuya_device_selector,
	Tuya_datapoint_id_selector,
	Tuya_datapoint_id_number,
	Tuya_datapoint_type_selector,
	Tuya_datapoint_length_number,
	Tuya_datapoint_value_number,
	Tuya_datapoint_description_label,
	Tuya_device_info_label,
	Tuya_device_cmd_result_label,
	Tuya_device_payload_label,
	Tuya_device_data_request_button,
	Tuya_device_data_query_button,
	Tuya_device_last_enum_position
};

uint16_t Tuya_devices_tab_controls_table[Tuya_device_last_enum_position];

uint16_t actions_table_label_2;

uint16_t action_details_label;
uint16_t action_enabled_switcher;
uint16_t action_name_text;
uint16_t action_description_text;
uint16_t action_source_channel_selector = 0xFFFF;
uint16_t action_destination_channel_selector = 0xFFFF;
uint16_t action_event_selector = 0xFFFF;
uint16_t action_action_selector = 0xFFFF;
uint16_t action_subaction_number;
uint16_t action_condition_threshold_1_number;
uint16_t action_condition_threshold_2_number;
uint16_t action_save_button;
uint16_t action_cancel_button;
uint16_t action_edit_button;
uint16_t action_new_button;
uint16_t action_copy_button;
uint16_t action_remove_button;
uint16_t action_state_label;

uint16_t sb_channel_selector = 0xFFFF;
uint16_t sb_device_id_text;
uint16_t sb_token_text;
uint16_t sb_json_payload_text;
uint16_t sb_json_payload_2_text;
uint16_t sb_status_label;

uint16_t pushover_message_first_button;
uint16_t pushover_message_next_button;
uint16_t pushover_message_prev_button;
uint16_t pushover_message_last_button;

uint16_t pushover_message_subaction_label;
uint16_t pushover_message_save_button;
uint16_t pushover_message_cancel_button;
uint16_t pushover_message_edit_button;
uint16_t pushover_message_new_button;
uint16_t pushover_message_copy_button;
uint16_t pushover_message_remove_button;
uint16_t pushover_message_state_label;

uint16_t ir_device_selector = 0xFFFF;
uint16_t ir_device_info_label;
uint16_t ir_codes_table_label;

uint16_t ir_code_new_button;
uint16_t ir_code_edit_button;
uint16_t ir_code_copy_button;
uint16_t ir_code_save_button;
uint16_t ir_code_cancel_button;
uint16_t ir_code_remove_button;
uint16_t ir_code_learn_button;
uint16_t ir_code_send_button;
uint16_t ir_code_state_label;

uint16_t ir_code_details_label;
uint16_t ir_code_name_text;
uint16_t ir_code_base64_text;


volatile bool data_ready = false;

volatile bool _github_ota_enabled = false;

/*****************************************************************************/
/*****************************************************************************/

struct gui_command_object {

	bool in_use = false;
	gui_commands gui_command = gui_cmd_none;
	int32_t gui_command_value = 0;
};

static gui_command_object gui_commands_pool[GUI_COMMANDS_POOL_SIZE];

QueueHandle_t gui_commands_queue;

/*****************************************************************************/

gui_command_object *getGUICommandObject(
  gui_commands gui_command, int32_t gui_command_value) {

	for (int i = 0; i < GUI_COMMANDS_POOL_SIZE; i++) {

		if (!gui_commands_pool[i].in_use) {

			gui_commands_pool[i].in_use = true;
			gui_commands_pool[i].gui_command = gui_command;
			gui_commands_pool[i].gui_command_value = gui_command_value;
			return &gui_commands_pool[i];
		}
	}
	return nullptr;
}

/*****************************************************************************/

bool addGUICommand(gui_commands gui_command, int32_t gui_command_value = 0) {

	gui_command_object *free_gui_command_slot = getGUICommandObject(
	  gui_command, gui_command_value);

	if (free_gui_command_slot == nullptr) {

		log_e("GUI commands pool full!");
		return false;
	}

	if (xQueueSend(gui_commands_queue, &free_gui_command_slot, 0) == pdTRUE)
		return true;
	else {

		log_e("GUI commands queue send failed!");
		return false;
	}
}

/*****************************************************************************/
/*****************************************************************************/

int32_t current_cluster_id = -1;
int32_t current_attribute_idx = -1;
int32_t current_Tuya_device_id = -1;
int32_t current_Tuya_datapoint_id = -1;

volatile uint8_t remove_all_devices_counter = 2;

volatile uint32_t ota_image_size = 0;
volatile uint32_t ota_image_last_offset = 0;
volatile uint32_t ota_image_last_update_ms = 0;
//volatile uint32_t ota_image_last_offset = 0;

enum GuiBuildControlFlag : uint16_t {
	GUI_BUILD_CONTROL_FLAG_GATEWAY = 1 << 0,      // 0x0001
	GUI_BUILD_CONTROL_FLAG_CREDENTIALS = 1 << 1,  // 0x0002
	GUI_BUILD_CONTROL_FLAG_ZIGBEE = 1 << 2,       // 0x0004
	GUI_BUILD_CONTROL_FLAG_DEVICES = 1 << 3,      // 0x0008
	GUI_BUILD_CONTROL_FLAG_CHANNELS = 1 << 4,     // 0x0010
	GUI_BUILD_CONTROL_FLAG_ACTIONS = 1 << 5,      // 0x0020
	GUI_BUILD_CONTROL_FLAG_CA = 1 << 6,           // 0x0040
	GUI_BUILD_CONTROL_FLAG_AD = 1 << 7,           // 0x0080
	GUI_BUILD_CONTROL_FLAG_TCC = 1 << 8,          // 0x0100
	GUI_BUILD_CONTROL_FLAG_SB = 1 << 9,           // 0x0200
	GUI_BUILD_CONTROL_FLAG_PO = 1 << 10,          // 0x0400
	GUI_BUILD_CONTROL_FLAG_IR = 1 << 11,          // 0x0800
	GUI_BUILD_CONTROL_FLAG_LAST = 1 << 12         // 0x8000
};

volatile uint8_t controls_enabled_flags = 0xFF;

#define DEVICES_CONTROLS_ENABLED_FLAG 0x01
#define CLUSTERS_ATTRIBUTES_CONTROLS_ENABLED_FLAG 0x02
#define CHANNELS_CONTROLS_ENABLED_FLAG 0x04
#define TUYA_DEVICES_ENABLED_FLAG 0x08

volatile uint32_t advanced_device_present_flags = 0x00;

#define SONOFF_VALVE_PRESENT_FLAG 0x01
#define TUYA_GAS_DETECTOR_PRESENT_FLAG 0x02
#define MOES_ALARM_PRESENT_FLAG 0x04

bool isSonoffValvePresent = false;
bool isSonoffDualValvePresent = false;
bool isTuyaGasDetectorPresent = false;
bool isMoesAlarmPresent = false;

volatile uint16_t custom_cmd_tsn = 0;
volatile uint8_t Tuya_custom_cmd_dp = 0xFF;
volatile uint16_t current_Tuya_payload_label = 0;

volatile int16_t current_action_counter = -1;
volatile int16_t new_action_id = -1;
volatile int16_t current_action_id = -1;

int32_t current_ir_device_id = -1;
volatile int16_t current_ir_code_counter = -1;
volatile int16_t current_ir_code_id = -1;
volatile int16_t new_ir_code_id = -1;
volatile bool ir_code_learning_gui_active = false;

volatile ActionGUIState current_action_gui_state = VIEW_ACTION;
volatile ActionGUIState previous_action_gui_state = VIEW_ACTION;
volatile IrCodeGUIState current_ir_gui_state = VIEW_IR_CODE;

volatile int16_t current_message_counter = -1;
volatile int16_t new_message_id = -1;
volatile int16_t current_message_id = -1;


volatile PushoverMessageGUIState current_pushover_message_gui_state =
  VIEW_PUSHOVER_MESSAGE;
volatile PushoverMessageGUIState previous_pushover_message_gui_state =
  VIEW_PUSHOVER_MESSAGE;


#define GUI_CB_ENABLE_GUI_FLAG 0x0100
#define GUI_CB_FORCE_CONFIG_FLAG 0x0101
#define GUI_CB_GUI_DELAY_FLAG 0x0102
#define GUI_CB_ACR_TIMEOUT_FLAG 0x0103
#define GUI_CB_SAVE_MDNS_NAME_FLAG 0x0104
#define GUI_CB_REBUILD_CHANNELS_FLAG 0x0105
#define GUI_CB_USE_NEW_AT_FLAG 0x0106
#define GUI_CB_GUI_RESTART_FLAG 0x0107
#define GUI_CB_GUI_CHECK_GITHUB_OTA_FLAG 0x0108
#define GUI_CB_GUI_PERFORM_GITHUB_OTA_FLAG 0x0109

#define GUI_CB_SAVE_FLAG 0x1000
#define GUI_CB_RESTART_FLAG 0x1001

#define GUI_CB_PAIRING_FLAG 0x2001
#define GUI_CB_FORCE_LEAVE_FLAG 0x2002
#define GUI_CB_FACTORY_FLAG 0x2003
#define GUI_CB_GET_TX_FLAG 0x2004
#define GUI_CB_SET_TX_FLAG 0x2005
#define GUI_CB_GET_PC_FLAG 0x2006
#define GUI_CB_SET_PC_FLAG 0x2007
#define GUI_CB_SET_INSTALLATION_CODE_FLAG 0x2008
#define GUI_CB_CLEAR_INSTALLATION_CODES_FLAG 0x2009

#define GUI_CB_BATTERY_VOLTAGE_MIN_FLAG 0x3000
#define GUI_CB_BATTERY_VOLTAGE_MAX_FLAG 0x3001
#define GUI_CB_DISABLE_PERCENTAGE_MSG_FLAG 0x3002
#define GUI_CB_DISABLE_VOLTAGE_MSG_FLAG 0x3003

#define GUI_CB_UPDATE_DEVICE_NAME_FLAG 0x3005

#define GUI_CB_SWBUILD_FLAG 0x3010
#define GUI_CB_RSSI_FLAG 0x3011

#define GUI_CB_READ_ATTR_FLAG 0x3020
#define GUI_CB_WRITE_ATTR_FLAG 0x3021
#define GUI_CB_CONFIG_REPORT_FLAG 0x3022
#define GUI_CB_READ_CONFIG_FLAG 0x3023
#define GUI_CB_CUSTOM_CMD_FLAG 0x3024

#define GUI_CB_BIND_CLUSTER_FLAG 0x3028
#define GUI_CB_BIND_CLUSTER_2_FLAG 0x3029

#define GUI_CB_SINGLE_FLAG 0x3030
#define GUI_CB_WITH_CHANNELS_FLAG 0x3031
#define GUI_CB_ALL_FLAG 0x3032

#define GUI_CB_UPDATE_CHANNEL_NAME_FLAG 0x4000
#define GUI_CB_UPDATE_CHANNEL_DESC_FLAG 0x4001
#define GUI_CB_UPDATE_CHANNEL_SUB_ID_FLAG 0x4002
#define GUI_CB_UPDATE_CHANNEL_FUNC_FLAG 0x4003
#define GUI_CB_UPDATE_LOCAL_CHANNEL_FUNCTION_FLAG 0x4004
#define GUI_CB_DISABLE_CHANNEL_NOTIFICATIONS_FLAG 0x4005
#define GUI_CB_TRV_AUTO_TO_SCHEDULE_FLAG 0x4006
#define GUI_CB_SET_SORWNS_ON_START_FLAG 0x4007
#define GUI_CB_TRV_FIXED_CALIBRATION_FLAG 0x4008
#define GUI_CB_TRV_AUTO_TO_SCHEDULE_MANUAL_FLAG 0x4009
#define GUI_CB_TRV_COOPERATIVE_CHILDLOCK_FLAG 0x400A
#define GUI_CB_ENABLE_RESEND_TEMPERATURE_FLAG 0x400B
#define GUI_CB_SKIP_SUBDEVICE_REGISTRATION_FLAG 0x400C

#define GUI_CB_UPDATE_KEEPALIVE_FLAG 0x4020
#define GUI_CB_UPDATE_TIMEOUT_FLAG 0x4021
#define GUI_CB_UPDATE_REFRESH_FLAG 0x4022

#define GUI_CB_UPDATE_PARAM_1_FLAG 0x4030
#define GUI_CB_UPDATE_PARAM_2_FLAG 0x4031
#define GUI_CB_UPDATE_PARAM_3_FLAG 0x4032

#define GUI_CB_ADD_AND_HANDLER_FLAG 0x4050
#define GUI_CB_ADD_OR_HANDLER_FLAG 0x4051
#define GUI_CB_ADD_XOR_HANDLER_FLAG 0x4052
#define GUI_CB_ADD_NOT_HANDLER_FLAG 0x4053
#define GUI_CB_ADD_NAND_HANDLER_FLAG 0x4054
#define GUI_CB_ADD_NOR_HANDLER_FLAG 0x4055
#define GUI_CB_ADD_AND3_HANDLER_FLAG 0x4056
#define GUI_CB_ADD_OR3_HANDLER_FLAG 0x4057
#define GUI_CB_ADD_NOP_HANDLER_FLAG 0x4058

#define GUI_CB_SAVE_PROGRAM_FLAG 0x5000
#define GUI_CB_LOAD_PROGRAM_FLAG 0x5001
#define GUI_CB_START_PROGRAM_FLAG 0x5002
#define GUI_CB_STOP_PROGRAM_FLAG 0x5003
#define GUI_CB_SEND_PROGRAM_FLAG 0x5004
#define GUI_CB_SEND_PROGRAM_2_FLAG 0x5005

#define GUI_CB_SELF_TEST_FLAG 0x6000
#define GUI_CB_SILENCE_FLAG 0x6001
#define GUI_CB_SEND_RINGTONE_FLAG 0x6002
#define GUI_CB_SEND_TIME_FLAG 0x6003

#define GUI_CB_SEND_VOLUME_FLAG 0x7000
#define GUI_CB_SEND_MELODY_FLAG 0x7001
#define GUI_CB_SEND_DURATION_FLAG 0x7002

#define GUI_CB_SEND_TUYA_REQUEST_FLAG 0x8000
#define GUI_CB_SEND_TUYA_QUERY_FLAG 0x8001

#define GUI_CB_ACTION_FIRST_FLAG 0x9000
#define GUI_CB_ACTION_NEXT_FLAG 0x9001
#define GUI_CB_ACTION_PREV_FLAG 0x9002
#define GUI_CB_ACTION_LAST_FLAG 0x9003

#define GUI_CB_ACTION_EDIT_FLAG 0x9010
#define GUI_CB_ACTION_NEW_FLAG 0x9011
#define GUI_CB_ACTION_SAVE_FLAG 0x9012
#define GUI_CB_ACTION_CANCEL_FLAG 0x9013
#define GUI_CB_ACTION_REMOVE_FLAG 0x9014
#define GUI_CB_ACTION_COPY_FLAG 0x9015

#define GUI_CB_ADD_SB_1X_FLAG 0xA000
#define GUI_CB_ADD_SB_2X_FLAG 0xA001

#define GUI_CB_PUSHOVER_SAVE_DATA_FLAG 0xB000
#define GUI_CB_PUSHOVER_MESSAGE_FIRST_FLAG 0xB001
#define GUI_CB_PUSHOVER_MESSAGE_NEXT_FLAG 0xB002
#define GUI_CB_PUSHOVER_MESSAGE_PREV_FLAG 0xB003
#define GUI_CB_PUSHOVER_MESSAGE_LAST_FLAG 0xB004

#define GUI_CB_PUSHOVER_MESSAGE_EDIT_FLAG 0xB010
#define GUI_CB_PUSHOVER_MESSAGE_NEW_FLAG 0xB011
#define GUI_CB_PUSHOVER_MESSAGE_SAVE_FLAG 0xB012
#define GUI_CB_PUSHOVER_MESSAGE_CANCEL_FLAG 0xB013
#define GUI_CB_PUSHOVER_MESSAGE_REMOVE_FLAG 0xB014
#define GUI_CB_PUSHOVER_MESSAGE_COPY_FLAG 0xB015

#define GUI_CB_IR_CODE_NEW_FLAG    0xC001
#define GUI_CB_IR_CODE_EDIT_FLAG   0xC002
#define GUI_CB_IR_CODE_COPY_FLAG   0xC003
#define GUI_CB_IR_CODE_SAVE_FLAG   0xC004
#define GUI_CB_IR_CODE_CANCEL_FLAG 0xC005
#define GUI_CB_IR_CODE_REMOVE_FLAG 0xC006
#define GUI_CB_IR_CODE_LEARN_FLAG  0xC007
#define GUI_CB_IR_CODE_SEND_FLAG   0xC008


static constexpr char *three_dots_str PROGMEM = "...";
static constexpr char *empty_str PROGMEM = "";

static constexpr char *device_query_failed_str PROGMEM =
  "Device data query failed - try to wake it up first!";

static constexpr char *device_async_query_str PROGMEM =
  "Device data query sent asynchronously";

constexpr const char device_query_attr_size_error_str[] PROGMEM =
  PAYLOAD_ERROR_MSG("Error - attribute size has to be in range 0...",
                    MAX_ZIGBEE_PAYLOAD_SIZE);

static constexpr char *Tuya_device_payload_size_error_str PROGMEM =
  "Error - Tuya DP payload size has to be in range 0..58";

static constexpr char *device_query_attr_size_mismatch_str PROGMEM =
  "Error - attribute size and attribute value length mismatch";

static constexpr char *Tuya_device_payload_size_mismatch_str PROGMEM =
  "Error - Tuya payload type length and payload value length mismatch";

static constexpr char *factory_reset_enabled_str PROGMEM =
  "Zigbee stack factory reset enabled";

static constexpr char *factory_reset_disabled_str PROGMEM =
  "Zigbee stack factory reset disabled";

/*static constexpr char *zigbee_tx_power_text_str PROGMEM =
  "Press Read to get current value or enter value between "
  "-24 and 20 and press Update";*/

/*static constexpr char *zigbee_primary_channel_info_str PROGMEM =
  "Press Read to get current value or enter value between "
  "11 and 26 and press Update";*/

static constexpr char *zigbee_primary_channel_update_str PROGMEM =
  "New Zigbee primary channel write success!<br>"
  "Unfortunately, due to ZBOSS stack limitation, "
  "new channel can be activated only after "
  "<b><i>Zigbee factory reset</i></b>, "
  "which itself will require re-pairing every device!<br><br>"
  //"with active switch "
  //"<b><i>Force device to bind gateway again</i></b>"
  "TIP: Before re-pairing device <b>don't remove</b> that device from gateway"
	"via Cloud/GUI - all channels settings and actions will remain unchanged.";

static constexpr char *OTA_server_notify_success_str PROGMEM =
  "OTA upgrade server notify sent successfully!";

static constexpr char *OTA_server_notify_failure_str PROGMEM =
  "OTA upgrade server notify failed!";

static constexpr char *dest_thermometer_address_desc =
  "&#10023; Enter destination thermometer IP address or mDNS name &#10023;"
  "<br>for mDNS use <b><i>mdns://</i></b> prefix ie. mdns://my_gateway<br>"
  "or enter 0.0.0.0 for local temperature forwarding";

static constexpr char *dest_thermometer_channel_desc =
  "&#10023; Enter destination thermometer channel # &#10023;";
static constexpr char *src_thermometer_channel_desc =
  "&#10023; Enter source thermometer local channel # &#10023;<br>";

/*static constexpr char *keepalive_desc =
  "&#10023; keepalive (s) &#10023;<br>&#10023;turn on delay (s) [logical"
  " gates]&#10023;<br>&#10023; action trigger hold repeat (ms) &#10023;";*/

static constexpr char *keepalive_desc =
  "keepalive (s) \u2022 turn on delay (s) \u2022 hold repeat (ms)";

static constexpr char *timeout_desc =
	"timeout (s) \u2022 turn off delay (s) [logical gates]";

  //"&#10023; timeout (s) &#10023;<br>&#10023;turn off delay (s) [logical"
  //" gates]&#10023;";

/*static constexpr char *refresh_desc =
  "&#10023; refresh(s) [ElectricityMeter] &#10023;<br>"
  "&#10023; autoset(s) [VirtualBinary] &#10023;<br>"
  "&#10023; debounce(ms) [VirtualSceneSwitch] &#10023; <br>"
  "&#10023; thermometers timeout(s) [Remote Thermometer] &#10023;<br>"
  "&#10023; read interval(s) [VirtualThermhygrometer] &#10023;";*/

static constexpr char *refresh_desc =
  "refresh(s) [ElectricityMeter] \u2022 autoset(s) [VirtualBinary] \u2022 "
  "debounce(ms) [VirtualSceneSwitch] \u2022 thermometers timeout(s) "
	"[Remote Thermometer] \u2022 read interval(s) [VirtualThermhygrometer]";


//static char general_purpose_gui_buffer[1024] = {};

static constexpr char *disabledstyle PROGMEM =
  "background-color: #bbb; border-bottom: #999 3px solid;";

static constexpr char *clearLabelStyle PROGMEM =
  "background-color: unset; width: 100%;";

static constexpr char *clearFlagsLabelStyle PROGMEM =
  "background-color: unset; width: 50%; "
  "vertical-align: text-top;"
  //"margin-left: .3rem; margin-right: .3rem; "
  "font-size: 85%; font-style: normal; "
  "font-weight: normal;";

//

static const char *myCustomJS = R"=====(

function redirect2ZigbeeOTA() {
	
	window.location.assign("/zigbee_ota");
};

// This function catches all "CMD_" payloads from controls.js
window.onGatewayCommand = function(command, labelId) {
    
		console.log("Received gateway command:", command);

    switch (command) {
        
				case "Z2S_GUI_CMD_START_BACKUP":
            
            $("#l" + labelId).html("Downloading...");
            
            (async () => {
                await downloadPartition("nvs", null);
                await downloadPartition("spiffs", null);
                await downloadPartition("zb_storage", null);
                
            })();
            break;

        case "Z2S_GUI_CMD_UI_LOCKDOWN":
            
            console.log("Locking UI for critical OTA...");
            document.body.style.pointerEvents = "none";
            document.body.style.opacity = "0.5";
            break;

				case "CMD_UI_UNLOCK":
            console.log("Unlocking UI...");
            document.body.style.pointerEvents = "auto"; 
            document.body.style.opacity = "1";     
          	break;

        default:
            console.warn("Unknown command received:", command);
            break;
    }
};

/**
 * Populates one or multiple <select> elements with options and preselects a given option.
 * 
 * @param {string|Array<string|HTMLElement>} targetSelects - A single ID/name/selector, or an array of them.
 * @param {Object|Array} optionsData - Pairs of options (Object, Array of pairs, or Array of objects).
 * @param {string|number|null} [selectedValue=null] - Value or ID of the option to preselect.
 * @param {boolean} [clearExisting=true] - Set to false if you want to append instead of replace.
 */
function updateSelectOptions(targetSelects, optionsData, selectedValue = null, clearExisting = true) {
  // 1. Normalize targets into an array
  const targets = Array.isArray(targetSelects) ? targetSelects : [targetSelects];

  // 2. Normalize option data ONCE for efficiency
  let optionsList = [];

  if (Array.isArray(optionsData)) {
    optionsList = optionsData.map(item => {
      if (Array.isArray(item)) {
        return { 
          value: item[0], 
          label: item[1],
          selected: Boolean(item[2])
        };
      }
      return { 
        value: item.value ?? item.val ?? item.id, 
        label: item.label ?? item.name ?? item.text,
        selected: Boolean(item.selected ?? item.isSelected)
      };
    });
  } else if (typeof optionsData === 'object' && optionsData !== null) {
    optionsList = Object.entries(optionsData).map(([value, label]) => ({ 
      value, 
      label,
      selected: false 
    }));
  }

  // Convert target selectedValue to string for safe type matching (e.g. 5 vs "5")
  const targetSelectedStr = (selectedValue !== null && selectedValue !== undefined) 
    ? String(selectedValue) 
    : null;

  // 3. Populate each target <select>
  targets.forEach(target => {
    let selectEl = null;

    if (typeof target === 'string') {
      selectEl = document.getElementById(target) || 
                 document.querySelector(`select[name="${target}"]`) ||
                 document.querySelector(target);
    } else if (target instanceof HTMLElement) {
      selectEl = target;
    }

    if (!selectEl) {
      console.warn(`[Select Populator] Target element '${target}' not found.`);
      return;
    }

    // Clear existing options if requested
    if (clearExisting) {
      selectEl.innerHTML = '';
    }

    // Create fragment for DOM efficiency
    const fragment = document.createDocumentFragment();

    optionsList.forEach(({ value, label, selected }) => {
      const option = document.createElement('option');
      option.value = value;
      option.textContent = label;

      // Match preselection: explicit parameter takes priority, then item-level flag
      const valStr = String(value);
      if (targetSelectedStr !== null) {
        if (valStr === targetSelectedStr) {
          option.selected = true;
        }
      } else if (selected) {
        option.selected = true;
      }

      fragment.appendChild(option);
    });

    selectEl.appendChild(fragment);

    // Trigger change event for listeners on this select
    selectEl.dispatchEvent(new Event('change', { bubbles: true }));
  });
}

function sendActionClickToESP32(actionIndex) {
  // Replace 'text15' with whatever HTML ID ESPUI assigned to your hidden input control
  console.log("sendActionClickToESP32");
	const hiddenInput = document.getElementById('num2'); 
  if (hiddenInput) {
    hiddenInput.value = actionIndex;
    console.log("num2 present");
    // Dispatch a standard browser event. ESPUI listens for 'change' or 'input' 
    // to automatically send data over its WebSocket to your C++ callback!
    //hiddenInput.dispatchEvent(new Event('change', { bubbles: true }));
		numberchange(2);
  }
}


function updateActionTable(data) {

  const targetLabel = document.getElementById(data.target);
  if (!targetLabel) return;

	const isIr = Boolean(data.is_ir);
	const colSpan = isIr ? 3 : 5;
	const emptyMsg = isIr ? "No IR codes saved for this device" : "No actions available";

  let rowsHtml = "";

  // 1. Handle Empty Table State vs Row Building
  if (!data.d || data.d.length === 0) {
    rowsHtml = `<tr>
      <td colspan="5" style="text-align: center; padding: 16px; color: #6c757d; font-style: italic;">`
        + emptyMsg +
      `</td>
    </tr>`;
  } else {
    data.d.forEach((row, index) => {
      let absoluteIndex = (data.start_idx !== undefined ? data.start_idx : 0) + index;
      
      let style = "";
      if (absoluteIndex === data.s) {
        style = "background-color: rgba(0, 123, 255, 0.2); font-weight: bold;";
      } else {
        style = "font-weight: normal;"; 
      }
        
      rowsHtml += `<tr style="${style}" data-action-idx="${absoluteIndex}">`;
      row.forEach(cell => {
        const cleanCell = String(cell).replace(/"/g, '&quot;');
        rowsHtml += `<td style="padding: 6px; border-bottom: 1px solid #eee;" title="${cleanCell}">${cell}</td>`;
      });
      rowsHtml += "</tr>";
    });
  }

  // 2. Append Chunks vs Full Re-render
  if (data.append) {
    let tbody = targetLabel.querySelector("tbody");
    if (tbody) {
      tbody.insertAdjacentHTML('beforeend', rowsHtml);
    }
  } else {
    // 3. Inject CSS (Sticky Headers, Hover, Container Focus Glow, Footer Hints)
    let html = `<style>
      #${data.target} tbody tr[data-action-idx] { cursor: pointer; transition: background 0.15s; }
      #${data.target} tbody tr[data-action-idx]:hover { background-color: rgba(0, 123, 255, 0.2) !important; }
      #${data.target} th { 
        position: sticky; top: 0; background: #e9ecef; color: #212529; 
        font-weight: 600; border-bottom: 2px solid #adb5bd; z-index: 10; 
        box-shadow: 0 2px 4px -2px rgba(0,0,0,0.25); 
      }
      #${data.target} th, #${data.target} td {
        overflow: hidden; text-overflow: ellipsis; white-space: nowrap;
      }
      /* Base container styling with fixed height & scroll containment */
      /* Container supporting dual vertical and horizontal scrolling */
      #${data.target} div[tabindex="0"] {
        height: 210px;
        overflow: auto; /* Enables horizontal scrollbar when width < 720px */
        overscroll-behavior: contain;
        margin-top: 8px; 
        border: 1px solid rgba(128, 128, 128, 0.25);
        border-top-left-radius: 6px;
        border-top-right-radius: 6px;
        transition: border-color 0.15s ease-in-out, box-shadow 0.15s ease-in-out;
      }

      /* Compact Slim Scrollbars for WebKit browsers */
      #${data.target} div[tabindex="0"]::-webkit-scrollbar {
        width: 6px;
        height: 6px; /* Horizontal scrollbar height */
      }
      #${data.target} div[tabindex="0"]::-webkit-scrollbar-track {
        background: rgba(128, 128, 128, 0.1);
        border-radius: 4px;
      }
      #${data.target} div[tabindex="0"]::-webkit-scrollbar-thumb {
        background: rgba(128, 128, 128, 0.35);
        border-radius: 4px;
      }
      #${data.target} div[tabindex="0"]::-webkit-scrollbar-thumb:hover {
        background: rgba(128, 128, 128, 0.55);
      }

      /* Keyboard focus glow state (kept intact) */
      #${data.target} div[tabindex="0"]:focus {
        border-color: #80bdff !important;
        box-shadow: 0 0 0 0.2rem rgba(0, 123, 255, 0.25) !important;
        outline: none !important;
      }
      }
      #${data.target} .table-footer-hints {
        display: flex !important;
        flex-wrap: wrap !important;
        gap: 14px !important;
        justify-content: center !important;
        align-items: center !important;
        background: rgba(0, 0, 0, 0.3) !important;
        border: 1px solid rgba(255, 255, 255, 0.15) !important;
        border-top: none !important;
        border-bottom-left-radius: 6px !important;
        border-bottom-right-radius: 6px !important;
        padding: 5px 10px !important;
        font-size: 10px !important; /* Overrides ESPUI's ~14px body font on plain text */
        line-height: 1 !important;
        color: #cbd5e1 !important;
        user-select: none !important;
      }

      /* Text + Keycap Groupings */
      #${data.target} .table-footer-hints span {
        display: inline-flex !important;
        align-items: center !important;
        gap: 4px !important;
        font-size: 10px !important; /* Strictly enforces 10px on text nodes like "Navigate" */
        line-height: 1 !important;
      }

      /* Keycap Styling */
      #${data.target} .table-footer-hints kbd {
        background-color: #334155 !important;
        border: 1px solid #475569 !important;
        border-radius: 3px !important;
        box-shadow: 0 1px 1px rgba(0,0,0,0.4) !important;
        color: #f8fafc !important;
        display: inline-flex !important;
        align-items: center !important;
        justify-content: center !important;
        font-family: inherit !important;
        font-size: 9px !important;
        font-weight: 600 !important;
        padding: 2px 4px !important;
        line-height: 1 !important;
        height: 16px !important;
      }
			/* Disabled Overlay & Input Lock */
      #${data.target} div.table-disabled {
        pointer-events: none !important;   /* Blocks mouse clicks, hover, and wheel scrolling */
        overflow-y: hidden !important;     /* Locks scrollbar in place */
        opacity: 0.5 !important;           /* Visual disabled state */
        filter: grayscale(50%);
        user-select: none;
      }
			/* Status Badge Styles */
      #${data.target} .table-status-badge {
        display: inline-block;
        padding: 2px 8px;
        border-radius: 12px;
        font-size: 0.7rem;
        font-weight: 600;
        text-transform: uppercase;
        letter-spacing: 0.5px;
        transition: all 0.2s ease;
      }
      #${data.target} .badge-active {
        background-color: #e6f4ea;
        color: #137333;
        border: 1px solid #ceedd5;
      }
      #${data.target} .badge-locked {
        background-color: #fce8e6;
        color: #c5221f;
        border: 1px solid #f5c6cb;
      }
    </style>`;

		const headerHtml = isIr
  	? "<th style='padding: 6px; width: 80px; min-width: 80px;'>ID (sid)</th>" +
    	"<th style='padding: 6px; width: 220px; min-width: 180px;'>Command Name</th>" +
    	"<th style='padding: 6px; width: 420px; min-width: 260px;'>IR Code (Base64)</th>"
  	: "<th style='padding: 6px; width: 130px; min-width: 130px;'>Name (enabled)</th>" +
    	"<th style='padding: 6px; width: 150px; min-width: 150px;'>Supla event</th>" +
    	"<th style='padding: 6px; width: 150px; min-width: 150px;'>Source channel</th>" +
    	"<th style='padding: 6px; width: 140px; min-width: 140px;'>Supla action</th>" +
    	"<th style='padding: 6px; width: 150px; min-width: 150px;'>Destination channel</th>";

    // Scrollable Table Container with Horizontal & Vertical Scroll
    html += "<div tabindex='0' style='height: 210px; margin-top: 8px; overflow: auto; overscroll-behavior: contain; border: 1px solid rgba(128,128,128,0.25); border-top-left-radius: 6px; border-top-right-radius: 6px;'>" +
      "<table style='width:100%; min-width: 720px; table-layout: fixed; border-collapse: collapse; text-align: left;'>" +
      "<thead><tr>" +
       headerHtml +
      "</tr></thead><tbody>" + rowsHtml + "</tbody></table></div>";

    // 5. Footer Hotkey Legend
    // Footer Bar with Hotkey Legend + Status Badge
    html += `<div class="table-footer-hints">
      <span><span class="table-status-badge badge-active">Active</span></span>
      <span><kbd>↑</kbd><kbd>↓</kbd> Navigate</span>
      <span><kbd>PgUp</kbd><kbd>PgDn</kbd> ±5 Rows</span>
      <span><kbd>Home</kbd><kbd>End</kbd> First/Last</span>
    </div>`;

    targetLabel.innerHTML = html;
    targetLabel.style.display = "block";

    const container = targetLabel.querySelector("div[tabindex='0']");
    const tbody = targetLabel.querySelector("tbody");

    if (tbody && container) {
      // Unified Selection & Event Dispatch Handler
      const activateRow = (rowElement, shouldScroll = true) => {
        if (!rowElement) return;

        const actionIndex = parseInt(rowElement.getAttribute("data-action-idx"));
        if (isNaN(actionIndex)) return;

        // 1. Visual highlight
        highlightActionRow({ target: data.target, s: actionIndex }, shouldScroll);

        // Trigger custom callback to send this index back to the ESP32
      
        sendActionClickToESP32(isIr ? (actionIndex | 0x4000) : actionIndex);
        
      };

      // 1. Mouse Click Listener
      tbody.addEventListener("click", function(e) {
        const clickedRow = e.target.closest("tr[data-action-idx]");
        if (!clickedRow) return;

        container.focus();
        activateRow(clickedRow, false); // Mouse click: do not scroll jump
      });

      // 2. Keyboard Navigation Listener
      container.addEventListener("keydown", function(e) {
        const navKeys = ["ArrowUp", "ArrowDown", "Home", "End", "PageUp", "PageDown", "Enter", " "];
        if (!navKeys.includes(e.key)) return;

        const rows = tbody.querySelectorAll("tr[data-action-idx]");
        if (!rows.length) return;

        let currentIndex = 0;
        rows.forEach((row, idx) => {
          if (row.style.fontWeight === "bold") {
            currentIndex = idx;
          }
        });

        // Pressing Enter or Space triggers an explicit row click
        if (e.key === "Enter" || e.key === " ") {
          e.preventDefault();
          rows[currentIndex].click();
          return;
        }

        let nextIndex = currentIndex;
        if (e.key === "ArrowUp") {
          nextIndex = Math.max(0, currentIndex - 1);
        } else if (e.key === "ArrowDown") {
          nextIndex = Math.min(rows.length - 1, currentIndex + 1);
        } else if (e.key === "PageUp") {
          nextIndex = Math.max(0, currentIndex - 5);
        } else if (e.key === "PageDown") {
          nextIndex = Math.min(rows.length - 1, currentIndex + 5);
        } else if (e.key === "Home") {
          nextIndex = 0;
        } else if (e.key === "End") {
          nextIndex = rows.length - 1;
        }

        if (nextIndex !== currentIndex) {
          e.preventDefault(); // Prevent standard page viewport scrolling
          activateRow(rows[nextIndex], true); // Keyboard move: auto-scroll to view
        }
      });
    }
  }
}

function setTableDisabled(targetId, isDisabled) {
  const targetLabel = document.getElementById(targetId);
  if (!targetLabel) return;

  const container = targetLabel.querySelector("div[tabindex]");
  const badge = targetLabel.querySelector(".table-status-badge");

  if (isDisabled) {
    if (container) {
      container.classList.add("table-disabled");
      container.setAttribute("tabindex", "-1"); // Disable keyboard nav
      container.blur();
    }
    if (badge) {
      badge.textContent = "Locked";
      badge.className = "table-status-badge badge-locked";
    }
  } else {
    if (container) {
      container.classList.remove("table-disabled");
      container.setAttribute("tabindex", "0");  // Enable keyboard nav
    }
    if (badge) {
      badge.textContent = "Active";
      badge.className = "table-status-badge badge-active";
    }
  }
}

function highlightActionRow(data) {
  const targetLabel = document.getElementById(data.target);
  if (!targetLabel) return;

  // Select all table rows inside the tbody of your target label
  const rows = targetLabel.querySelectorAll("tbody tr");
  
  rows.forEach((row, index) => {
    if (index === data.s) {
  		row.style.backgroundColor = "rgba(0, 123, 255, 0.2)";
  		row.style.fontWeight = "bold";
			row.scrollIntoView({ behavior: "smooth", block: "nearest" });
  		// row.style.color = "white"; 
		} else {
  		row.style.backgroundColor = "";
  		row.style.fontWeight = "normal";
  		// row.style.color = ""; 
		}
	});
}

function editActionRow(data) {
  const targetLabel = document.getElementById(data.target);
  if (!targetLabel) return;

  const rows = targetLabel.querySelectorAll("tbody tr");
  
  if (data.idx < rows.length) {
    const cells = rows[data.idx].querySelectorAll("td");
    
    data.d.forEach((newCellContent, i) => {
      if (cells[i]) {
        cells[i].innerHTML = newCellContent;
        cells[i].title = newCellContent; // Updates tooltip dynamically
      }
    });
  }
}

const originalParse = JSON.parse;

JSON.parse = function(text, reviver) {
  const data = originalParse(text, reviver);
  
  if (data && typeof data === 'object') {
    
    // Custom Command 1: Execute arbitrary JS
    if (data.type === "EXEC_JS") {
      console.log("Custom payload intercepted!", data.code);
      try { eval(data.code); } catch (err) { console.error(err); }
      return { type: -999, id: -1, value: "" }; 
    }

    // Custom Command 2: Update select options
    if (data.type === 'update_select') {
      updateSelectOptions(
        data.targets || data.target, data.options, data.selected, 
        data.clear ?? true
      );
      return { type: -999, id: -1, value: "" }; 
    }

    // Custom Command 3: Render action table
    if (data.type === 'update_action_table') {
      updateActionTable(data);
      return { type: -998, id: -1, value: "" }; 
    }

		if (data.type === 'highlight_action_row') {
      highlightActionRow(data);
      return { type: -997, id: -1, value: "" }; 
    }

		if (data.type === 'edit_action_row') {
      editActionRow(data);
      return { type: -999, id: -1, value: "" }; 
    }
  }
  
  // Normal ESPUI message passthrough
  return data;
};

console.log("JSON.parse hook successfully installed!");

async function downloadPartition(label, btn) {
  if (btn) btn.innerText = `Downloading ${label}... 0%`;

  const response = await fetch(`/partition?label=${label}`);
  if (!response.ok) {
    throw new Error(`Failed to fetch ${label} (HTTP ${response.status})`);
  }

  const contentLength = response.headers.get('content-length');
  const total = contentLength ? parseInt(contentLength, 10) : 0;
  
  const reader = response.body.getReader();
  const chunks = [];
  let loaded = 0;

  while (true) {
    const { done, value } = await reader.read();
    
    if (done) break; 
    
    chunks.push(value);
    loaded += value.length;
    
    if (total && btn) {
      const percent = Math.round((loaded / total) * 100);
      btn.innerText = `Downloading ${label}... ${percent}%`;
    }
  }

  const blob = new Blob(chunks, { type: 'application/octet-stream' });
  const downloadUrl = window.URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.style.display = 'none';
  a.href = downloadUrl;
  
  const deviceName = window.location.hostname.replace('.local', '');
  a.download = `${deviceName}_${label}_backup.bin`;
  
  document.body.appendChild(a);
  a.click();
  
  window.URL.revokeObjectURL(downloadUrl);
  a.remove();

  return new Promise(resolve => setTimeout(resolve, 300));
}

async function backupAndRedirectToOTA(buttonId) {
  const btn = document.getElementById(buttonId);
  const originalText = btn ? btn.innerText : "";
  
  if (btn) btn.disabled = true; 

  try {
    await downloadPartition("nvs", btn);
    await downloadPartition("spiffs", btn);
    await downloadPartition("zb_storage", btn);
    
    if (btn) btn.innerText = "Redirecting to OTA...";

    setTimeout(() => {
      window.location.assign("/update");
    }, 500);

  } catch (error) {
    console.error("Backup failed:", error);
    alert(`Backup failed: ${error.message}. Redirecting to OTA anyway...`);
    
    window.location.assign("/update"); 
    
    if (btn) {
      btn.innerText = originalText;
      btn.disabled = false;
    }
  }
}

async function backupAndContinue(buttonId) {
  
	const btn = document.getElementById(buttonId);
  const originalText = btn ? btn.innerText : "";
  
  if (btn) btn.disabled = true; 

  await downloadPartition("nvs", btn);
  await downloadPartition("spiffs", btn);
  await downloadPartition("zb_storage", btn);
  
  
  if (btn) {
    btn.innerText = originalText;
    btn.disabled = false;
  }

	const numericId = buttonId.replace("btn", "");

  websock.send("bdown:" + numericId);
  
  setTimeout(() => {
      websock.send("bup:" + numericId);
  }, 50);
}

const originalButtonClick = window.buttonclick;

window.buttonclick = function(number, isdown) {
  
  if (number == 34) {
    if (isdown) return;
    
    backupAndContinue("btn34");
    return; 
  }

  if (number == 30) {
    if (isdown) return;
    backupAndRedirectToOTA("btn30");
    return;
  }

  if (number == 31) {
    if (isdown) return;
    redirect2ZigbeeOTA();
    return;
  }

  originalButtonClick(number, isdown);
};

)=====";

static String working_str;

bool isNumber(String &str);

const char *getSuplaChannelTypeName(int32_t channelType);
const char *getSuplaChannelFuncName(int32_t channelType, int32_t channelFunc);
const char *getZ2SDeviceDescName(uint32_t modelID);
const char *getZigbeeDataTypeName(uint8_t dataType);

uint16_t Z2S_getGUIBuildControlFlags(gui_modes_t mode);

void buildGatewayTabGUI();
void buildCredentialsGUI();
void buildZigbeeTabGUI();
void buildDevicesTabGUI();
void buildChannelsTabGUI();
void buildAdvancedDevicesTabGUI();
void buildTuyaCustomClusterTabGUI();
void buildPushoverTabGUI();

void updateChannelInfoLabel(int16_t channel_slot = -2);
void updateDeviceInfoLabel(uint8_t device_slot);

void gatewayCallback(BasicControl *sender, int type, void *param);
void selectGuiModeCallback(BasicControl *sender, int type, void *param);
void switchGUIBuildFlagCallback(BasicControl *sender, int type, void *param);
void enterWifiDetailsCallback(BasicControl *sender, int type, void *param);
void textCallback(BasicControl *sender, int type, void *param);
void generalCallback(BasicControl *sender, int type, void *param);
void onZigbeeTabCallback(BasicControl *sender, int type, void *param);
void onChannelsTabCallback(BasicControl *sender, int type, void *param);
void generalMinMaxCallback(BasicControl *sender, int type, void *param);
void endpointCallback(BasicControl *sender, int type, void *param);
void switchCallback(BasicControl *sender, int type, void *param);
void deviceSelectorCallback(BasicControl *sender, int type, void *param);
void clustersattributesdeviceSelectorCallback(BasicControl *sender, int type, void *param);
void advancedDeviceSelectorCallback(BasicControl *sender, int type, void *param);
void TuyaDeviceSelectorCallback(BasicControl *sender, int type, void *param);
void TuyaDatapointIdSelectorCallback(BasicControl *sender, int type, void *param);
void TuyaDatapointTypeSelectorCallback(BasicControl *sender, int type, void *param);
void enableDeviceControls(bool enable);
void enableClustersAttributesControls(bool enable);
void removeDeviceCallback(BasicControl *sender, int type, void *param);
void startDeviceOTACallback(BasicControl *sender, int type, void *param);
void channelSelectorCallback(BasicControl *sender, int type, void *param);
void enableChannelControls(bool enable);
void enableLAHPanel(bool enable);
void enableTuyaDevicesControls(bool enable);
void editDeviceCallback(BasicControl *sender, int type, void *param);
void editChannelCallback(BasicControl *sender, int type, void *param);
void editChannelFlagsCallback(BasicControl *sender, int type, void *param);
void batteryCallback(BasicControl *sender, int type, void *param);
void batterySwitcherCallback(BasicControl *sender, int type, void *param);
void removeChannelCallback(BasicControl *sender, int type, void *param);
void removeChannelCallbackCmd();
void getClustersAttributesQueryCallback(BasicControl *sender, int type, void *param);
void getZigbeeDeviceQueryCallback(BasicControl *sender, int type, void *param);
void pairingSwitcherCallback(BasicControl *sender, int type, void *param);
void generalZigbeeCallback(BasicControl *sender, int type, void *param);
void datatypeCallback(BasicControl *sender, int type, void *param);
void clusterCallback(BasicControl *sender, int type, void *param);
void attributeCallback(BasicControl *sender, int type, void *param);
void valueCallback(BasicControl *sender, int type, void *param);
void valveCallback(BasicControl *sender, int type, void *param);
void TuyaCustomCmdCallback(BasicControl *sender, int type, void *param);
void actionsTableCallback(BasicControl *sender, int type, void *param);
void actionSelectorCallback(BasicControl *sender, int type, void *param);
void addLocalActionHandlerCallback(
  BasicControl *sender, int type, void *param);
void addLocalChannelCallback(
  BasicControl *sender, int type, void *param);
void addSwitchbotCallback(BasicControl *sender, int type, void *param);
void saveSwitchbotCallback(BasicControl *sender, int type, void *param);
void pushoverMessagesCallback(BasicControl *sender, int type, void *param);
void hiddenNumberCallback(BasicControl *sender, int type, void *param);

void enableControlStyle(uint16_t control_id, bool enable);

void clusterCallbackCmd();
void TuyaDeviceSelectorCmd(int32_t device_slot);
void TuyaCustomCmdMain(int32_t custom_flag);
void getClustersAttributesQueryMain(int32_t custom_flag);

void editChannelMain(uint32_t update_channel_flag);

void rebuildTuyaDevicesDatapointsList(uint8_t Tuya_device_slot);

void fillGatewayGeneralnformation(char *buf);
void fillMemoryUptimeInformation(char *buf, uint16_t buf_max_len = 512);

void updateActionSummary(z2s_channel_action_t &new_action);

void irDeviceSelectorCallback(BasicControl *sender, int type, void *param);
void irCodesTableCallback(BasicControl *sender, int type, void *param);
void irDeviceSelectorCmd(int32_t device_slot);

static bool appendEscapedJsonString(
  char *buf, size_t bufSize, size_t &offset, const char *str);

/*****************************************************************************/

bool new_connect = false;

void onWsConnectCallback() {

	log_i("onWsConnectCallback");

	new_connect = true;
}

/*****************************************************************************/

void onWsCustomCmdCallback(uint16_t ws_custom_cmd) {

	log_i("onWsCustomCmdCallback %u", ws_custom_cmd);

	switch (ws_custom_cmd) {


		case 1:
		
			addGUICommand(gui_cmd_synchronize_all);
		break;
	}

	
}

/*****************************************************************************/

static const char *json_payload_fmt =
  "{\"type\":\"EXEC_JS\",\"code\":\" %s \"}";

	
void sendJSToWebsocket(char *jsCode) {

	char json_payload[256];

	snprintf(json_payload, sizeof(json_payload), json_payload_fmt, jsCode);

	ESPUI.WebSocket()->textAll(json_payload, strlen(json_payload));
}

/*****************************************************************************/

size_t parseCsvNumbers(const char *input, long *outArray, size_t maxElements) {

	if (!input || !outArray || maxElements == 0)
		return 0;

	size_t count = 0;
	char *endPtr = nullptr;
	const char *current = input;

	while (*current != '\0' && count < maxElements) {

		long val = strtol(current, &endPtr, 10);

		if (current == endPtr)
			break;

		outArray[count++] = val;
		current = endPtr;

		if (*current == ',')
			current++;
	}
	return count;
}

/*****************************************************************************/

bool bytesToHex(const uint8_t src[8], char *dst, size_t dstSize) {

	if (!src || !dst || dstSize < 17)
		return false;

	snprintf(
	  dst, dstSize, "%02X%02X%02X%02X%02X%02X%02X%02X", src[0], src[1], src[2],
	  src[3], src[4], src[5], src[6], src[7]);

	return true;
}

/*****************************************************************************/

static inline int hexCharToNibble(char c) {

	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	return -1;  // Invalid hex character
}

/*****************************************************************************/

bool hexToBytes(const char *src, uint8_t dst[8]) {

	if (!src || !dst)
		return false;

	for (size_t i = 0; i < 8; ++i) {

		int high = hexCharToNibble(src[i * 2]);
		int low = hexCharToNibble(src[i * 2 + 1]);

		if (high < 0 || low < 0)
			return false;

		dst[i] = static_cast<uint8_t>((high << 4) | low);
	}
	return true;
}

/*****************************************************************************/

uint16_t addEmptyLineLabel(uint16_t parent_id) {

	uint16_t label_id = ESPUI.addControl(
	  Control::Type::Label, empty_str, empty_str, Control::Color::None,
	  parent_id);
	ESPUI.setElementStyle(label_id, clearLabelStyle);
	return label_id;
}

void addFlagsLabel(const char *label, uint16_t parent_id) {

	ESPUI.setElementStyle(
	  ESPUI.addControl(
	    Control::Type::Label, empty_str, label, Control::Color::None,
	    parent_id),
	  clearFlagsLabelStyle);
}

uint16_t addClearLabel(const char *label, uint16_t parent_id) {

	auto label_id = ESPUI.addControl(
	  Control::Type::Label, empty_str, label, Control::Color::None,
	  parent_id);
	ESPUI.setElementStyle(label_id, clearLabelStyle);
	return label_id;
}

/*****************************************************************************/

bool isNumber(String &str) {

	unsigned int stringLength = str.length();

	if (stringLength == 0) {
		return false;
	}

	//boolean seenDecimal = false;

	for (unsigned int i = 0; i < stringLength; ++i) {
		if (isDigit(str.charAt(i))) {
			continue;
		}

		/*if (str.charAt(i) == '.') {
            if (seenDecimal) {
                return false;
            }
            seenDecimal = true;
            continue;
        }*/
		return false;
	}
	return true;
}

/*****************************************************************************/

const char *getSuplaChannelTypeName(int32_t channelType) {

	switch (channelType) {

		case SUPLA_CHANNELTYPE_BINARYSENSOR:
			return "Binary sensor";
		case SUPLA_CHANNELTYPE_RELAY:
			return "Relay";
		case SUPLA_CHANNELTYPE_THERMOMETER:
			return "Thermometer";
		case SUPLA_CHANNELTYPE_HUMIDITYSENSOR:
			return "Humidity sensor";
		case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR:
			return "Humidity and temperature sensor";
		case SUPLA_CHANNELTYPE_PRESSURESENSOR:
			return "Pressure sensor";
		case SUPLA_CHANNELTYPE_DIMMER:
			return "Dimmer";
		case SUPLA_CHANNELTYPE_RGBLEDCONTROLLER:
			return "RGB controller";
		case SUPLA_CHANNELTYPE_DIMMERANDRGBLED:
			return "Dimmer and RGB controller";
		case SUPLA_CHANNELTYPE_ELECTRICITY_METER:
			return "Electricity meter";
		case SUPLA_CHANNELTYPE_THERMOSTAT:
			return "Thermostat";
		case SUPLA_CHANNELTYPE_HVAC:
			return "HVAC";
		case SUPLA_CHANNELTYPE_VALVE_OPENCLOSE:
			return "Valve open/close";
		case SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT:
			return "General Purpose Measurement";
		case SUPLA_CHANNELTYPE_ACTIONTRIGGER:
			return "Action Trigger";
		case -1:
			return "Undefined Supla channel type";
		default:
			return "Unknown Supla channel type";
	}
}

/*****************************************************************************/

const char *getSuplaChannelFuncName(int32_t channelType, int32_t channelFunc) {

	switch (channelType) {

		case SUPLA_CHANNELTYPE_BINARYSENSOR:
			return Supla::getBinarySensorChannelName(channelFunc);
		case SUPLA_CHANNELTYPE_RELAY:
			return Supla::getRelayChannelName(channelFunc);
		default:
			return getSuplaChannelTypeName(channelType);
	}
	//currently unused
	/*switch (channelFunc) {

		case SUPLA_CHANNELFNC_THERMOMETER:
			return "Temperature measurement";
		case SUPLA_CHANNELFNC_HUMIDITY:
			return "Humidity measurement";
		case SUPLA_CHANNELFNC_HUMIDITYANDTEMPERATURE:
			return "Humidity and temperature measurement";
		case SUPLA_CHANNELFNC_DIMMER:
			return "Dimmer control";
		case SUPLA_CHANNELFNC_RGBLIGHTING:
			return "RGB control";
		case SUPLA_CHANNELFNC_ELECTRICITY_METER:
			return "Electricity measurement";
		case SUPLA_CHANNELFNC_HVAC_THERMOSTAT:
			return "HVAC/Thermostat control";
		case SUPLA_CHANNELFNC_VALVE_OPENCLOSE:
			return "Valve open/close control";
		case SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT:
			return "General puropose measurement";
		case SUPLA_CHANNELFNC_ACTIONTRIGGER:
			return "Action trigger control";
		default:
			return "Unknown Supla channel function";
	}*/
}

/*****************************************************************************/

const char *getZ2SDeviceDescName(uint32_t modelID) {

	switch (modelID) {

		case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR:
		case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1:
		case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR:
			return "Temperature and humidity sensor(0x402, 0x405)";

		case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL:
		case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL_EXT:
			return "Temperature and humidity sensor(0x20, 0x402, 0x405)";

		case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR:
			return "Temperature and humidity sensor(Tuya 0xEF00)";

		case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10:
			return "Temperature and humidity(x10) sensor(0x402, 0x405)";

		case Z2S_DEVICE_DESC_TEMPHUMIPRESSURE_SENSOR:
		case Z2S_DEVICE_DESC_LUMI_TEMPHUMIPRESSURE_SENSOR:
		case Z2S_DEVICE_DESC_LUMI_TEMPHUMIPRESSURE_SENSOR_2:
		case Z2S_DEVICE_DESC_SONOFF_TEMPHUMIPRESSURE_SENSOR:
			return "Temperature, pressure and humidity sensor(0x402, 0x403, 0x405)";

		case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR:
			return "Tuya soil temperature(/10) and humidity sensor(0xEF00)";

		case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1:
			return "Tuya soil temperature(/1) and humidity sensor(0xEF00)";

		case Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_SENSOR:
			return "Tuya illuminance sensor (0x400)";

		case Z2S_DEVICE_DESC_TUYA_ILLUZONE_SENSOR:
			return "Illuminance and IAS Zone sensor (0x400, 0x500)";

		case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR:
			return "Tuya rain and illuminance sensor (0xEF00)";

		case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR:
			return "IAS Zone sensor: ALARM1(0x500)";

		case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T:
			return "IAS Zone sensor: ALARM1, ALARM2, TAMPER(0x500)";

		case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B:
			return "IAS Zone sensor: ALARM1, TAMPER, LOW BATTERY(0x500)";

		case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B:
		case Z2S_DEVICE_DESC_TUYA_IAS_ZONE_1_B_SENSOR:
			return "IAS Zone sensor: ALARM1, LOW BATTERY(0x500)";

		case Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR:
			return "IKEA IAS Zone sensor(0x6, 0x500)";

		case Z2S_DEVICE_DESC_IKEA_VALLHORN_1:
			return "IKEA VALLHORN Wireless Motion Sensor(0x6, 0x406)";

		case Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR:
			return "LUMI magnet sensor(0x6)";

		case Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR:
			return "LUMI motion sensor(0x400, 0x406)";

		case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_SONOFF_T_B:
			return "Sonoff IAS Zone sensor: ALARM1, TAMPER, LOW BATTERY(0x500, 0xFC11)";

		case Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR:
			return "Tuya vibration sensor(0xEF00)";

		case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR:
			return "Tuya smoke sensor: BATTERY LEVEL(0xEF00)";

		case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1:
			return "Tuya vibration sensor: BATTERY STATE(0xEF00)";

		case Z2S_DEVICE_DESC_TUYA_CO_DETECTOR:
			return "Tuya CO sensor(0xEF00)";

		case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR:
			return "Tuya presence and motion state sensor (0xEF00)";

		case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5:
			return "Tuya presence sensor (0xEF00)";

		case Z2S_DEVICE_DESC_ADEO_SMART_PIRTH_SENSOR:
			return "ADEO smart PIRTH sensor";

		case Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR:
			return "ADEO contact and vibration sensor";

		case Z2S_DEVICE_DESC_SONOFF_PIR_SENSOR:
			return "Sonoff PIR sensor";

		default:
			return "Unknown Zigbee model";
	}
}

/*****************************************************************************/

const char *getZigbeeDataTypeName(uint8_t dataType) {

	uint32_t zigbee_datatypes_count = sizeof(zigbee_datatypes) / 
		sizeof(zigbee_datatype_t);

	for (uint8_t datatypes_counter = 0; 
			 datatypes_counter < zigbee_datatypes_count; datatypes_counter++) {

		if (zigbee_datatypes[datatypes_counter].zigbee_datatype_id == dataType)
			return zigbee_datatypes[datatypes_counter].zigbee_datatype_name;
	}
	return "Unknown data type";
}

/*****************************************************************************/

void fillGatewayGeneralnformation(char *buf) {

	if (buf) {

		char Supla_GUID_str[128] = {};
		char extended_pan_id_str[24] = {};
		esp_zb_ieee_addr_t extended_pan_id;

		generateHexString(
		  Supla::RegisterDevice::getGUID(), Supla_GUID_str, SUPLA_GUID_SIZE);

		esp_zb_get_extended_pan_id(extended_pan_id);
		ieee_addr_to_str(extended_pan_id_str, extended_pan_id);

		snprintf_P(
		  buf, 1024,
		  "<b><i>Flash chip real size:</b></i> %u B <b>| <i>Free Sketch Space:"
		  "</b></i> %u B <b>| <i>HeapSize:</b></i> %u B<br>"
		  "<br><b><i>Supla Device SDK version:</i></b> <i>%s</i><br>"
		  "<b><i>Supla GUID:</i></b> <i>%s</i><br><br>"
		  "<b><i>Z2S Gateway version:</i></b> <i>%s</i><br><br>"
		  "<b><i>Network extended PAN ID:</i></b> <i>%s</i><br>"
		  "<b><i>Network current channel:</i></b> <i>%u</i><br><br>",
		  ESP.getFlashChipSize(), ESP.getFreeSketchSpace(), ESP.getHeapSize(),
		  suplaDeviceVersion, Supla_GUID_str, Z2S_VERSION, extended_pan_id_str,
		  esp_zb_get_current_channel());

		log_i("Device information %s", buf);
	}
}

/*****************************************************************************/

void fillMemoryUptimeInformation(char *buf, uint16_t buf_max_len) {

	if (buf) {

		time_t local_time_info;
		time(&local_time_info);

		char current_time_buffer[25];

		strftime(
		  current_time_buffer, 25, "%H:%M [%d.%m.%Y]%t",
		  localtime(&local_time_info));

		uint32_t Supla_uptime_s = SuplaDevice.uptime.getUptime();

		uint32_t Supla_uptime_d = Supla_uptime_s / 86400;
		uint32_t Supla_uptime_h = (Supla_uptime_s % 86400) / 3600;
		uint32_t Supla_uptime_m = (Supla_uptime_s % 3600) / 60;
		uint32_t Supla_uptime_ss = Supla_uptime_s % 60;

		log_i("\n\rcurrent time %s\n\r", current_time_buffer);

		multi_heap_info_t multi_heap_info;
    multi_heap_get_info(_z2s_custom_heap, &multi_heap_info);

		uint16_t meminfbuf_size = snprintf_P(
		  buf, buf_max_len, "<b><i>FreeHeap:</b></i> %u B <b>| <i>MinimalFreeHeap"
		  ":</b></i> %u B <b>| <i>MaxAllocHeap:</b></i> %u B<b>| <i>CPU usage:"
	   	"</b></i> %04.1f %%<br><br>"
			"<b><i>Custom FreeHeap:</b></i> %u B <b>| <i>Custom MinimalFreeHeap"
		  ":</b></i> %u B <b>| <i>CustomMaxAllocHeap:</b></i> %u B<br><br>"
		  //"<b><i>uxTaskGetStackHighWaterMark (main):</b></i> %u B <b>| <i>"
		  //"uxTaskGetStackHighWaterMark (ZigBee):</b></i> %u B <br><br>"
		  //"<b><i>Total PSRAM:</b></i> %u B <b>| <i>Free PSRAM:</b></i> %u B<br>"
		  "<b><i>Local time:</i></b> %s<b><i>Supla uptime:</i></b> "
		  "%lu dni %02lu:%02lu:%02lu (%lu seconds)",
		  ESP.getFreeHeap(), ESP.getMinFreeHeap(),
		  ESP.getMaxAllocHeap(), GatewayTaskInfo.getCpuUsage(),
			multi_heap_info.total_free_bytes, multi_heap_info.minimum_free_bytes, 
			multi_heap_info.largest_free_block,
		  //uxTaskGetStackHighWaterMark(NULL),
		  //uxTaskGetStackHighWaterMark(Zigbee.getZigbeeTaskHandle()),
		  //ESP.getPsramSize(), ESP.getFreePsram(),
		  current_time_buffer,
		  Supla_uptime_d, Supla_uptime_h, Supla_uptime_m, Supla_uptime_ss,
		  Supla_uptime_s);
	}
}

/*****************************************************************************/

void enableControlStyle(uint16_t control_id, bool enable) {

	ESPUI.setEnabled(control_id, enable);
	if (enable) {

		ESPUI.setPanelStyle(control_id, ";");
	} else {

		ESPUI.setPanelStyle(control_id, disabledstyle);
	}
}

/*****************************************************************************/

void updateLabel_P(Control::ControlId_t id, const char *value) {

	working_str = value;
	ESPUI.updateLabel(id, working_str);
}

void updateLabel_C(Control::ControlId_t id, const char *value) {

	working_str = value;
	ESPUI.updateLabel(id, working_str);
}

/*****************************************************************************/

void buildGatewayTabGUI() {

	char general_purpose_gui_buffer[768] = {};

	auto gatewaytab = ESPUI.addControl(
	  Control::Type::Tab, empty_str, "Gateway", Control::Color::Dark,
	  Control::noParent, generalCallback);

	/*gui_cmd_hidden_label = ESPUI.addControl(
	  Control::Type::Label, empty_str, empty_str, Control::Color::Dark,
	  gatewaytab);*/

	gui_cmd_hidden_response_number = ESPUI.addControl(
	  Control::Type::Number, empty_str, (long int)0,
	  Control::Color::Dark, gatewaytab, hiddenNumberCallback);

	/*log_i(
	  "gui_cmd_hidden_label = %u, gui_cmd_hidden_response_number = %u",
	  gui_cmd_hidden_label, gui_cmd_hidden_response_number);*/

	//ESPUI.updateVisibility(gui_cmd_hidden_label, false);
	//ESPUI.updateVisibility(gui_cmd_hidden_response_number, false);
	ESPUI.setPanelStyle(gui_cmd_hidden_response_number, "display: none;");

	ESPUI.addControl(
	  Control::Type::Separator, "General information", empty_str,
	  Control::Color::None, gatewaytab);

	fillGatewayGeneralnformation(general_purpose_gui_buffer);

	working_str = general_purpose_gui_buffer;
	gateway_general_info = ESPUI.addControl(
	  Control::Type::Label, empty_str, working_str, Control::Color::Dark,
	  gatewaytab);

	ESPUI.setElementStyle(
	  gateway_general_info,
	  "color:white;text-align: justify; font-family:tahoma;"
	  " background-color: unset; font-size: 4 px; font-style: normal;"
	  " font-weight: normal;");

	ESPUI.setPanelWide(gateway_general_info, true);

	fillMemoryUptimeInformation(general_purpose_gui_buffer, 768);

	ESPUI.addControl(
	  Control::Type::Separator, "Status", empty_str, Control::Color::None,
	  gatewaytab);

	working_str = general_purpose_gui_buffer;
	gateway_memory_info = ESPUI.addControl(
	  Control::Type::Label, empty_str, working_str, Control::Color::Dark,
	  gatewaytab);


	ESPUI.setElementStyle(
	  gateway_memory_info,
	  "color:white;text-align: justify; font-family:tahoma;"
	  "background-color: unset; font-size: 8 px; font-style: normal; "
	  "font-weight: normal;");
	ESPUI.setPanelWide(gateway_memory_info, true);

	auto gateway_settings_label = ESPUI.addControl(
	  Control::Type::Label, "Gateway settings", empty_str, Control::Color::Dark,
		gatewaytab);

	ESPUI.setElementStyle(gateway_settings_label, clearLabelStyle);

	gateway_mdns_name_text = ESPUI.addControl(
	  Control::Type::Text, 
		"Gateway local mDNS name - max. 11 characters, no spaces!", emptyString,
	  Control::Color::Dark, gateway_settings_label, generalCallback);

	ESPUI.getControl(gateway_mdns_name_text)->reserve(12);

	ESPUI.setElementStyle(
		gateway_mdns_name_text,"width: 30%; min-width: 220px; "
		"margin-bottom: 10px;");

	auto gateway_mdns_name_save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Save", Control::Color::Dark,
	  gateway_settings_label, gatewayCallback,
	  (void *)GUI_CB_SAVE_MDNS_NAME_FLAG);
	
	//addEmptyLineLabel(gateway_settings_label);

	ESPUI.setElementStyle(
		gateway_mdns_name_save_button, 
		"width: 10%; min-width: 80px; margin: 0 7% 10px 3%;");

		auto_connection_reset_timeout_number = ESPUI.addControl(
	  Control::Type::Number, "Automatic connection reset timeout (s) -"
		" enter 0 to disable this function.",
	  (long int)0, Control::Color::Dark, gateway_settings_label, 
		generalMinMaxCallback, (void *)3600);

	ESPUI.setElementStyle(
		auto_connection_reset_timeout_number,"width: 30%; min-width: 220px; "
		"margin-bottom: 10px;");

	auto auto_connection_reset_timeout_save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Save", Control::Color::Dark,
	  gateway_settings_label, gatewayCallback, (void *)GUI_CB_ACR_TIMEOUT_FLAG);

	ESPUI.setElementStyle(
		auto_connection_reset_timeout_save_button, 
		"width: 10%; min-width: 80px; margin: 0 7% 10px 3%;");

	force_config_switcher = ESPUI.addControl(
	  Control::Type::Switcher, "Force config mode on next gateway restart.",
	  (long int)0, Control::Color::Dark, gateway_settings_label, 
		gatewayCallback, (void *)GUI_CB_FORCE_CONFIG_FLAG);

	use_new_at_model_switcher = ESPUI.addControl(
	  Control::Type::Switcher, "Use new AT model for smart buttons - "
		"new ones will add as AT with Cloud and local actions.",
		(long int)0, Control::Color::Dark, gateway_settings_label, 
		gatewayCallback, (void *)GUI_CB_USE_NEW_AT_FLAG);

	
	ESPUI.setPanelWide(gateway_settings_label, true);
	//ESPUI.setPanelStyle(gateway_settings_label, "text-align: left;")

	auto build_gateway_switcher = addClearLabel(
	  "Select GUI tabs (requires restart)", gatewaytab);
	addEmptyLineLabel(build_gateway_switcher);

	uint16_t gui_build_control_flags = _gui_build_flags ? _gui_build_flags : 
		Z2S_getGUIBuildControlFlags((gui_modes_t)_enable_gui_on_start);

	for (uint8_t gui_build_flag_bit = 1; gui_build_flag_bit < 12;
	     gui_build_flag_bit++) {

		ESPUI.addControl(
		  Control::Type::Switcher, GUI_BUILD_FLAGS_NAMES[gui_build_flag_bit], 
			(gui_build_control_flags & (1 << gui_build_flag_bit)) ? (long)1 : 
			(long)0, Control::Color::Dark, build_gateway_switcher, 
			switchGUIBuildFlagCallback, (void *)gui_build_flag_bit);
	}
	addEmptyLineLabel(build_gateway_switcher);

	gui_start_delay_number = ESPUI.addControl(
	  Control::Type::Number, "GUI start delay (s)", (long int)0,
	  Control::Color::Dark, build_gateway_switcher, generalMinMaxCallback, 
		(void *)3600);

	auto gui_start_delay_save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "SAVE",
	  Control::Color::Dark, build_gateway_switcher, gatewayCallback,
	  (void *)GUI_CB_GUI_DELAY_FLAG);

	ESPUI.setElementStyle(
		gui_start_delay_number,"width: 70%; min-width: 220px; "
		"margin-bottom: 10px;");

	ESPUI.setElementStyle(
		gui_start_delay_save_button, 
		"width: 10%; min-width: 80px; margin: 0 7% 10px 3%;");

	auto web_ota_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Gateway firmware update",
	  Control::Color::Dark, gatewaytab);

	auto upload_zigbee_ota_file_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Upload Zigbee OTA file",
	  Control::Color::Alizarin, web_ota_button);

	addEmptyLineLabel(web_ota_button);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Check GitHub gateway firmware version",
	  Control::Color::Dark, web_ota_button, gatewayCallback,
	  (void *)GUI_CB_GUI_CHECK_GITHUB_OTA_FLAG);

	github_ota_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "GitHub gateway firmware update",
	  Control::Color::Dark, web_ota_button, gatewayCallback,
	  (void *)GUI_CB_GUI_PERFORM_GITHUB_OTA_FLAG);

	github_ota_info_label = ESPUI.addControl(
	  Control::Type::Label, "", emptyString,
	  Control::Color::Dark, web_ota_button);
	ESPUI.getControl(github_ota_info_label)->reserve(60);

	addEmptyLineLabel(web_ota_button);

	save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Restart gateway", Control::Color::Dark,
	  web_ota_button, gatewayCallback, (void *)GUI_CB_GUI_RESTART_FLAG);

	ESPUI.updateNumber(gui_start_delay_number, _gui_start_delay);
	ESPUI.updateNumber(
	  auto_connection_reset_timeout_number, _auto_connection_reset_timeout);
	working_str = GatewayMDNSLocalName;
	ESPUI.updateText(gateway_mdns_name_text, working_str);
	ESPUI.updateNumber(force_config_switcher, _force_config_on_start);
	ESPUI.updateNumber(use_new_at_model_switcher, _use_new_at_model);
	enableControlStyle(github_ota_button, false);
}

/*****************************************************************************/

void buildCredentialsGUI() {

	char general_purpose_gui_buffer[512] = {};

	char *working_str_ptr = "WiFi & Supla credentials";
	auto wifitab = ESPUI.addControl(
	  Control::Type::Tab, empty_str, working_str_ptr,
	  Control::Color::Dark, Control::noParent);

	wifi_ssid_text = ESPUI.addControl(
	  Control::Type::Text, "SSID (max 32 characters)", emptyString,
	  Control::Color::Dark, wifitab, textCallback);
	ESPUI.getControl(wifi_ssid_text)->reserve(33);

	wifi_pass_text = ESPUI.addControl(
	  Control::Type::Text, "Password", emptyString,
	  Control::Color::Dark, wifitab, textCallback);
	//ESPUI.getControl(wifi_pass_text)->reserve(16);

	ESPUI.setInputType(wifi_pass_text, "password");

	Supla_server = ESPUI.addControl(
	  Control::Type::Text, "Supla server", emptyString,
	  Control::Color::Dark, wifitab, textCallback);
	ESPUI.getControl(Supla_server)->reserve(17);

	Supla_email = ESPUI.addControl(
	  Control::Type::Text, "Supla email", emptyString,
	  Control::Color::Dark, wifitab, textCallback);
	ESPUI.getControl(Supla_email)->reserve(28);

	Supla_skip_certificate_switcher = ESPUI.addControl(
	  Control::Type::Switcher, "Skip CA certificate check", (long int)0,
	  Control::Color::Dark, wifitab, generalCallback);

	working_str_ptr = "Save";
	save_button = ESPUI.addControl(
	  Control::Type::Button, "Save", working_str_ptr,
	  Control::Color::Dark, wifitab, enterWifiDetailsCallback,
	  (void *)GUI_CB_SAVE_FLAG);

	working_str_ptr = "Save & Restart";
	auto save_n_restart_button = ESPUI.addControl(
	  Control::Type::Button, "Save & Restart", working_str_ptr,
	  Control::Color::Dark, save_button, enterWifiDetailsCallback,
	  (void *)GUI_CB_RESTART_FLAG);

	save_label = ESPUI.addControl(
	  Control::Type::Label, "Status", "Missing data...",
	  Control::Color::Dark, save_button);

	auto cfg = Supla::Storage::ConfigInstance();

	if (cfg) {

		if (cfg->getWiFiSSID(general_purpose_gui_buffer))
			ESPUI.updateText(wifi_ssid_text, general_purpose_gui_buffer);

		if (cfg->getSuplaServer(general_purpose_gui_buffer))
			ESPUI.updateText(Supla_server, general_purpose_gui_buffer);


		if (cfg->getEmail(general_purpose_gui_buffer))
			ESPUI.updateText(Supla_email, general_purpose_gui_buffer);

		ESPUI.updateNumber(
		  Supla_skip_certificate_switcher, _z2s_security_level == 2 ? 1 : 0);
	}
}

void buildZigbeeTabGUI() {

	char *working_str_ptr = "ZigBee settings";

	auto zigbeetab = ESPUI.addControl(
	  Control::Type::Tab, empty_str, working_str_ptr,
	  Control::Color::Dark, Control::noParent, onZigbeeTabCallback);

	auto zigbee_settings_label = ESPUI.addControl(
	  Control::Type::Label, "ZigBee network settings", empty_str, 
		Control::Color::Dark, zigbeetab);

	ESPUI.setElementStyle(zigbee_settings_label, clearLabelStyle);

	pairing_mode_switcher = ESPUI.addControl(
	  Control::Type::Switcher, "Pairing mode (disable/enable for 180 s)",
		(long int)0, Control::Color::Dark,zigbee_settings_label, 
		pairingSwitcherCallback, (void *)GUI_CB_PAIRING_FLAG);

	zigbee_tx_power_number = ESPUI.addControl(
	  Control::Type::Number, "Zigbee TX power (-15...20)", (long int)0,
	  Control::Color::Dark, zigbee_settings_label, generalCallback);

	auto zigbee_get_tx_power_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Read",Control::Color::Dark, 
		zigbee_settings_label, generalZigbeeCallback, (void *)GUI_CB_GET_TX_FLAG);

	auto zigbee_set_tx_power_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Update",Control::Color::Dark, 
		zigbee_settings_label, generalZigbeeCallback, (void *)GUI_CB_SET_TX_FLAG);

	zigbee_primary_channel_text = ESPUI.addControl(
	  Control::Type::Number, "Zigbee primary channel (11...26)", (long int)0,
	  Control::Color::Dark, zigbee_settings_label, generalCallback);

	auto zigbee_get_primary_channel_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Read",
	  Control::Color::Dark, zigbee_settings_label,
	  generalZigbeeCallback, (void *)GUI_CB_GET_PC_FLAG);
	
	auto zigbee_set_primary_channel_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Update",
	  Control::Color::Dark, zigbee_settings_label,
	  generalZigbeeCallback, (void *)GUI_CB_SET_PC_FLAG);

	zigbee_primary_channel_label = ESPUI.addControl(
	  Control::Type::Label, empty_str, empty_str, Control::Color::Dark, 
		zigbee_settings_label);

	ESPUI.setElementStyle(zigbee_primary_channel_label, clearLabelStyle);

	working_str = three_dots_str;
	zigbee_last_binding_result_label = ESPUI.addControl(
	  Control::Type::Label, "Last binding result", working_str,
	  Control::Color::Dark, zigbeetab);

	working_str = "Enter device IEEE address";
	zigbee_installation_code_ieee_address = ESPUI.addControl(
	  Control::Type::Text, "Installation code", working_str,
	  Control::Color::Dark, zigbeetab, generalCallback);

	working_str = "Enter device installation code";
	zigbee_installation_code_str = ESPUI.addControl(
	  Control::Type::Text, empty_str, working_str,
	  Control::Color::Dark, zigbee_installation_code_ieee_address,
	  generalCallback);

	working_str_ptr = "Save installation code for single device";
	auto zigbee_set_installation_code = ESPUI.addControl(
	  Control::Type::Button, empty_str, working_str_ptr,
	  Control::Color::Dark, zigbee_installation_code_ieee_address,
	  generalZigbeeCallback, (void *)GUI_CB_SET_INSTALLATION_CODE_FLAG);

	working_str_ptr = "Clear all saved installation codes";
	auto zigbee_clear_installation_codes = ESPUI.addControl(
	  Control::Type::Button, empty_str, working_str_ptr,
	  Control::Color::Dark, zigbee_installation_code_ieee_address,
	  generalZigbeeCallback, (void *)GUI_CB_CLEAR_INSTALLATION_CODES_FLAG);

	ESPUI.addControl(
		Control::Type::Separator, "Zigbee stack factory reset",
	                 empty_str, Control::Color::None, zigbeetab);

	factory_reset_switcher = ESPUI.addControl(
	  Control::Type::Switcher, "Enable Zigbee stack factory reset",
	  (long int)0, Control::Color::Alizarin, zigbeetab, switchCallback);

	working_str_ptr = factory_reset_disabled_str;
	factory_reset_label = ESPUI.addControl(
	  Control::Type::Label, empty_str, working_str_ptr,
	  Control::Color::Dark, factory_reset_switcher);

	working_str_ptr = "FACTORY RESET!";
	auto factory_reset_button = ESPUI.addControl(
	  Control::Type::Button, "FACTORY RESET!", working_str_ptr,
	  Control::Color::Alizarin, zigbeetab, generalZigbeeCallback,
	  (void *)GUI_CB_FACTORY_FLAG);

	ESPUI.setElementStyle(factory_reset_label, clearLabelStyle);

	ESPUI.updateNumber(
	  pairing_mode_switcher, Zigbee.isNetworkOpen() ? 1 : 0);
}

/*****************************************************************************/

void buildDevicesTabGUI() {

	char *working_str_ptr = "ZigBee devices";
	devicestab = ESPUI.addControl(
	  Control::Type::Tab, empty_str, working_str_ptr,
	  Control::Color::Dark, Control::noParent);

	device_selector = ESPUI.addControl(
	  Control::Type::Select, "Devices", (long int)-1,
	  Control::Color::Dark, devicestab, deviceSelectorCallback);

	ESPUI.setPanelWide(device_selector, true);

	zb_device_info_label = ESPUI.addControl(
	  Control::Type::Label, "Device info", emptyString,
	  Control::Color::Dark, devicestab);
	ESPUI.getControl(zb_device_info_label)->reserve(600);

	ESPUI.setElementStyle(
	  zb_device_info_label,
	  "color:white; background-color: unset; text-align: justify; "
	  "font-family:tahoma; font-size: 4 px; font-style: normal; "
	  "font-weight: normal;");

	ESPUI.setPanelWide(zb_device_info_label, true);

	device_name_text = ESPUI.addControl(
	  Control::Type::Text, "Device panel", emptyString,
	  Control::Color::Dark, devicestab, generalCallback);
	ESPUI.getControl(device_name_text)->reserve(36);

	working_str_ptr = "&#10023; device name (local) &#10023;";
	ESPUI.setElementStyle(
	  ESPUI.addControl(
	    Control::Type::Label, empty_str, working_str_ptr,
	    Control::Color::None, device_name_text),
	  clearLabelStyle);

	working_str_ptr = "Save";
	device_name_save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, working_str_ptr,
	  Control::Color::Dark, device_name_text, editDeviceCallback,
	  (void *)GUI_CB_UPDATE_DEVICE_NAME_FLAG);

	working_str_ptr = "Read Software Build ID";
	getswbuild_button = ESPUI.addControl(
	  Control::Type::Button, "Software Build ID / RSSI", working_str_ptr,
	  Control::Color::Dark, devicestab, getZigbeeDeviceQueryCallback,
	  (void *)GUI_CB_SWBUILD_FLAG);

	working_str_ptr = "Read device RSSI";
	getrssi_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, working_str_ptr,
	  Control::Color::Dark, getswbuild_button, getZigbeeDeviceQueryCallback,
	  (void *)GUI_CB_RSSI_FLAG);

	working_str = three_dots_str;
	rssilabel = ESPUI.addControl(
	  Control::Type::Label, empty_str, working_str,
	  Control::Color::Dark, getswbuild_button);

	battery_panel_label = ESPUI.addControl(
	  Control::Type::Label, "Battery panel", empty_str,Control::Color::None,
		 devicestab);

	ESPUI.setElementStyle(battery_panel_label, clearLabelStyle);

	battery_voltage_min_number = ESPUI.addControl(
	  Control::Type::Number, "V(min) x10, ie. 28 = 2,8V", (long int)0,
		Control::Color::Dark, battery_panel_label, generalMinMaxCallback, 
		(void *)255);

	ESPUI.setElementStyle(
	  battery_voltage_min_number, "width: calc(25% - 10px); min-width: "
		"220px; margin: 0 5px 10px 5px;");

	battery_voltage_min_save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Save", Control::Color::Dark, 
		battery_panel_label, batteryCallback,
		(void *)GUI_CB_BATTERY_VOLTAGE_MIN_FLAG);	

	ESPUI.setElementStyle(
	  battery_voltage_min_save_button, 
		"width: 10%; min-width: 60px; margin: 0 7% 10px 3%;");

	battery_voltage_max_number = ESPUI.addControl(
	  Control::Type::Number, "V(max) x10, ie. 33 = 3,3V", (long int)0,
	  Control::Color::Dark, battery_panel_label,
	  generalMinMaxCallback, (void *)255);

	ESPUI.setElementStyle(
	  battery_voltage_max_number, "width: calc(25% - 10px); min-width: "
		"220px; margin: 0 5px 10px 5px;");
	
	battery_voltage_max_save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Save", Control::Color::Dark, 
		battery_panel_label, batteryCallback, 
		(void *)GUI_CB_BATTERY_VOLTAGE_MAX_FLAG);

	ESPUI.setElementStyle(
	  battery_voltage_max_save_button, 
		"width: 10%; min-width: 60px; margin: 0 7% 10px 3%;");
	
	disable_battery_percentage_msg_switcher = ESPUI.addControl(
	  Control::Type::Switcher, "<i>disable battery percentage messages</i>",
		(long int)0, Control::Color::Dark, battery_panel_label,
	  batterySwitcherCallback, (void *)GUI_CB_DISABLE_PERCENTAGE_MSG_FLAG);

	disable_battery_voltage_msg_switcher = ESPUI.addControl(
	  Control::Type::Switcher, "<i>disable battery voltage messages</i>",
		(long int)0, Control::Color::Dark, battery_panel_label,
	  batterySwitcherCallback, (void *)GUI_CB_DISABLE_VOLTAGE_MSG_FLAG);

	ESPUI.addControl(
	  Control::Type::Separator, empty_str, empty_str,
	  Control::Color::None, devicestab);

	working_str_ptr = "Remove device with channels!";
	remove_device_and_channels_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, working_str_ptr,
	  Control::Color::Alizarin, devicestab, removeDeviceCallback,
	  (void *)GUI_CB_WITH_CHANNELS_FLAG);

	working_str_ptr = "Remove all devices with channels!!!";
	remove_all_devices_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, working_str_ptr,
	  Control::Color::Alizarin, remove_device_and_channels_button,
	  removeDeviceCallback, (void *)GUI_CB_ALL_FLAG);

	working_str = three_dots_str;
	device_status_label = ESPUI.addControl(
	  Control::Type::Label, "Status", working_str,
	  Control::Color::Alizarin, remove_device_and_channels_button);

	start_zigbee_ota_button = ESPUI.addControl(
	  Control::Type::Button, "OTA upgrade panel", "Start OTA Upgrade",
	  Control::Color::Dark, devicestab, startDeviceOTACallback);

	working_str = three_dots_str;
	zigbee_ota_status_label = ESPUI.addControl(
	  Control::Type::Label, empty_str, working_str,
	  Control::Color::Dark, start_zigbee_ota_button);


	enableDeviceControls(false);
}

/*****************************************************************************/

void rebuildChannelsSelector(
  bool rebuild_channels_list, uint16_t channelstab = 0xFFFF) {

	if (rebuild_channels_list) {

		ESPUI.updateControlValue(channel_selector, -1);
	} else {

		channel_selector = ESPUI.addControl(
		  Control::Type::Select, "Channels", (long int)-1,
		  Control::Color::Dark, channelstab, channelSelectorCallback);

		ESPUI.setPanelWide(channel_selector, true);
	}

	if (rebuild_channels_list)
		channelSelectorCallback(
		  ESPUI.getControl(channel_selector), SL_VALUE, nullptr);
}

/*****************************************************************************/

void sbChannelCallback(BasicControl *sender, int type, void *param) {

	int16_t sb_channel_slot = sender->getValueInt();

	if (sb_channel_slot < 0) {

		working_str = empty_str;
		ESPUI.updateText(sb_device_id_text, working_str);
		ESPUI.updateText(sb_token_text, working_str);
		ESPUI.updateText(sb_json_payload_text, working_str);
		ESPUI.updateText(sb_json_payload_2_text, working_str);
	}

	channel_extended_data_sb_t channel_extended_data_sb = {};

	auto sbInstance = Z2S_Core::getSwitchBotRelayInstance(sb_channel_slot);

	if (sbInstance) {

		sbInstance->updateSwitchBotData(
		  channel_extended_data_sb, SB_UPDATE_DATA_SAVE_DIR);

		working_str = channel_extended_data_sb.ble_mac_address;
		ESPUI.updateText(sb_device_id_text, working_str);

		working_str = channel_extended_data_sb.token;
		ESPUI.updateText(sb_token_text, working_str);

		working_str = channel_extended_data_sb.json_payload;
		ESPUI.updateText(sb_json_payload_text, working_str);

		working_str = channel_extended_data_sb.json_payload_2;
		ESPUI.updateText(sb_json_payload_2_text, working_str);
	}
}

/*****************************************************************************/

void buildSwitchBotTabGUI() {

	auto sbchannelstab = ESPUI.addControl(
	  Control::Type::Tab, empty_str, "Switchbot channels",
	  Control::Color::Dark, Control::noParent);

	sb_channel_selector = ESPUI.addControl(
	  Control::Type::Select, "Switchbot channels", (long int)-1,
	  Control::Color::Dark, sbchannelstab, sbChannelCallback);

	ESPUI.setPanelWide(sb_channel_selector, true);

	working_str = empty_str;

	sb_device_id_text = ESPUI.addControl(
	  Control::Type::Text, "Switchbot data", working_str,
	  Control::Color::Dark, sbchannelstab, generalCallback);

	addClearLabel(
	  "&#10023; Device id (without ':') &#10023;", sb_device_id_text);

	sb_token_text = ESPUI.addControl(
	  Control::Type::Text, empty_str, working_str,
	  Control::Color::Dark, sb_device_id_text, generalCallback);

	addClearLabel(
	  "&#10023; Switchbot token &#10023;", sb_device_id_text);

	sb_json_payload_text = ESPUI.addControl(
	  Control::Type::Text, empty_str, working_str,
	  Control::Color::Dark, sb_device_id_text, generalCallback);

	addClearLabel(
	  "&#10023; JSON payload (1) &#10023;", sb_device_id_text);

	sb_json_payload_2_text = ESPUI.addControl(
	  Control::Type::Text, empty_str, working_str,
	  Control::Color::Dark, sb_device_id_text, generalCallback);

	addClearLabel(
	  "&#10023; JSON payload (2) &#10023;", sb_device_id_text);

	auto sb_save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Save data",
	  Control::Color::Dark, sb_device_id_text, saveSwitchbotCallback);

	auto sb_panel = ESPUI.addControl(
	  Control::Type::Button, "Switchbot objects", "Add BOT (1x)",
	  Control::Color::Dark, sbchannelstab, addSwitchbotCallback,
	  (void *)GUI_CB_ADD_SB_1X_FLAG);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add BOT (2x)", Control::Color::Dark,
	  sb_panel, addSwitchbotCallback, (void *)GUI_CB_ADD_SB_2X_FLAG);

	addEmptyLineLabel(sb_panel);
	sb_status_label = ESPUI.addControl(
	  Control::Type::Label, empty_str, three_dots_str,
	  Control::Color::Dark, sb_panel);
}

/*****************************************************************************/

void buildChannelsTabGUI() {

	auto channelstab = ESPUI.addControl(
	  Control::Type::Tab, empty_str, "Supla channels", Control::Color::Dark,
	  Control::noParent, onChannelsTabCallback);

	rebuildChannelsSelector(false, channelstab);

	zb_channel_info_label = ESPUI.addControl(
	  Control::Type::Label, "Channel info", emptyString,
	  Control::Color::Dark, channelstab);
	ESPUI.getControl(zb_channel_info_label)->reserve(800);

	ESPUI.setElementStyle(
	  zb_channel_info_label, "color:white;  background-color: unset;"
		"text-align: justify; font-family:tahoma; font-size: "
	  "4 px; font-style: normal; font-weight: normal;");
	ESPUI.setPanelWide(zb_channel_info_label, true);

	auto zb_channel_name_label = ESPUI.addControl(
	  Control::Type::Label, "Channel description panel", empty_str,
	  Control::Color::Dark, channelstab);

	ESPUI.setElementStyle(zb_channel_name_label, clearLabelStyle);

	channel_name_text = ESPUI.addControl(
	  Control::Type::Text, "Channel name (local)",
	  emptyString, Control::Color::Dark, zb_channel_name_label,
	  generalCallback);
	ESPUI.getControl(channel_name_text)->reserve(33);

	ESPUI.setElementStyle(
		channel_name_text,"width: 40%; min-width: 220px; "
		"margin-bottom: 10px;");

	//addClearLabel(	
	//  "&#10023; channel name (local) &#10023;", channel_name_text);

	channel_name_save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Save", Control::Color::Dark,
	  zb_channel_name_label, editChannelCallback,
	  (void *)GUI_CB_UPDATE_CHANNEL_NAME_FLAG);

	ESPUI.setElementStyle(
		channel_name_save_button, 
		"width: 10%; min-width: 80px; margin: 0 7% 10px 3%;");

	local_channel_function_selector = ESPUI.addControl(
	  Control::Type::Select, "Select local channel function", (long int)-1,
	  Control::Color::Dark, zb_channel_name_label,
	  editChannelCallback, (void *)GUI_CB_UPDATE_LOCAL_CHANNEL_FUNCTION_FLAG);
	
	ESPUI.setElementStyle(
		local_channel_function_selector, 
		"width: 40%; min-width: 220px; margin-bottom: 10px;");

	ESPUI.setPanelWide(zb_channel_name_label, true);

	zb_channel_flags_label = ESPUI.addControl(
	  Control::Type::Label, "Channel custom flags", 
		"here you can set channel flags (changes are saved immediately)",
	  Control::Color::Dark, channelstab);

	ESPUI.setElementStyle(zb_channel_flags_label, clearLabelStyle);

	disable_channel_notifications_switcher = ESPUI.addControl(
	  Control::Type::Switcher, "disable single channel notifications", 
		(long int)0, Control::Color::Dark, zb_channel_flags_label, 
		editChannelFlagsCallback, 
		(void *)GUI_CB_DISABLE_CHANNEL_NOTIFICATIONS_FLAG);
	
	//ESPUI.setElementStyle(
	//  disable_channel_notifications_switcher, "margin: 0% 15%;");

	set_sorwns_on_start_switcher = ESPUI.addControl(
	  Control::Type::Switcher, "sensor starts in offline state<br>(NO REMOTE"
		" WAKEUP SUPPORTED)", (long int)0, Control::Color::Dark, 
		zb_channel_flags_label,editChannelFlagsCallback, 
		(void *)GUI_CB_SET_SORWNS_ON_START_FLAG);
	
	//ESPUI.setElementStyle(set_sorwns_on_start_switcher, "margin: 0% 15%;");


	/*ESPUI.setElementStyle(
	  ESPUI.addControl(
	    Control::Type::Label, empty_str, empty_str, Control::Color::None,
	    zb_channel_flags_label),
	  clearLabelStyle);

	working_str_ptr =
	  "&#10023; <i>disable single channel notofications</i> &#10023;";
	ESPUI.setElementStyle(
	  ESPUI.addControl(
	    Control::Type::Label, empty_str, working_str_ptr,
	    Control::Color::None, zb_channel_flags_label),
	  clearFlagsLabelStyle);

	working_str_ptr =
	  "&#10023; <i>sensor starts in offline state  &#10023;<br>"
	  "(NO REMOTE WAKEUP SUPPORTED)</i>";
	ESPUI.setElementStyle(
	  ESPUI.addControl(
	    Control::Type::Label, empty_str, working_str_ptr,
	    Control::Color::None, zb_channel_flags_label),
	  clearFlagsLabelStyle);

	ESPUI.setElementStyle(
	  ESPUI.addControl(
	    Control::Type::Label, empty_str, empty_str,
	    Control::Color::None, zb_channel_flags_label),
	  clearLabelStyle);*/

	trv_auto_to_schedule_switcher = ESPUI.addControl(
	  Control::Type::Switcher, 
		"TRV AUTO mode => Supla schedule", (long int)0,Control::Color::Dark, 
		zb_channel_flags_label,editChannelFlagsCallback, 
		(void *)GUI_CB_TRV_AUTO_TO_SCHEDULE_FLAG);
	
	//ESPUI.setElementStyle(trv_auto_to_schedule_switcher, "margin: 0% 15%;");

	trv_fixed_calibration_switcher = ESPUI.addControl(
	  Control::Type::Switcher, 
		"TRV fixed correction value <br>(ignores external thermometer "
		"settings)", (long int)0,Control::Color::Dark, 
		zb_channel_flags_label, editChannelFlagsCallback, 
		(void *)GUI_CB_TRV_FIXED_CALIBRATION_FLAG);
	
	//ESPUI.setElementStyle(trv_fixed_calibration_switcher, "margin: 0% 15%;");

	/*ESPUI.setElementStyle(
	  ESPUI.addControl(
	    Control::Type::Label, empty_str, empty_str,
	    Control::Color::None, zb_channel_flags_label),
	  clearLabelStyle);

	ESPUI.setElementStyle(
	  ESPUI.addControl(
	    Control::Type::Label, empty_str, 
			"&#10023; <i>TRV AUTO mode => Supla schedule</i> &#10023;",
	    Control::Color::None, zb_channel_flags_label),
	  clearFlagsLabelStyle);

	addFlagsLabel(
	  "&#10023; <i>TRV fixed correction value &#10023;<br>"
	  "(ignores external thermometer settings)</i>",
	  zb_channel_flags_label);

	addEmptyLineLabel(zb_channel_flags_label);*/

	trv_auto_to_schedule_manual_switcher = ESPUI.addControl(
	  Control::Type::Switcher, 
		"TRV AUTO mode =><br>Supla schedule/manual", (long int)0, 
		Control::Color::Dark, zb_channel_flags_label, editChannelFlagsCallback, 
		(void *)GUI_CB_TRV_AUTO_TO_SCHEDULE_MANUAL_FLAG);

	//ESPUI.setElementStyle(
	//  trv_auto_to_schedule_manual_switcher, "margin: 0% 15%;");

	trv_cooperative_childlock_switcher = ESPUI.addControl(
	  Control::Type::Switcher, "TRV cooperative childlock", (long int)0, 
		Control::Color::Dark, zb_channel_flags_label,editChannelFlagsCallback, 
		(void *)GUI_CB_TRV_COOPERATIVE_CHILDLOCK_FLAG);
	
	//ESPUI.setElementStyle(
	//  trv_cooperative_childlock_switcher, "margin: 0% 15%;");

	//addEmptyLineLabel(zb_channel_flags_label);

	/*addFlagsLabel(
	  "&#10023; <i>TRV AUTO mode =><br>Supla schedule/manual</i> &#10023;",
	  zb_channel_flags_label);

	addFlagsLabel(
	  "&#10023; <i>TRV cooperative childlock &#10023;",
	  zb_channel_flags_label);

	addEmptyLineLabel(zb_channel_flags_label);*/

	enable_resend_temperature_switcher = ESPUI.addControl(
	  Control::Type::Switcher, "Enable temperature forwarding", 
		(long int)0, Control::Color::Dark, zb_channel_flags_label,
	  editChannelFlagsCallback, (void *)GUI_CB_ENABLE_RESEND_TEMPERATURE_FLAG);

	//ESPUI.setElementStyle(
	//  enable_resend_temperature_switcher, "margin: 0% 15%;");

	/*skip_subdevice_registation_switcher = ESPUI.addControl(
	  Control::Type::Switcher, empty_str, (long int)0,
	  Control::Color::Dark, zb_channel_flags_label,
	  editChannelFlagsCallback, (void *)GUI_CB_SKIP_SUBDEVICE_REGISTRATION_FLAG);

	ESPUI.setElementStyle(
	  skip_subdevice_registation_switcher, "margin: 0% 15%;");

	addEmptyLineLabel(zb_channel_flags_label);

	addFlagsLabel(
	  "&#10023; <i>Enable temperature forwarding<br>",
	  zb_channel_flags_label);
	addFlagsLabel(
	  "&#10023; <i>Skip subdevice registration at startup</i> &#10023;",
	  zb_channel_flags_label);*/

	ESPUI.setPanelWide(zb_channel_flags_label, true);

	zb_channel_timings_label = ESPUI.addControl(
	  Control::Type::Label, "Channel timings panel", "Input new value and press"
	  " Save", Control::Color::Dark, channelstab);
	
	ESPUI.setElementStyle(zb_channel_timings_label, clearLabelStyle);

	keepalive_number = ESPUI.addControl(
	  Control::Type::Number, keepalive_desc, (long int)0, Control::Color::Dark,
	  zb_channel_timings_label, generalCallback);

	keepalive_save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Save", Control::Color::Dark,
	  zb_channel_timings_label, editChannelCallback,
	  (void *)GUI_CB_UPDATE_KEEPALIVE_FLAG);

	ESPUI.setElementStyle(keepalive_number, "width: calc(23% - 7px); min-width: "
    "120px; margin: 0 2px 10px 5px;");  
  ESPUI.setElementStyle(keepalive_save_button, "width: calc(10% - 17px); min-width: "
    "60px; margin: 0 15px 10px 2px;");

	timeout_number = ESPUI.addControl(
	  Control::Type::Number, timeout_desc, (long int)0,
	  Control::Color::Dark, zb_channel_timings_label, generalCallback);

	timeout_save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Save", Control::Color::Dark,
	  zb_channel_timings_label, editChannelCallback,
	  (void *)GUI_CB_UPDATE_TIMEOUT_FLAG);

	ESPUI.setElementStyle(timeout_number, "width: calc(23% - 7px); min-width: "
    "120px; margin: 0 2px 10px 5px;");  
  ESPUI.setElementStyle(timeout_save_button, "width: calc(10% - 17px); min-width: "
    "60px; margin: 0 15px 10px 2px;");

	refresh_number = ESPUI.addControl(
	  Control::Type::Number, refresh_desc, (long int)0, Control::Color::Dark,
	  zb_channel_timings_label, generalCallback);

	refresh_save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Save", Control::Color::Dark,
	  zb_channel_timings_label, editChannelCallback,
	  (void *)GUI_CB_UPDATE_REFRESH_FLAG);

	ESPUI.setElementStyle(refresh_number, "width: calc(23% - 7px); min-width: "
    "120px; margin: 0 2px 10px 5px;");  
  ESPUI.setElementStyle(refresh_save_button, "width: calc(10% - 17px); min-width: "
    "60px; margin: 0 15px 10px 2px;");

	ESPUI.setPanelWide(zb_channel_timings_label, true);

	zb_channel_params_label = ESPUI.addControl(
	  Control::Type::Label, "Channel custom parameters",
	  "here you can change channel custom parameters<br>"
	  "(restart may be required!)",
	  Control::Color::Dark, channelstab);
	ESPUI.setElementStyle(zb_channel_params_label, clearLabelStyle);

  working_str = "0";
  param_1_text = ESPUI.addControl(
    Control::Type::Text, "&#10023; PARAM(1) - currently not used &#10023;", 
    working_str, Control::Color::Dark, zb_channel_params_label, 
    generalCallback);

  BasicControl* param_1_textarea = ESPUI.getControl(param_1_text);
  param_1_textarea->control_flags |= CONTROL_FLAG_TEXTAREA;
  param_1_textarea->rows = 3; 
  param_1_textarea->cols = 56;

  param_1_save_button = ESPUI.addControl(
    Control::Type::Button, empty_str, "Save", Control::Color::Dark,
    zb_channel_params_label, editChannelCallback,
    (void *)GUI_CB_UPDATE_PARAM_1_FLAG);

  ESPUI.setElementStyle(
		param_1_text, 
		"resize: none; width: calc(80% - 7px); min-width: 200px; "
		"margin: 0 2px 10px 5px;");
  ESPUI.setElementStyle(
		param_1_save_button, "width: calc(20% - 17px); min-width: 60px; "
		"margin: 0 15px 10px 2px;");

  param_2_number = ESPUI.addControl(
    Control::Type::Number, "&#10023; PARAM(2) - currently not used &#10023;", 
    (long int)0, Control::Color::Dark, zb_channel_params_label, 
		generalCallback);

  param_2_save_button = ESPUI.addControl(
    Control::Type::Button, empty_str, "Save", Control::Color::Dark,
    zb_channel_params_label, editChannelCallback,
    (void *)GUI_CB_UPDATE_PARAM_2_FLAG);

  ESPUI.setElementStyle(
		param_2_number, "width: calc(35% - 7px); min-width: 120px; "
		"margin: 0 2px 10px 5px;");
  ESPUI.setElementStyle(
		param_2_save_button, "width: calc(15% - 17px); min-width: 60px; "
		"margin: 0 15px 10px 2px;");

  param_3_number = ESPUI.addControl(
    Control::Type::Number, "&#10023; PARAM(3) - currently not used &#10023;", 
    (long int)0, Control::Color::Dark, zb_channel_params_label, 
		generalCallback);

  param_3_save_button = ESPUI.addControl(
    Control::Type::Button, empty_str, "Save", Control::Color::Dark,
    zb_channel_params_label, editChannelCallback,
    (void *)GUI_CB_UPDATE_PARAM_3_FLAG);

  ESPUI.setElementStyle(
		param_3_number, "width: calc(35% - 7px); min-width: 120px; "
		"margin: 0 2px 10px 5px;");
  ESPUI.setElementStyle(
		param_3_save_button, "width: calc(15% - 17px); min-width: 60px; "
		"margin: 0 15px 10px 2px;");

	ESPUI.setPanelWide(zb_channel_params_label, true);

	ESPUI.addControl(
	  Control::Type::Separator, empty_str, empty_str, Control::Color::None,
	  channelstab);

	lah_panel = ESPUI.addControl(
	  Control::Type::Button, "Local logic & channels", "Add AND gate",
	  Control::Color::Dark, channelstab, addLocalActionHandlerCallback,
	  (void *)GUI_CB_ADD_AND_HANDLER_FLAG);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add OR gate", Control::Color::Dark,
	  lah_panel, addLocalActionHandlerCallback,
	  (void *)GUI_CB_ADD_OR_HANDLER_FLAG);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add XOR gate", Control::Color::Dark,
	  lah_panel, addLocalActionHandlerCallback,
	  (void *)GUI_CB_ADD_XOR_HANDLER_FLAG);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add NOT gate", Control::Color::Dark,
	  lah_panel, addLocalActionHandlerCallback,
	  (void *)GUI_CB_ADD_NOT_HANDLER_FLAG);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add NAND gate", Control::Color::Dark,
	  lah_panel, addLocalActionHandlerCallback,
	  (void *)GUI_CB_ADD_NAND_HANDLER_FLAG);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add NOR gate", Control::Color::Dark,
	  lah_panel, addLocalActionHandlerCallback,
	  (void *)GUI_CB_ADD_NOR_HANDLER_FLAG);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add AND gate (3 inputs)",
	  Control::Color::Dark, lah_panel, addLocalActionHandlerCallback,
	  (void *)GUI_CB_ADD_AND3_HANDLER_FLAG);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add OR gate (3 inputs)",
	  Control::Color::Dark, lah_panel, addLocalActionHandlerCallback,
	  (void *)GUI_CB_ADD_OR3_HANDLER_FLAG);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add NOP gate", Control::Color::Dark,
	  lah_panel, addLocalActionHandlerCallback,
	  (void *)GUI_CB_ADD_NOP_HANDLER_FLAG);

	//addEmptyLineLabel(lah_panel);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add virtual relay",
	  Control::Color::Dark, lah_panel, addLocalChannelCallback,
	  (void *)gui_cmd_add_virtual_relay);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add virtual binary",
	  Control::Color::Dark, lah_panel, addLocalChannelCallback,
	  (void *)gui_cmd_add_virtual_binary);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add virtual hvac",
	  Control::Color::Dark, lah_panel, addLocalChannelCallback,
	  (void *)gui_cmd_add_virtual_hvac);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add remote relay",
	  Control::Color::Dark, lah_panel, addLocalChannelCallback,
	  (void *)gui_cmd_add_remote_relay);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add remote thermometer",
	  Control::Color::Dark, lah_panel, addLocalChannelCallback,
	  (void *)gui_cmd_add_remote_thermometer);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add virtual thermhygrometer",
	  Control::Color::Dark, lah_panel, addLocalChannelCallback,
	  (void *)gui_cmd_add_virtual_thermhygrometer);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add calculated thermometer",
	  Control::Color::Dark, lah_panel, addLocalChannelCallback,
	  (void *)gui_cmd_add_calculated_thermometer);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add virtual GPM",
	  Control::Color::Dark, lah_panel, addLocalChannelCallback,
	  (void *)gui_cmd_add_virtual_gpm);

	//addEmptyLineLabel(lah_panel);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add local relay (GPIO)",
	  Control::Color::Dark, lah_panel, addLocalChannelCallback,
	  (void *)gui_cmd_add_local_relay);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add local hvac (GPIO)",
	  Control::Color::Dark, lah_panel, addLocalChannelCallback,
	  (void *)gui_cmd_add_local_hvac);
	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add local binary (GPIO)",
	  Control::Color::Dark, lah_panel, addLocalChannelCallback,
	  (void *)gui_cmd_add_local_binary);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add local DS18B20 (GPIO)",
	  Control::Color::Dark, lah_panel, addLocalChannelCallback,
	  (void *)gui_cmd_add_local_ds18b20);

	ESPUI.addControl(
	  Control::Type::Button, empty_str, "Add local HC-SR04 (GPIO)",
	  Control::Color::Dark, lah_panel, addLocalChannelCallback,
	  (void *)gui_cmd_add_local_hcsr04);


	//addEmptyLineLabel(lah_panel);
	lah_status_label = ESPUI.addControl(
	  Control::Type::Label, empty_str, three_dots_str,
	  Control::Color::Dark, lah_panel);

	ESPUI.setPanelWide(lah_panel, true);

	remove_channel_button = ESPUI.addControl(
	  Control::Type::Button, "Remove local channels", "Remove local channel",
	  Control::Color::Alizarin, channelstab, removeChannelCallback);

	working_str = three_dots_str;
	channel_status_label = ESPUI.addControl(
	  Control::Type::Label, "Status", working_str, Control::Color::Alizarin,
	  remove_channel_button);

	enableChannelControls(false);
}

/*****************************************************************************/
/*
void buildClustersAttributesTab() {

	clusters_attributes_table[clusters_attributes_tab] = ESPUI.addControl(
	  Control::Type::Tab, empty_str, "Clusters & Attributes",
	  Control::Color::Dark, Control::noParent);

	clusters_attributes_table[clusters_attributes_device_selector] =
	  ESPUI.addControl(
	    Control::Type::Select, "ZigBee Devices", (long int)-1,
	    Control::Color::Dark,
	    clusters_attributes_table[clusters_attributes_tab],
	    clustersattributesdeviceSelectorCallback);

	working_str = three_dots_str;
	clusters_attributes_table[clusters_attributes_device_info_label] =
	  ESPUI.addControl(
	    Control::Type::Label, "Device information", working_str,
	    Control::Color::Dark,
	    clusters_attributes_table[clusters_attributes_tab]);

	ESPUI.setPanelWide(
	  clusters_attributes_table[clusters_attributes_device_selector], false);

	clusters_attributes_table[device_endpoint_number] = ESPUI.addControl(
	  Control::Type::Number, "Clusters & Attributes", (long int)1,
	  Control::Color::Dark,
	  clusters_attributes_table[clusters_attributes_tab], endpointCallback);

	addClearLabel(
	  "Endpoint number", clusters_attributes_table[device_endpoint_number]);

	clusters_attributes_table[device_cluster_selector] = ESPUI.addControl(
	  Control::Type::Select, empty_str, (long int)-1,
	  Control::Color::Dark, clusters_attributes_table[device_endpoint_number],
	  clusterCallback);

	addClearLabel(
	  "ZigBee cluster", clusters_attributes_table[device_endpoint_number]);

	clusters_attributes_table[device_attribute_id_selector] = ESPUI.addControl(
	  Control::Type::Select, empty_str, (long int)-1,
	  Control::Color::Dark, clusters_attributes_table[device_endpoint_number],
	  attributeCallback);

	working_str = "0";
	clusters_attributes_table[device_attribute_id_text] = ESPUI.addControl(
	  Control::Type::Text, empty_str, working_str, Control::Color::Dark,
	  clusters_attributes_table[device_endpoint_number], generalCallback);

	addClearLabel(
	  "Select or enter cluster attribute id/custom command id"
	  "<br>use 0x for hexadecimal values)",
	  clusters_attributes_table[device_endpoint_number]);

	clusters_attributes_table[device_attribute_type_selector] = ESPUI.addControl(
	  Control::Type::Select, empty_str, (long int)-1,
	  Control::Color::Dark,
	  clusters_attributes_table[device_endpoint_number], datatypeCallback);

	clusters_attributes_table[device_attribute_size_number] = ESPUI.addControl(
	  Control::Type::Number, empty_str, (long int)0,
	  Control::Color::Dark,
	  clusters_attributes_table[device_endpoint_number], generalMinMaxCallback,
	  (void *)MAX_ZIGBEE_PAYLOAD_SIZE);

	addClearLabel(
	  "Select or enter attribute/custom command payload size"
	  "<br>(use 0x for hexadecimal values)",
	  clusters_attributes_table[device_endpoint_number]);

	clusters_attributes_table[device_attribute_value_selector] =
	  ESPUI.addControl(Control::Type::Select, empty_str, (long int)-1,
	                   Control::Color::Dark, clusters_attributes_table[device_endpoint_number],
	                   valueCallback);

	working_str = "0";
	clusters_attributes_table[device_attribute_value_text] = ESPUI.addControl(
	  Control::Type::Text, "Value", working_str, Control::Color::Dark,
	  clusters_attributes_table[device_endpoint_number], generalCallback);

	addClearLabel(
	  "Select or enter attribute value<br>or custom command payload<br>"
	  "use 0x for hexadecimal values or hexstring without 0x",
	  clusters_attributes_table[device_endpoint_number]);

	clusters_attributes_table[device_config_min_number] = ESPUI.addControl(
	  Control::Type::Number, empty_str, (long int)0,
	  Control::Color::Dark, clusters_attributes_table[device_endpoint_number],
	  generalMinMaxCallback, (void *)65535);

	addClearLabel(
	  "Min interval", clusters_attributes_table[device_endpoint_number]);

	clusters_attributes_table[device_config_max_number] = ESPUI.addControl(
	  Control::Type::Number, empty_str, (long int)0,
	  Control::Color::Dark, clusters_attributes_table[device_endpoint_number],
	  generalMinMaxCallback, (void *)65535);

	addClearLabel(
	  "Max interval", clusters_attributes_table[device_endpoint_number]);

	clusters_attributes_table[device_config_delta_number] = ESPUI.addControl(
	  Control::Type::Number, empty_str, (long int)0,
	  Control::Color::Dark,
	  clusters_attributes_table[device_endpoint_number], generalMinMaxCallback,
	  (void *)65535);

	addClearLabel(
	  "Delta", clusters_attributes_table[device_endpoint_number]);

	ESPUI.addControl(
	  Control::Type::Option, "Select attribute type...", (long int)-1,
	  Control::Color::None,
	  clusters_attributes_table[device_attribute_type_selector]);

	ESPUI.addControl(
	  Control::Type::Option, "Select attribute value...", (long int)-1,
	  Control::Color::None,
	  clusters_attributes_table[device_attribute_value_selector]);


	uint32_t zigbee_datatypes_count =
	  sizeof(zigbee_datatypes) / sizeof(zigbee_datatype_t);

	for (uint8_t datatypes_counter = 0;
	     datatypes_counter < zigbee_datatypes_count; datatypes_counter++) {

		working_str = datatypes_counter;
		ESPUI.addControl(
		  Control::Type::Option,
		  zigbee_datatypes[datatypes_counter].zigbee_datatype_name,
		  datatypes_counter, Control::Color::None,
		  clusters_attributes_table[device_attribute_type_selector]);
	}

	clusters_attributes_table[device_read_attribute_button] = ESPUI.addControl(
	  Control::Type::Button, "Attribute commands", "Read attribute",
	  Control::Color::Dark,
	  clusters_attributes_table[clusters_attributes_tab],
	  getClustersAttributesQueryCallback, (void *)GUI_CB_READ_ATTR_FLAG);

	clusters_attributes_table[device_read_config_button] = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Read reporting settings",
	  Control::Color::Dark,
	  clusters_attributes_table[device_read_attribute_button],
	  getClustersAttributesQueryCallback, (void *)GUI_CB_READ_CONFIG_FLAG);

	clusters_attributes_table[device_write_attribute_button] = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Write attribute",
	  Control::Color::Dark,
	  clusters_attributes_table[device_read_attribute_button],
	  getClustersAttributesQueryCallback, (void *)GUI_CB_WRITE_ATTR_FLAG);

	clusters_attributes_table[device_write_config_button] = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Configure reporting",
	  Control::Color::Dark,
	  clusters_attributes_table[device_read_attribute_button],
	  getClustersAttributesQueryCallback, (void *)GUI_CB_CONFIG_REPORT_FLAG);

	clusters_attributes_table[device_custom_command_button] = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Send custom command",
	  Control::Color::Dark,
	  clusters_attributes_table[device_read_attribute_button],
	  getClustersAttributesQueryCallback, (void *)GUI_CB_CUSTOM_CMD_FLAG);

	clusters_attributes_table[manufacturer_code_switcher] = ESPUI.addControl(
	  Control::Type::Switcher, "Manufacturer code", (long int)0,
	  Control::Color::Dark,
	  clusters_attributes_table[clusters_attributes_tab], generalCallback);

	addClearLabel(
	  "&#10023; <i>Use manufacturer code</i> &#10023;",
	  clusters_attributes_table[manufacturer_code_switcher]);

	clusters_attributes_table[manufacturer_code_selector] = ESPUI.addControl(
	  Control::Type::Select, empty_str, (long int)-1,
	  Control::Color::Dark,
	  clusters_attributes_table[manufacturer_code_switcher], generalCallback);

	addClearLabel(
	  "Manufacturer code",
	  clusters_attributes_table[manufacturer_code_switcher]);

	ESPUI.addControl(
	  Control::Type::Option, "Select manufacturer code...", (long int)-1,
	  Control::Color::None,
	  clusters_attributes_table[manufacturer_code_selector]);

	uint32_t zigbee_manufacturer_codes_count =
	  sizeof(zigbee_manufacturer_codes) / sizeof(zigbee_manufacturer_code_t);

	for (uint8_t manufacturer_codes_counter = 0;
	     manufacturer_codes_counter < zigbee_manufacturer_codes_count;
	     manufacturer_codes_counter++) {

		ESPUI.addControl(
		  Control::Type::Option,
		  zigbee_manufacturer_codes[manufacturer_codes_counter].manufacturer_name,
		  zigbee_manufacturer_codes[manufacturer_codes_counter].manufacturer_code,
		  Control::Color::None,
		  clusters_attributes_table[manufacturer_code_selector]);
	}

	clusters_attributes_table[device_async_switcher] = ESPUI.addControl(
	  Control::Type::Switcher, "Asynchronous command", (long int)0,
	  Control::Color::Dark,
	  clusters_attributes_table[clusters_attributes_tab], generalCallback);

	working_str = three_dots_str;
	clusters_attributes_table[device_read_attribute_label] = ESPUI.addControl(
	  Control::Type::Label, "ZigBee command result", working_str,
	  Control::Color::Dark,
	  clusters_attributes_table[clusters_attributes_tab]);

	clusters_attributes_table[device_Tuya_payload_label] = ESPUI.addControl(
	  Control::Type::Label, "Tuya custom cluster payload", working_str,
	  Control::Color::Dark,
	  clusters_attributes_table[clusters_attributes_tab]);

	enableClustersAttributesControls(false);
}*/
/*
void buildClustersAttributesTab() {

  clusters_attributes_table[clusters_attributes_tab] = ESPUI.addControl(
    Control::Type::Tab, empty_str, "Clusters & Attributes",
    Control::Color::Dark, Control::noParent);

  // 1. Create a single master panel to hold everything
  uint16_t main_panel = ESPUI.addControl(
    Control::Type::Label, "ZigBee Operations Panel", empty_str,
    Control::Color::Dark, clusters_attributes_table[clusters_attributes_tab]);
  
  ESPUI.setPanelWide(main_panel, true);
  ESPUI.setElementStyle(main_panel, clearLabelStyle);

  // --- ROW 1: Device Selection (40%) & Info (60%) ---
  clusters_attributes_table[clusters_attributes_device_selector] =
    ESPUI.addControl(Control::Type::Select, "ZigBee Devices", (long int)-1,
    Control::Color::Dark, main_panel, clustersattributesdeviceSelectorCallback);

  working_str = three_dots_str;
  clusters_attributes_table[clusters_attributes_device_info_label] =
    ESPUI.addControl(Control::Type::Label, "Device information", working_str,
    Control::Color::Dark, main_panel);

  ESPUI.setElementStyle(
    clusters_attributes_table[clusters_attributes_device_selector],
    "width: calc(40% - 10px); min-width: 200px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[clusters_attributes_device_info_label],
    "width: calc(60% - 10px); min-width: 200px; margin: 0 5px 10px 5px;");

  // --- ROW 2: Endpoint (15%), Cluster (35%), Attr Sel (25%), Attr Txt (25%)
  clusters_attributes_table[device_endpoint_number] = ESPUI.addControl(
    Control::Type::Number, "Endpoint number", (long int)1,
    Control::Color::Dark, main_panel, endpointCallback);

  clusters_attributes_table[device_cluster_selector] = ESPUI.addControl(
    Control::Type::Select, "ZigBee cluster", (long int)-1,
    Control::Color::Dark, main_panel, clusterCallback);

  clusters_attributes_table[device_attribute_id_selector] = ESPUI.addControl(
    Control::Type::Select, empty_str, (long int)-1,
    Control::Color::Dark, main_panel, attributeCallback);

  working_str = "0";
  clusters_attributes_table[device_attribute_id_text] = ESPUI.addControl(
    Control::Type::Text, 
    "Select or enter cluster attribute id/custom command id"
    "<br>use 0x for hexadecimal values)", 
    working_str, Control::Color::Dark, main_panel, generalCallback);

  ESPUI.setElementStyle(
    clusters_attributes_table[device_endpoint_number],
    "width: calc(15% - 10px); min-width: 100px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_cluster_selector],
    "width: calc(35% - 10px); min-width: 160px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_attribute_id_selector],
    "width: calc(25% - 10px); min-width: 140px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_attribute_id_text],
    "width: calc(25% - 10px); min-width: 160px; margin: 0 5px 10px 5px;");

  // --- ROW 3: Type (25%), Size (15%), Val Sel (30%), Val Txt (30%) ---
  clusters_attributes_table[device_attribute_type_selector] = ESPUI.addControl(
    Control::Type::Select, empty_str, (long int)-1,
    Control::Color::Dark, main_panel, datatypeCallback);

  clusters_attributes_table[device_attribute_size_number] = ESPUI.addControl(
    Control::Type::Number, 
    "Select or enter attribute/custom command payload size"
    "<br>(use 0x for hexadecimal values)", 
    (long int)0, Control::Color::Dark, main_panel, generalMinMaxCallback,
    (void *)MAX_ZIGBEE_PAYLOAD_SIZE);

  clusters_attributes_table[device_attribute_value_selector] = ESPUI.addControl(
    Control::Type::Select, empty_str, (long int)-1,
    Control::Color::Dark, main_panel, valueCallback);

  working_str = "0";
  clusters_attributes_table[device_attribute_value_text] = ESPUI.addControl(
    Control::Type::Text, 
    "Select or enter attribute value<br>or custom command payload<br>"
    "use 0x for hexadecimal values or hexstring without 0x", 
    working_str, Control::Color::Dark, main_panel, generalCallback);

  ESPUI.setElementStyle(
    clusters_attributes_table[device_attribute_type_selector],
    "width: calc(25% - 10px); min-width: 140px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_attribute_size_number],
    "width: calc(15% - 10px); min-width: 120px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_attribute_value_selector],
    "width: calc(30% - 10px); min-width: 150px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_attribute_value_text],
    "width: calc(30% - 10px); min-width: 180px; margin: 0 5px 10px 5px;");

  // --- ROW 4: Reporting (3 items, 33.3% each) ---
  clusters_attributes_table[device_config_min_number] = ESPUI.addControl(
    Control::Type::Number, "Min interval", (long int)0,
    Control::Color::Dark, main_panel, generalMinMaxCallback, (void *)65535);

  clusters_attributes_table[device_config_max_number] = ESPUI.addControl(
    Control::Type::Number, "Max interval", (long int)0,
    Control::Color::Dark, main_panel, generalMinMaxCallback, (void *)65535);

  clusters_attributes_table[device_config_delta_number] = ESPUI.addControl(
    Control::Type::Number, "Delta", (long int)0,
    Control::Color::Dark, main_panel, generalMinMaxCallback, (void *)65535);

  ESPUI.setElementStyle(
    clusters_attributes_table[device_config_min_number],
    "width: calc(33.3% - 10px); min-width: 100px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_config_max_number],
    "width: calc(33.3% - 10px); min-width: 100px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_config_delta_number],
    "width: calc(33.3% - 10px); min-width: 100px; margin: 0 5px 10px 5px;");

  // --- ROW 5: Settings (Manuf Switch 20%, Manuf Select 60%, Async 20%) ---
  clusters_attributes_table[manufacturer_code_switcher] = ESPUI.addControl(
    Control::Type::Switcher, "&#10023; <i>Use manufacturer code</i> &#10023;",
    (long int)0, Control::Color::Dark, main_panel, generalCallback);

  clusters_attributes_table[manufacturer_code_selector] = ESPUI.addControl(
    Control::Type::Select, "Manufacturer code", (long int)-1,
    Control::Color::Dark, main_panel, generalCallback);

  clusters_attributes_table[device_async_switcher] = ESPUI.addControl(
    Control::Type::Switcher, "Asynchronous command", (long int)0,
    Control::Color::Dark, main_panel, generalCallback);

  ESPUI.setElementStyle(
    clusters_attributes_table[manufacturer_code_switcher],
    "width: calc(20% - 10px); min-width: 120px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[manufacturer_code_selector],
    "width: calc(60% - 10px); min-width: 200px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_async_switcher],
    "width: calc(20% - 10px); min-width: 120px; margin: 0 5px 10px 5px;");

  // --- ROW 6: Buttons (5 buttons, 20% each) ---
  clusters_attributes_table[device_read_attribute_button] = ESPUI.addControl(
    Control::Type::Button, "Attribute commands", "Read attribute", 
    Control::Color::Dark, main_panel, getClustersAttributesQueryCallback,
    (void *)GUI_CB_READ_ATTR_FLAG);

  clusters_attributes_table[device_read_config_button] = ESPUI.addControl(
    Control::Type::Button, empty_str, "Read reporting settings", 
    Control::Color::Dark, main_panel, getClustersAttributesQueryCallback,
    (void *)GUI_CB_READ_CONFIG_FLAG);

  clusters_attributes_table[device_write_attribute_button] = ESPUI.addControl(
    Control::Type::Button, empty_str, "Write attribute", 
    Control::Color::Dark, main_panel, getClustersAttributesQueryCallback,
    (void *)GUI_CB_WRITE_ATTR_FLAG);

  clusters_attributes_table[device_write_config_button] = ESPUI.addControl(
    Control::Type::Button, empty_str, "Configure reporting", 
    Control::Color::Dark, main_panel, getClustersAttributesQueryCallback,
    (void *)GUI_CB_CONFIG_REPORT_FLAG);

  clusters_attributes_table[device_custom_command_button] = ESPUI.addControl(
    Control::Type::Button, empty_str, "Send custom command", 
    Control::Color::Dark, main_panel, getClustersAttributesQueryCallback,
    (void *)GUI_CB_CUSTOM_CMD_FLAG);

  // Increased min-widths so the long button text doesn't overflow
  ESPUI.setElementStyle(
    clusters_attributes_table[device_read_attribute_button],
    "width: calc(20% - 10px); min-width: 130px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_read_config_button],
    "width: calc(20% - 10px); min-width: 180px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_write_attribute_button],
    "width: calc(20% - 10px); min-width: 130px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_write_config_button],
    "width: calc(20% - 10px); min-width: 150px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_custom_command_button],
    "width: calc(20% - 10px); min-width: 160px; margin: 0 5px 10px 5px;");

  // --- ROW 7: Result Labels (50% each) ---
  working_str = three_dots_str;
  clusters_attributes_table[device_read_attribute_label] = ESPUI.addControl(
    Control::Type::Label, "ZigBee command result", working_str,
    Control::Color::Dark, main_panel);

  clusters_attributes_table[device_Tuya_payload_label] = ESPUI.addControl(
    Control::Type::Label, "Tuya custom cluster payload", working_str,
    Control::Color::Dark, main_panel);

  ESPUI.setElementStyle(
    clusters_attributes_table[device_read_attribute_label],
    "width: calc(50% - 10px); min-width: 200px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_Tuya_payload_label],
    "width: calc(50% - 10px); min-width: 200px; margin: 0 5px 10px 5px;");

  // --- ADD OPTIONS ---
  ESPUI.addControl(Control::Type::Option, "Select attribute type...",
    (long int)-1, Control::Color::None,
    clusters_attributes_table[device_attribute_type_selector]);

  ESPUI.addControl(Control::Type::Option, "Select attribute value...",
    (long int)-1, Control::Color::None,
    clusters_attributes_table[device_attribute_value_selector]);

  uint32_t zigbee_datatypes_count =
    sizeof(zigbee_datatypes) / sizeof(zigbee_datatype_t);
  
  for (uint8_t i = 0; i < zigbee_datatypes_count; i++) {
    ESPUI.addControl(Control::Type::Option,
      zigbee_datatypes[i].zigbee_datatype_name, i, Control::Color::None,
      clusters_attributes_table[device_attribute_type_selector]);
  }

  ESPUI.addControl(Control::Type::Option, "Select manufacturer code...",
    (long int)-1, Control::Color::None,
    clusters_attributes_table[manufacturer_code_selector]);

  uint32_t zigbee_manufacturer_codes_count =
    sizeof(zigbee_manufacturer_codes) / sizeof(zigbee_manufacturer_code_t);
  
  for (uint8_t i = 0; i < zigbee_manufacturer_codes_count; i++) {
    ESPUI.addControl(Control::Type::Option,
      zigbee_manufacturer_codes[i].manufacturer_name,
      zigbee_manufacturer_codes[i].manufacturer_code, Control::Color::None,
      clusters_attributes_table[manufacturer_code_selector]);
  }

  enableClustersAttributesControls(false);
}
*/

void buildClustersAttributesTab() {

  clusters_attributes_table[clusters_attributes_tab] = ESPUI.addControl(
    Control::Type::Tab, empty_str, "Clusters & Attributes",
    Control::Color::Dark, Control::noParent);

	clusters_attributes_table[clusters_attributes_device_selector] =
	  ESPUI.addControl(
	    Control::Type::Select, "ZigBee devices", (long int)-1,
	    Control::Color::Dark,
	    clusters_attributes_table[clusters_attributes_tab],
	    clustersattributesdeviceSelectorCallback);

	working_str = three_dots_str;
	clusters_attributes_table[clusters_attributes_device_info_label] =
	  ESPUI.addControl(
	    Control::Type::Label, "Device information", working_str,
	    Control::Color::Dark,
	    clusters_attributes_table[clusters_attributes_tab]);

	ESPUI.setPanelWide(
	  clusters_attributes_table[clusters_attributes_device_selector], false);

  uint16_t main_panel = ESPUI.addControl(
    Control::Type::Label, "ZigBee clusters command panel", empty_str,
    Control::Color::Dark, clusters_attributes_table[clusters_attributes_tab]);
  
  ESPUI.setPanelWide(main_panel, true);
  ESPUI.setElementStyle(main_panel, clearLabelStyle);

  clusters_attributes_table[device_endpoint_number] = ESPUI.addControl(
    Control::Type::Number, "endpoint", (long int)1, 
    Control::Color::Dark, main_panel, endpointCallback);

  clusters_attributes_table[device_cluster_selector] = ESPUI.addControl(
    Control::Type::Select, "cluster", (long int)-1,
    Control::Color::Dark, main_panel, clusterCallback);

  clusters_attributes_table[device_attribute_id_selector] = ESPUI.addControl(
    Control::Type::Select, "attribute/command", (long int)-1,
    Control::Color::Dark, main_panel, attributeCallback);

  working_str = "0";
  clusters_attributes_table[device_attribute_id_text] = ESPUI.addControl(
    Control::Type::Text, "attribute/command id", working_str, 
    Control::Color::Dark, main_panel, generalCallback);

  clusters_attributes_table[device_attribute_type_selector] = ESPUI.addControl(
    Control::Type::Select, "attribute type", (long int)-1,
    Control::Color::Dark, main_panel, datatypeCallback);

  clusters_attributes_table[device_attribute_size_number] = ESPUI.addControl(
    Control::Type::Number, "payload size", (long int)0, 
    Control::Color::Dark, main_panel, generalMinMaxCallback,
    (void *)MAX_ZIGBEE_PAYLOAD_SIZE);

  ESPUI.setElementStyle(
    clusters_attributes_table[device_endpoint_number],
    "width: calc(8% - 10px); min-width: 70px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_cluster_selector],
    "width: calc(22% - 10px); min-width: 140px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_attribute_id_selector],
    "width: calc(25% - 10px); min-width: 160px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_attribute_id_text],
    "width: calc(18% - 10px); min-width: 120px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_attribute_type_selector],
    "width: calc(17% - 10px); min-width: 120px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_attribute_size_number],
    "width: calc(10% - 10px); min-width: 80px; margin: 0 5px 10px 5px;");

  clusters_attributes_table[device_attribute_value_selector] = ESPUI.addControl(
    Control::Type::Select, "attribute values", (long int)-1,
    Control::Color::Dark, main_panel, valueCallback);

  working_str = "0";
  clusters_attributes_table[device_attribute_value_text] = ESPUI.addControl(
    Control::Type::Text, "attribute/payload value", working_str, 
    Control::Color::Dark, main_panel, generalCallback);

  clusters_attributes_table[device_config_min_number] = ESPUI.addControl(
    Control::Type::Number, "minimal interval", (long int)0,
    Control::Color::Dark, main_panel, generalMinMaxCallback, (void *)65535);

  clusters_attributes_table[device_config_max_number] = ESPUI.addControl(
    Control::Type::Number, "maximal interval", (long int)0,
    Control::Color::Dark, main_panel, generalMinMaxCallback, (void *)65535);

  clusters_attributes_table[device_config_delta_number] = ESPUI.addControl(
    Control::Type::Number, "delta", (long int)0,
    Control::Color::Dark, main_panel, generalMinMaxCallback, (void *)65535);

  ESPUI.setElementStyle(
    clusters_attributes_table[device_attribute_value_selector],
    "width: calc(25% - 10px); min-width: 160px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_attribute_value_text],
    "width: calc(25% - 10px); min-width: 160px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_config_min_number],
    "width: calc(18% - 10px); min-width: 100px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_config_max_number],
    "width: calc(18% - 10px); min-width: 100px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_config_delta_number],
    "width: calc(14% - 10px); min-width: 80px; margin: 0 5px 10px 5px;");

  clusters_attributes_table[manufacturer_code_switcher] = ESPUI.addControl(
    Control::Type::Switcher, "use manufacturer code",
    (long int)0, Control::Color::Dark, main_panel, generalCallback);

  clusters_attributes_table[manufacturer_code_selector] = ESPUI.addControl(
    Control::Type::Select, "manufacturer code", (long int)-1,
    Control::Color::Dark, main_panel, generalCallback);

  clusters_attributes_table[device_async_switcher] = ESPUI.addControl(
    Control::Type::Switcher, "asynchronous command", (long int)0,
    Control::Color::Dark, main_panel, generalCallback);

  ESPUI.setElementStyle(
    clusters_attributes_table[manufacturer_code_switcher],
    "width: calc(20% - 10px); min-width: 120px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[manufacturer_code_selector],
    "width: calc(60% - 10px); min-width: 200px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_async_switcher],
    "width: calc(20% - 10px); min-width: 120px; margin: 0 5px 10px 5px;");


  clusters_attributes_table[device_read_attribute_button] = ESPUI.addControl(
    Control::Type::Button, "attribute commands", "Read attribute", 
    Control::Color::Dark, main_panel, getClustersAttributesQueryCallback,
    (void *)GUI_CB_READ_ATTR_FLAG);

  clusters_attributes_table[device_read_config_button] = ESPUI.addControl(
    Control::Type::Button, empty_str, "Read config", 
    Control::Color::Dark, main_panel, getClustersAttributesQueryCallback,
    (void *)GUI_CB_READ_CONFIG_FLAG);

  clusters_attributes_table[device_write_attribute_button] = ESPUI.addControl(
    Control::Type::Button, empty_str, "Write attribute", 
    Control::Color::Dark, main_panel, getClustersAttributesQueryCallback,
    (void *)GUI_CB_WRITE_ATTR_FLAG);

  clusters_attributes_table[device_write_config_button] = ESPUI.addControl(
    Control::Type::Button, empty_str, "Configure reporting", 
    Control::Color::Dark, main_panel, getClustersAttributesQueryCallback,
    (void *)GUI_CB_CONFIG_REPORT_FLAG);

  clusters_attributes_table[device_custom_command_button] = ESPUI.addControl(
    Control::Type::Button, empty_str, "Custom cmd", 
    Control::Color::Dark, main_panel, getClustersAttributesQueryCallback,
    (void *)GUI_CB_CUSTOM_CMD_FLAG);

  ESPUI.setElementStyle(
    clusters_attributes_table[device_read_attribute_button],
    "width: calc(20% - 10px); min-width: 110px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_read_config_button],
    "width: calc(20% - 10px); min-width: 110px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_write_attribute_button],
    "width: calc(20% - 10px); min-width: 110px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_write_config_button],
    "width: calc(20% - 10px); min-width: 110px; margin: 0 5px 10px 5px;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_custom_command_button],
    "width: calc(20% - 10px); min-width: 110px; margin: 0 5px 10px 5px;");

  working_str = three_dots_str;
  
  clusters_attributes_table[device_read_attribute_label] = ESPUI.addControl(
    Control::Type::Label, "ZigBee command result", working_str,
    Control::Color::Dark, main_panel);

  clusters_attributes_table[device_Tuya_payload_label] = ESPUI.addControl(
    Control::Type::Label, "Tuya custom cluster payload", working_str,
    Control::Color::Dark, main_panel);

  ESPUI.setElementStyle(
    clusters_attributes_table[device_read_attribute_label],
    "width: calc(50% - 10px); min-width: 200px; margin: 0 5px 10px 5px; "
    "pointer-events: none; opacity: 0.7;");
  ESPUI.setElementStyle(
    clusters_attributes_table[device_Tuya_payload_label],
    "width: calc(50% - 10px); min-width: 200px; margin: 0 5px 10px 5px; "
    "pointer-events: none; opacity: 0.7;");
  
  const char* legend_html = 
    "<div style='text-align: left; font-size: 13px; line-height: 1.5;'>"
    "<b>Legend:</b><br>"
    "&#8226; <b>attribute/command ID</b> & <b>payload size</b>: "
    "Use <i>0x</i> prefix for hexadecimal values.<br>"
    "&#8226; <b>attribute/payload value</b>: "
    "Use <i>0x</i> for hex values or a hexstring without <i>0x</i>."
    "</div>";

  uint16_t legend_label = ESPUI.addControl(
    Control::Type::Label, empty_str, legend_html, 
    Control::Color::None, main_panel);

  ESPUI.setElementStyle(legend_label, 
    "width: calc(100% - 10px); margin: 5px 5px 10px 5px; "
    "background-color: transparent; border: none; box-shadow: none;");

  ESPUI.addControl(Control::Type::Option, "Select attribute type...",
    (long int)-1, Control::Color::None,
    clusters_attributes_table[device_attribute_type_selector]);

  uint32_t zigbee_datatypes_count =
    sizeof(zigbee_datatypes) / sizeof(zigbee_datatype_t);
  
  for (uint8_t i = 0; i < zigbee_datatypes_count; i++) {
    ESPUI.addControl(Control::Type::Option,
      zigbee_datatypes[i].zigbee_datatype_name, i, Control::Color::None,
      clusters_attributes_table[device_attribute_type_selector]);
  }

  ESPUI.addControl(Control::Type::Option, "Select manufacturer code...",
    (long int)-1, Control::Color::None,
    clusters_attributes_table[manufacturer_code_selector]);

  uint32_t zigbee_manufacturer_codes_count =
    sizeof(zigbee_manufacturer_codes) / sizeof(zigbee_manufacturer_code_t);
  
  for (uint8_t i = 0; i < zigbee_manufacturer_codes_count; i++) {
    ESPUI.addControl(Control::Type::Option,
      zigbee_manufacturer_codes[i].manufacturer_name,
      zigbee_manufacturer_codes[i].manufacturer_code, Control::Color::None,
      clusters_attributes_table[manufacturer_code_selector]);
  }

  enableClustersAttributesControls(false);
}
/*****************************************************************************/

void buildSonoffValveGUI(uint16_t advanced_devices_tab) {

	valve_program_selector = ESPUI.addControl(
	  Control::Type::Select, "SONOFF SWV VALVE", (long int)-1,
	  Control::Color::Dark, advanced_devices_tab, generalCallback);

	ESPUI.addControl(
	  Control::Type::Option, "Select program...", (long int)-1,
	  Control::Color::None, valve_program_selector);

	ESPUI.addControl(
	  Control::Type::Option, "Time program...", 1, Control::Color::None,
	  valve_program_selector);

	ESPUI.addControl(
	  Control::Type::Option, "Volume program...", 2, Control::Color::None,
	  valve_program_selector);

	valve_cycles_number = ESPUI.addControl(
	  Control::Type::Number, empty_str, (long int)0, Control::Color::Dark,
	  valve_program_selector, generalMinMaxCallback, (void *)100);

	addClearLabel("Valve cycles count (0-100)", valve_program_selector);

	valve_worktime_number = ESPUI.addControl(
	  Control::Type::Number, empty_str, (long int)0, Control::Color::Dark,
	  valve_program_selector, generalMinMaxCallback, (void *)86400);

	addClearLabel(
	  "Valve cycle worktime (0 s - 86400 s)", valve_program_selector);

	valve_volume_number = ESPUI.addControl(
	  Control::Type::Number, empty_str, (long int)0, Control::Color::Dark,
	  valve_program_selector, generalMinMaxCallback, (void *)6500);

	addClearLabel("Valve cycle volume (0 L - 6500 L)", valve_program_selector);

	valve_pause_number = ESPUI.addControl(
	  Control::Type::Number, empty_str, (long int)0, Control::Color::Dark,
	  valve_program_selector, generalMinMaxCallback, (void *)86400);

	addClearLabel("Valve cycle pause (0 s - 86400 s)", valve_program_selector);

	save_program_button = ESPUI.addControl(
	  Control::Type::Button, "Valve programs", "Save program",
	  Control::Color::Dark, valve_program_selector, valveCallback,
	  (void *)GUI_CB_SAVE_PROGRAM_FLAG);

	load_program_button = ESPUI.addControl(
	  Control::Type::Button, "Valve programs", "Load program",
	  Control::Color::Dark, valve_program_selector, valveCallback,
	  (void *)GUI_CB_LOAD_PROGRAM_FLAG);

	start_program_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Start program",
	  Control::Color::Dark, valve_program_selector, valveCallback,
	  (void *)GUI_CB_START_PROGRAM_FLAG);

	stop_program_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Stop program",
	  Control::Color::Dark, valve_program_selector, valveCallback,
	  (void *)GUI_CB_STOP_PROGRAM_FLAG);

	send_program_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Save program in Supla channel (#1)",
	  Control::Color::Dark, valve_program_selector, valveCallback,
	  (void *)GUI_CB_SEND_PROGRAM_FLAG);

	send_program_2_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Save program in Supla channel (#2)",
	  Control::Color::Dark, valve_program_selector, valveCallback,
	  (void *)GUI_CB_SEND_PROGRAM_2_FLAG);

	valve_info_label = ESPUI.addControl(
	  Control::Type::Label, empty_str, three_dots_str, Control::Color::Dark,
	  valve_program_selector);
}

/*****************************************************************************/

void enableSonoffValveGUI(bool enable) {

	ESPUI.updateLabel(valve_info_label, three_dots_str);

	ESPUI.updateSelect(valve_program_selector, -1);
	ESPUI.updateNumber(valve_cycles_number, 0);
	ESPUI.updateNumber(valve_worktime_number, 0);
	ESPUI.updateNumber(valve_volume_number, 0);
	ESPUI.updateNumber(valve_pause_number, 0);

	enableControlStyle(valve_program_selector, enable);
	enableControlStyle(valve_cycles_number, enable);
	enableControlStyle(valve_worktime_number, enable);
	enableControlStyle(valve_volume_number, enable);
	enableControlStyle(valve_pause_number, enable);
	enableControlStyle(save_program_button, enable);
	enableControlStyle(load_program_button, enable);
	enableControlStyle(start_program_button, enable);
	enableControlStyle(stop_program_button, enable);
	enableControlStyle(send_program_button, enable);
	enableControlStyle(send_program_2_button, enable);
	enableControlStyle(valve_info_label, enable);
}

/*****************************************************************************/

uint16_t dual_valve_gui_base_id;

#define DV_CHANNEL_SELECTOR (dual_valve_gui_base_id + 0)
#define DV_PROGRAM_SELECTOR (dual_valve_gui_base_id + 1)
#define DV_TOTAL_DURATION (dual_valve_gui_base_id + 2)       //+label
#define DV_IRRIGATION_DURATION (dual_valve_gui_base_id + 4)  //+label
#define DV_PAUSE_TIME (dual_valve_gui_base_id + 6)           //+label
#define DV_FAILSAFE_TIME (dual_valve_gui_base_id + 8)        //+label
#define DV_VOLUME (dual_valve_gui_base_id + 10)              //+label
#define DV_SAVE_BUTTON (dual_valve_gui_base_id + 12)
#define DV_LOAD_BUTTON (dual_valve_gui_base_id + 13)
#define DV_START_BUTTON (dual_valve_gui_base_id + 14)
#define DV_STOP_BUTTON (dual_valve_gui_base_id + 15)
#define DV_SEND_BUTTON (dual_valve_gui_base_id + 16)
#define DV_SEND2_BUTTON (dual_valve_gui_base_id + 17)
#define DV_INFO_LABEL (dual_valve_gui_base_id + 18)

template<typename... Args>
void addGUIObject(uint16_t check_id, Args &&...args) {

	uint16_t id = ESPUI.addControl(std::forward<Args>(args)...);
	if (check_id != id)
		log_e("GUI ID CREATION FATAL ERROR ID MISMATCH %u vs %u", check_id, id);
}

/*const char *gui_mismatch_str = "GUI ID CREATION FATAL ERROR ID MISMATCH";

#define addGUIObject(id,...) \
if (id != ESPUI.addControl(__VA_ARGS__)) \
log_e("%s %u", gui_mismatch_str, id)*/

void buildSonoffDualValveGUI(uint16_t advanced_devices_tab) {

	dual_valve_gui_base_id = ESPUI.addControl(
	  Control::Type::Select, "SONOFF SWV-ZF2 VALVE", (long)-1,
	  Control::Color::Dark, advanced_devices_tab, generalCallback);

	addGUIObject(
	  DV_PROGRAM_SELECTOR, Control::Type::Select, "SONOFF SWV-ZF2 VALVE",
	  (long)-1, Control::Color::Dark, dual_valve_gui_base_id,
	  generalCallback);

	addGUIObject(
	  DV_TOTAL_DURATION, Control::Type::Number, empty_str, (long)0,
	  Control::Color::Dark, dual_valve_gui_base_id, generalMinMaxCallback,
	  (void *)719);

	addClearLabel(
	  "Total duration time (1 - 719 min)", dual_valve_gui_base_id);

	addGUIObject(
	  DV_IRRIGATION_DURATION, Control::Type::Number, empty_str, (long)0,
	  Control::Color::Dark, dual_valve_gui_base_id, generalMinMaxCallback,
	  (void *)719);

	addClearLabel(
	  "Irrigation duration time (1 - 719 min)", dual_valve_gui_base_id);

	addGUIObject(
	  DV_PAUSE_TIME, Control::Type::Number, empty_str, (long)0,
	  Control::Color::Dark, dual_valve_gui_base_id, generalMinMaxCallback,
	  (void *)719);

	addClearLabel(
	  "Irrigation pause time (1 - 719 min)", dual_valve_gui_base_id);

	addGUIObject(
	  DV_FAILSAFE_TIME, Control::Type::Number, empty_str, (long)0,
	  Control::Color::Dark, dual_valve_gui_base_id, generalMinMaxCallback,
	  (void *)719);

	addClearLabel(
	  "Failsafe time (0 - 719 min)", dual_valve_gui_base_id);

	addGUIObject(
	  DV_VOLUME, Control::Type::Number, empty_str, (long)0,
	  Control::Color::Dark, dual_valve_gui_base_id, generalMinMaxCallback,
	  (void *)10000);

	addClearLabel(
	  "Valve cycle volume (0 L - 10 000 L)", dual_valve_gui_base_id);

	addGUIObject(
	  DV_SAVE_BUTTON, Control::Type::Button, empty_str, "Save program",
	  Control::Color::Dark, dual_valve_gui_base_id, valveCallback,
	  (void *)GUI_CB_SAVE_PROGRAM_FLAG);

	addGUIObject(
	  DV_LOAD_BUTTON, Control::Type::Button, empty_str, "Load program",
	  Control::Color::Dark, dual_valve_gui_base_id, valveCallback,
	  (void *)GUI_CB_LOAD_PROGRAM_FLAG);

	addGUIObject(
	  DV_START_BUTTON, Control::Type::Button, empty_str, "Start program",
	  Control::Color::Dark, dual_valve_gui_base_id, valveCallback,
	  (void *)GUI_CB_START_PROGRAM_FLAG);

	addGUIObject(
	  DV_STOP_BUTTON, Control::Type::Button, empty_str, "Stop program",
	  Control::Color::Dark, dual_valve_gui_base_id, valveCallback,
	  (void *)GUI_CB_STOP_PROGRAM_FLAG);

	addGUIObject(
	  DV_SEND_BUTTON, Control::Type::Button, empty_str,
	  "Save program in Supla channel (#1)", Control::Color::Dark,
	  dual_valve_gui_base_id, valveCallback, (void *)GUI_CB_SEND_PROGRAM_FLAG);

	addGUIObject(
	  DV_SEND2_BUTTON, Control::Type::Button, empty_str,
	  "Save program in Supla channel (#2)", Control::Color::Dark,
	  dual_valve_gui_base_id, valveCallback, (void *)GUI_CB_SEND_PROGRAM_2_FLAG);

	addGUIObject(
	  DV_INFO_LABEL, Control::Type::Label, empty_str, three_dots_str,
	  Control::Color::Dark, dual_valve_gui_base_id);

	ESPUI.addControl(
	  Control::Type::Option, "Select valve channel...", (long)-1,
	  Control::Color::None, DV_CHANNEL_SELECTOR);

	ESPUI.addControl(
	  Control::Type::Option, "Channel A", (long)1, Control::Color::None,
	  DV_CHANNEL_SELECTOR);

	ESPUI.addControl(
	  Control::Type::Option, "Channel B", (long)2, Control::Color::None,
	  DV_CHANNEL_SELECTOR);

	ESPUI.addControl(
	  Control::Type::Option, "Select program...", (long)-1,
	  Control::Color::None, DV_PROGRAM_SELECTOR);

	ESPUI.addControl(
	  Control::Type::Option, "Time program...", (long)0, Control::Color::None,
	  DV_PROGRAM_SELECTOR);

	ESPUI.addControl(
	  Control::Type::Option, "Volume program...", (long)1, Control::Color::None,
	  DV_PROGRAM_SELECTOR);

	ESPUI.addControl(
	  Control::Type::Option, "Time + Interval program...", (long)2,
	  Control::Color::None, DV_PROGRAM_SELECTOR);
}

/*****************************************************************************/

void enableSonoffDualValveGUI(bool enable) {

	ESPUI.updateLabel(DV_INFO_LABEL, three_dots_str);

	ESPUI.updateSelect(DV_CHANNEL_SELECTOR, -1);
	ESPUI.updateSelect(DV_PROGRAM_SELECTOR, -1);
	ESPUI.updateNumber(DV_TOTAL_DURATION, 0);
	ESPUI.updateNumber(DV_IRRIGATION_DURATION, 0);
	ESPUI.updateNumber(DV_VOLUME, 0);
	ESPUI.updateNumber(DV_PAUSE_TIME, 0);
	ESPUI.updateNumber(DV_FAILSAFE_TIME, 0);

	enableControlStyle(DV_CHANNEL_SELECTOR, enable);
	enableControlStyle(DV_PROGRAM_SELECTOR, enable);
	enableControlStyle(DV_TOTAL_DURATION, enable);
	enableControlStyle(DV_IRRIGATION_DURATION, enable);
	enableControlStyle(DV_FAILSAFE_TIME, enable);
	enableControlStyle(DV_VOLUME, enable);
	enableControlStyle(DV_PAUSE_TIME, enable);
	enableControlStyle(DV_SAVE_BUTTON, enable);
	enableControlStyle(DV_LOAD_BUTTON, enable);
	enableControlStyle(DV_START_BUTTON, enable);
	enableControlStyle(DV_STOP_BUTTON, enable);
	enableControlStyle(DV_SEND_BUTTON, enable);
	enableControlStyle(DV_SEND2_BUTTON, enable);
	enableControlStyle(DV_INFO_LABEL, enable);
}

/*****************************************************************************/

void enableTuyaGasDetectorGUI(bool enable) {

	ESPUI.updateSelect(gas_alarm_ringtone_selector, -1);
	ESPUI.updateNumber(gas_alarm_time_number, 0);

	ESPUI.updateLabel(gas_alarm_info_label, three_dots_str);
	updateLabel_C(gas_alarm_Tuya_payload_label, three_dots_str);

	enableControlStyle(gas_alarm_ringtone_selector, enable);
	enableControlStyle(gas_alarm_time_number, enable);
	enableControlStyle(gas_alarm_ringtone_button, enable);
	enableControlStyle(gas_alarm_time_button, enable);
	enableControlStyle(gas_alarm_self_test_button, enable);
	enableControlStyle(gas_alarm_silence_button, enable);
}

/*****************************************************************************/

void buildTuyaGasDetectorGUI(uint16_t advanced_devices_tab) {

	static const char ringtones[][9] PROGMEM = {
		"MELODY 1", "MELODY 2", "MELODY 3", "MELODY 4", "MELODY 5"
	};

	gas_alarm_ringtone_selector = ESPUI.addControl(
	  Control::Type::Select, "GAS DETECTOR", (long int)-1,
	  Control::Color::Dark, advanced_devices_tab, generalCallback);

	ESPUI.addControl(
	  Control::Type::Option, "Select alarm ringtone...", (long int)-1,
	  Control::Color::None, gas_alarm_ringtone_selector);

	for (uint8_t r = 0; r < 5; r++) {

		ESPUI.addControl(
		  Control::Type::Option, ringtones[r], r,
		  Control::Color::None, gas_alarm_ringtone_selector);
	}

	ESPUI.addControl(
	  Control::Type::Separator, empty_str, empty_str,
	  Control::Color::None, gas_alarm_ringtone_selector);

	gas_alarm_time_number = ESPUI.addControl(
	  Control::Type::Number, empty_str, (long int)0,
	  Control::Color::Dark, gas_alarm_ringtone_selector,
	  generalMinMaxCallback, (void *)180);

	addClearLabel(
	  "Alarm duration (0-180 s)", gas_alarm_ringtone_selector);

	gas_alarm_ringtone_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Send alarm ringtone",
	  Control::Color::Dark, gas_alarm_ringtone_selector, valveCallback,
	  (void *)GUI_CB_SEND_RINGTONE_FLAG);

	gas_alarm_time_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Send alarm duration",
	  Control::Color::Dark, gas_alarm_ringtone_selector,
	  valveCallback, (void *)GUI_CB_SEND_TIME_FLAG);

	gas_alarm_self_test_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Self test",
	  Control::Color::Dark, gas_alarm_ringtone_selector, valveCallback,
	  (void *)GUI_CB_SELF_TEST_FLAG);

	gas_alarm_silence_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Silence alarm",
	  Control::Color::Dark, gas_alarm_ringtone_selector, valveCallback,
	  (void *)GUI_CB_SILENCE_FLAG);

	//working_str = three_dots_str;
	gas_alarm_info_label = ESPUI.addControl(
	  Control::Type::Label, empty_str, three_dots_str,
	  Control::Color::Dark, gas_alarm_ringtone_selector);

	gas_alarm_Tuya_payload_label = ESPUI.addControl(
	  Control::Type::Label, empty_str, working_str,
	  Control::Color::Dark, gas_alarm_ringtone_selector);
}

/*****************************************************************************/

void enableMoesAlarmGUI(bool enable) {

	ESPUI.updateSelect(moes_alarm_melody_selector, -1);
	ESPUI.updateNumber(moes_alarm_duration_number, 0);
	ESPUI.updateNumber(moes_alarm_volume_number, 0);

	ESPUI.updateLabel(moes_alarm_info_label, three_dots_str);
	updateLabel_C(moes_alarm_Tuya_payload_label, three_dots_str);

	enableControlStyle(moes_alarm_melody_selector, enable);
	enableControlStyle(moes_alarm_duration_number, enable);
	enableControlStyle(moes_alarm_volume_number, enable);
	enableControlStyle(moes_alarm_melody_button, enable);
	enableControlStyle(moes_alarm_duration_button, enable);
	enableControlStyle(moes_alarm_volume_button, enable);
}

/*****************************************************************************/

void buildMoesAlarmGUI(uint16_t advanced_devices_tab) {

	static const char melodies[][10] PROGMEM = {
		"MELODY 1", "MELODY 2", "MELODY 3", "MELODY 4", "MELODY 5", "MELODY 6",
		"MELODY 7", "MELODY 8", "MELODY 9", "MELODY 10", "MELODY 11", "MELODY 12",
		"MELODY 13", "MELODY 14", "MELODY 15", "MELODY 16", "MELODY 17",
		"MELODY 18"
	};

	moes_alarm_melody_selector = ESPUI.addControl(
	  Control::Type::Select, "SIREN ALARM", (long int)-1,
	  Control::Color::Dark, advanced_devices_tab, generalCallback);

	ESPUI.addControl(
	  Control::Type::Option, "Select alarm melody...", (long int)-1,
	  Control::Color::None, moes_alarm_melody_selector);

	for (uint8_t m = 0; m < 18; m++) {

		ESPUI.addControl(
		  Control::Type::Option, melodies[m], m, Control::Color::None,
		  moes_alarm_melody_selector);
	}

	ESPUI.addControl(
	  Control::Type::Separator, empty_str, empty_str, Control::Color::None,
	  moes_alarm_melody_selector);

	moes_alarm_duration_number = ESPUI.addControl(
	  Control::Type::Number, empty_str, (long int)0,
	  Control::Color::Dark, moes_alarm_melody_selector,
	  generalMinMaxCallback, (void *)1800);

	addClearLabel(
	  "Alarm duration (0 - 1800 s)", moes_alarm_melody_selector);

	moes_alarm_volume_number = ESPUI.addControl(
	  Control::Type::Number, empty_str, (long int)0,
	  Control::Color::Dark, moes_alarm_melody_selector,
	  generalMinMaxCallback, (void *)2);

	addClearLabel(
	  "Alarm volume (0 - 2)", moes_alarm_melody_selector);

	moes_alarm_melody_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Send alarm melody",
	  Control::Color::Dark, moes_alarm_melody_selector, valveCallback,
	  (void *)GUI_CB_SEND_MELODY_FLAG);

	moes_alarm_duration_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Send alarm duration",
	  Control::Color::Dark, moes_alarm_melody_selector, valveCallback,
	  (void *)GUI_CB_SEND_DURATION_FLAG);

	moes_alarm_volume_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "Send alarm volume",
	  Control::Color::Dark, moes_alarm_melody_selector, valveCallback,
	  (void *)GUI_CB_SEND_VOLUME_FLAG);

	//working_str = three_dots_str;
	moes_alarm_info_label = ESPUI.addControl(
	  Control::Type::Label, empty_str, three_dots_str,
	  Control::Color::Dark, moes_alarm_melody_selector);

	moes_alarm_Tuya_payload_label = ESPUI.addControl(
	  Control::Type::Label, empty_str, working_str,
	  Control::Color::Dark, moes_alarm_melody_selector);
}

/*****************************************************************************/

void enableTuyaDevicesControls(bool enable) {

	if (enable == (controls_enabled_flags & TUYA_DEVICES_ENABLED_FLAG))
		return;

	updateLabel_C(
	  Tuya_devices_tab_controls_table[Tuya_device_info_label], three_dots_str);

	for (uint16_t i = Tuya_datapoint_id_selector;
	     i < Tuya_device_last_enum_position; i++)

		enableControlStyle(Tuya_devices_tab_controls_table[i], enable);

	enable ? controls_enabled_flags |= TUYA_DEVICES_ENABLED_FLAG : controls_enabled_flags &= ~TUYA_DEVICES_ENABLED_FLAG;
}

/*****************************************************************************/

void rebuildTuyaDevicesDatapointsList(uint8_t Tuya_device_slot) {

	ESPUI.updateControlValue(
	  Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector], -1);
}

/*****************************************************************************/

void buildTuyaCustomClusterTabGUI() {

	auto Tuya_custom_cluster_tab = ESPUI.addControl(
	  Control::Type::Tab, empty_str, "Tuya custom cluster (0xEF00) devices",
	  Control::Color::Dark, Control::noParent);

	Tuya_devices_tab_controls_table[Tuya_device_selector] =
	  ESPUI.addControl(
	    Control::Type::Select, "TCC ZigBee devices", (long int)-1,
	    Control::Color::Dark, Tuya_custom_cluster_tab,
	    TuyaDeviceSelectorCallback);

	working_str = three_dots_str;
	Tuya_devices_tab_controls_table[Tuya_device_info_label] = ESPUI.addControl(
	  Control::Type::Label, "TCC ZigBee device", working_str,
	  Control::Color::Dark, Tuya_custom_cluster_tab);

	auto main_panel = ESPUI.addControl(
    Control::Type::Label, "Tuya custom cluster panel", empty_str,
		Control::Color::Dark, Tuya_custom_cluster_tab);
  
  ESPUI.setPanelWide(main_panel, true);
  ESPUI.setElementStyle(main_panel, clearLabelStyle);

  Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector] =
	ESPUI.addControl(
      Control::Type::Select, "TCC ZigBee device datapoints", (long int)-1,
      Control::Color::Dark, main_panel, TuyaDatapointIdSelectorCallback);

  ESPUI.setElementStyle(
    Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector],
    "width: calc(25% - 10px); min-width: 160px; margin: 0 5px 10px 5px;");

  Tuya_devices_tab_controls_table[Tuya_datapoint_id_number] =
    ESPUI.addControl(
      Control::Type::Number, "DP ID (1-255)", (long int)0, Control::Color::Dark,
      main_panel, generalMinMaxCallback, (void *)255);

  ESPUI.setElementStyle(
    Tuya_devices_tab_controls_table[Tuya_datapoint_id_number],
    "width: calc(12% - 10px); min-width: 90px; margin: 0 5px 10px 5px;");

  Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector] =
    ESPUI.addControl(
      Control::Type::Select, "Tuya datapoint type", (long int)-1,
      Control::Color::Dark, main_panel, TuyaDatapointTypeSelectorCallback);

  ESPUI.setElementStyle(
    Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector],
    "width: calc(25% - 10px); min-width: 160px; margin: 0 5px 10px 5px;");

  Tuya_devices_tab_controls_table[Tuya_datapoint_length_number] =
    ESPUI.addControl(
      Control::Type::Number, "Length (1-58)", (long int)0, Control::Color::Dark,
      main_panel, generalMinMaxCallback, (void *)58);

  ESPUI.setElementStyle(
    Tuya_devices_tab_controls_table[Tuya_datapoint_length_number],
    "width: calc(13% - 10px); min-width: 90px; margin: 0 5px 10px 5px;");

  Tuya_devices_tab_controls_table[Tuya_datapoint_value_number] =
    ESPUI.addControl(
      Control::Type::Number, "Value (no range check)", (long int)0, 
      Control::Color::Dark, main_panel, generalCallback);

  ESPUI.setElementStyle(
    Tuya_devices_tab_controls_table[Tuya_datapoint_value_number],
    "width: calc(25% - 10px); min-width: 160px; margin: 0 5px 10px 5px;");

  ESPUI.addControl(
    Control::Type::Option, "Select Tuya datapoint type...", (long int)-1,
    Control::Color::None,
    Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector]);

  for (uint8_t t = 0; t < 6; t++) {
    working_str = Tuya_datapoint_types[t].Tuya_datapoint_type_id;
    ESPUI.addControl(
      Control::Type::Option, Tuya_datapoint_types[t].Tuya_datapoint_type_name,
      Tuya_datapoint_types[t].Tuya_datapoint_type_id, Control::Color::None,
      Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector]);
  }

	working_str = three_dots_str;
	Tuya_devices_tab_controls_table[Tuya_datapoint_description_label] =
	  ESPUI.addControl(
	    Control::Type::Label, "Datapoint description", working_str, 
			Control::Color::Dark, main_panel);

	/*addClearLabel(
	  "Datapoint acceptable values description.<br>"
	  "Use it to fill datapoint value field.", main_panel);*/

	ESPUI.setElementStyle(
	  Tuya_devices_tab_controls_table[Tuya_datapoint_description_label],
	  "text-align: left; font-family:tahoma; font-size: "
	  "4 px; font-style: normal; font-weight: normal;");

  Tuya_devices_tab_controls_table[Tuya_device_data_request_button] =
    ESPUI.addControl(
      Control::Type::Button, empty_str, "Data request",
      Control::Color::Dark, main_panel, TuyaCustomCmdCallback, 
      (void *)GUI_CB_SEND_TUYA_REQUEST_FLAG);

  Tuya_devices_tab_controls_table[Tuya_device_data_query_button] =
    ESPUI.addControl(
      Control::Type::Button, empty_str, "Data query",
      Control::Color::Dark, main_panel, TuyaCustomCmdCallback, 
      (void *)GUI_CB_SEND_TUYA_QUERY_FLAG);

  ESPUI.setElementStyle(
    Tuya_devices_tab_controls_table[Tuya_device_data_request_button],
    "width: calc(45% - 15px); min-width: 130px; margin: 0 15px 10px 5px;");
    
  ESPUI.setElementStyle(
    Tuya_devices_tab_controls_table[Tuya_device_data_query_button],
    "width: calc(45% - 15px); min-width: 130px; margin: 0 5px 10px 15px;");

  working_str = three_dots_str;
  
  Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label] =
    ESPUI.addControl(
      Control::Type::Label, "Command Result", working_str,
      Control::Color::Dark, main_panel);

  Tuya_devices_tab_controls_table[Tuya_device_payload_label] =
    ESPUI.addControl(
      Control::Type::Label, "Command Response payload", working_str,
      Control::Color::Dark, main_panel);

  ESPUI.setElementStyle(
    Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label],
    "width: calc(50% - 10px); min-width: 200px; margin: 0 5px 10px 5px;");
    
  ESPUI.setElementStyle(
    Tuya_devices_tab_controls_table[Tuya_device_payload_label],
    "width: calc(50% - 10px); min-width: 200px; margin: 0 5px 10px 5px; ");
}

/*****************************************************************************/

void buildAdvancedDevicesTabGUI() {

	auto advanced_devices_tab = ESPUI.addControl(
	  Control::Type::Tab, empty_str, "Advanced devices", Control::Color::Dark,
	  Control::noParent);

	advanced_device_selector = ESPUI.addControl(
	  Control::Type::Select, "Advanced ZigBee devices", (long int)-1,
	  Control::Color::Dark, advanced_devices_tab,
	  advancedDeviceSelectorCallback);

	for (Z2S_Device *device : Z2S_Devices) {

    if (device->isAdvancedDevice()) {

      switch (device->getDeviceDescId()) {

        case Z2S_DEVICE_DESC_SONOFF_SMART_VALVE:
          isSonoffValvePresent = true;
          break;

        case Z2S_DEVICE_DESC_SONOFF_SMART_DUAL_VALVE:
          isSonoffDualValvePresent = true;
          break;

        case Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR:
          isTuyaGasDetectorPresent = true;
          break;

        case Z2S_DEVICE_DESC_MOES_ALARM:
          isMoesAlarmPresent = true;
          break;
      }
    }
  }
	working_str = three_dots_str;
	advanced_device_info_label = ESPUI.addControl(
	  Control::Type::Label, "Advanced ZigBee device", working_str,
	  Control::Color::Dark, advanced_devices_tab);

	//SONOFF VALVE SWV
	if (Z2S_Device::hasZbDevice(Z2S_DEVICE_DESC_SONOFF_SMART_VALVE)) {

		buildSonoffValveGUI(advanced_devices_tab);
		enableSonoffValveGUI(false);
	}

	//SONOFF VALVE SWV-ZF2
	if (Z2S_Device::hasZbDevice(Z2S_DEVICE_DESC_SONOFF_SMART_DUAL_VALVE)) {

		buildSonoffDualValveGUI(advanced_devices_tab);
		enableSonoffDualValveGUI(false);
	}

	//GAS DETECTOR
	if (Z2S_Device::hasZbDevice(Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR)) {

		buildTuyaGasDetectorGUI(advanced_devices_tab);
		enableTuyaGasDetectorGUI(false);
	}
	//MOES ALARM
	if (Z2S_Device::hasZbDevice(Z2S_DEVICE_DESC_MOES_ALARM)) {

		buildMoesAlarmGUI(advanced_devices_tab);
		enableMoesAlarmGUI(false);
	}
}

/*****************************************************************************/

const char *getSuplaActionName(Supla::Action action_id) {

	uint16_t actions_number = sizeof(Supla_actions) / sizeof(Supla_action_type_t);

	for (uint16_t i = 0; i < actions_number; i++)
		if (Supla_actions[i].Supla_action_id == action_id)
			return Supla_actions[i].Supla_action_name;
	return "Unknown";
}

/*****************************************************************************/

const char *getSuplaEventName(Supla::Event event_id, bool is_condition) {

	uint16_t events_number = sizeof(Supla_events) / sizeof(Supla_event_type_t);

	for (uint16_t i = 0; i < events_number; i++)

		if ((Supla_events[i].Supla_event_id == event_id) && (Supla_events[i].is_condition == is_condition))
			return Supla_events[i].Supla_event_name;
	return "Unknown";
}

	/*****************************************************************************/

	void sprintfAction(z2s_channel_action_t &action) {

		char general_purpose_gui_buffer[512] = {};

		if (action.is_condition)
			snprintf(
				general_purpose_gui_buffer, 512,
				"<b>Action#:</b> <i>%d</i> <b>of</b> <i>%d</i><br><br>"
				"<b>Action name:</b> <i>%s (%s)</i><br><br>"
				"<b>Condition:</b> <i>{%s}</i><br>"
				"with <b>value(s) = </b> <i>%.2f, %.2f </i><br>"
				"<b>for source channel:</b> <i>[%s]</i><br><br>"
				"<b>Action:</b> <i>{%s}</i> <b><br>"
				"on destination channel:</b> <i>[%s]</i> (subaction id %u::%lu)",
				current_action_counter, Z2S_getActionsNumber(), action.action_name,
				action.is_enabled ? "enabled" : "disabled",
				getSuplaEventName(action.src_Supla_event, action.is_condition),
				action.min_value, action.max_value,
				Z2S_Core::getZ2SChannelNameByChannelNumber(action.src_Supla_channel),
				getSuplaActionName(action.dst_Supla_action),
				Z2S_Core::getZ2SChannelNameByChannelNumber(action.dst_Supla_channel),
				action.subaction_id, action.reserved32);
		else
			snprintf(
				general_purpose_gui_buffer, 512,
				"<b>Action#:</b> <i>%d</i> <b>of</b> <i>%d</i><br><br>"
				"<b>Action name:</b> <i>%s (%s)</i><br><br>"
				"<b>Event:</b> <i>{%s}</i><br>"
				"<b>from source channel:</b> <i>[%s]</i><br><br>"
				"<b>Action:</b> <i>{%s}</i> <b><br>"
				"on destination channel:</b> <i>[%s]</i> (subaction id %u::%lu)",
				current_action_counter, Z2S_getActionsNumber(), action.action_name,
				action.is_enabled ? "enabled" : "disabled", getSuplaEventName(action.src_Supla_event, action.is_condition),
				Z2S_Core::getZ2SChannelNameByChannelNumber(action.src_Supla_channel),
				getSuplaActionName(action.dst_Supla_action),
				Z2S_Core::getZ2SChannelNameByChannelNumber(action.dst_Supla_channel),
				action.subaction_id, action.reserved32);

		working_str = general_purpose_gui_buffer;
	}

/*****************************************************************************/

void enableActionDetails(bool enable) {

	if (!enable) {

		ESPUI.updateSwitcher(action_enabled_switcher, false);
		working_str = "";
		ESPUI.updateText(action_name_text, working_str);
		ESPUI.updateText(action_description_text, working_str);
		ESPUI.updateSelect(action_source_channel_selector, -1);
		ESPUI.updateSelect(action_event_selector, -1);
		ESPUI.updateSelect(action_destination_channel_selector, -1);
		ESPUI.updateSelect(action_action_selector, -1);
		ESPUI.updateNumber(action_subaction_number, 0);
		working_str = "0.00";
		ESPUI.updateText(action_condition_threshold_1_number, working_str);
		ESPUI.updateText(action_condition_threshold_2_number, working_str);
	}

	enableControlStyle(action_details_label, enable);
	enableControlStyle(action_enabled_switcher, enable);
	enableControlStyle(action_name_text, enable);
	enableControlStyle(action_description_text, enable);
	enableControlStyle(action_source_channel_selector, enable);
	enableControlStyle(action_event_selector, enable);
	enableControlStyle(action_destination_channel_selector, enable);
	enableControlStyle(action_action_selector, enable);
	enableControlStyle(action_subaction_number, enable);
	enableControlStyle(action_condition_threshold_1_number, enable);
	enableControlStyle(action_condition_threshold_2_number, enable);
	if (enable)
		actionSelectorCallback(nullptr, -1, nullptr);
}

void enableActionControls(bool enable) {

	//enableControlStyle(table_first_button, enable);
	//enableControlStyle(table_next_button, enable);
	//enableControlStyle(table_prev_button, enable);
	//enableControlStyle(table_last_button, enable);
}

/*****************************************************************************/

void updateActionButtons() {

	bool action_save_button_enable;
	bool action_new_button_enable;
	bool action_edit_button_enable;
	bool action_copy_button_enable;
	bool action_cancel_button_enable;
	bool action_remove_button_enable;

	switch (current_action_gui_state) {


		case VIEW_ACTION:
			{

				action_save_button_enable = false;
				action_cancel_button_enable = false;

				if (current_action_id == -1) {

					action_edit_button_enable = false;
					action_remove_button_enable = false;
					action_copy_button_enable = false;
				} else {

					action_edit_button_enable = true;
					action_remove_button_enable = true;
					action_copy_button_enable = true;
				}

				action_new_button_enable = true;
			}
			break;


		case NEW_ACTION:
			{

				action_save_button_enable = true;
				action_new_button_enable = false;
				action_edit_button_enable = false;
				action_cancel_button_enable = true;
				action_remove_button_enable = false;
			}
			break;


		case EDIT_ACTION:
			{

				action_save_button_enable = true;
				action_new_button_enable = false;
				action_edit_button_enable = false;
				action_cancel_button_enable = true;
				action_remove_button_enable = false;
			}
			break;


		case COPY_ACTION:
			{

				action_save_button_enable = true;
				action_new_button_enable = false;
				action_edit_button_enable = false;
				action_cancel_button_enable = true;
				action_remove_button_enable = false;
			}
			break;
	}

	enableControlStyle(action_save_button, action_save_button_enable);
	enableControlStyle(action_new_button, action_new_button_enable);
	enableControlStyle(action_edit_button, action_edit_button_enable);
	enableControlStyle(action_cancel_button, action_cancel_button_enable);
	enableControlStyle(action_remove_button, action_remove_button_enable);
	enableControlStyle(action_copy_button, action_remove_button_enable);
}

/*****************************************************************************/

void updateActionDetails(
  z2s_channel_action_t &action, bool empty_action = false) {

	if (current_action_gui_state == VIEW_ACTION) {

		if (previous_action_gui_state != VIEW_ACTION)
			enableActionDetails(false);
		return;
	}

	if (current_action_gui_state != VIEW_ACTION)
		enableActionControls(false);

	ESPUI.updateSwitcher(
		action_enabled_switcher, empty_action ? true : action.is_enabled);

	working_str = empty_action ? empty_str : action.action_name;
	ESPUI.updateText(action_name_text, working_str);

	working_str = empty_action ? empty_str : action.action_description;
	ESPUI.updateText(action_description_text, working_str);

	ESPUI.updateSelect(
	  action_source_channel_selector, empty_action ? -1 : 
		Z2S_Core::getZ2SChannelIndexByChannelNumber(action.src_Supla_channel));

	long action_id = empty_action ? -1 : action.is_condition ? (0xFFFF + action.src_Supla_event)
	                                                         : action.src_Supla_event;
	ESPUI.updateSelect(action_event_selector, action_id);

	ESPUI.updateSelect(
	  action_destination_channel_selector, empty_action ? -1 : Z2S_Core::getZ2SChannelIndexByChannelNumber(action.dst_Supla_channel));

	ESPUI.updateSelect(
	  action_action_selector, empty_action ? -1 : action.dst_Supla_action);

	actionSelectorCallback(
	  nullptr, empty_action ? -1 : action.dst_Supla_action, nullptr);

	ESPUI.updateNumber(action_subaction_number, empty_action ? 0 : action.subaction_id);

	working_str = empty_action ? String(0, 2) : String(action.min_value, 2);
	ESPUI.updateText(action_condition_threshold_1_number, working_str);

	working_str = empty_action ? String(0, 2) : String(action.max_value, 2);
	ESPUI.updateText(action_condition_threshold_2_number, working_str);
}

/*****************************************************************************/

bool fillActionDetails(z2s_channel_action_t &action) {

	action.is_enabled = ESPUI.getControl(action_enabled_switcher)->getValueInt();

	const char *action_name_str = ESPUI.getControl(action_name_text)->getValueCstr();
	if (strlen(action_name_str) >= 0) {

		size_t action_name_size = strnlen(
		  action_name_str, ACTION_NAME_MAX_SIZE - 1);

		strncpy(action.action_name, action_name_str, action_name_size);
		action.action_name[action_name_size] = '\0';
	} else
		return false;

	size_t action_description_size = strnlen(
	  ESPUI.getControl(action_description_text)->getValueCstr(),
	  ACTION_DESCRIPTION_MAX_SIZE - 1);

	strncpy(
	  action.action_description,
	  ESPUI.getControl(action_description_text)->getValueCstr(),
	  action_description_size);

	action.action_description[action_description_size] = '\0';

	int32_t selector_value =
	  ESPUI.getControl(action_source_channel_selector)->getValueInt();

	if (selector_value >= 0) {

		action.src_Supla_channel = Z2S_Core::getZ2SChannelNumberByChannelIndex(
		  selector_value);
	} else
		return false;

	selector_value = ESPUI.getControl(action_event_selector)->getValueInt();
	if (selector_value >= 0) {

		if (selector_value >= 0xFFFF) {

			action.is_condition = true;
			action.src_Supla_event = (Supla::Event)(selector_value - 0xFFFF);
		} else {
			action.is_condition = false;
			action.src_Supla_event = (Supla::Event)selector_value;
		}
	} else
		return false;

	selector_value = ESPUI.getControl(
	                        action_destination_channel_selector)
	                   ->getValueInt();

	if (selector_value >= 0) {

		action.dst_Supla_channel = Z2S_Core::getZ2SChannelNumberByChannelIndex(
		  selector_value);
	} else
		return false;

	selector_value = ESPUI.getControl(action_action_selector)->getValueInt();

	if (selector_value >= 0)
		action.dst_Supla_action = (Supla::Action)selector_value;
	else
		return false;

	selector_value = ESPUI.getControl(action_subaction_number)->getValueInt();

	if (selector_value >= 0)
		action.subaction_id = selector_value;
	else
		return false;

	action.min_value = ESPUI.getControl(
	                          action_condition_threshold_1_number)
	                     ->getValue()
	                     .toDouble();

	action.max_value = ESPUI.getControl(
	                          action_condition_threshold_2_number)
	                     ->getValue()
	                     .toDouble();

	return true;
}

/*****************************************************************************/

uint16_t getSubactionsNumber(Supla::Action action_id) {

	uint16_t actions_number =
	  sizeof(Supla_actions) / sizeof(Supla_action_type_t);

	for (uint16_t i = 0; i < actions_number; i++)
		if (Supla_actions[i].Supla_action_id == action_id)
			return Supla_actions[i].max_subactions_number;
	return 0;
}

/*****************************************************************************/

void actionSelectorCallback(BasicControl *sender, int type, void *param) {

	int sender_value = type;

	if (sender)
		sender_value = sender->getValueInt();

	if ((sender_value < 0) || (sender_value > UINT16_MAX)) {

		enableControlStyle(action_subaction_number, false);
		return;
	}

	uint16_t max_subactions_number =
	  getSubactionsNumber((Supla::Action)sender_value);

	if (max_subactions_number) {

		enableControlStyle(action_subaction_number, true);
		ESPUI.getControl(
		       action_subaction_number)
		  ->setUserData((void *)max_subactions_number);
	} else
		enableControlStyle(action_subaction_number, false);
}

/*****************************************************************************/

static const char *json_payload_begin_fmt =
  "{\"type\":\"EXEC_JS\",\"code\":\" ";

static const char *json_payload_end_fmt = " \"}";

static const char *update_select_start_fmt =
  "{\"type\":\"update_select\",\"clear\":%s,\"selected\":\"%ld\",\"targets\":[";

/*****************************************************************************/

static bool appendToBuffer(
  char *buf, size_t bufSize, size_t &offset, const char *format, ...) {

	if (offset >= bufSize) return false;

	va_list args;
	va_start(args, format);
	int32_t written = vsnprintf(buf + offset, bufSize - offset, format, args);
	va_end(args);

	if (written < 0 || (written >= (bufSize - offset)))
		return false;

	offset += written;
	return true;
}

/*****************************************************************************/

void buildActionsTableJsonHeader(
  char* buffer, size_t max_size, size_t& offset, uint16_t label_id, 
	int selected_index, bool append, int start_idx) {

  if (offset >= max_size) return;
  
  int len = snprintf(
    buffer + offset, max_size - offset, 
    "{\"type\":\"update_action_table\",\"target\":\"id%d\",\"s\":%d,"
    "\"append\":%s,\"start_idx\":%d,\"d\":[", 
    label_id, selected_index, append ? "true" : "false", start_idx);
  
  if (len > 0 && len < (max_size - offset)) offset += len;
}

/*****************************************************************************/

void addActionsTableJsonRow(
  char* buffer, size_t max_size, size_t& offset, z2s_channel_action_t &action, 
	bool is_first) {

  if (offset >= max_size) return;

  char trig_buf[128] = {};

  if (action.is_condition) {
    snprintf(
      trig_buf, sizeof(trig_buf), "%s (v: %.2f, %.2f)", 
      getSuplaEventName(action.src_Supla_event, action.is_condition),
      action.min_value, action.max_value
    );
  } else {
    snprintf(
      trig_buf, sizeof(trig_buf), "%s", 
      getSuplaEventName(action.src_Supla_event, action.is_condition)
    );
  }

  int len = snprintf(
    buffer + offset, max_size - offset,
    "%s[\"%s (%s)\",\"%s\",\"%s\",\"%s (sid: %u)\",\"%s\"]",
    is_first ? "" : ",", 
    action.action_name, action.is_enabled ? "on" : "off",
    trig_buf,
    Z2S_Core::getZ2SChannelNameByChannelNumber(action.src_Supla_channel),
    getSuplaActionName(action.dst_Supla_action), action.subaction_id,
    Z2S_Core::getZ2SChannelNameByChannelNumber(action.dst_Supla_channel)
  );

  if (len > 0 && len < (max_size - offset)) offset += len;
}

/*****************************************************************************/

void buildActionsTableJsonFooter(
	char* buffer, size_t max_size, size_t& offset) {

  if (offset >= max_size) return;
  
  int len = snprintf(buffer + offset, max_size - offset, "]}");
  if (len > 0 && len < (max_size - offset)) offset += len;
}

/*****************************************************************************/

void buildActionsTableJsonPayload(
	uint16_t ui_label_id, int target_action_index) {
		
  const size_t MAX_JSON_SIZE = 2048; 
  char json_payload[MAX_JSON_SIZE] = {};

  const int BATCH_SIZE = 10; 
  size_t offset = 0;

  int total_actions = Z2S_getActionsNumber();

  log_i("total actions %u", total_actions);

  if (total_actions == 0) {
    
		offset = 0;
    
    buildActionsTableJsonHeader(
      json_payload, MAX_JSON_SIZE, offset, ui_label_id, -1, 
      false, 0);
  
    buildActionsTableJsonFooter(json_payload, MAX_JSON_SIZE, offset);

    log_i("Empty payload size %u: %s", strlen(json_payload), json_payload);

    ESPUI.WebSocket()->textAll(json_payload, strlen(json_payload));
    return;
  }

  int16_t next_action = Z2S_findNextActionPosition(0);
  z2s_channel_action_t new_action = {};

  for (int i = 0; i < total_actions; i++) {
    if (i % BATCH_SIZE == 0) {
      offset = 0; 
      bool is_appending = (i > 0);
      buildActionsTableJsonHeader(
        json_payload, MAX_JSON_SIZE, offset, ui_label_id, target_action_index, 
        is_appending, i
      );
    }

    bool is_first_in_batch = (i % BATCH_SIZE == 0);
    
    Z2S_loadAction(next_action, new_action);
    
    addActionsTableJsonRow(
      json_payload, MAX_JSON_SIZE, offset, new_action, is_first_in_batch
    );

    next_action = Z2S_findNextActionPosition(next_action + 1);

    if (((i + 1) % BATCH_SIZE == 0) || (i == total_actions - 1)) {
      buildActionsTableJsonFooter(json_payload, MAX_JSON_SIZE, offset);

      log_i("size %u, %s", strlen(json_payload), json_payload);

      ESPUI.WebSocket()->textAll(json_payload, strlen(json_payload));
    }
  }
}
/*****************************************************************************/


void sendActionsTableHighlight(uint16_t ui_label_id, int new_action_index) {
  
  char json_payload[128] = {};
  
  snprintf(
    json_payload, sizeof(json_payload), 
    "{\"type\":\"highlight_action_row\",\"target\":\"id%d\",\"s\":%d}", 
    ui_label_id, new_action_index);

  
  ESPUI.WebSocket()->textAll(json_payload, strlen(json_payload));
}

/*****************************************************************************/

void sendActionRowEdit(uint16_t ui_label_id, int target_action_index) {
  
	char json_payload[512] = {};
  size_t offset = 0;
  
  int len = snprintf(
    json_payload, sizeof(json_payload), 
    "{\"type\":\"edit_action_row\",\"target\":\"id%d\",\"idx\":%d,\"d\":", 
    ui_label_id, target_action_index);

  if (len > 0) offset += len;

	z2s_channel_action_t action {};

	//uint16_t action_position = Z2S_getActionPosition(target_action_index);
	Z2S_loadAction(current_action_id, action);

	//Z2S_loadAction(target_action_index, action);

  addActionsTableJsonRow(
		json_payload, sizeof(json_payload), offset, action, true);

  len = snprintf(json_payload + offset, sizeof(json_payload) - offset, "}");
  if (len > 0) offset += len;

	log_i("size %u, %s", strlen(json_payload), json_payload);

  ESPUI.WebSocket()->textAll(json_payload, strlen(json_payload));
}

/*****************************************************************************/
	
void enableActionsTable(bool enable) {

	char json_payload[256];

	snprintf(
		json_payload, sizeof(json_payload), 
		"{\"type\":\"EXEC_JS\",\"code\":\" setTableDisabled('id%u', %s); \"}", 
		actions_table_label_2, enable ? "false" : "true");

	log_i("size %u, %s", strlen(json_payload), json_payload);

	ESPUI.WebSocket()->textAll(json_payload, strlen(json_payload));
}

/*****************************************************************************/

void addIrCodesTableJsonRow(
  char *buffer, size_t max_size, size_t &offset, int16_t slot_id,
  z2s_ir_code_t &ir_code, bool is_first) {

  char preview_buf[68] = {};
  strncpy(preview_buf, ir_code.ir_code_base64, 60);
  if (strlen(ir_code.ir_code_base64) > 60) {
    strcat(preview_buf, "...");
  }

  appendToBuffer(
    buffer, max_size, offset,
    "%s[\"#%03d\",\"", is_first ? "" : ",", slot_id);
  appendEscapedJsonString(buffer, max_size, offset, ir_code.ir_code_name);
  appendToBuffer(buffer, max_size, offset, "\",\"");
  appendEscapedJsonString(buffer, max_size, offset, preview_buf);
  appendToBuffer(buffer, max_size, offset, "\"]");
}

/*****************************************************************************/

void buildIrCodesTableJsonPayload(
  uint16_t ui_label_id, int target_ir_code_index) {

  const size_t MAX_JSON_SIZE = 2048;
  char json_payload[MAX_JSON_SIZE] = {};
  const int BATCH_SIZE = 10;
  size_t offset = 0;

  int total_codes = irCodesStorage.getCount();

  if (total_codes == 0) {
    current_ir_code_id = -1;
    current_ir_code_counter = -1;
    snprintf(
      json_payload, MAX_JSON_SIZE,
      "{\"type\":\"update_action_table\",\"is_ir\":true,\"target\":\"id%d\","
      "\"s\":-1,\"append\":false,\"start_idx\":0,\"d\":[]}",
      ui_label_id);
    ESPUI.WebSocket()->textAll(json_payload, strlen(json_payload));
    return;
  }

  int16_t next_slot = irCodesStorage.findNext(0);
  z2s_ir_code_t ir_code = {};

  for (int i = 0; i < total_codes; i++) {
    if (i % BATCH_SIZE == 0) {
      offset = 0;
      bool is_appending = (i > 0);
      appendToBuffer(
        json_payload, MAX_JSON_SIZE, offset,
        "{\"type\":\"update_action_table\",\"is_ir\":true,\"target\":\"id%d\","
        "\"s\":%d,\"append\":%s,\"start_idx\":%d,\"d\":[",
        ui_label_id, target_ir_code_index, is_appending ? "true" : "false", i);
    }

    bool is_first_in_batch = (i % BATCH_SIZE == 0);
    irCodesStorage.load(next_slot, ir_code);

    addIrCodesTableJsonRow(
      json_payload, MAX_JSON_SIZE, offset, next_slot, ir_code, is_first_in_batch);

    next_slot = irCodesStorage.findNext(next_slot + 1);

    if (((i + 1) % BATCH_SIZE == 0) || (i == total_codes - 1)) {
      appendToBuffer(json_payload, MAX_JSON_SIZE, offset, "]}");
      ESPUI.WebSocket()->textAll(json_payload, strlen(json_payload));
    }
  }
}

/*****************************************************************************/

void sendIrCodeRowEdit(uint16_t ui_label_id, int target_row_index) {

  char json_payload[512] = {};
  size_t offset = 0;

  appendToBuffer(
    json_payload, sizeof(json_payload), offset,
    "{\"type\":\"edit_action_row\",\"target\":\"id%d\",\"idx\":%d,\"d\":",
    ui_label_id, target_row_index);

  z2s_ir_code_t ir_code = {};
  irCodesStorage.load(current_ir_code_id, ir_code);

  addIrCodesTableJsonRow(
    json_payload, sizeof(json_payload), offset, current_ir_code_id, ir_code, 
		true);

  appendToBuffer(json_payload, sizeof(json_payload), offset, "}");
  ESPUI.WebSocket()->textAll(json_payload, strlen(json_payload));
}
/*****************************************************************************/

bool buildSelectJsonPreamble(
  char *json_payload, size_t json_payload_size, size_t &offset, bool clear,
  int32_t selected_value, uint8_t selectors_number,
  const uint16_t *selector_ids) {

	if (!appendToBuffer(json_payload, json_payload_size, offset,
	                    update_select_start_fmt, clear ? "true" : "false", selected_value)) {
		return false;
	}

	for (size_t i = 0; i < selectors_number; i++) {

		uint16_t select_index = selector_ids[i];

		if (select_index == 0xFFFF)
			continue;

		if (!appendToBuffer(json_payload, json_payload_size, offset,
		                    "\"select%u\",", select_index))
			return false;
	}

	offset--;

	if (!appendToBuffer(json_payload, json_payload_size, offset,
	                    "],\"options\":["))
		return false;

	return true;
}

/*****************************************************************************/

inline void extractSelectorIds(
  uint8_t count, va_list args, uint16_t *out_ids) {

	for (size_t i = 0; i < count; i++) {
		out_ids[i] = (uint16_t)va_arg(args, unsigned int);
	}
}

/*****************************************************************************/

static bool appendEscapedJsonString(
  char *buf, size_t bufSize, size_t &offset, const char *str) {

	if (!str) return true;

	while (*str) {

		if (offset + 2 >= bufSize) {

			return false;
		}

		if (*str == '"' || *str == '\\') {

			buf[offset++] = '\\';
			buf[offset++] = *str;
		} else if (*str == '\n') {

			buf[offset++] = '\\';
			buf[offset++] = 'n';
		} else if (*str == '\r') {

			buf[offset++] = '\\';
			buf[offset++] = 'r';
		} else if (*str == '\t') {

			buf[offset++] = '\\';
			buf[offset++] = 't';
		} else {
			buf[offset++] = *str;
		}
		str++;
	}

	buf[offset] = '\0';
	return true;
}

/*****************************************************************************/

template<typename YieldFunc>

bool buildSelectJsonPayloadGeneric(
  int32_t selected_value, uint8_t selectors_number,
  const uint16_t *selector_ids, const char *default_prompt,
  YieldFunc yield_options) {

	char json_payload[1024];
	const size_t json_payload_size = sizeof(json_payload);
	size_t offset = 0;


	if (!buildSelectJsonPreamble(
	      json_payload, json_payload_size, offset, true, selected_value,
	      selectors_number, selector_ids)) {
		return false;
	}

	size_t options_added_number = 0;

	auto append_entry = [&](int32_t id, const char *name) -> bool {
		if ((json_payload_size - offset) < 128) {

			if (options_added_number > 0) offset--;

			if (!appendToBuffer(json_payload, json_payload_size, offset, "]}")) {
				return false;
			}

			log_i("size %u, %s", strlen(json_payload), json_payload);

			ESPUI.WebSocket()->textAll(json_payload, strlen(json_payload));

			options_added_number = 0;
			offset = 0;

			if (!buildSelectJsonPreamble(
			      json_payload, json_payload_size, offset, false, selected_value,
			      selectors_number, selector_ids)) {
				return false;
			}
		}

		if (!appendToBuffer(
		      json_payload, json_payload_size, offset, "[\"%ld\",\"", (long)id)) {
			return false;
		}

		if (!appendEscapedJsonString(
		      json_payload, json_payload_size, offset, name ? name : "")) {
			return false;
		}

		if (!appendToBuffer(
		      json_payload, json_payload_size, offset, "\"],")) {
			return false;
		}

		options_added_number++;
		return true;
	};

	if (default_prompt) {
		if (!append_entry(-1, default_prompt)) return false;
	}

	if (!yield_options(append_entry)) {
		return false;
	}

	if (options_added_number > 0) {
		offset--;

		if (!appendToBuffer(json_payload, json_payload_size, offset, "]}")) {
			return false;
		}

		log_i("size %u, %s", strlen(json_payload), json_payload);

		ESPUI.WebSocket()->textAll(json_payload, strlen(json_payload));
	}

	return true;
}

/*****************************************************************************/

bool buildChannelsSelectJsonPayload(
  bool switchbot_select, int32_t selected_value, uint8_t selectors_number,
  ...) {

	uint16_t selector_ids[selectors_number];
	va_list args;
	va_start(args, selectors_number);
	extractSelectorIds(selectors_number, args, selector_ids);
	va_end(args);

	const char *prompt = switchbot_select ? "Select SwitchBot channel..." : "Select Supla channel...";

	return buildSelectJsonPayloadGeneric(
	  selected_value, selectors_number, selector_ids, prompt,
	  [&](auto add_channel_option) {
		  for (auto *z2s_core : Z2S_Cores) {
			  if (!switchbot_select || z2s_core->isSwitchBot()) {

				  if (!add_channel_option(
				        z2s_core->getZ2SChannelIndex(), z2s_core->getZ2SChannelName()))
					  return false;
			  }
		  }

		  if (!switchbot_select) {
			  if (!add_channel_option(
			        GATEWAY_EVENTS_CHANNEL_INDEX, gateway_events_channel_name))
				  return false;
		  }

		  return true;
	  });
}

/*****************************************************************************/

enum ZbDeviceSelectType {
	ZB_DEVICE_SELECT_TYPE_ALL,
	ZB_DEVICE_SELECT_TYPE_AD,
	ZB_DEVICE_SELECT_TYPE_TCC,
	ZB_DEVICE_SELECT_TYPE_IR
};

/*****************************************************************************/

bool buildZbDevicesSelectJsonPayload(
  ZbDeviceSelectType select_type, int32_t selected_value,
  uint8_t selectors_number, ...) {

	uint16_t selector_ids[selectors_number];
	va_list args;
	va_start(args, selectors_number);
	extractSelectorIds(selectors_number, args, selector_ids);
	va_end(args);

		return buildSelectJsonPayloadGeneric(
			selected_value, selectors_number, selector_ids, "Select device...",
			[&](auto add_device_option) {
				
				for (Z2S_Device *device : Z2S_Devices) {

					if (device->getRecordId() && 
    					((select_type == ZB_DEVICE_SELECT_TYPE_ALL) || 
     					 (select_type == ZB_DEVICE_SELECT_TYPE_AD && 
							 	device->isAdvancedDevice()) ||
     					 (select_type == ZB_DEVICE_SELECT_TYPE_TCC && 
							 	device->hasTuyaCustomCluster()) ||
     					 (select_type == ZB_DEVICE_SELECT_TYPE_IR && 
							 	device->isIrRemote()))) {

						if (!add_device_option(device->getDeviceIndex(), 
									device->getDeviceLocalName())) return false;
					}
				}
				return true;
  	});
}

/*****************************************************************************/

bool buildEventsSelectJsonPayload(
  int32_t selected_value, uint16_t selector_id) {

	return buildSelectJsonPayloadGeneric(
	  selected_value, 1, &selector_id, "Select source event...",
	  [&](auto add_event_option) {
		  for (const auto &evt : Supla_events) {

			  int event_id = evt.is_condition ? evt.Supla_event_id + 0xFFFF : 
					evt.Supla_event_id;

			  if (!add_event_option(event_id, evt.Supla_event_name)) return false;
		  }
		  return true;
	  });
}

/*****************************************************************************/

bool buildActionsSelectJsonPayload(
  int32_t selected_value, uint16_t selector_id) {


	return buildSelectJsonPayloadGeneric(
	  selected_value, 1, &selector_id,"Select destination action...", 
		[&](auto add_action_option) {
		  for (const auto &act : Supla_actions) {
			  if (!add_action_option(act.Supla_action_id, act.Supla_action_name))
				  return false;
		  }
		  return true;
	  });
}

/*****************************************************************************/

bool buildClustersSelectJsonPayload(
  int32_t selected_value, uint16_t selector_id) {

	return buildSelectJsonPayloadGeneric(
	  selected_value, 1, &selector_id,"Select ZigBee cluster...", 
		[&](auto add_cluster_option) {
		  for (const auto &zb_cluster : zigbee_clusters) {
			  if (!add_cluster_option(
			        zb_cluster.zigbee_cluster_id, zb_cluster.zigbee_cluster_name))
				  return false;
		  }
		  return true;
	  });
}

/*****************************************************************************/

bool buildAttributesSelectJsonPayload(
  int32_t selected_value, uint16_t selector_id) {

	return buildSelectJsonPayloadGeneric(
	  selected_value, 1, &selector_id, "Select cluster attribute/command...", 
		[&](auto add_attribute_option) {
		  if (current_cluster_id == -1)
			  return true;

		  for (const auto &zb_attribute : zigbee_attributes) {

			  if (zb_attribute.zigbee_attribute_cluster_id == current_cluster_id) {

				  uint32_t attribute_position = &zb_attribute - zigbee_attributes;

				  if (!add_attribute_option(
				        attribute_position, zb_attribute.zigbee_attribute_name))
					  return false;
			  }
		  }
		  return true;
	  });
}

/*****************************************************************************/

bool buildTuyaDatapointsSelectJsonPayload(
  int32_t selected_value, uint8_t selectors_number, ...) {

	uint16_t selector_ids[selectors_number];
	va_list args;
	va_start(args, selectors_number);
	extractSelectorIds(selectors_number, args, selector_ids);
	va_end(args);

	return buildSelectJsonPayloadGeneric(
    selected_value, selectors_number, selector_ids,
    "Select predefined datapoint...", [&](auto add_Tuya_datapoint_option) {
      if (current_Tuya_device_id == -1)
        return true;

      Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(
				current_Tuya_device_id);

      if (!device)
        return true;

      uint32_t z2s_current_device_desc_id = device->getDeviceDescId();

      for (const auto &Tuya_datapoint : Tuya_datapoints) {

        if (Tuya_datapoint.z2s_device_desc_id == z2s_current_device_desc_id) {

          uint32_t datapoint_position = &Tuya_datapoint - Tuya_datapoints;
          if (!add_Tuya_datapoint_option(
                datapoint_position, Tuya_datapoint.Tuya_datapoint_name))
            return false;
        }
      }
      return true;
    });
}

/*****************************************************************************/

bool buildAttributeValuesSelectJsonPayload(
  int32_t selected_value, uint16_t selector_id) {


	return buildSelectJsonPayloadGeneric(
	  selected_value, 1, &selector_id,"Select attribute/command value...", 
		[&](auto add_value_option) {
		  if ((current_cluster_id == -1) || (current_attribute_idx == -1))
			  return true;

		  uint32_t current_attribute_id = zigbee_attributes[
					current_attribute_idx].zigbee_attribute_id;

		  for (const auto &zb_attribute_value : zigbee_attribute_values) {

			  if ((zb_attribute_value.zigbee_cluster_id == current_cluster_id) && 
						(zb_attribute_value.zigbee_attribute_id == 
							current_attribute_id)) {

				  uint32_t value_position = &zb_attribute_value - 
						zigbee_attribute_values;
				  if (!add_value_option(
				        value_position,
				        zb_attribute_value.zigbee_attribute_value_name))
					  return false;
			  }
		  }
		  return true;
	  });
}

/*****************************************************************************/

bool buildRemoteThermometerLocalFunctionsSelectJsonPayload(
  int32_t selected_value, uint16_t selector_id) {

	return buildSelectJsonPayloadGeneric(
	  selected_value, 1, &selector_id,
	  "Select connected local channel function...", [&](auto add_function_option) {
		  for (uint8_t ctf = 1; ctf < 4; ctf++) {

			  if (!add_function_option(
							ctf, CONNECTED_THERMOMETERS_FUNCTION_NAMES[ctf - 1]))
				  return false;
		  }
		  return true;
	  });
}
/*****************************************************************************/

bool buildCalculatedThermometerLocalFunctionsSelectJsonPayload(
  int32_t selected_value, uint16_t selector_id) {

	return buildSelectJsonPayloadGeneric(
	  selected_value, 1, &selector_id,
	  "Select local channel function...", [&](auto add_function_option) {		

		  for (uint8_t ctf = 1; ctf < 4; ctf++) {

			  if (!add_function_option(
							ctf, calculated_thermomether_functions_names[ctf-1]))
				  return false;
		  }
		  return true;
	  });
}

/*****************************************************************************/

void sortChannelsSelectors(int32_t selected_value) {

	addGUICommand(gui_cmd_sort_channels_selectors, selected_value);
}

/*****************************************************************************/

void sortChannelsSelectorsMain(int32_t select_value = -1) {

	if ((channel_selector < 0xFFFF) || 
			(action_source_channel_selector < 0xFFFF) || 
			(action_destination_channel_selector < 0xFFFF)) {

		buildChannelsSelectJsonPayload(
		  false, select_value, 3, channel_selector,
		  action_source_channel_selector, action_destination_channel_selector);
	}

	if (sb_channel_selector < 0xFFFF) {

		buildChannelsSelectJsonPayload(true, select_value, 1, sb_channel_selector);
	}
}

/*****************************************************************************/

void sortZbDevicesSelectors(int32_t selected_value) {

	addGUICommand(gui_cmd_sort_zbdevices_selectors, selected_value);
}

/*****************************************************************************/

void sortZbDevicesSelectorsMain(int32_t select_value = -1) {

	if ((device_selector < 0xFFFF) || 
			(clusters_attributes_table[clusters_attributes_device_selector] < 
				0xFFFF)) {

		buildZbDevicesSelectJsonPayload(
		  ZB_DEVICE_SELECT_TYPE_ALL, select_value, 2, device_selector,
		  clusters_attributes_table[clusters_attributes_device_selector]);
	}

	if (advanced_device_selector < 0xFFFF) {

		buildZbDevicesSelectJsonPayload(
		  ZB_DEVICE_SELECT_TYPE_AD, select_value, 1, advanced_device_selector);
	}

	if (Tuya_devices_tab_controls_table[Tuya_device_selector] < 0xFFFF) {

		buildZbDevicesSelectJsonPayload(
		  ZB_DEVICE_SELECT_TYPE_TCC, select_value, 1,
		  Tuya_devices_tab_controls_table[Tuya_device_selector]);
	}

	if (ir_device_selector < 0xFFFF) {
		buildZbDevicesSelectJsonPayload(
		  ZB_DEVICE_SELECT_TYPE_IR, select_value, 1, ir_device_selector);
	}
}

/*****************************************************************************/

void sortActionsEventsSelectorsMain(int32_t select_value = -1) {

	if (action_event_selector < 0xFFFF)
		buildEventsSelectJsonPayload(select_value, action_event_selector);

	if (action_action_selector < 0xFFFF)
		buildActionsSelectJsonPayload(select_value, action_action_selector);
}

/*****************************************************************************/

void sortClustersSelectorsMain(int32_t select_value = -1) {

	if (clusters_attributes_table[device_cluster_selector] < 0xFFFF)
		buildClustersSelectJsonPayload(
		  select_value, clusters_attributes_table[device_cluster_selector]);
}

/*****************************************************************************/

void sortAttributesSelectors() {

	addGUICommand(gui_cmd_sort_attributes_selectors);
}

/*****************************************************************************/

void sortAttributesSelectorsMain(int32_t select_value = -1) {

	if (clusters_attributes_table[device_attribute_id_selector] < 0xFFFF)
		buildAttributesSelectJsonPayload(
		  select_value,
		  clusters_attributes_table[device_attribute_id_selector]);
}

/*****************************************************************************/

void sortAttributeValuesSelectors() {


	addGUICommand(gui_cmd_sort_attribute_values_selectors);
}

/*****************************************************************************/

void sortAttributeValuesSelectorsMain(int32_t select_value = -1) {

	if (clusters_attributes_table[device_attribute_value_selector] < 0xFFFF)
		buildAttributeValuesSelectJsonPayload(
		  select_value,
		  clusters_attributes_table[device_attribute_value_selector]);
}

/*****************************************************************************/

void sortZbDevicesAndChannelsSelectors() {

	addGUICommand(gui_cmd_sort_zbdevices_and_channels_selectors);
}

/*****************************************************************************/

void sortTuyaDatapointsSelectors(int32_t selected_value) {

	addGUICommand(gui_cmd_sort_Tuya_datapoints_selectors, selected_value);
}

/*****************************************************************************/

void sortTuyaDatapointsSelectorsMain(int32_t select_value = -1) {

	if (Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector] < 0xFFFF)
		buildTuyaDatapointsSelectJsonPayload(
		  select_value, 1,
		  Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector]);
}

/*****************************************************************************/

void sortRemoteThermometerFunctionsSelectorsMain(int32_t select_value = -1) {

	if (local_channel_function_selector < 0xFFFF)
		buildRemoteThermometerLocalFunctionsSelectJsonPayload(
		  select_value, local_channel_function_selector);
}

/*****************************************************************************/

void sortCalculatedThermometerFunctionsSelectorsMain(
	int32_t select_value = -1) {

	if (local_channel_function_selector < 0xFFFF)
		buildCalculatedThermometerLocalFunctionsSelectJsonPayload(
		  select_value, local_channel_function_selector);
}

/*****************************************************************************/

void buildActionsTabGUI() {

	char general_purpose_gui_buffer[512] = {};

	char *working_str_ptr = "Local actions";
	auto actions_tab = ESPUI.addControl(
	  Control::Type::Tab, empty_str, working_str_ptr, Control::Color::Dark,
	  Control::noParent);

	z2s_channel_action_t new_action = {};

	working_str = "No actions";

	int16_t first_action = Z2S_findNextActionPosition(0);
	if (first_action >= 0) {

		current_action_id = first_action;
		current_action_counter = Z2S_getActionCounter(first_action);

		Z2S_loadAction(first_action, new_action);
	}
	
	actions_table_label_2 = ESPUI.addControl(
	  Control::Type::Label, "Table label", empty_str, Control::Color::Dark,
	  actions_tab);

	ESPUI.setPanelWide(actions_table_label_2, true);

	action_new_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "NEW ACTION", Control::Color::Dark,
	  actions_tab, actionsTableCallback, (void *)GUI_CB_ACTION_NEW_FLAG);

	action_edit_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "EDIT ACTION",
	  Control::Color::Dark, action_new_button, actionsTableCallback,
	  (void *)GUI_CB_ACTION_EDIT_FLAG);

	action_copy_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "COPY ACTION", Control::Color::Dark,
	  action_new_button, actionsTableCallback, (void *)GUI_CB_ACTION_COPY_FLAG);

	action_save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "SAVE ACTION", Control::Color::Dark,
	  action_new_button, actionsTableCallback, (void *)GUI_CB_ACTION_SAVE_FLAG);

	action_cancel_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "CANCEL CHANGES",
	  Control::Color::Dark, action_new_button, actionsTableCallback,
	  (void *)GUI_CB_ACTION_CANCEL_FLAG);

	action_remove_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "DELETE ACTION", Control::Color::Dark,
	  action_new_button, actionsTableCallback, (void *)GUI_CB_ACTION_REMOVE_FLAG);

	action_state_label = ESPUI.addControl(
	  Control::Type::Label, empty_str, three_dots_str, Control::Color::Dark,
	  action_new_button);

	ESPUI.setPanelWide(action_new_button, true);

	action_details_label = ESPUI.addControl(
	  Control::Type::Label, "Action details", empty_str, Control::Color::Dark,
	  actions_tab);

	ESPUI.setElementStyle(action_details_label, clearLabelStyle);

	action_enabled_switcher = ESPUI.addControl(
	  Control::Type::Switcher, "action enabled" , (long int)0,
	  Control::Color::Dark, action_details_label, generalCallback);

	ESPUI.setElementStyle(
	  action_enabled_switcher, "width: calc(20% - 10px); min-width: "
		"80px; margin: 0 5px 10px 5px;");

	action_name_text = ESPUI.addControl(
	  Control::Type::Text, "action name (max 32 characters)", emptyString,
	  Control::Color::Dark, action_details_label, generalCallback);

	ESPUI.getControl(action_name_text)->reserve(33);

	ESPUI.setElementStyle(
	  action_name_text, "width: calc(40% - 10px); min-width: "
		"220px; margin: 0 5px 10px 5px;");

	action_description_text = ESPUI.addControl(
	  Control::Type::Text, "action short description (max 127 characters)", 
		emptyString, Control::Color::Dark, action_details_label, generalCallback);

	ESPUI.getControl(action_description_text)->reserve(128);

	ESPUI.setElementStyle(
	  action_description_text, "width: calc(40% - 10px); min-width: "
		"220px; margin: 0 5px 10px 5px;");

	action_source_channel_selector = ESPUI.addControl(
	  Control::Type::Select, "Source Supla channel", (long int)-1, 
		Control::Color::Dark, action_details_label, generalCallback);

	ESPUI.setElementStyle(
	  action_source_channel_selector, "width: calc(25% - 10px); min-width: "
		"220px; margin: 0 5px 10px 5px;");

	action_event_selector = ESPUI.addControl(
	  Control::Type::Select, "Supla event", (long int)-1, Control::Color::Dark,
	  action_details_label, generalCallback);

	ESPUI.setElementStyle(
	  action_event_selector, "width: calc(25% - 10px); min-width: "
		"220px; margin: 0 5px 10px 5px;");

	action_destination_channel_selector = ESPUI.addControl(
	  Control::Type::Select, "Destination Supla channel", (long int)-1, 
		Control::Color::Dark, action_details_label, generalCallback);

	ESPUI.setElementStyle(
	  action_destination_channel_selector, "width: calc(25% - 10px); min-width: "
		"220px; margin: 0 5px 10px 5px;");

	action_action_selector = ESPUI.addControl(
	  Control::Type::Select, "Supla action", (long int)-1, Control::Color::Dark,
	  action_details_label, actionSelectorCallback);

	ESPUI.setElementStyle(
	  action_action_selector, "width: calc(25% - 10px); min-width: "
		"220px; margin: 0 5px 10px 5px;");

	action_subaction_number = ESPUI.addControl(
	  Control::Type::Number, "Subaction ID", (long int)0,
	  Control::Color::Dark, action_details_label, generalMinMaxCallback,
	  (void *)100);

	ESPUI.setElementStyle(
	  action_subaction_number, "width: calc(30% - 10px); min-width: "
		"220px; margin: 0 5px 10px 5px;");

	working_str = "0.00";
	action_condition_threshold_1_number = ESPUI.addControl(
	  Control::Type::Text, "condition main threshold value (for =, >, <, ≥, ≤)"
		"<br>enter number ie. 21.5 or -8.0", working_str, Control::Color::Dark,
	  action_details_label, generalCallback);

	ESPUI.setElementStyle(
	  action_condition_threshold_1_number, "width: calc(35% - 10px); min-width: "
		"220px; margin: 0 5px 10px 5px;");

	action_condition_threshold_2_number = ESPUI.addControl(
	  Control::Type::Text, "condition secondary threshold value (only for"
		" between)<br>enter number ie. 21.5 or -8.0", working_str, 
		Control::Color::Dark, action_details_label, generalCallback);
	
	ESPUI.setElementStyle(
	  action_condition_threshold_2_number, "width: calc(35% - 10px); min-width: "
		"220px; margin: 0 5px 10px 5px;");

	ESPUI.setPanelWide(action_details_label, true);

	enableActionDetails(false);
	updateActionButtons();
}

/*****************************************************************************/

/*****************************************************************************/

void enableIrTable(bool enable) {
  
	char json_payload[256];
  snprintf(
    json_payload, sizeof(json_payload),
    "{\"type\":\"EXEC_JS\",\"code\":\" setTableDisabled('id%u', %s); \"}",
    ir_codes_table_label, enable ? "false" : "true");
  ESPUI.WebSocket()->textAll(json_payload, strlen(json_payload));
}

/*****************************************************************************/

void enableIrCodeDetails(bool enable) {

  if (!enable) {
    working_str = empty_str;
    ESPUI.updateText(ir_code_name_text, working_str);
    ESPUI.updateText(ir_code_base64_text, working_str);
  }
  enableControlStyle(ir_code_details_label, enable);
  enableControlStyle(ir_code_name_text, enable);
  enableControlStyle(ir_code_base64_text, enable);
}

/*****************************************************************************/

void updateIrCodeButtons() {

  bool has_device = (current_ir_device_id >= 0);
  bool has_selected_code = (current_ir_code_id >= 0);
  bool is_viewing = (current_ir_gui_state == VIEW_IR_CODE);
  bool is_learning = ir_code_learning_gui_active;

  enableControlStyle(ir_device_selector, is_viewing && !is_learning);

  enableControlStyle(ir_code_new_button, is_viewing && !is_learning);
  enableControlStyle(
    ir_code_edit_button, is_viewing && has_selected_code && !is_learning);
  enableControlStyle(
    ir_code_copy_button, is_viewing && has_selected_code && !is_learning);
  enableControlStyle(
    ir_code_remove_button, is_viewing && has_selected_code && !is_learning);

  enableControlStyle(ir_code_save_button, !is_viewing && !is_learning);
  enableControlStyle(ir_code_cancel_button, !is_viewing || is_learning);

  enableControlStyle(ir_code_learn_button, has_device && !is_learning);
  enableControlStyle(
    ir_code_send_button,
    has_device && is_viewing && has_selected_code && !is_learning);
}

/*****************************************************************************/

void buildIrRemoteTabGUI() {

  int16_t first_ir_code = irCodesStorage.findNext(0);

  if (first_ir_code >= 0) {
    current_ir_code_id = first_ir_code;
    current_ir_code_counter = irCodesStorage.getCounter(first_ir_code);
  }
	
	auto ir_tab = ESPUI.addControl(
    Control::Type::Tab, empty_str, "IR remote devices",
    Control::Color::Dark, Control::noParent);
  
  ir_device_selector = ESPUI.addControl(
    Control::Type::Select, "IR ZigBee devices", (long int)-1,
    Control::Color::Dark, ir_tab, irDeviceSelectorCallback);

  // Panel 2: IR Device Manufacturer / Model Label
  working_str = three_dots_str;
  ir_device_info_label = ESPUI.addControl(
    Control::Type::Label, "IR ZigBee device", working_str,
    Control::Color::Dark, ir_tab);

  // Panel 3: IR Commands Table
  ir_codes_table_label = ESPUI.addControl(
    Control::Type::Label, "IR commands table", empty_str,
    Control::Color::Dark, ir_tab);
  ESPUI.setPanelWide(ir_codes_table_label, true);

  // Panel 4: Action & Learn Buttons
  ir_code_new_button = ESPUI.addControl(
    Control::Type::Button, "IR commands management", "ADD CODE",
    Control::Color::Dark, ir_tab, irCodesTableCallback,
    (void *)GUI_CB_IR_CODE_NEW_FLAG);

  ir_code_edit_button = ESPUI.addControl(
    Control::Type::Button, empty_str, "EDIT CODE",
    Control::Color::Dark, ir_code_new_button, irCodesTableCallback,
    (void *)GUI_CB_IR_CODE_EDIT_FLAG);

  ir_code_copy_button = ESPUI.addControl(
    Control::Type::Button, empty_str, "COPY CODE",
    Control::Color::Dark, ir_code_new_button, irCodesTableCallback,
    (void *)GUI_CB_IR_CODE_COPY_FLAG);

  ir_code_save_button = ESPUI.addControl(
    Control::Type::Button, empty_str, "SAVE CODE",
    Control::Color::Dark, ir_code_new_button, irCodesTableCallback,
    (void *)GUI_CB_IR_CODE_SAVE_FLAG);

  ir_code_cancel_button = ESPUI.addControl(
    Control::Type::Button, empty_str, "CANCEL",
    Control::Color::Dark, ir_code_new_button, irCodesTableCallback,
    (void *)GUI_CB_IR_CODE_CANCEL_FLAG);

  ir_code_remove_button = ESPUI.addControl(
    Control::Type::Button, empty_str, "DELETE CODE",
    Control::Color::Alizarin, ir_code_new_button, irCodesTableCallback,
    (void *)GUI_CB_IR_CODE_REMOVE_FLAG);

  ir_code_learn_button = ESPUI.addControl(
    Control::Type::Button, empty_str, "LEARN NEW CODE",
    Control::Color::Emerald, ir_code_new_button, irCodesTableCallback,
    (void *)GUI_CB_IR_CODE_LEARN_FLAG);

  ir_code_send_button = ESPUI.addControl(
    Control::Type::Button, empty_str, "TEST / SEND CODE",
    Control::Color::Dark, ir_code_new_button, irCodesTableCallback,
    (void *)GUI_CB_IR_CODE_SEND_FLAG);

  ir_code_state_label = ESPUI.addControl(
    Control::Type::Label, empty_str, three_dots_str,
    Control::Color::Dark, ir_code_new_button);

  ESPUI.setPanelWide(ir_code_new_button, true);

  // Panel 5: IR Code Editor (Name + Base64 Textarea)
  ir_code_details_label = ESPUI.addControl(
    Control::Type::Label, "IR command details", empty_str,
    Control::Color::Dark, ir_tab);
  ESPUI.setElementStyle(ir_code_details_label, clearLabelStyle);

  ir_code_name_text = ESPUI.addControl(
    Control::Type::Text, "Command name (max 31 characters)", emptyString,
    Control::Color::Dark, ir_code_details_label, generalCallback);
  ESPUI.getControl(ir_code_name_text)->reserve(32);
  ESPUI.setElementStyle(
    ir_code_name_text,
    "width: calc(30% - 10px); min-width: 200px; margin: 0 5px 10px 5px;");

  ir_code_base64_text = ESPUI.addControl(
    Control::Type::Text, "Base64 IR code (click LEARN NEW CODE or paste Base64)",
    emptyString, Control::Color::Dark, ir_code_details_label, generalCallback);
  ESPUI.getControl(ir_code_base64_text)->reserve(IR_CODE_BASE64_MAX_SIZE);

  BasicControl *ir_base64_textarea = ESPUI.getControl(ir_code_base64_text);
  ir_base64_textarea->control_flags |= CONTROL_FLAG_TEXTAREA;
  ir_base64_textarea->rows = 4;
  ir_base64_textarea->cols = 64;

  ESPUI.setElementStyle(
    ir_code_base64_text,
    "resize: none; width: calc(70% - 10px); min-width: 260px; "
    "margin: 0 5px 10px 5px;");

  ESPUI.setPanelWide(ir_code_details_label, true);

  enableIrCodeDetails(false);
  updateIrCodeButtons();
}

/*****************************************************************************/

void Z2S_initWebGUI() {

	clusters_attributes_table[clusters_attributes_device_selector] = 0xFFFF;
	clusters_attributes_table[device_attribute_id_selector] = 0xFFFF;
	clusters_attributes_table[device_attribute_value_selector] = 0xFFFF;

	Tuya_devices_tab_controls_table[Tuya_device_selector] = 0xFFFF;
	Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector] = 0xFFFF;
}


uint16_t Z2S_getGUIBuildControlFlags(gui_modes_t mode) {

	uint16_t gui_build_control_flags = 0x0000;

	switch (mode) {


		case no_gui_mode:

			gui_build_control_flags = 0x0000;
			break;


		case minimal_gui_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_CREDENTIALS | GUI_BUILD_CONTROL_FLAG_ZIGBEE;
			}
			break;


		case standard_gui_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_CREDENTIALS | GUI_BUILD_CONTROL_FLAG_ZIGBEE | GUI_BUILD_CONTROL_FLAG_DEVICES | GUI_BUILD_CONTROL_FLAG_CHANNELS | GUI_BUILD_CONTROL_FLAG_ACTIONS | GUI_BUILD_CONTROL_FLAG_PO;
			}
			break;


		case extended_gui_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_CREDENTIALS | GUI_BUILD_CONTROL_FLAG_ZIGBEE | GUI_BUILD_CONTROL_FLAG_DEVICES | GUI_BUILD_CONTROL_FLAG_CHANNELS | GUI_BUILD_CONTROL_FLAG_ACTIONS | GUI_BUILD_CONTROL_FLAG_PO | GUI_BUILD_CONTROL_FLAG_CA;
			}
			break;


		case full_gui_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_CREDENTIALS | GUI_BUILD_CONTROL_FLAG_ZIGBEE | GUI_BUILD_CONTROL_FLAG_DEVICES | GUI_BUILD_CONTROL_FLAG_CHANNELS | GUI_BUILD_CONTROL_FLAG_ACTIONS | GUI_BUILD_CONTROL_FLAG_PO | GUI_BUILD_CONTROL_FLAG_CA | GUI_BUILD_CONTROL_FLAG_TCC;
			}
			break;


		case full_ad_gui_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_CREDENTIALS | GUI_BUILD_CONTROL_FLAG_ZIGBEE | GUI_BUILD_CONTROL_FLAG_DEVICES | GUI_BUILD_CONTROL_FLAG_CHANNELS | GUI_BUILD_CONTROL_FLAG_ACTIONS | GUI_BUILD_CONTROL_FLAG_PO | GUI_BUILD_CONTROL_FLAG_CA | GUI_BUILD_CONTROL_FLAG_AD | GUI_BUILD_CONTROL_FLAG_TCC;
			}
			break;


		case full_sb_gui_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_CREDENTIALS | GUI_BUILD_CONTROL_FLAG_ZIGBEE | GUI_BUILD_CONTROL_FLAG_DEVICES | GUI_BUILD_CONTROL_FLAG_CHANNELS | GUI_BUILD_CONTROL_FLAG_ACTIONS | GUI_BUILD_CONTROL_FLAG_CA | GUI_BUILD_CONTROL_FLAG_SB | GUI_BUILD_CONTROL_FLAG_TCC;
			}
			break;


		case developer_gui_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_ZIGBEE | GUI_BUILD_CONTROL_FLAG_CA | GUI_BUILD_CONTROL_FLAG_AD | GUI_BUILD_CONTROL_FLAG_TCC;
			}
			break;


		case supla_gui_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_CREDENTIALS | GUI_BUILD_CONTROL_FLAG_CHANNELS | GUI_BUILD_CONTROL_FLAG_ACTIONS;
				GUI_BUILD_CONTROL_FLAG_PO;
			}
			break;


		case gateway_ca_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_ZIGBEE | GUI_BUILD_CONTROL_FLAG_CA;
			}
			break;


		case gateway_tcc_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_ZIGBEE | GUI_BUILD_CONTROL_FLAG_TCC;
			}
			break;


		case gateway_ca_tcc_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_ZIGBEE | GUI_BUILD_CONTROL_FLAG_CA | GUI_BUILD_CONTROL_FLAG_TCC;
			}
			break;


		case gateway_devices_channels_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_ZIGBEE | GUI_BUILD_CONTROL_FLAG_DEVICES | GUI_BUILD_CONTROL_FLAG_CHANNELS;
			}
			break;


		case gateway_no_local_actions_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_ZIGBEE | GUI_BUILD_CONTROL_FLAG_DEVICES | GUI_BUILD_CONTROL_FLAG_CHANNELS | GUI_BUILD_CONTROL_FLAG_CA | GUI_BUILD_CONTROL_FLAG_TCC;
			}
			break;


		case standard_ad_gui_mode:
			{

				gui_build_control_flags =
				  GUI_BUILD_CONTROL_FLAG_GATEWAY | GUI_BUILD_CONTROL_FLAG_CREDENTIALS | GUI_BUILD_CONTROL_FLAG_ZIGBEE | GUI_BUILD_CONTROL_FLAG_DEVICES | GUI_BUILD_CONTROL_FLAG_CHANNELS | GUI_BUILD_CONTROL_FLAG_ACTIONS | GUI_BUILD_CONTROL_FLAG_PO | GUI_BUILD_CONTROL_FLAG_AD;
			}
			break;
	}
	return gui_build_control_flags;
}


void Z2S_buildWebGUI(gui_modes_t mode, uint32_t gui_custom_flags) {

	if (GUIbuilt) {

		log_i(" GUI already built...");

		return;
	}

	log_i(" GUI building STARTED...");

	log_i(
	  "sizeof(BasicControl) %u, sizeof(Control) %u", sizeof(BasicControl),
	  sizeof(Control));

	ESPUI.sliderContinuous = true;
	//ESPUI.setVerbosity(Verbosity::VerboseJSON);
	ESPUI.captivePortal = false;

	gui_commands_queue = xQueueCreate(
	  GUI_COMMANDS_POOL_SIZE, sizeof(gui_command_object *));

	working_str.reserve(1056);

	uint16_t gui_build_control_flags = 0x000;

	if (gui_custom_flags)
		gui_build_control_flags = gui_custom_flags;
	else
		gui_build_control_flags = Z2S_getGUIBuildControlFlags(mode);

	clusters_attributes_table[clusters_attributes_device_selector] = 0xFFFF;
	Tuya_devices_tab_controls_table[Tuya_device_selector] = 0xFFFF;

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_GATEWAY) {

		buildGatewayTabGUI();
	}

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_CREDENTIALS) {

		buildCredentialsGUI();
	}

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_ZIGBEE) {

		buildZigbeeTabGUI();
	}

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_DEVICES) {

		buildDevicesTabGUI();
	}

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_CHANNELS) {

		buildChannelsTabGUI();
	}

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_ACTIONS) {

		buildActionsTabGUI();
	}

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_PO) {

		buildPushoverTabGUI();
	}

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_CA) {

		buildClustersAttributesTab();
	}

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_AD) {

		buildAdvancedDevicesTabGUI();
	}

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_TCC) {

		buildTuyaCustomClusterTabGUI();
	}

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_SB) {

		buildSwitchBotTabGUI();
	}

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_IR) {
		buildIrRemoteTabGUI();
	}

	//buildAllChannelSelectors();

	log_i(" ...GUI building FINISHED");

	GUIbuilt = true;
	ESPUI.setOnWsConnectCallback(onWsConnectCallback);
	ESPUI.setOnWsCustomCmdCallback(onWsCustomCmdCallback);
}



void Z2S_startWebGUI() {

	log_i("STARTING WEB GUI");

	esp_log_level_set("async_ws", ESP_LOG_ERROR);

	ESPUI.setCustomJS(myCustomJS);
	//ESPUI.setVerbosity(Verbosity::VerboseJSON);

	ESPUI.begin("ZIGBEE <=> SUPLA CONTROL PANEL");
	GUIstarted = true;
	handleGatewayEvent(Z2S_SUPLA_EVENT_ON_GUI_STARTED);

	if (ESPUI.WebServer())
		ESPUI.WebServer()->on("/zigbee_ota", HTTP_GET, [](AsyncWebServerRequest *request) {
			static const char *html =
			  "<!DOCTYPE HTML><html><head><title>ZigBee OTA file upload</title>"
			  "<meta name='viewport' content='width=device-width, initial-scale=1'>"
			  "<style>body{font-family:Arial; margin:20px;} th, td{padding:10px; text-align:left;}"
			  " a{color:white;}</style></head><body>"
			  "<h2>ZigBee OTA file upload</h2>"
			  "<section style='background:#f4f4f4; padding:15px;'>"
			  "<strong>Upload File:</strong><br><br>"
			  "<input type='file' id='file-input'><br><br>"
			  "<button class='btn' onclick='uploadFile()'>Upload Now</button>"
			  "<div class='progress-wrapper' id='p-wrap'><div id='progress-bar'>0%</div></div>"
			  "<p id='status'></p></section><br>"
			  "<script>"
			  "function uploadFile() {"
			  "  var file = document.getElementById('file-input').files[0];"
			  "  if(!file) { alert('Select a file first!'); return; }"
			  "  var formData = new FormData();"
			  "  formData.append('data', file);"
			  "  var xhr = new XMLHttpRequest();"
			  "  document.getElementById('p-wrap').style.display = 'block';"
			  "  xhr.upload.addEventListener('progress', function(e) {"
			  "    var percent = (e.loaded / e.total) * 100;"
			  "    var bar = document.getElementById('progress-bar');"
			  "    bar.style.width = Math.round(percent) + '%';"
			  "    bar.innerHTML = Math.round(percent) + '%';"
			  "  });"
			  "  xhr.onreadystatechange = function() {"
			  "    if (xhr.readyState == 4) {"
			  "      if(xhr.status == 200) {"
			  "        document.getElementById('status').innerHTML = "
			  "'<b>Success: </b>' + xhr.responseText;"
			  "        setTimeout(function(){ location.reload(); }, 2000);"
			  "      } else { alert('Upload Failed'); }"
			  "    }"
			  "  };"
			  "  xhr.open('POST', '/upload_zigbee_ota', true);"
			  "  xhr.send(formData);"
			  "}"
			  "</script></body></html>"
			  "<div style='margin-bottom:20px;'>"
			  "<button class='btn' onclick=\"window.location.href='/'\">Back to GUI page</button>"
			  "</div>"
			  "</body></html>";

			request->send(200, "text/html", html);
		});

	if (ESPUI.WebServer())
		ESPUI.WebServer()->on(
		  "/upload_zigbee_ota", HTTP_POST, [](AsyncWebServerRequest *request) {
			  //request->redirect("/"); // Go back to home after upload
			  request->send(200, "text/plain", "Upload OK");
		  },
		  [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
			  //LittleFS.begin(false);
			  Z2S_initLittleFs();

			  if (index)
				  request->_tempFile = LittleFS.open("/zigbee_ota/zigbee_ota_file.ota", "a");
			  else {

				  if (!LittleFS.exists("/zigbee_ota/zigbee_ota_file.ota"))
					  LittleFS.mkdir("/zigbee_ota");

				  request->_tempFile = LittleFS.open("/zigbee_ota/zigbee_ota_file.ota", "w");
			  }

			  if (len) {
				  request->_tempFile.write(data, len);
			  }
			  //if (final) {
			  request->_tempFile.close();
			  //}
			  //LittleFS.end();
		  });

	if (ESPUI.WebServer())
		ESPUI.WebServer()->on("/partition", HTTP_GET, [](AsyncWebServerRequest *request) {
			const AsyncWebParameter *pLabel = request->getParam("label");
			const AsyncWebParameter *pType = request->getParam("type");
			const AsyncWebParameter *pSubtype = request->getParam("subtype");
			const AsyncWebParameter *pRaw = request->getParam("raw");

			if (!pLabel && !pType && !pSubtype) {
				request->send(400, "text/plain", "Bad request: missing parameter");
				return;
			}

			esp_partition_type_t type = ESP_PARTITION_TYPE_ANY;
			esp_partition_subtype_t subtype = ESP_PARTITION_SUBTYPE_ANY;
			const char *label = nullptr;
			bool raw = true;

			if (pLabel) {
				label = pLabel->value().c_str();
			}

			if (pType) {
				type = (esp_partition_type_t)pType->value().toInt();
			}

			if (pSubtype) {
				subtype = (esp_partition_subtype_t)pSubtype->value().toInt();
			}

			if (pRaw && pRaw->value() == "false") {
				raw = false;
			}

			const esp_partition_t *partition = esp_partition_find_first(type, subtype, label);

			if (!partition) {
				request->send(404, "text/plain", "Partition not found");
				return;
			}

			AsyncWebServerResponse *response = request->beginChunkedResponse(
			  "application/octet-stream", [partition, raw](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
				  const size_t remaining = partition->size - index;
				  if (!remaining) {
					  return 0;
				  }
				  const size_t len = std::min(maxLen, remaining);
				  if (raw && esp_partition_read_raw(partition, index, buffer, len) == ESP_OK) {
					  return len;
				  }
				  if (!raw && esp_partition_read(partition, index, buffer, len) == ESP_OK) {
					  return len;
				  }
				  return 0;
			  });

			response->addHeader(
			  "Content-Disposition", "attachment; filename=" + String(partition->label) + ".bin");
			response->setContentLength(partition->size);

			request->send(response);
		});
}

/*****************************************************************************/

void Z2S_stopWebGUI() {

	log_i("STOPPING WEB GUI");

	if (ESPUI.WebServer())
		ESPUI.WebServer()->end();
	GUIstarted = false;
	current_Tuya_payload_label = 0;
}

/*****************************************************************************/

bool Z2S_isGUIStarted() {

	return GUIstarted;
}

/*****************************************************************************/

bool Z2S_isGUIBuilt() {

	return GUIbuilt;
}

/*****************************************************************************/

void onUpdateBegin(const UpdateType type, int &result) {

	rgbLedWrite(RGB_BUILTIN, 255, 0, 255);
}

/*****************************************************************************/

void onUpdateEnd(const UpdateType type, int &result) {

	rgbLedWrite(RGB_BUILTIN, 0, 0, 0);

	if (result == UpdateResult::UPDATE_OK) {

		log_i("OTA update finished successfully - cleaning before reset");

		Zigbee.stop();
		delay(500);

		log_i("software reset - closing LittleFs");

		Z2S_endLittleFs(true);
	}
}

/*****************************************************************************/

void Z2S_startUpdateServer() {

	if (ESPUI.WebServer()) {
		updateServer.setup(ESPUI.WebServer());
		updateServer.onUpdateBegin = onUpdateBegin;
		updateServer.onUpdateEnd = onUpdateEnd;
	}
}

/*****************************************************************************/

void Z2S_updateWebGUI() {

	char memory_info_gui_buffer[768] = {};

	fillMemoryUptimeInformation(memory_info_gui_buffer, 768);

	updateLabel_P(gateway_memory_info, memory_info_gui_buffer);
	//ESPUI.updateLabel(gateway_memory_info, memory_info_gui_buffer);
}

/*****************************************************************************/

void clusterCallbackCmd() {

	current_cluster_id = ESPUI.getControl(
	                            clusters_attributes_table[device_cluster_selector])
	                       ->getValueInt();

	log_i("cluster_id %li", current_cluster_id);

	ESPUI.updateNumber(
	  clusters_attributes_table[device_attribute_id_selector], -1);

	ESPUI.updateNumber(
	  clusters_attributes_table[device_attribute_type_selector], -1);

	ESPUI.updateNumber(
	  clusters_attributes_table[device_attribute_value_selector], -1);

	ESPUI.updateNumber(
	  clusters_attributes_table[device_attribute_size_number], 0);
	ESPUI.updateNumber(clusters_attributes_table[device_attribute_id_text], 0);
	ESPUI.updateNumber(
	  clusters_attributes_table[device_attribute_value_text], 0);

	sortAttributesSelectors();

	attributeCallback(nullptr, -1, nullptr);
}

/*****************************************************************************/

static int16_t previousOtaProgress = -1;

void guiGitHubOtaProgress(size_t current, size_t total) {

	if (total == 0) {

		switch (current) {


			case 0:
				{

					rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
					working_str = "OTA Successful. Rebooting...";
					ESPUI.updateLabel(github_ota_info_label, working_str);
				}
				break;


			case 1:
				{

					rgbLedWrite(RGB_BUILTIN, 255, 0, 255);
					working_str = "Starting OTA from GitHub...";
					ESPUI.updateLabel(github_ota_info_label, working_str);
				}
				break;


			case 2:
				{

					rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
					working_str = "GitHub OTA failed - check logs for error code!";
					ESPUI.updateLabel(github_ota_info_label, working_str);
				}
				break;
		}
		return;
	}

	size_t currentOtaProgress = (current * 100) / total;

	if ((currentOtaProgress % 5 == 0) && (currentOtaProgress != previousOtaProgress)) {

		previousOtaProgress = currentOtaProgress;

		char ota_info_buffer[64];

		sprintf(
		  ota_info_buffer, "GitHub OTA progress: %02u%%", currentOtaProgress);

		working_str = ota_info_buffer;

		ESPUI.updateLabel(github_ota_info_label, working_str);
	}
}

/*****************************************************************************/

void Z2S_loopWebGUI() {

	/*if (new_connect) {

		if (ESPUI.isSynchronized()) {

			log_i("synchronized");
			new_connect = false;
			sortZbDevicesSelectorsMain();
			sortChannelsSelectorsMain();
			sortActionsEventsSelectorsMain();
			sortClustersSelectorsMain();
			sortAttributesSelectorsMain();
			sortAttributeValuesSelectorsMain();
			sortTuyaDatapointsSelectorsMain();
			buildActionsTableJsonPayload(actions_table_label_2, 0);
		} else {

			//log_i("not yet synchronized");
		}
	}*/

	uint32_t local_func = 0;

	gui_command_object *pending_gui_command = nullptr;

	while (xQueueReceive(
	         gui_commands_queue, &pending_gui_command, 0)
	       == pdTRUE) {

		switch (pending_gui_command->gui_command) {


			case gui_cmd_synchronize_all: {

				sortZbDevicesSelectorsMain();
				sortChannelsSelectorsMain();
				sortActionsEventsSelectorsMain();
				sortClustersSelectorsMain();
				sortAttributesSelectorsMain();
				sortAttributeValuesSelectorsMain();
				sortTuyaDatapointsSelectorsMain();
				buildActionsTableJsonPayload(actions_table_label_2, 0);
				buildIrCodesTableJsonPayload(ir_codes_table_label, 0);
			} break;


			case gui_cmd_check_github_ota:
				{

					char response_buffer[128];

					if (checkGitHubForUpdate(response_buffer, sizeof(response_buffer))) {

						enableControlStyle(github_ota_button, true);
						_github_ota_enabled = true;
					}

					working_str = response_buffer;
					ESPUI.updateLabel(github_ota_info_label, working_str);
				}
				break;


			case gui_cmd_perform_github_ota:
				{

					if (_github_ota_enabled)
						startBackgroundGitHubOtaTask(guiGitHubOtaProgress);
				}
				break;


			case gui_cmd_sort_channels_selectors:
				{

					sortChannelsSelectorsMain(pending_gui_command->gui_command_value);
				}
				break;


			case gui_cmd_sort_zbdevices_selectors:
				{

					sortZbDevicesSelectorsMain(pending_gui_command->gui_command_value);
				}
				break;


			case gui_cmd_sort_zbdevices_and_channels_selectors:
				{

					sortZbDevicesSelectorsMain();
					sortChannelsSelectorsMain();
				}
				break;


			case gui_cmd_sort_attributes_selectors:
				{

					sortAttributesSelectorsMain();
				}
				break;


			case gui_cmd_sort_attribute_values_selectors:
				{

					sortAttributeValuesSelectorsMain();
				}
				break;


			case gui_cmd_sort_Tuya_datapoints_selectors:
				{

					sortTuyaDatapointsSelectorsMain();
				}
				break;


			case gui_cmd_edit_actions_table: {

					sendActionRowEdit(
						actions_table_label_2, pending_gui_command->gui_command_value);
					enableActionsTable(true);
			} break;


			case gui_cmd_build_actions_table: {
					
				buildActionsTableJsonPayload(
					actions_table_label_2, pending_gui_command->gui_command_value);
				enableActionsTable(true);
			} break;


			case gui_cmd_edit_channel_data:
				{

					editChannelMain(pending_gui_command->gui_command_value);
				}
				break;


			case gui_cmd_update_channel_info:
				{

					updateChannelInfoLabel(pending_gui_command->gui_command_value);
				}
				break;


			case gui_cmd_update_zbdevice_info:
				{

					updateDeviceInfoLabel(pending_gui_command->gui_command_value);
				}
				break;


			case gui_cmd_cluster_callback:
				{

					clusterCallbackCmd();
				}
				break;


			case gui_cmd_Tuya_device_callback:
				{

					TuyaDeviceSelectorCmd(pending_gui_command->gui_command_value);
				}
				break;


			case gui_cmd_Tuya_custom_cmd_callback:
				{

					TuyaCustomCmdMain(pending_gui_command->gui_command_value);
				}
				break;


			case gui_cmd_get_clusters_attributes_query_callback:
				{

					getClustersAttributesQueryMain(pending_gui_command->gui_command_value);
				}
				break;


			case gui_cmd_add_logic_gate:
				{

					uint8_t logic_operator = PIN_LOGIC_OPERATOR_NONE;

					switch (pending_gui_command->gui_command_value) {


						case GUI_CB_ADD_AND_HANDLER_FLAG:

							logic_operator = PIN_LOGIC_OPERATOR_AND;
							break;


						case GUI_CB_ADD_OR_HANDLER_FLAG:

							logic_operator = PIN_LOGIC_OPERATOR_OR;
							break;


						case GUI_CB_ADD_XOR_HANDLER_FLAG:

							logic_operator = PIN_LOGIC_OPERATOR_XOR;
							break;


						case GUI_CB_ADD_NOT_HANDLER_FLAG:

							logic_operator = PIN_LOGIC_OPERATOR_NOT;
							break;


						case GUI_CB_ADD_NAND_HANDLER_FLAG:

							logic_operator = PIN_LOGIC_OPERATOR_NAND;
							break;


						case GUI_CB_ADD_NOR_HANDLER_FLAG:

							logic_operator = PIN_LOGIC_OPERATOR_NOR;
							break;


						case GUI_CB_ADD_AND3_HANDLER_FLAG:

							logic_operator = PIN_LOGIC_OPERATOR_AND3;
							break;


						case GUI_CB_ADD_OR3_HANDLER_FLAG:

							logic_operator = PIN_LOGIC_OPERATOR_OR3;
							break;


						case GUI_CB_ADD_NOP_HANDLER_FLAG:

							logic_operator = PIN_LOGIC_OPERATOR_NOP;
							break;
					}

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_ACTION_HANDLER, 0, logic_operator);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local logical object has been successfully added and is "
						  "available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_virtual_relay:
				{

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY, 0);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local virtual relay has been successfully added and is "
						  "available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_local_relay:
				{

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_GPIO_RELAY, 0);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local relay has been successfully added and is "
						  "available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_local_binary:
				{

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_GPIO_BINARY, 0);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local binary has been successfully added and is "
						  "available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_local_hvac:
				{

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_GPIO_HVAC, 0);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local hvac has been successfully added and is "
						  "available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_local_ds18b20:
				{

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_GPIO_DS18B20, 0);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local DS18B20 sensor has been successfully added and is "
						  "available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_local_hcsr04:
				{

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_GPIO_HCSR04, 0);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local HC-SR04 sensor has been successfully added and is "
						  "available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_virtual_binary:
				{

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY, 0);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local virtual binary has been successfully added and is "
						  "available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_remote_relay:
				{

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_REMOTE_RELAY, 0);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local remote relay has been successfully added and is "
						  "available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_remote_thermometer:
				{

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER, 
						CONNECTED_THERMOMETERS_FNC_AVG);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local remote thermometer has been successfully added and is "
						  "available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_virtual_hvac:
				{

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_VIRTUAL_HVAC, 0);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local virtual HVAC has been successfully added and is "
						  "available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_virtual_thermhygrometer:
				{

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_VIRTUAL_THERM_HYGRO_METER, 0);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local virtual thermhygrometer has been successfully added and"
						  " is available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_virtual_gpm:
				{

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_VIRTUAL_GPM, 0);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local virtual GPM has been successfully added and"
						  " is available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_calculated_thermometer:
				{

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_CALC_THERMOMETER, 
						CALCULATED_THERMOMETER_FUNC_DEW_POINT);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  lah_status_label,
						  "The local calculated thermometer has been successfully added and"
						  " is available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_add_switchbot_1x:
			case gui_cmd_add_switchbot_2x:
				{

					local_func = (pending_gui_command->gui_command == gui_cmd_add_switchbot_1x) ? 0 : 1;

					int16_t new_channel_slot = addZ2SDeviceLocalActionHandler(
					  LOCAL_CHANNEL_TYPE_SWITCHBOT, local_func);

					if (new_channel_slot >= 0) {

						ESPUI.updateLabel(
						  sb_status_label,
						  "The Switchbot object has been successfully added and is "
						  "available for use.");

						addGUICommand(gui_cmd_sort_channels_selectors, new_channel_slot);
					}
				}
				break;


			case gui_cmd_remove_local_channel:
				{

					removeChannelCallbackCmd();
				}
				break;


			case gui_cmd_save_switchbot_data:
				{

					int16_t channel_slot =
					  ESPUI.getControl(sb_channel_selector)->getValueInt();

					if (channel_slot >= 0) {

						channel_extended_data_sb_t channel_extended_data_sb = {};

						strncpy(
						  channel_extended_data_sb.ble_mac_address,
						  ESPUI.getControl(sb_device_id_text)->getValueCstr(),
						  sizeof(channel_extended_data_sb.ble_mac_address));

						channel_extended_data_sb.token_size = strlen(
						  ESPUI.getControl(sb_token_text)->getValueCstr());
						strncpy(
						  channel_extended_data_sb.token,
						  ESPUI.getControl(sb_token_text)->getValueCstr(),
						  sizeof(channel_extended_data_sb.token));

						channel_extended_data_sb.json_payload_size = strlen(
						  ESPUI.getControl(sb_json_payload_text)->getValueCstr());
						strncpy(
						  channel_extended_data_sb.json_payload,
						  ESPUI.getControl(sb_json_payload_text)->getValueCstr(),
						  sizeof(channel_extended_data_sb.json_payload));

						channel_extended_data_sb.json_payload_2_size = strlen(
						  ESPUI.getControl(sb_json_payload_2_text)->getValueCstr());
						strncpy(
						  channel_extended_data_sb.json_payload_2,
						  ESPUI.getControl(sb_json_payload_2_text)->getValueCstr(),
						  sizeof(channel_extended_data_sb.json_payload_2));

						Z2S_saveChannelExtendedData(
						  channel_slot, CHANNEL_EXTENDED_DATA_TYPE_SB,
						  (uint8_t *)&channel_extended_data_sb);

						auto sbInstance = Z2S_Core::getSwitchBotRelayInstance(channel_slot);
						if (sbInstance)
							sbInstance->updateSwitchBotData(
							  channel_extended_data_sb, SB_UPDATE_DATA_LOAD_DIR);
					}
				}
				break;


			case gui_cmd_ir_device_callback: {
				irDeviceSelectorCmd(pending_gui_command->gui_command_value);
			} break;


			case gui_cmd_build_ir_codes_table: {
				buildIrCodesTableJsonPayload(
				  ir_codes_table_label, pending_gui_command->gui_command_value);
				enableIrTable(true);
				updateIrCodeButtons();
			} break;


			case gui_cmd_edit_ir_codes_table: {
				sendIrCodeRowEdit(
				  ir_codes_table_label, pending_gui_command->gui_command_value);
				enableIrTable(true);
				updateIrCodeButtons();
			} break;


			case gui_cmd_learn_ir_code: {
				int32_t val = pending_gui_command->gui_command_value;
				if (val >= 0) {
					Z2S_Device *dev = Z2S_Device::getZ2SDeviceByDeviceIndex(val);
					if (dev) {
						startLearnIRCode(dev->getDeviceShortAddress(), 1);
					}
				} else {
					int32_t cancel_dev_idx = (-val) - 1;
					Z2S_Device *dev = Z2S_Device::getZ2SDeviceByDeviceIndex(cancel_dev_idx);
					if (dev) {
						cancelLearnIRCode(dev->getDeviceShortAddress(), 1);
					}
				}
			} break;

			case gui_cmd_ir_code_learned: {
				if (!ir_code_learning_gui_active) {
					break; // Ignore late packet if user already cancelled
				}
				ir_code_learning_gui_active = false;

				// Automatically open NEW_IR_CODE if in VIEW mode, or fill textarea directly!
				if (current_ir_gui_state == VIEW_IR_CODE && current_ir_device_id >= 0) {
					new_ir_code_id = irCodesStorage.findFreeIndex();
					if (new_ir_code_id >= 0) {
						current_ir_gui_state = NEW_IR_CODE;
						enableIrCodeDetails(true);
						enableIrTable(false);
					}
				}
				updateIrCodeButtons();
				working_str = getLearnedIrCodeBase64();
				ESPUI.updateText(ir_code_base64_text, working_str);
				ESPUI.updateLabel(
				  ir_code_state_label,
				  "IR code received & decoded! Enter a name and click SAVE CODE.");
			} break;

			case gui_cmd_send_ir_code: {
				z2s_ir_code_t ir_code = {};
				Z2S_Device *dev = Z2S_Device::getZ2SDeviceByDeviceIndex(current_ir_device_id);
				if (dev && irCodesStorage.load(pending_gui_command->gui_command_value, ir_code)) {
					
					sendLearnedIRCode(
					  dev->getDeviceShortAddress(), 1, ir_code.ir_code_base64);
					ESPUI.updateLabel(ir_code_state_label, "IR code transmission started!");
				}
			} break;


			case gui_cmd_delete_ir_code: {
				
				z2s_ir_code_t ir_code = {};
        bool loaded = irCodesStorage.load(current_ir_code_id, ir_code);
        
        if (loaded && irCodesStorage.remove(current_ir_code_id)) {
          // Find next available code or reset selection
          int16_t next_code = irCodesStorage.findNext(0);
          if (next_code >= 0) {
            current_ir_code_id = next_code;
            current_ir_code_counter = irCodesStorage.getCounter(next_code);
          } else {
            current_ir_code_id = -1;
            current_ir_code_counter = -1;
            enableIrCodeDetails(false);
          }

          buildIrCodesTableJsonPayload(ir_codes_table_label, 0);
          updateIrCodeButtons();
          ESPUI.updateLabel(ir_code_state_label, "IR code deleted successfully!");
        } else {
          ESPUI.updateLabel(ir_code_state_label, "Failed to delete IR code!");
        }
			} break;
		}
		pending_gui_command->in_use = false;
	}
}

/*****************************************************************************/

void enterWifiDetailsCallback(BasicControl *sender, int type, void *param) {

	if ((type == B_UP) && data_ready) {

		auto cfg = Supla::Storage::ConfigInstance();

		if (cfg) {

			cfg->setWiFiSSID(ESPUI.getControl(wifi_ssid_text)->getValueCstr());
			cfg->setWiFiPassword(ESPUI.getControl(wifi_pass_text)->getValueCstr());
			cfg->setSuplaServer(ESPUI.getControl(Supla_server)->getValueCstr());
			cfg->setEmail(ESPUI.getControl(Supla_email)->getValueCstr());
			cfg->setUInt8(
			  "security_level",
			  ESPUI.getControl(
			         Supla_skip_certificate_switcher)
			        ->getValueInt()
			      > 0
			    ? 2
			    : 0);

			cfg->commit();
			if ((uint32_t)param == GUI_CB_RESTART_FLAG) SuplaDevice.softRestart();
		}
	}
}

/*****************************************************************************/

void textCallback(BasicControl *sender, int type, void *param) {

	if ((ESPUI.getControl(wifi_ssid_text)->getValue().length() > 0) && 
			(ESPUI.getControl(wifi_pass_text)->getValue().length() > 0) && 
			(ESPUI.getControl(Supla_server)->getValue().length() > 0) && 
			(ESPUI.getControl(Supla_email)->getValue().length() > 0)) {

		ESPUI.updateLabel(save_label, "Data complete. Press Save");
		data_ready = true;
	} 
	else {

		ESPUI.updateLabel(save_label, "Data incomplete!");
		data_ready = false;
	}
}

/*****************************************************************************/

void generalCallback(BasicControl *sender, int type, void *param) {

	/*Serial.print("CB: id(");
	Serial.print(sender->GetId());
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");*/
	//Serial.println(sender->getValue());
}

/*****************************************************************************/

void hiddenNumberCallback(BasicControl *sender, int type, void *param) {

	int32_t raw_val = sender->getValueInt();

	if (raw_val & 0x4000) {
		uint16_t row_idx = raw_val & 0x3FFF;
		current_ir_code_id = irCodesStorage.getPosition(row_idx + 1);
		current_ir_code_counter = irCodesStorage.getCounter(current_ir_code_id);

		log_i(
			"current_ir_code_id %d, current_ir_code_counter %d",
			current_ir_code_id, current_ir_code_counter);
		return;
	}

	current_action_id = Z2S_getActionPosition(raw_val + 1);
	current_action_counter = Z2S_getActionCounter(current_action_id);

	log_i(
		"current_action_id %u, current_action_counter %u", current_action_id,
		current_action_counter);
}

/*****************************************************************************/

void onZigbeeTabCallback(BasicControl *sender, int type, void *param) {

	ESPUI.updateNumber(pairing_mode_switcher, Zigbee.isNetworkOpen() ? 1 : 0);
	ESPUI.updateNumber(force_leave_switcher, force_leave_global_flag ? 1 : 0);
}

/*****************************************************************************/

void onChannelsTabCallback(BasicControl *sender, int type, void *param) {
}

/*****************************************************************************/

void generalMinMaxCallback(BasicControl *sender, int type, void *param) {

	Serial.print("CB: id(");
	Serial.print(sender->GetId());
	if (sender->getValueInt() < 0)
		ESPUI.updateNumber(sender->GetId(), 0);

	uint32_t max_value = (uint32_t)param;

	if (sender->getValueInt() > max_value)
		ESPUI.updateNumber(sender->GetId(), max_value);
}

/*****************************************************************************/

void endpointCallback(BasicControl *sender, int type, void *param) {

	Serial.print("CB: id(");
	Serial.print(sender->GetId());
	if (sender->getValueInt() < 1)
		ESPUI.updateNumber(sender->GetId(), 1);
	if (sender->getValueInt() > 255)
		ESPUI.updateNumber(sender->GetId(), 255);
}

/*****************************************************************************/

void enableDeviceControls(bool enable) {

	if (enable == (controls_enabled_flags & DEVICES_CONTROLS_ENABLED_FLAG))
		return;

	updateLabel_C(zb_device_info_label, three_dots_str);
	enableControlStyle(zb_device_info_label, enable);

	working_str = empty_str;
	ESPUI.updateText(device_name_text, working_str);

	ESPUI.updateNumber(battery_voltage_min_number, 0);
	ESPUI.updateNumber(battery_voltage_max_number, 0);

	enableControlStyle(device_name_text, enable);
	enableControlStyle(device_name_save_button, enable);


	enableControlStyle(battery_panel_label, enable);
	enableControlStyle(battery_voltage_min_number, enable);
	enableControlStyle(battery_voltage_min_save_button, enable);
	enableControlStyle(battery_voltage_max_number, enable);
	enableControlStyle(battery_voltage_max_save_button, enable);
	enableControlStyle(disable_battery_percentage_msg_switcher, enable);
	enableControlStyle(disable_battery_voltage_msg_switcher, enable);

	updateLabel_C(rssilabel, three_dots_str);

	enableControlStyle(getswbuild_button, enable);
	enableControlStyle(getrssi_button, enable);

	enableControlStyle(remove_device_and_channels_button, enable);
	enableControlStyle(remove_all_devices_button, true);
	enableControlStyle(start_zigbee_ota_button, enable);

	enable ? controls_enabled_flags |= DEVICES_CONTROLS_ENABLED_FLAG : 
		controls_enabled_flags &= ~DEVICES_CONTROLS_ENABLED_FLAG;
}

/*****************************************************************************/

void enableClustersAttributesControls(bool enable) {

	if (enable == (controls_enabled_flags & CLUSTERS_ATTRIBUTES_CONTROLS_ENABLED_FLAG))
		return;

	updateLabel_P(
	  clusters_attributes_table[clusters_attributes_device_info_label],
	  three_dots_str);

	updateLabel_C(
	  clusters_attributes_table[device_read_attribute_label], three_dots_str);
	updateLabel_C(
	  clusters_attributes_table[device_Tuya_payload_label], three_dots_str);

	ESPUI.updateSelect(clusters_attributes_table[device_cluster_selector], -1);
	ESPUI.updateSelect(
	  clusters_attributes_table[device_attribute_id_selector], -1);
	ESPUI.updateSelect(
	  clusters_attributes_table[device_attribute_value_selector], -1);
	ESPUI.updateSelect(
	  clusters_attributes_table[device_attribute_type_selector], -1);

	ESPUI.updateNumber(clusters_attributes_table[device_endpoint_number], 1);
	ESPUI.updateNumber(
	  clusters_attributes_table[device_attribute_size_number], 0);
	ESPUI.updateNumber(clusters_attributes_table[device_config_min_number], 0);
	ESPUI.updateNumber(clusters_attributes_table[device_config_max_number], 0);
	ESPUI.updateNumber(clusters_attributes_table[device_config_delta_number], 0);

	working_str = "0";
	ESPUI.updateText(
	  clusters_attributes_table[device_attribute_id_text], working_str);

	for (uint8_t i = device_endpoint_number; i < device_last_enum_position; i++)
		enableControlStyle(clusters_attributes_table[i], enable);

	enable ? controls_enabled_flags |= CLUSTERS_ATTRIBUTES_CONTROLS_ENABLED_FLAG : 
		controls_enabled_flags &= ~CLUSTERS_ATTRIBUTES_CONTROLS_ENABLED_FLAG;
}

/*****************************************************************************/

void updateDeviceInfoLabel(uint8_t device_index) {

  Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(device_index);

  if (device == nullptr) {

    enableDeviceControls(false);
    return;
  }

  enableDeviceControls(true);

  char general_purpose_gui_buffer[768] = {};
  char ieee_addr_str[24] = {};
  uint8_t *ieee_addr = device->getDeviceIeeeAddress();

  snprintf_P(
    ieee_addr_str, 24, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
    ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4],
    ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0]);

  snprintf_P(
    general_purpose_gui_buffer, 768,
    "<b><i><style=color:white;background-color:unset;>Slot#</b></i> %02u "
    "<b>| <i>Manufacturer name<style=;></i></b> %s "
    "<b>| <i>model ID</b></i> %s <b>| <i>Z2S model</b></i> %s [0x%04X]<br>"
    "<b><i>IEEE address</b></i> %s <b>| <i>Short address</b></i> 0x%04X "
    "<b>| <i>Power source</b></i> 0x%02X<br>"
    "<b><i>Battery percentage</b></i> %u <b>| <i>Last seen (ms)</b></i> %lu "
    "Last RSSI</b></i> %d<br><b><i>Device flags</b></i> 0x%08X <b>| <i>ud(1)"
    "</b></i> 0x%08X <b>| <i>ud(2)</b></i> 0x%08X<br>",
    device_index,
    device->getManufacturerName(),
    device->getModelName(),
    getZ2SDeviceDescName(device->getDeviceDescId()),
    device->getDeviceDescId(),
    ieee_addr_str,
    device->getDeviceShortAddress(),
    device->getPowerSource(),
    device->getBatteryPercentage(),
    device->getLastSeenMs(),
    device->getRssi(),
    device->getUserDataFlags(),
    device->getUserData1(),
    device->getUserData2());

  log_i(
    "value = %s, length = %u", general_purpose_gui_buffer,
    strlen(general_purpose_gui_buffer));

  updateLabel_P(zb_device_info_label, general_purpose_gui_buffer);

  working_str = device->getDeviceLocalName();
  ESPUI.updateText(device_name_text, working_str);

  ESPUI.updateNumber(
    battery_voltage_min_number, device->getBatteryVoltageMin());

  ESPUI.updateNumber(
    battery_voltage_max_number, device->getBatteryVoltageMax());

  ESPUI.updateNumber(
    disable_battery_percentage_msg_switcher,
    device->checkDeviceUserDataFlags(
      ZBD_USER_DATA_FLAG_DISABLE_BATTERY_PERCENTAGE_MSG) ? 1 : 0);

  ESPUI.updateNumber(
    disable_battery_voltage_msg_switcher,
    device->checkDeviceUserDataFlags(
      ZBD_USER_DATA_FLAG_DISABLE_BATTERY_VOLTAGE_MSG) ? 1 : 0);
}
/*****************************************************************************/

void selectGuiModeCallback(BasicControl *sender, int type, void *param) {

	if ((sender->getValueInt() < 0) || (sender->getValueInt() >= gui_modes_number)) {

		return;
	} else {

		if (Supla::Storage::ConfigInstance()->setInt8(Z2S_ENABLE_GUI_ON_START_V2,
		                                              sender->getValueInt()))
			Supla::Storage::ConfigInstance()->commit();
	}
}

/*****************************************************************************/

void switchGUIBuildFlagCallback(BasicControl *sender, int type, void *param) {

	int32_t build_flag_bit = (int32_t)param;

	bool set_flag = (sender->getValueInt() == 1);

	if (set_flag)
		_gui_build_flags = _gui_build_flags | (1 << build_flag_bit);
	else
		_gui_build_flags = _gui_build_flags & (~(1 << build_flag_bit));

	_gui_build_flags = _gui_build_flags | (1 << 0);  //Gateway

	log_i(
	  "build_flag_bit %u, set_flag %u, Z2S_GUI_BUILD_FLAGS_V2 0x%04X",
	  build_flag_bit, set_flag, _gui_build_flags);

	if (Supla::Storage::ConfigInstance()->setUInt32(
	      Z2S_GUI_BUILD_FLAGS_V2, _gui_build_flags))
		Supla::Storage::ConfigInstance()->commit();
	else
		log_e("error writing Z2S_GUI_BUILD_FLAGS_V2");
}

/*****************************************************************************/

void deviceSelectorCallback(BasicControl *sender, int type, void *param) {

	int sender_value = type;

	if (sender)
		sender_value = sender->getValueInt();

	addGUICommand(gui_cmd_update_zbdevice_info, sender_value);
}

/*****************************************************************************/

void clustersattributesdeviceSelectorCallback(
  BasicControl *sender, int type, void *param) {

	char general_purpose_gui_buffer[128];

	int sender_value = type;

	if (sender)
		sender_value = sender->getValueInt();

	if ((sender_value < 0) || (sender_value >= Z2S_DEVICES_MAX_NUMBER)) {

		enableClustersAttributesControls(false);
		clusterCallbackCmd();

	} else {

		enableClustersAttributesControls(true);

		uint8_t device_slot = sender_value;

		Z2S_Device *z2s_device = Z2S_Device::getZ2SDeviceByDeviceIndex(
			device_slot);

		sprintf_P(
		  general_purpose_gui_buffer, "<b><i>Manufacturer name</i></b> %s <b>| "
		  "<i>model ID</b></i> %s",z2s_device->getManufacturerName(),
			z2s_device->getModelName());

		updateLabel_P(
		  clusters_attributes_table[clusters_attributes_device_info_label],
		  general_purpose_gui_buffer);
		clusterCallbackCmd();
	}
}

/*****************************************************************************/

void enableLAHPanel(bool enable) {

	/*for (uint8_t i = 0; i < 15; i++)
		enableControlStyle(lah_panel + i, enable);*/
}

/*****************************************************************************/

void enableChannelControls(bool enable) {

	if (enable == (controls_enabled_flags & CHANNELS_CONTROLS_ENABLED_FLAG))
		return;

	updateLabel_C(zb_channel_info_label, three_dots_str);
	working_str = empty_str;
	ESPUI.updateText(channel_name_text, working_str);

	ESPUI.updateNumber(param_1_text, 0);
	ESPUI.updateNumber(param_2_number, 0);
	ESPUI.updateNumber(param_3_number, 0);
	ESPUI.updateNumber(keepalive_number, 0);
	ESPUI.updateNumber(timeout_number, 0);
	ESPUI.updateNumber(refresh_number, 0);
	ESPUI.updateNumber(disable_channel_notifications_switcher, 0);
	ESPUI.updateNumber(trv_auto_to_schedule_switcher, 0);
	ESPUI.updateNumber(trv_auto_to_schedule_manual_switcher, 0);
	ESPUI.updateNumber(trv_fixed_calibration_switcher, 0);
	ESPUI.updateNumber(trv_cooperative_childlock_switcher, 0);
	ESPUI.updateNumber(set_sorwns_on_start_switcher, 0);
	ESPUI.updateNumber(enable_resend_temperature_switcher, 0);
	ESPUI.updateNumber(skip_subdevice_registation_switcher, 0);
	ESPUI.updateSelect(local_channel_function_selector, -1);

	enableControlStyle(channel_name_text, enable);
	enableControlStyle(channel_name_save_button, enable);
	enableControlStyle(disable_channel_notifications_switcher, enable);
	enableControlStyle(trv_auto_to_schedule_switcher, enable);
	enableControlStyle(trv_auto_to_schedule_manual_switcher, enable);
	enableControlStyle(trv_fixed_calibration_switcher, enable);
	enableControlStyle(trv_cooperative_childlock_switcher, enable);
	enableControlStyle(set_sorwns_on_start_switcher, enable);
	enableControlStyle(enable_resend_temperature_switcher, enable);
	enableControlStyle(skip_subdevice_registation_switcher, enable);
	enableControlStyle(param_1_text, enable);
	enableControlStyle(param_2_number, enable);
	enableControlStyle(param_3_number, enable);
	enableControlStyle(param_1_save_button, enable);
	enableControlStyle(param_2_save_button, enable);
	enableControlStyle(param_3_save_button, enable);
	enableControlStyle(keepalive_number, enable);
	enableControlStyle(keepalive_save_button, enable);
	enableControlStyle(timeout_number, enable);
	enableControlStyle(timeout_save_button, enable);
	enableControlStyle(refresh_number, enable);
	enableControlStyle(refresh_save_button, enable);
	enableControlStyle(remove_channel_button, enable);
	//enableControlStyle(remove_all_channels_button, enable);
	enableControlStyle(zb_channel_timings_label, enable);
	enableControlStyle(zb_channel_flags_label, enable);
	enableControlStyle(zb_channel_params_label, enable);
	enableControlStyle(local_channel_function_selector, enable);

	enable ? controls_enabled_flags |= CHANNELS_CONTROLS_ENABLED_FLAG : 
		controls_enabled_flags &= ~CHANNELS_CONTROLS_ENABLED_FLAG;
}

/*****************************************************************************/

void enableChannelTimings(uint8_t timings_mask) {

	if (timings_mask & 1)
		enableControlStyle(keepalive_number, true);
	else {
		ESPUI.updateNumber(keepalive_number, 0);
		enableControlStyle(keepalive_number, false);
	}

	if (timings_mask & 2)
		enableControlStyle(timeout_number, true);
	else {
		ESPUI.updateNumber(timeout_number, 0);
		enableControlStyle(timeout_number, false);
	}

	if (timings_mask & 4)
		enableControlStyle(refresh_number, true);
	else {
		ESPUI.updateNumber(refresh_number, 0);
		enableControlStyle(refresh_number, false);
	}

	if (timings_mask == 0)
		enableControlStyle(zb_channel_timings_label, false);
	else
		enableControlStyle(zb_channel_timings_label, true);
}

/*****************************************************************************/

void enableChannelFlags(uint8_t flags_mask) {

	if (flags_mask & 1)
		enableControlStyle(disable_channel_notifications_switcher, true);
	else {
		ESPUI.updateNumber(disable_channel_notifications_switcher, 0);
		enableControlStyle(disable_channel_notifications_switcher, false);
	}

	if (flags_mask & 2) {

		enableControlStyle(trv_auto_to_schedule_switcher, true);
		enableControlStyle(trv_auto_to_schedule_manual_switcher, true);
		enableControlStyle(trv_fixed_calibration_switcher, true);
		enableControlStyle(trv_cooperative_childlock_switcher, true);

	} else {

		ESPUI.updateNumber(trv_auto_to_schedule_switcher, 0);
		enableControlStyle(trv_auto_to_schedule_switcher, false);

		ESPUI.updateNumber(trv_auto_to_schedule_manual_switcher, 0);
		enableControlStyle(trv_auto_to_schedule_manual_switcher, false);

		ESPUI.updateNumber(trv_fixed_calibration_switcher, 0);
		enableControlStyle(trv_fixed_calibration_switcher, false);

		ESPUI.updateNumber(trv_cooperative_childlock_switcher, 0);
		enableControlStyle(trv_cooperative_childlock_switcher, false);
	}

	if (flags_mask & 4)
		enableControlStyle(set_sorwns_on_start_switcher, true);
	else {
		ESPUI.updateNumber(set_sorwns_on_start_switcher, 0);
		enableControlStyle(set_sorwns_on_start_switcher, false);
	}

	if (flags_mask & 8)
		enableControlStyle(
		  enable_resend_temperature_switcher, true);
	else {
		ESPUI.updateNumber(enable_resend_temperature_switcher, 0);
		enableControlStyle(enable_resend_temperature_switcher, false);
	}

	if (flags_mask & 16)
		enableControlStyle(
		  skip_subdevice_registation_switcher, true);
	else {
		ESPUI.updateNumber(skip_subdevice_registation_switcher, 0);
		enableControlStyle(skip_subdevice_registation_switcher, false);
	}

	if (flags_mask == 0)
		enableControlStyle(zb_channel_flags_label, false);
	else
		enableControlStyle(zb_channel_flags_label, true);
}

/*****************************************************************************/

void enableChannelParams(uint8_t params_mask) {


	if (params_mask & 1) {

		enableControlStyle(param_1_text, true);
		enableControlStyle(param_1_save_button, true);
	} else {

		ESPUI.updateNumber(param_1_text, 0);
		char *working_str_ptr =
		  "&#10023; PARAM(1) - currently not used &#10023;";
		ESPUI.updateLabel(param_1_desc_label, working_str_ptr);
		enableControlStyle(param_1_text, false);
		enableControlStyle(param_1_save_button, false);
	}

	if (params_mask & 2) {

		enableControlStyle(param_2_number, true);
		enableControlStyle(param_2_save_button, true);

	} else {

		ESPUI.updateNumber(param_2_number, 0);
		char *working_str_ptr =
		  "&#10023; PARAM(2) - currently not used &#10023;";
		ESPUI.updateLabel(param_2_desc_label, working_str_ptr);
		enableControlStyle(param_2_number, false);
		enableControlStyle(param_2_save_button, false);
	}

	if (params_mask & 4) {

		enableControlStyle(param_3_number, true);
		enableControlStyle(param_3_save_button, true);

	} else {

		ESPUI.updateNumber(param_3_number, 0);
		char *working_str_ptr =
		  "&#10023; PARAM(3) - currently not used &#10023;";
		ESPUI.updateLabel(param_3_desc_label, working_str_ptr);
		enableControlStyle(param_3_number, false);
		enableControlStyle(param_3_save_button, false);
	}

	if (params_mask == 0)
		enableControlStyle(zb_channel_params_label, false);
	else
		enableControlStyle(zb_channel_params_label, true);
}

/*****************************************************************************/

void fillRemoteAddressData(uint8_t channel_slot) {

	char general_purpose_gui_buffer[512] = {};

	Z2S_Core *z2s_core = Z2S_Core::getZ2SCoreByChannelIndex(channel_slot);

	uint8_t remote_address_type = z2s_core->checkChannelUserDataFlags(
		USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS) ? REMOTE_ADDRESS_TYPE_MDNS
	  : REMOTE_ADDRESS_TYPE_IP4;


	if (remote_address_type == REMOTE_ADDRESS_TYPE_IP4) {

		IPAddress ip(z2s_core->getRemoteIPAddress());

		ESPUI.updateText(param_1_text, ip.toString());

		ESPUI.updateNumber(param_2_number, z2s_core->getSuplaRemoteChannel());
	} else {
		if (remote_address_type == REMOTE_ADDRESS_TYPE_MDNS) {

			sprintf(
			  general_purpose_gui_buffer, "mdns://%s", z2s_core->getMDNSName());

			working_str = general_purpose_gui_buffer;
			ESPUI.updateText(param_1_text, working_str);

			ESPUI.updateNumber(param_2_number, z2s_core->getSuplaRemoteChannel());
		}
	}
}

/*****************************************************************************/

void updateResendTemperatureControls(Z2S_Core *z2s_core, bool source_channel) {

	uint8_t enable_resend_temperature_flag = z2s_core->checkChannelUserDataFlags(
		USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE) ? 1 : 0;

	ESPUI.updateNumber(
	  enable_resend_temperature_switcher, enable_resend_temperature_flag);

	if (enable_resend_temperature_flag) {

		if (source_channel)
			enableChannelParams(7);
		else
			enableChannelParams(3);

		log_i(
		  "remote address type = %s", z2s_core->checkChannelUserDataFlags(
		  USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS) ? "MDNS" : "IP4");

		fillRemoteAddressData(z2s_core->getZ2SChannelIndex());

		ESPUI.updateLabel(param_1_desc_label, dest_thermometer_address_desc);
		ESPUI.updateLabel(param_2_desc_label, dest_thermometer_channel_desc);
	} else {

		if (source_channel)
			enableChannelParams(4);
		else
			enableChannelParams(0);
	}

	if (source_channel) {

		ESPUI.updateLabel(param_3_desc_label, src_thermometer_channel_desc);
		ESPUI.updateNumber(param_3_number, z2s_core->getSourceChannel());
	}
}

/*****************************************************************************/

void updateChannelInfoLabel(int16_t channel_slot) {

	char general_purpose_gui_buffer[896] = {};

	char ieee_addr_str[24] = {};

	if (channel_slot == -2)
		channel_slot = ESPUI.getControl(channel_selector)->getValueInt();

	log_i("channel slot %i", channel_slot);

	auto z2s_core = Z2S_Core::getZ2SCoreByChannelIndex(channel_slot);

	if (!z2s_core)
		return;

	z2s_device_params_t z2s_channel = {};

	if (!z2s_core->fillZ2SChannelStruct(z2s_channel))
		return;

	snprintf_P(
	  ieee_addr_str, 24, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
	  z2s_channel.ieee_addr[7], z2s_channel.ieee_addr[6],
	  z2s_channel.ieee_addr[5], z2s_channel.ieee_addr[4],
	  z2s_channel.ieee_addr[3], z2s_channel.ieee_addr[2],
	  z2s_channel.ieee_addr[1], z2s_channel.ieee_addr[0]);

	log_i("z2s_channel ieee addr %s", ieee_addr_str);

	snprintf_P(
	  general_purpose_gui_buffer, 896, 
		"<meta charset=\"UTF-8\">"
	  "<b><i>IEEE address</i></b> %s <b>| <i>Short address</i></b> 0x%04X "
		"<b>| <i>endpoint</i></b> 0x%02X <b>| <i>cluster</i></b> 0x%04X<br>"
		"<b><i>Model id</i></b> %s [0x%04X] <b>| <i>channel</i></b> #%u "
		"<b>| <i>secondary channel</i></b> #%u<br><b><i>Type</b></i> %s "
		"<b>| <i>Function</b></i> %s <b>| <i>Sub id</b></i> %d<br>"
		"<b><i>Channel flags</b></i> 0x%08X <b>| <i>ud(1)</b></i> 0x%08X "
		"<b>| <i>ud(2)</b></i> 0x%08X <b>| <i>ud(3)</b></i> 0x%08X "
		"<b>| <i>ud(4)</b></i> 0x%08X <b>| <i>edt</b></i> 0x%02X<br>"
		"<b><i>ZB device</b></i> %s (%s::%s)<br>"
		"<b><i>GUI id</b></i> %u <b>| <i>dc</b></i> 0x%016llX (%s) [[%u]]",
	  ieee_addr_str, z2s_channel.short_addr, z2s_channel.endpoint,
	  z2s_channel.cluster_id, getZ2SDeviceDescName(z2s_channel.model_id),
	  z2s_channel.model_id, z2s_channel.Supla_channel,
	  z2s_channel.Supla_secondary_channel, 
		getSuplaChannelTypeName(z2s_channel.Supla_channel_type), 
		z2s_channel.Supla_channel_func > 0 ? 
		getSuplaChannelFuncName(z2s_channel.Supla_channel_type, 
			z2s_channel.Supla_channel_func) : "none", z2s_channel.sub_id,
	  z2s_channel.user_data_flags, z2s_channel.user_data_1,
	  z2s_channel.user_data_2, z2s_channel.user_data_3, z2s_channel.user_data_4,
	  z2s_channel.extended_data_type, 
		z2s_core->getZbDeviceLocalName(), (z2s_channel.local_channel_type == 0) ? 
			z2s_core->getZbDeviceManufacturerName() : 
			getZ2SDeviceLocalActionHandlerTypeName(&z2s_channel),
	  (z2s_channel.local_channel_type == 0) ? z2s_core->getZbDeviceModelName() : 
			getZ2SDeviceLocalActionHandlerLogicOperatorName(&z2s_channel),
	  z2s_channel.gui_control_id, z2s_core->getChannelExtendedDataCounter(),
	  z2s_core->getChannelExtendedDataCounterKey(), z2s_core->getZ2SChannelIndex());

	updateLabel_P(
	  zb_channel_info_label, general_purpose_gui_buffer);

	working_str = z2s_core->getZ2SChannelName();

	ESPUI.updateText(channel_name_text, working_str);

	enableChannelParams(0);

	ESPUI.updateSelect(local_channel_function_selector, -1);
	enableControlStyle(local_channel_function_selector, false);

	switch (z2s_channel.Supla_channel_type) {

		case SUPLA_CHANNELTYPE_LOCALCHANNEL:
			{


				enableChannelFlags(0);

				if (z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_ACTION_HANDLER) {

					enableChannelTimings(1 + 2);  //turn on delay
					ESPUI.updateNumber(keepalive_number, z2s_core->getKeepAliveValue());
					ESPUI.updateNumber(timeout_number, z2s_core->getTimeoutValue());
				}

				if (z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_REMOTE_RELAY) {

					enableChannelTimings(0);
					enableChannelParams(3);
					fillRemoteAddressData(channel_slot);

					char *working_str_ptr = "&#10023; Enter remote relay IP address or "
						"mDNS name &#10023;<br>for mDNS use <b><i>mdns://</i></b> prefix"
					  " ie. mdns://my_gateway";

					ESPUI.updateLabel(param_1_desc_label, working_str_ptr);

					working_str_ptr = "&#10023; Enter remote relay channel # &#10023;";
					ESPUI.updateLabel(param_2_desc_label, working_str_ptr);
				}

				if (z2s_channel.local_channel_type == 
						LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER) {

					enableChannelTimings(2 + 4);
					enableChannelFlags(8);
					enableControlStyle(local_channel_function_selector, true);

					sortRemoteThermometerFunctionsSelectorsMain(
						z2s_channel.local_channel_func);

					ESPUI.updateSelect(
						local_channel_function_selector, z2s_channel.local_channel_func);

					ESPUI.updateNumber(timeout_number, z2s_core->getTimeoutValue());
					ESPUI.updateNumber(refresh_number, z2s_core->getRefreshValue());

					updateResendTemperatureControls(z2s_core, false);
				}

				if (z2s_channel.local_channel_type == 
						LOCAL_CHANNEL_TYPE_CALC_THERMOMETER) {

					enableChannelTimings(4);
					enableChannelFlags(8);
					enableControlStyle(local_channel_function_selector, true);

					sortCalculatedThermometerFunctionsSelectorsMain(
						z2s_channel.local_channel_func);

					ESPUI.updateSelect(
						local_channel_function_selector, z2s_channel.local_channel_func);

					//ESPUI.updateNumber(timeout_number, z2s_core->getTimeoutValue());
					ESPUI.updateNumber(refresh_number, z2s_core->getRefreshValue());

					updateResendTemperatureControls(z2s_core, true);
				}

				if (z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY) {

					enableChannelTimings(4);

					ESPUI.updateNumber(refresh_number, z2s_core->getRefreshValue());
				}

				if (z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY) {

					enableChannelTimings(0);
				}

				if (z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_GPIO_RELAY) {

					enableChannelTimings(0);
					enableChannelParams(1 + 2);

					char *working_str_ptr =
					  "&#10023; Enter local relay GPIO pin number &#10023;";

					ESPUI.updateLabel(param_1_desc_label, working_str_ptr);
					ESPUI.updateNumber(param_1_text, z2s_core->getLocalRelayGpioPin());

					working_str_ptr = "&#10023; Enter local relay ON value &#10023;";
					ESPUI.updateLabel(param_2_desc_label, working_str_ptr);
					ESPUI.updateNumber(param_2_number, z2s_core->getLocalRelayHighIsOn());
				}

				if (z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_GPIO_HVAC) {

					enableChannelTimings(0);
					enableChannelParams(1 + 2);

					char *working_str_ptr =
					  "&#10023; Enter local hvac GPIO pin number &#10023;";

					ESPUI.updateLabel(param_1_desc_label, working_str_ptr);
					ESPUI.updateNumber(param_1_text, z2s_core->getLocalHvacGpioPin());

					working_str_ptr = "&#10023; Enter local hvac ON value &#10023;";
					ESPUI.updateLabel(param_2_desc_label, working_str_ptr);
					ESPUI.updateNumber(param_2_number, z2s_core->getLocalHvacHighIsOn());
				}

				if (z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_GPIO_BINARY) {

					enableChannelTimings(0);
					enableChannelParams(1 + 2 + 4);

					char *working_str_ptr =
					  "&#10023; Enter local binary GPIO pin number &#10023;";

					ESPUI.updateLabel(param_1_desc_label, working_str_ptr);
					ESPUI.updateNumber(param_1_text, z2s_core->getLocalBinaryGpioPin());

					working_str_ptr = "&#10023; Enter local binary logic type value "
					                  "&#10023;<br> 0 = normal logic, 1 = inverted logic";
					ESPUI.updateLabel(param_2_desc_label, working_str_ptr);
					ESPUI.updateNumber(param_2_number, z2s_core->getLocalBinaryInvertLogic());

					working_str_ptr = "&#10023; Enter local binary pull up value "
					                  "&#10023;<br> 0 = no pull up, 1 = pull up";
					ESPUI.updateLabel(param_3_desc_label, working_str_ptr);
					ESPUI.updateNumber(param_3_number, z2s_core->getLocalBinaryPullUp());
				}

				if (z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_GPIO_DS18B20) {

					enableChannelTimings(0);
					enableChannelParams(1 + 2);

					char *working_str_ptr =
					  "&#10023; Enter local DS18B20 address &#10023; <br>"
					  "i.e. 28FF55CA6B18018D or leave it blank for single sensor";

					ESPUI.updateLabel(param_1_desc_label, working_str_ptr);

					if (z2s_core->getLocalDS18B20Address()) {

						char ds_address_str[17] = {};

						bytesToHex(
						  z2s_core->getLocalDS18B20Address(), ds_address_str,
						  sizeof(ds_address_str));
						working_str = ds_address_str;
					} else
						working_str = empty_str;
					ESPUI.updateText(param_1_text, working_str);

					working_str_ptr = "&#10023; Enter local DS18B20 pin value &#10023;";
					ESPUI.updateLabel(param_2_desc_label, working_str_ptr);
					ESPUI.updateNumber(param_2_number, z2s_core->getLocalDS18B20GpioPin());
				}

				if (z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_GPIO_HCSR04) {

					enableChannelTimings(0);
					enableChannelParams(1);

					char *working_str_ptr =
					  "&#10023; Enter local HC-SR04 parameters&#10023;<br>"
					  "use this format:<br>TrigPin,EchoPin,MinIn,MaxIn,MinOut,MaxOut<br>"
					  "i.e. 5,6,0,500,0,500";

					ESPUI.updateLabel(param_1_desc_label, working_str_ptr);
					sprintf(
					  general_purpose_gui_buffer, "%i,%i,%u,%u,%u,%u",
					  z2s_core->getLocalHCSR04TrigPin(), z2s_core->getLocalHCSR04EchoPin(),
					  z2s_core->getLocalHCSR04MinIn(), z2s_core->getLocalHCSR04MaxIn(),
					  z2s_core->getLocalHCSR04MinOut(), z2s_core->getLocalHCSR04MaxOut());

					working_str = general_purpose_gui_buffer;
					ESPUI.updateText(param_1_text, working_str);

					/*working_str_ptr = "&#10023; Enter local hvac ON value &#10023;";
				ESPUI.updateLabel(param_2_desc_label, working_str_ptr);
				ESPUI.updateNumber(param_2_number, z2s_core->getLocalHvacHighIsOn());*/
				}

				if (z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_VIRTUAL_THERM_HYGRO_METER) {

					enableChannelTimings(4);
					enableChannelFlags(8);

					ESPUI.updateNumber(refresh_number, z2s_core->getRefreshValue());

					updateResendTemperatureControls(z2s_core, true);
				}
			}
			break;


		case SUPLA_CHANNELTYPE_BINARYSENSOR:
			{

				enableChannelTimings(6);  //timeout+debounce
				ESPUI.updateNumber(timeout_number, z2s_core->getTimeoutValue());

				ESPUI.updateNumber(refresh_number, z2s_core->getRefreshValue());

				enableChannelFlags(5 + 16);

				ESPUI.updateNumber(
				  disable_channel_notifications_switcher, (z2s_channel.user_data_flags & USER_DATA_FLAG_DISABLE_NOTIFICATIONS) ? 1 : 0);

				ESPUI.updateNumber(
				  set_sorwns_on_start_switcher, (z2s_channel.user_data_flags & USER_DATA_FLAG_SET_SORWNS_ON_START) ? 1 : 0);

				enableChannelParams(1);

				ESPUI.updateNumber(
				  param_1_text, z2s_channel.rain_intensity_treshold);

				char *working_str_ptr =
				  "&#10023; Virtual Binary custom param<br>"
				  "enter rain intensity threshold<br>"
				  "currently unused for other sensors &#10023;";

				ESPUI.updateLabel(param_1_desc_label, working_str_ptr);
			}
			break;


		case SUPLA_CHANNELTYPE_ACTIONTRIGGER:
			{

				//action_trigger_hold_ms + timeout + debounce
				enableChannelTimings(1 + 2 + 4);

				ESPUI.updateNumber(keepalive_number, z2s_channel.action_trigger_hold_ms);

				ESPUI.updateNumber(timeout_number, z2s_core->getTimeoutValue());

				ESPUI.updateNumber(refresh_number, z2s_channel.debounce_ms);

				enableChannelFlags(16);
			}
			break;


		case SUPLA_CHANNELTYPE_THERMOMETER:
		case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR:
		case SUPLA_CHANNELTYPE_PRESSURESENSOR:
			{

				enableChannelFlags(4 + 8 + 16);
				enableChannelTimings(6);  //timeout only

				ESPUI.updateNumber(
				  set_sorwns_on_start_switcher, (z2s_channel.user_data_flags & USER_DATA_FLAG_SET_SORWNS_ON_START) ? 1 : 0);

				ESPUI.updateNumber(timeout_number, z2s_core->getTimeoutValue());
				ESPUI.updateNumber(refresh_number, z2s_core->getRefreshValue());

				updateResendTemperatureControls(z2s_core, false);
			}
			break;


		case SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT:
			{

				enableChannelTimings(2);  //timeout
				enableChannelFlags(4 + 16);
				enableChannelParams(3);

				char *working_str_ptr =
				  "&#10023; Enter GPM temporary deactivation threshold value &#10023;";

				ESPUI.updateLabel(param_1_desc_label, working_str_ptr);

				working_str_ptr = "&#10023; Enter GPM temporary deactivation duration "
				                  "[s] &#10023;";
				ESPUI.updateLabel(param_2_desc_label, working_str_ptr);
			}
			break;


		case SUPLA_CHANNELTYPE_HVAC:
			{

				enableChannelTimings(1 + 2);  //keepalive + timeout

				ESPUI.updateNumber(keepalive_number, z2s_core->getKeepAliveValue());

				ESPUI.updateNumber(timeout_number, z2s_core->getTimeoutValue());

				enableChannelFlags(2 + 16);
				ESPUI.updateNumber(
				  trv_auto_to_schedule_switcher, (z2s_channel.user_data_flags & 
					USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE) ? 1 : 0);

				uint8_t fixed_correction_flag = (z2s_channel.user_data_flags & 
				USER_DATA_FLAG_TRV_FIXED_CORRECTION) ? 1 : 0;

				ESPUI.updateNumber(
				  trv_fixed_calibration_switcher, fixed_correction_flag);

				ESPUI.updateNumber(
				  trv_auto_to_schedule_manual_switcher, (z2s_channel.user_data_flags & 
					USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE_MANUAL) ? 1 : 0);

				ESPUI.updateNumber(
				  trv_auto_to_schedule_manual_switcher, (z2s_channel.user_data_flags & 
					USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE_MANUAL) ? 1 : 0);

				ESPUI.updateNumber(
				  trv_cooperative_childlock_switcher, (z2s_channel.user_data_flags & 
					USER_DATA_FLAG_TRV_COOPERATIVE_CHILDLOCK) ? 1 : 0);

				if (fixed_correction_flag) {

					enableChannelParams(1);

					ESPUI.updateNumber(
					  param_1_text, z2s_channel.hvac_fixed_temperature_correction);

					char *working_str_ptr = "&#10023; Thermostat custom parameter<br>"
					                        "enter calibration fixed value (temperature x100)<br>"
					                        "ie. to set correction to -1 enter -100 &#10023;";
					ESPUI.updateLabel(param_1_desc_label, working_str_ptr);
				} else
					enableChannelParams(0);
			}
			break;


		case SUPLA_CHANNELTYPE_RGBLEDCONTROLLER:
		case SUPLA_CHANNELTYPE_DIMMERANDRGBLED:
			{

				enableChannelTimings(3);  //timeout + keepalive

				ESPUI.updateNumber(keepalive_number, z2s_core->getKeepAliveValue());

				ESPUI.updateNumber(timeout_number, z2s_core->getTimeoutValue());

				enableChannelParams(1);

				ESPUI.updateNumber(param_1_text, z2s_channel.rgb_color_mode);

				char *working_str_ptr = "&#10023; RGB custom param<br>"
				                        "enter numeric value to select RGB MODE &#10023;";

				ESPUI.updateLabel(param_1_desc_label, working_str_ptr);

				enableChannelFlags(16);
			}
			break;


		case SUPLA_CHANNELTYPE_RELAY:
		case SUPLA_CHANNELTYPE_VALVE_OPENCLOSE:
		case SUPLA_CHANNELTYPE_DIMMER:
			{

				if (z2s_channel.local_channel_type == 0) {

					enableChannelTimings(1 + 2 + 4);  //timeout + keepalive + refresh

					ESPUI.updateNumber(keepalive_number, z2s_core->getKeepAliveValue());
					ESPUI.updateNumber(timeout_number, z2s_core->getTimeoutValue());
					ESPUI.updateNumber(refresh_number, z2s_core->getRefreshValue());

					enableChannelFlags(16);
				}
			}
			break;


		case SUPLA_CHANNELTYPE_ELECTRICITY_METER:
			{

				enableChannelTimings(7);  //timeout + keepalive + refresh
				ESPUI.updateNumber(keepalive_number, z2s_core->getKeepAliveValue());
				ESPUI.updateNumber(timeout_number, z2s_core->getTimeoutValue());
				ESPUI.updateNumber(refresh_number, z2s_core->getRefreshValue());

				enableChannelFlags(16);
			}
			break;


		default:
			{

				enableChannelTimings(0);
				enableChannelFlags(16);
			}
			break;
	}
	if (z2s_channel.local_channel_type != 0)
		ESPUI.updateNumber(
		  skip_subdevice_registation_switcher, (z2s_channel.user_data_flags & USER_DATA_FLAG_SKIP_SUBDEVICE_REGISTRATION) ? 1 : 0);
}

/*****************************************************************************/

void channelSelectorCallback(BasicControl *sender, int type, void *param) {

	int sender_value = type;

	if (sender)
		sender_value = sender->getValueInt();

	log_i("selector value = %ld", sender_value);

	if ((sender_value < 0) || (sender_value >= Z2S_ELEMENTS_MAX_NUMBER) || (sender_value == GATEWAY_EVENTS_CHANNEL_INDEX)) {

		enableChannelControls(false);
		return;
	}

	enableChannelControls(true);

	addGUICommand(gui_cmd_update_channel_info, sender_value);
}

void getZigbeeDeviceQueryCallback(BasicControl *sender, int type, void *param) {

	char general_purpose_gui_buffer[512] = {};

	if ((type == B_UP) && (ESPUI.getControl(device_selector)->getValueInt() >= 0)) {

		zbg_device_params_t device;

		uint8_t device_selector_value = ESPUI.getControl(
			device_selector)->getValueInt();

		log_i("device_selector value %u", device_selector_value);

		device.endpoint = 1;
		device.cluster_id = 0;
		/*memcpy(
		  &device.ieee_addr,
		  z2s_zb_devices_table[device_selector_value].ieee_addr,
		  sizeof(esp_zb_ieee_addr_t));

		device.short_addr =
		  z2s_zb_devices_table[device_selector_value].short_addr;*/

		Z2S_Device *z2s_device = Z2S_Device::getZ2SDeviceByDeviceIndex(
			device_selector_value);

    if (z2s_device) {
      memcpy(
        &device.ieee_addr,
        z2s_device->getDeviceIeeeAddress(),
        sizeof(esp_zb_ieee_addr_t));

      device.short_addr = z2s_device->getDeviceShortAddress();
    }
		else 
			return;


		switch ((uint32_t)param) {


			case GUI_CB_SWBUILD_FLAG:
				{  //software build ID

					zbGateway.clearQueryBasicClusterData();

					if (zbGateway.zbQueryDeviceBasicCluster(
					      &device, true, ESP_ZB_ZCL_ATTR_BASIC_SW_BUILD_ID)) {

						if (strlen(
						      zbGateway.getQueryBasicClusterData()->software_build_ID)
						    > 0)
							updateLabel_P(
							  rssilabel,
							  zbGateway.getQueryBasicClusterData()->software_build_ID);
						else
							updateLabel_P(rssilabel, three_dots_str);
					} else
						updateLabel_C(rssilabel, device_query_failed_str);
				}
				break;


			case GUI_CB_RSSI_FLAG:
				{  //RSSI

					if (zbGateway.sendCustomClusterCmd(
					      &device, 0x0003, 0x0000, ESP_ZB_ZCL_ATTR_TYPE_NULL, 0, nullptr,
					      true)) {

						sprintf_P(
							general_purpose_gui_buffer, "%d", z2s_device->getRssi());
						updateLabel_P(rssilabel, general_purpose_gui_buffer);
					} else
						updateLabel_C(rssilabel, device_query_failed_str);
				}
				break;
		}
	}
}

/*****************************************************************************/

void getClustersAttributesQueryCallback(
  BasicControl *sender, int type, void *param) {

	if (type == B_UP)
		addGUICommand(
		  gui_cmd_get_clusters_attributes_query_callback, (int32_t)param);
}

/*****************************************************************************/

void getClustersAttributesQueryMain(int32_t custom_flag) {

	char general_purpose_gui_buffer[512] = {};

	if (ESPUI.getControl(
	           clusters_attributes_table[clusters_attributes_device_selector])
	      ->getValueInt()
	    >= 0) {

		zbg_device_params_t device;

		uint16_t c_a_device_selector = clusters_attributes_table[
			clusters_attributes_device_selector];

		log_i("device_selector value %u",
		      ESPUI.getControl(c_a_device_selector)->getValueInt());

		device.endpoint = ESPUI.getControl(
			clusters_attributes_table[device_endpoint_number])->getValueInt();

		device.cluster_id = 0;

		/*memcpy(
		  &device.ieee_addr, z2s_zb_devices_table[ESPUI.getControl(c_a_device_selector)->getValueInt()].ieee_addr,
		  sizeof(esp_zb_ieee_addr_t));*/

		Z2S_Device *z2s_device = Z2S_Device::getZ2SDeviceByDeviceIndex(
      ESPUI.getControl(c_a_device_selector)->getValueInt());

    if (z2s_device) {
      memcpy(
        &device.ieee_addr, z2s_device->getDeviceIeeeAddress(),
        sizeof(esp_zb_ieee_addr_t));
    }
		else
			return;

		/*device.short_addr = z2s_zb_devices_table[ESPUI.getControl(
		                                                c_a_device_selector)
		                                           ->getValueInt()]
		                      .short_addr;*/

		device.short_addr = z2s_device->getDeviceShortAddress();

		uint16_t cluster_id = ESPUI.getControl(clusters_attributes_table[
			device_cluster_selector])->getValueInt();

		uint16_t attribute_id = strtoul(ESPUI.getControl(
			clusters_attributes_table[device_attribute_id_text])->getValueCstr(),
		  nullptr, 0);

		uint8_t attribute_type_idx = ESPUI.getControl(
			clusters_attributes_table[
				device_attribute_type_selector])->getValueInt();

		uint8_t attribute_type = attribute_type_idx < 0xFF ? 
			zigbee_datatypes[attribute_type_idx].zigbee_datatype_id : 0xFF;

		bool sync_cmd = ESPUI.getControl(
		  clusters_attributes_table[device_async_switcher])->getValueInt() == 0;

		uint8_t manuf_specific = (ESPUI.getControl(
			clusters_attributes_table[manufacturer_code_switcher])->getValueInt() ==
		  0) ? 0 : 1;

		uint16_t manuf_code = (ESPUI.getControl(
		  clusters_attributes_table[manufacturer_code_selector])->getValueInt() > 
			0) ? ESPUI.getControl(clusters_attributes_table[
				manufacturer_code_selector])->getValueInt()  : 0;

		uint8_t src_endpoint = GATEWAY_ENDPOINT_NUMBER;

		if ((device.endpoint == SHELLY_RPC_CLUSTER_ENDPOINT_ID) && 
				(cluster_id == SHELLY_CUSTOM_CLUSTER_ID_RPC) && manuf_specific && 
				(manuf_code == SHELLY_MANUFACTURER_CODE))
			src_endpoint++;

		switch (custom_flag) {


			case GUI_CB_READ_ATTR_FLAG:
				{  //read attribute

					bool result = zbGateway.sendAttributeRead(
					  &device, cluster_id, attribute_id, sync_cmd,
					  ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 1, manuf_specific, manuf_code,
					  src_endpoint);

					if (result) {
						if (*zbGateway.getReadAttrStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS) {

							uint64_t readAttrValue;
							esp_zb_uint48_t readAttrValue48;
							esp_zb_uint24_t readAttrValue24;
							uint8_t attribute_size =
							  zbGateway.getReadAttrLastResult()->data.size;

							uint8_t *data_value =
							  (uint8_t *)zbGateway.getReadAttrLastResult()->data.value;

							char read_attribute_str[(attribute_size * 2) + 1];
							const char hex_chars[] = "0123456789ABCDEF";

							for (uint8_t i = 0; i < attribute_size; ++i) {

								read_attribute_str[i * 2] = hex_chars[((*(data_value + i)) >> 4) & 0x0F];
								read_attribute_str[i * 2 + 1] = hex_chars[(*(data_value + i)) & 0x0F];
							}
							read_attribute_str[attribute_size * 2] = '\0';

							/*switch (attribute_size) {


							case 1: 
								
								readAttrValue = 
									*(uint8_t *)zbGateway.getReadAttrLastResult()->data.value; 
							break;
							

							case 2: 
								readAttrValue = 
									*(uint16_t *)zbGateway.getReadAttrLastResult()->data.value; 
							break;
							

							case 3: {
								
									readAttrValue24 = 
										*(esp_zb_uint24_t *)zbGateway.getReadAttrLastResult()->data.value; 

									readAttrValue = 
										(((uint64_t)readAttrValue24.high) << 16) + readAttrValue24.low;
							} break;
							
							case 4: 
								readAttrValue = 
									*(uint32_t *)zbGateway.getReadAttrLastResult()->data.value; 
							break;
							
							case 6: {

								readAttrValue48 = 
									*(esp_zb_uint48_t *)zbGateway.getReadAttrLastResult()->data.value; 
								readAttrValue = 
									(((uint64_t)readAttrValue48.high) << 32) + readAttrValue48.low;
							} break;
							
							case 8: 
								readAttrValue = 
									*(uint64_t *)zbGateway.getReadAttrLastResult()->data.value; 
							break;
						}*/

							sprintf_P(
							  general_purpose_gui_buffer,
							  "Reading attribute successful!<br>Data value is:<br> %s"
							  "<br>Data type is %s(0x%X)<br>Data size is 0x%02X",
							  read_attribute_str, getZigbeeDataTypeName(zbGateway.getReadAttrLastResult()->data.type),
							  zbGateway.getReadAttrLastResult()->data.type,
							  zbGateway.getReadAttrLastResult()->data.size);

							updateLabel_P(
							  clusters_attributes_table[device_read_attribute_label],
							  general_purpose_gui_buffer);
						} else {

							sprintf_P(
							  general_purpose_gui_buffer,
							  "Reading attribute failed!<br>"
							  "Status = %s(0x%02X)<br>"
							  "Attribute id = 0x%04X",
							  esp_zb_zcl_status_to_name(*zbGateway.getReadAttrStatusLastResult()),
							  *zbGateway.getReadAttrStatusLastResult(),
							  zbGateway.getReadAttrLastResult()->id);

							updateLabel_P(
							  clusters_attributes_table[device_read_attribute_label],
							  general_purpose_gui_buffer);
						}
					} else {
						if (sync_cmd)
							updateLabel_C(
							  clusters_attributes_table[device_read_attribute_label],
							  device_query_failed_str);
						else
							updateLabel_C(
							  clusters_attributes_table[device_read_attribute_label],
							  device_async_query_str);
					}
				}
				break;

			case GUI_CB_READ_CONFIG_FLAG:
				{  //read attribute config report

					bool result = zbGateway.readClusterReportCfgCmd(
					  &device, cluster_id, attribute_id, sync_cmd,
					  ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 1, manuf_specific, manuf_code);

					if (result) {

						if (zbGateway.getReportConfigRespVariableLastResult()->status == ESP_ZB_ZCL_STATUS_SUCCESS) {

							sprintf_P(
							  general_purpose_gui_buffer, "Reading attribute config report"
							                              " successful! <br>Attribute id is 0x%X<br>Data type is %s(0x%X)"
							                              "<br>Min interval is %u(0x%x)<br>Max interval is %u(0x%X)<br>"
							                              "Delta is %u(0x%X)",
							  zbGateway.getReportConfigRespVariableLastResult()->attribute_id,
							  getZigbeeDataTypeName(
							    zbGateway.getReportConfigRespVariableLastResult()->client.attr_type),
							  zbGateway.getReportConfigRespVariableLastResult()->client.attr_type,
							  zbGateway.getReportConfigRespVariableLastResult()->client.min_interval,
							  zbGateway.getReportConfigRespVariableLastResult()->client.min_interval,
							  zbGateway.getReportConfigRespVariableLastResult()->client.max_interval,
							  zbGateway.getReportConfigRespVariableLastResult()->client.max_interval,
							  zbGateway.getReportConfigRespVariableLastResult()->client.delta[0],
							  zbGateway.getReportConfigRespVariableLastResult()->client.delta[0]);

							updateLabel_P(
							  clusters_attributes_table[device_read_attribute_label],
							  general_purpose_gui_buffer);
						} else {

							sprintf_P(
							  general_purpose_gui_buffer, "Reading attribute config report "
							                              "failed!<br>Status = %s(0x%02X)<br>Attribute id = 0x%04X<br>",
							  esp_zb_zcl_status_to_name(
							    zbGateway.getReportConfigRespVariableLastResult()->status),
							  zbGateway.getReportConfigRespVariableLastResult()->status,
							  zbGateway.getReportConfigRespVariableLastResult()->status,
							  zbGateway.getReportConfigRespVariableLastResult()->attribute_id);

							updateLabel_P(
							  clusters_attributes_table[device_read_attribute_label],
							  general_purpose_gui_buffer);
						}
					} else {
						if (sync_cmd)
							updateLabel_P(
							  clusters_attributes_table[device_read_attribute_label],
							  device_query_failed_str);
						else
							updateLabel_P(
							  clusters_attributes_table[device_read_attribute_label],
							  device_async_query_str);
					}
				}
				break;

			case GUI_CB_CONFIG_REPORT_FLAG:
				{  //configure reporting

					uint16_t min_interval = ESPUI.getControl(
					                               clusters_attributes_table[device_config_min_number])
					                          ->getValueInt();

					uint16_t max_interval = ESPUI.getControl(
					                               clusters_attributes_table[device_config_max_number])
					                          ->getValueInt();

					uint16_t delta = ESPUI.getControl(
					                        clusters_attributes_table[device_config_delta_number])
					                   ->getValueInt();

					float delta_f = delta;
					uint8_t delta_8 = delta;
					void *delta_ptr = &delta;

					switch (attribute_type) {


						case ESP_ZB_ZCL_ATTR_TYPE_SINGLE:

							delta_ptr = &delta_f;
							break;


						case ESP_ZB_ZCL_ATTR_TYPE_U8:
						case ESP_ZB_ZCL_ATTR_TYPE_S8:
						case ESP_ZB_ZCL_ATTR_TYPE_8BITMAP:
						case ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM:

							delta_ptr = &delta_8;
							break;
					}

					bool result = zbGateway.setClusterReportingExt(
					  &device, cluster_id, attribute_id, attribute_type, min_interval,
					  max_interval, delta_ptr, sync_cmd, ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV,
					  1, manuf_specific, manuf_code);

					/*uint8_t cr_payload[8] = {0x00, 0x00, 0x00, 0x10, 0x05, 0x00, 0x3c, 0x00};
				esp_zb_lock_acquire(portMAX_DELAY);
				bool result = zbGateway.sendAPSDEDataRequestCmd(
					device.short_addr, 5, device.endpoint, cluster_id, 0x6, 8, 
					cr_payload, ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 0, 0, 0, true);
				esp_zb_lock_release();*/


					if (result) {
						if (*zbGateway.getConfigReportStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS) {

							sprintf_P(
							  general_purpose_gui_buffer, "Configure reporting successful! "
							                              "<br>Attribute id is 0x%X, data type is %s(0x%X)<br>Min "
							                              "interval is %u(0x%x), max interval is %u(0x%X)<br>"
							                              "Delta is %u(0x%X)",
							  attribute_id, getZigbeeDataTypeName(attribute_type), attribute_type, min_interval, min_interval,
							  max_interval, max_interval, delta, delta);

							updateLabel_P(
							  clusters_attributes_table[device_read_attribute_label],
							  general_purpose_gui_buffer);
						} else {
							sprintf_P(
							  general_purpose_gui_buffer, "Configure reporting failed! <br>"
							                              "Status = %s(0x%02X)<br>Attribute id = 0x%04X<br>",
							  esp_zb_zcl_status_to_name(
							    *zbGateway.getConfigReportStatusLastResult()),
							  *zbGateway.getConfigReportStatusLastResult(),
							  *zbGateway.getConfigReportStatusLastResult(), attribute_id);

							updateLabel_P(
							  clusters_attributes_table[device_read_attribute_label],
							  general_purpose_gui_buffer);
						}
					} else {
						if (sync_cmd)
							updateLabel_P(
							  clusters_attributes_table[device_read_attribute_label],
							  device_query_failed_str);
						else
							updateLabel_P(
							  clusters_attributes_table[device_read_attribute_label],
							  device_async_query_str);
					}
				}
				break;

			case GUI_CB_WRITE_ATTR_FLAG:
				{  //write attribute

					void *value = nullptr;

					uint8_t write_mask;
					uint16_t write_mask_16;
					uint32_t write_mask_32;

					uint16_t attribute_size =
					  ESPUI.getControl(
					         clusters_attributes_table[device_attribute_size_number])
					    ->getValueInt();

					if (attribute_size > MAX_ZIGBEE_PAYLOAD_SIZE) {

						updateLabel_P(
						  clusters_attributes_table[device_read_attribute_label],
						  device_query_attr_size_error_str);
						return;
					}

					uint8_t write_attribute_payload[attribute_size];

					const char *attribute_value = ESPUI.getControl(
					                                     clusters_attributes_table[device_attribute_value_text])
					                                ->getValueCstr();

					log_i(
					  "attribute_value = %s, length = %u", attribute_value,
					  strlen(attribute_value));

					if ((attribute_type >= ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING) && (attribute_type <= ESP_ZB_ZCL_ATTR_TYPE_BAG)) {

						if ((strlen(attribute_value) / 2) != attribute_size) {

							updateLabel_P(
							  clusters_attributes_table[device_read_attribute_label],
							  device_query_attr_size_mismatch_str);
							return;
						}

						char byte_str[3];
						byte_str[2] = '\0';

						memset(write_attribute_payload, 0, attribute_size);

						for (int i = 0; i < attribute_size; i++) {

							memcpy(byte_str, attribute_value + (i * 2), 2);
							write_attribute_payload[i] = strtoul(byte_str, nullptr, 16);
							log_i(
							  "write_attribute_payload[%u] = %u(0x%02X)", i,
							  write_attribute_payload[i], write_attribute_payload[i]);
						}

						value = write_attribute_payload;
					} else {

						switch (attribute_size) {

							case 1:
								{
									write_mask = strtoul(attribute_value, nullptr, 0);
									value = &write_mask;
								}
								break;

							case 2:
								{
									write_mask_16 = strtoul(attribute_value, nullptr, 0);
									value = &write_mask_16;
								}
								break;

							case 4:
								{
									write_mask_32 = strtoul(attribute_value, nullptr, 0);
									value = &write_mask_32;
								}
								break;
						}
					}

					if (value) {

						bool result = zbGateway.sendAttributeWriteExt(
						  &device, cluster_id, attribute_id,
						  (esp_zb_zcl_attr_type_t)attribute_type, attribute_size, value,
						  sync_cmd, manuf_specific, manuf_code, false,
						  ESP_ZB_AF_HA_PROFILE_ID);

						if (result) {
							if (*zbGateway.getWriteAttrStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS) {

								sprintf_P(
								  general_purpose_gui_buffer, "Write attribute successful! <br>"
								                              "Attribute id   = 0x%04X<br>Attribute type = %s(0x%02X)<br>"
								                              "Attribute size = %u(0x%04X)",
								  attribute_id,
								  getZigbeeDataTypeName(attribute_type), attribute_type,
								  attribute_size, attribute_size);

								updateLabel_P(
								  clusters_attributes_table[device_read_attribute_label],
								  general_purpose_gui_buffer);
							} else {

								sprintf_P(
								  general_purpose_gui_buffer, "Write attribute failed! <br>"
								                              "Status = %s(0x%02X)<br>Attribute id = 0x%04X<br>"
								                              "Attribute type = %s(0x%04X)<br>Attribute size = %u(0x%04X)",
								  esp_zb_zcl_status_to_name(
								    *zbGateway.getWriteAttrStatusLastResult()),
								  *zbGateway.getWriteAttrStatusLastResult(),
								  *zbGateway.getWriteAttrAttributeIdLastResult(),
								  getZigbeeDataTypeName(attribute_type), attribute_type,
								  attribute_size, attribute_size);

								updateLabel_P(
								  clusters_attributes_table[device_read_attribute_label],
								  general_purpose_gui_buffer);
							}
						} else {
							if (sync_cmd)
								updateLabel_C(
								  clusters_attributes_table[device_read_attribute_label],
								  device_query_failed_str);
							else
								updateLabel_C(
								  clusters_attributes_table[device_read_attribute_label],
								  device_async_query_str);
						}
					}
				}
				break;

			case GUI_CB_CUSTOM_CMD_FLAG:
				{  //custom command

					uint16_t payload_size = ESPUI.getControl(
					                               clusters_attributes_table[device_attribute_size_number])
					                          ->getValueInt();

					if (payload_size > MAX_ZIGBEE_PAYLOAD_SIZE) {
						updateLabel_C(
						  clusters_attributes_table[device_read_attribute_label],
						  device_query_attr_size_error_str);
						return;
					}

					uint8_t custom_cmd_payload[payload_size];

					const char *payload_value = ESPUI.getControl(
					                                   clusters_attributes_table[device_attribute_value_text])
					                              ->getValueCstr();

					log_i(
					  "payload_value = %s, length = %u", payload_value,
					  strlen(payload_value));

					//if ((attribute_type >= ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING) && (attribute_type <= ESP_ZB_ZCL_ATTR_TYPE_BAG))  {
					if (true) {
						if (payload_size > 0) {

							if ((strlen(payload_value) / 2) != payload_size) {
								updateLabel_C(
								  clusters_attributes_table[device_read_attribute_label],
								  device_query_attr_size_mismatch_str);
								return;
							}

							char byte_str[3];
							byte_str[2] = '\0';
							//custom_cmd_payload = (uint8_t*)malloc(payload_size); //2 by

							/*if (custom_cmd_payload == nullptr) {
							updateLabel_C(clusters_attributes_table[device_read_attribute_label], 
														"Error allocating custom command payload buffer!");
							return;
						}*/

							memset(custom_cmd_payload, 0, payload_size);

							for (int i = 0; i < payload_size; i++) {

								memcpy(byte_str, payload_value + (i * 2), 2);
								*(custom_cmd_payload + i) = strtoul(byte_str, nullptr, 16);  //here hex base must be explicit
								log_i("custom_cmd_payload[%u] = %u(0x%02X)",
								      i, *(custom_cmd_payload + i), *(custom_cmd_payload + i));
							}
						} else
							log_i("Payload size equals 0");
					} else {

						updateLabel_C(
						  clusters_attributes_table[device_read_attribute_label],
						  "Invalid data type for custom command!");
						return;
					}
					if (true) {

						if (cluster_id == 0xEF00) {

							custom_cmd_tsn = (((uint16_t)(*custom_cmd_payload)) << 16) + (*(custom_cmd_payload + 1));
							Tuya_custom_cmd_dp = *(custom_cmd_payload + 2);
							current_Tuya_payload_label = clusters_attributes_table[device_Tuya_payload_label];
						}

						bool result = zbGateway.sendCustomClusterCmd(
						  &device, cluster_id, attribute_id,
						  (esp_zb_zcl_attr_type_t)attribute_type,
						  payload_size, custom_cmd_payload, sync_cmd,
						  ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV,
						  1, manuf_specific, manuf_code);

						if (result) {

							if (*zbGateway.getCustomCmdStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS) {


								sprintf_P(general_purpose_gui_buffer,
								          "Custom command successful! <br>"
								          "Command id   = 0x%04X<br>"
								          "Data type = %s(0x%02X)<br>"
								          "Payload size = %u(0x%04X)",
								          attribute_id,
								          getZigbeeDataTypeName(attribute_type),
								          attribute_type,
								          payload_size,
								          payload_size);

								updateLabel_P(
								  clusters_attributes_table[device_read_attribute_label],
								  general_purpose_gui_buffer);
							} else {

								sprintf_P(
								  general_purpose_gui_buffer, "Custom command failed! <br>"
								                              "Status = %s(0x%02X)<br>Command id = 0x%04X<br>"
								                              "Data type = %s(0x%04X)<br>Payload size = %u(0x#04X)",
								  esp_zb_zcl_status_to_name(*zbGateway.getCustomCmdStatusLastResult()),
								  *zbGateway.getCustomCmdStatusLastResult(),
								  *zbGateway.getCustomCmdRespToCmdLastResult(),
								  getZigbeeDataTypeName(attribute_type), attribute_type,
								  payload_size, payload_size);

								updateLabel_P(clusters_attributes_table[device_read_attribute_label],
								              general_purpose_gui_buffer);
							}
						} else {
							if (sync_cmd)
								updateLabel_C(clusters_attributes_table[device_read_attribute_label],
								              device_query_failed_str);
							else
								updateLabel_C(clusters_attributes_table[device_read_attribute_label],
								              device_async_query_str);
						}
					}
					/*if (custom_cmd_payload) 
					free(custom_cmd_payload);*/
				}
				break;


			case GUI_CB_BIND_CLUSTER_FLAG:

				zbGateway.bindDeviceCluster(&device, cluster_id);
				break;


			case GUI_CB_BIND_CLUSTER_2_FLAG:

				zbGateway.bindDeviceCluster2(&device, cluster_id);
				break;
		}
	}
}


size_t Z2S_onFillOTABuffer(uint8_t *ota_buffer, uint32_t ota_offset, uint8_t size) {

	if (ota_buffer == nullptr) {  //for status report

		if (size == ESP_ZB_ZCL_OTA_UPGRADE_SERVER_ABORTED)
			updateLabel_P(
			  zigbee_ota_status_label, "OTA file transmission aborted");
		if (size == ESP_ZB_ZCL_OTA_UPGRADE_SERVER_END)
			updateLabel_P(
			  zigbee_ota_status_label, "OTA file transmission finished");
		return 0;
	}
	if (ota_offset == 0) {

		ota_image_last_update_ms = millis();
		updateLabel_P(
		  zigbee_ota_status_label, "OTA file transmission started");
	}

	if (Z2S_initLittleFs()) {

		size_t bytes_read = Z2S_loadBufferFromFile(
		  "/zigbee_ota/zigbee_ota_file.ota", ota_offset, size, ota_buffer);

		Z2S_endLittleFs();

		if (ota_offset - ota_image_last_offset > 1024) {

			char progress_str[128];

			uint32_t ota_etc_time_secs =
			  ((millis() - ota_image_last_update_ms) / 1000) * ((ota_image_size - ota_offset) / 1024);

			uint32_t ota_etc_hour = ota_etc_time_secs / 3600;
			uint32_t ota_etc_min = (ota_etc_time_secs - (ota_etc_hour * 3600)) / 60;
			uint32_t ota_etc_sec = ota_etc_time_secs % 60;

			sprintf(
			  progress_str, "OTA progress: %lu/%lu, ETC: %02uh:%02um:%02us",
			  ota_offset, ota_image_size, ota_etc_hour, ota_etc_min, ota_etc_sec);

			working_str = progress_str;
			ESPUI.updateLabel(zigbee_ota_status_label, working_str);

			ota_image_last_offset = ota_offset;
			ota_image_last_update_ms = millis();
		}
		return bytes_read;
	} else
		log_e("Z2S_initLittleFs() failed!");
	return 0;
}

void startDeviceOTACallback(BasicControl *sender, int type, void *param) {

	if ((type == B_UP) && (ESPUI.getControl(device_selector)->getValueInt() >= 0)) {

		uint8_t device_slot = ESPUI.getControl(device_selector)->getValueInt();

		//esp_zb_ota_image_header_t esp_zb_ota_image_header;
		uint8_t esp_zb_ota_image_header[62];

		Z2S_initLittleFs();

		size_t bytes_read = Z2S_loadBufferFromFile(
		  "/zigbee_ota/zigbee_ota_file.ota", 0, 62, esp_zb_ota_image_header);  //sizeof(esp_zb_ota_image_header_t),
		                                                                       //(uint8_t *)&esp_zb_ota_image_header);

		Z2S_endLittleFs();

		/*log_i(
    "upgrade_file_id = %lu, header_version = %u, header_length = %u\n\r"
    "field_control = %u, manufacturer_id = %u, image_type = %u\n\r"
    "file_version = %lu, stack_version = %u, image_size = %lu\n\r", 
    esp_zb_ota_image_header.upgrade_file_id, 
    esp_zb_ota_image_header.header_version, 
    esp_zb_ota_image_header.header_length, 
    esp_zb_ota_image_header.field_control, 
    esp_zb_ota_image_header.manufacturer_id, 
    esp_zb_ota_image_header.image_type, 
    esp_zb_ota_image_header.file_version, 
    esp_zb_ota_image_header.stack_version, 
    esp_zb_ota_image_header.image_size);*/

		zbGateway.onFillOTABuffer(Z2S_onFillOTABuffer);

		ota_image_size =
		  ((esp_zb_ota_image_header_t *)esp_zb_ota_image_header)->image_size;

		ota_image_last_offset = 0;

		log_i("ota_image_size = %lu", ota_image_size);

		/*if (zbGateway.sendOTAUpgradeServerNotifyRequest(
		      z2s_zb_devices_table[device_slot].ieee_addr,  esp_zb_ota_image_header,
		      nullptr) == ESP_OK) {*/
		
		Z2S_Device *z2s_device = Z2S_Device::getZ2SDeviceByDeviceIndex(
			device_slot);

    if (z2s_device && zbGateway.sendOTAUpgradeServerNotifyRequest(
          z2s_device->getDeviceIeeeAddress(), esp_zb_ota_image_header,
          nullptr) == ESP_OK) {

			log_i("%s", OTA_server_notify_success_str);

			updateLabel_P(zigbee_ota_status_label, OTA_server_notify_success_str);
		} else {

			log_i("%s", OTA_server_notify_failure_str);

			updateLabel_P(zigbee_ota_status_label, OTA_server_notify_failure_str);
		}
	}
}

void removeDeviceCallback(BasicControl *sender, int type, void *param) {

	char general_purpose_gui_buffer[512] = {};

	if ((type == B_UP) && (ESPUI.getControl(device_selector)->getValueInt() >= 0)) {

		uint8_t device_slot = ESPUI.getControl(device_selector)->getValueInt();

		//char status_line[128];
		bool restart_required = false;

		switch ((uint32_t)param) {

			case GUI_CB_SINGLE_FLAG:
				{

					if (Z2S_removeZbDevice(device_slot)) {

						sprintf_P(
						  general_purpose_gui_buffer,
						  "Device #%02u removed! Restarting...", device_slot);

						restart_required = true;
					} else
						sprintf_P(
						  general_purpose_gui_buffer,
						  "Device #%02u removal failed! Error saving ZB devices table!",
						  device_slot);
				}
				break;


			case GUI_CB_WITH_CHANNELS_FLAG:
				{

					if (Z2S_removeZbDeviceWithAllChannels(device_slot)) {

						sprintf_P(
						  general_purpose_gui_buffer,
						  "Device #%02u and it's all channels removed! Restarting...",
						  device_slot);
						restart_required = true;
					} else
						sprintf_P(
						  general_purpose_gui_buffer,
						  "Device #%02u or some of it's channels removal failed! "
						  "Error saving one of devices table!",
						  device_slot);
				}
				break;


			case GUI_CB_ALL_FLAG:
				{
					if (remove_all_devices_counter == 0) {

						if (Z2S_removeAllZbDeviceWithAllChannels()) {
							sprintf_P(
							  general_purpose_gui_buffer,
							  "All devices with channels removed! Restarting...");
							restart_required = true;
						} else
							sprintf_P(
							  general_purpose_gui_buffer,
							  "Devices removal failed! Error saving ZB devices table!");
					} else {

						sprintf_P(
						  general_purpose_gui_buffer,
						  "Press this button %s more to remove all devices",
						  (remove_all_devices_counter == 2) ? "twice" : "once");
						remove_all_devices_counter--;
					}
				}
				break;
		}

		updateLabel_P(device_status_label, general_purpose_gui_buffer);

		if (restart_required)
			SuplaDevice.scheduleSoftRestart(1000);
	}
}

void removeChannelCallback(BasicControl *sender, int type, void *param) {

	if (type == B_UP)
		addGUICommand(gui_cmd_remove_local_channel);
}

void removeChannelCallbackCmd() {

	char general_purpose_gui_buffer[512] = {};

	if (ESPUI.getControl(channel_selector)->getValueInt() >= 0) {

		uint8_t channel_slot = ESPUI.getControl(channel_selector)->getValueInt();

		Z2S_Core *z2s_core = Z2S_Core::getZ2SCoreByChannelIndex(channel_slot);

		bool is_zigbee_channel = (z2s_core->getZ2SLocalChannelType() == 0);

		if (is_zigbee_channel) {

			sprintf_P(
			  general_purpose_gui_buffer, "Single channel removal is depreciated "
			                              "- use SKIP_SUBDEVICE_REGISTRATION flag");

			updateLabel_P(channel_status_label, general_purpose_gui_buffer);
		} else {

			uint16_t gui_control_id = z2s_core->getZ2SChannelGUIControlId();
			uint8_t channel_number = z2s_core->getZ2SChannelNumber();

			if (Z2S_removeChannel(channel_slot)) {

				sortChannelsSelectors();

				sprintf_P(
				  general_purpose_gui_buffer, "Local channel #%02u with all "
				  "actions removed.", channel_number);

				updateLabel_P(channel_status_label, general_purpose_gui_buffer);
			} else {

				sprintf_P(
				  general_purpose_gui_buffer, "Local channel %s can't be removed.",
				  z2s_core->getZ2SChannelName());
				updateLabel_P(channel_status_label, general_purpose_gui_buffer);
			}
		}
	}
}

void pairingSwitcherCallback(BasicControl *sender, int type, void *param) {


	switch ((uint32_t)param) {

		case GUI_CB_PAIRING_FLAG:
			{  //open network

				if (type == S_ACTIVE)
					Zigbee.openNetwork(180);
				else {
					if (Zigbee.isNetworkOpen())
						Zigbee.openNetwork(0);  //ESPUI.updateNumber(pairing_mode_switcher, 1);
				}
			}
			break;


		case GUI_CB_FORCE_LEAVE_FLAG:
			{

				if (type == S_ACTIVE)
					force_leave_global_flag = true;
				else
					force_leave_global_flag = false;
			}
			break;
	}
}

void generalZigbeeCallback(BasicControl *sender, int type, void *param) {

	if (type == B_UP) {

		switch ((uint32_t)param) {


			case GUI_CB_PAIRING_FLAG:
				{  //open network

					Zigbee.openNetwork(180);
				}
				break;


			case GUI_CB_FACTORY_FLAG:
				{  //factory reset

					if (ESPUI.getControl(factory_reset_switcher)->getValueInt() > 0)
						Zigbee.factoryReset();
				}
				break;


			case GUI_CB_GET_TX_FLAG:
				{  //read TX power

					int8_t zb_tx_power;
					esp_zb_get_tx_power(&zb_tx_power);

					log_i("get tx power %d", zb_tx_power);

					ESPUI.updateNumber(zigbee_tx_power_number, zb_tx_power);
				}
				break;

			case GUI_CB_SET_TX_FLAG: {  //update TX power

				int8_t zb_tx_power = ESPUI.getControl(
					zigbee_tx_power_number)->getValueInt();

				log_i("TX power %d", zb_tx_power);
				if ((zb_tx_power >= -24) && (zb_tx_power <= 20))
					esp_zb_set_tx_power(zb_tx_power);
				else
					ESPUI.updateNumber(zigbee_tx_power_number, 0);
			} break;


			case GUI_CB_GET_PC_FLAG:
				{  //read primary channel

					uint32_t zb_primary_channel;

					if (Supla::Storage::ConfigInstance()->getUInt32(
					      Z2S_ZIGBEE_PRIMARY_CHANNEL, &zb_primary_channel)) {

						for (uint8_t i = 11; i <= 26; i++) {
							if (zb_primary_channel & (1 << i)) {

								//working_str = i;
								ESPUI.updateNumber(zigbee_primary_channel_text, i);
							}
						}
					}
					ESPUI.updateLabel(
					  zigbee_primary_channel_label, empty_str);
				}
				break;


			case GUI_CB_SET_PC_FLAG:
				{  //write primary channel

					if ((ESPUI.getControl(
					    	zigbee_primary_channel_text)->getValueInt()>= 11) && 
							(ESPUI.getControl(
					      zigbee_primary_channel_text)->getValueInt() <= 26)) {

						uint32_t zb_primary_channel = ESPUI.getControl(
						  zigbee_primary_channel_text)->getValueInt();

						if (Supla::Storage::ConfigInstance()->setUInt32(
						      Z2S_ZIGBEE_PRIMARY_CHANNEL, (1 << zb_primary_channel))) {

							ESPUI.updateLabel(
							  zigbee_primary_channel_label, 
								zigbee_primary_channel_update_str);

							log_i(
							  "New Zigbee primary channel set to 0x%08X",
							  (1 << zb_primary_channel));

							Supla::Storage::ConfigInstance()->commit();
						}
					} else
						ESPUI.updateLabel(zigbee_primary_channel_label, empty_str);
				}
				break;


			case GUI_CB_SET_INSTALLATION_CODE_FLAG:
				{

					if (ESPUI.getControl(
					    	zigbee_installation_code_ieee_address)->getValue().length() != 16) {

						working_str =
						  "Enter device IEEE address (16 characters), ie. 56AB23BE90004560";
						ESPUI.updateText(zigbee_installation_code_ieee_address, working_str);
						return;
					}

					if (ESPUI.getControl(
					    	zigbee_installation_code_str)->getValue().length() != 36) {

						working_str =
						  "Enter device installation code (36 characters), ie. BC2356...";
						ESPUI.updateText(zigbee_installation_code_str, working_str);
						return;
					}
					uint8_t device_ieee_addr[8];
					uint8_t device_installation_code[18];

					for (uint8_t i = 0; i < 8; i++) {

						working_str = ESPUI.getControl(
						                     zigbee_installation_code_ieee_address)
						                ->getValue()
						                .substring(2 * i, 2 + 2 * i);

						device_ieee_addr[7 - i] = strtoul(working_str.c_str(), nullptr, 16);
						log_i("device_ieee_addr[%u] = %02X", 7 - i, device_ieee_addr[7 - i]);
					}

					for (uint8_t i = 0; i < 18; i++) {

						working_str = ESPUI.getControl(
						  zigbee_installation_code_str)->getValue().substring(2 * i, 2 + 2 * i);

						device_installation_code[i] = strtoul(
						  working_str.c_str(), nullptr, 16);

						log_i(
						  "device_installation_code[%u] = %02X", i,
						  device_installation_code[i]);
					}

					esp_zb_secur_ic_add(
					  device_ieee_addr, ESP_ZB_IC_TYPE_128, device_installation_code);
				}
				break;


			case GUI_CB_CLEAR_INSTALLATION_CODES_FLAG:
				{

					esp_zb_secur_ic_remove_all_req();
				}
				break;
		}
	}
}

/*****************************************************************************/

void switchCallback(BasicControl *sender, int type, void *param) {

	if (sender->getValueInt() == 1)
		ESPUI.updateLabel(factory_reset_label, factory_reset_enabled_str);
	else
		ESPUI.updateLabel(factory_reset_label, factory_reset_disabled_str);
}

void editDeviceCallback(BasicControl *sender, int type, void *param) {

	if ((type == B_UP) && (ESPUI.getControl(device_selector)->getValueInt() >= 0)) {

		uint8_t device_slot = ESPUI.getControl(device_selector)->getValueInt();

		switch ((uint32_t)param) {


			case GUI_CB_UPDATE_DEVICE_NAME_FLAG: {

				Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(
					device_slot);

        if (device) {
                
        	device->setDeviceLocalName(
            ESPUI.getControl(device_name_text)->getValueCstr(), false);

          device->clearDeviceUserDataFlags(
            ZBD_USER_DATA_FLAG_SUBDEVICE_REGISTERED, false);

          
          if (device->saveDeviceData()) {

            sortZbDevicesSelectors(device_slot);
          }
        }
			} break;
		}
	}
}

/*****************************************************************************/

uint32_t saveRemoteAddressData(uint8_t channel_slot) {

	working_str = ESPUI.getControl(param_1_text)->getValue();

	int8_t prefix_pos = working_str.indexOf("mdns://");

	Z2S_Core *z2s_core = Z2S_Core::getZ2SCoreByChannelIndex(channel_slot);

	if (prefix_pos >= 0) {

		z2s_core->setChannelUserDataFlags(
			USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS, false);
		z2s_core->setMDNSName(working_str.c_str());
	} 
	else {

		IPAddress ip;
		ip.fromString(working_str);

		z2s_core->clearChannelUserDataFlags(
		  USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS, false);

		z2s_core->setRemoteIPAddress(ip);
	}

	return z2s_core->checkChannelUserDataFlags(
		USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS) ? REMOTE_ADDRESS_TYPE_MDNS : 
			REMOTE_ADDRESS_TYPE_IP4;
}

/*****************************************************************************/

uint8_t saveRemoteChannelData(uint8_t channel_slot) {

	Z2S_Core *z2s_core = Z2S_Core::getZ2SCoreByChannelIndex(channel_slot);

	uint8_t Supla_remote_channel = ESPUI.getControl(
		param_2_number)->getValueInt();

	z2s_core->setSuplaRemoteChannel(Supla_remote_channel);

	return Supla_remote_channel;
}

/*****************************************************************************/

uint8_t saveSourceChannelData(uint8_t channel_slot) {

	Z2S_Core *z2s_core = Z2S_Core::getZ2SCoreByChannelIndex(channel_slot);

	uint8_t Supla_source_channel = ESPUI.getControl(
	  param_3_number)->getValueInt();

	z2s_core->setSourceChannel(Supla_source_channel);

	return Supla_source_channel;
}

/*****************************************************************************/

void editChannelCallback(BasicControl *sender, int type, void *param) {

	if ((type == B_UP) || (type == S_VALUE))
		addGUICommand(gui_cmd_edit_channel_data, (uint32_t)param);
}

/*****************************************************************************/

void editChannelMain(uint32_t update_channel_flag) {

	if (ESPUI.getControl(channel_selector)->getValueInt() >= 0) {

		uint8_t channel_slot = ESPUI.getControl(channel_selector)->getValueInt();

		Z2S_Core *z2s_core = Z2S_Core::getZ2SCoreByChannelIndex(channel_slot);

		switch (update_channel_flag) {


			case GUI_CB_UPDATE_CHANNEL_NAME_FLAG:
				{

					if (z2s_core->setZ2SChannelName(
					      ESPUI.getControl(channel_name_text)->getValueCstr(), true)) {

						sortChannelsSelectors(channel_slot);
					}
				}
				break;


			case GUI_CB_UPDATE_LOCAL_CHANNEL_FUNCTION_FLAG:
				{

					int16_t local_channel_function =
					  ESPUI.getControl(local_channel_function_selector)->getValueInt();

					if (local_channel_function > 0)
						z2s_core->setZ2SLocalChannelFunc(local_channel_function);
				}
				break;


			case GUI_CB_UPDATE_PARAM_1_FLAG:
				{


					switch (z2s_core->getZ2SChannelType()) {


						case SUPLA_CHANNELTYPE_LOCALCHANNEL:
							{

								switch (z2s_core->getZ2SLocalChannelType()) {

									case LOCAL_CHANNEL_TYPE_REMOTE_RELAY:
									case LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER:
									case LOCAL_CHANNEL_TYPE_VIRTUAL_THERM_HYGRO_METER:
									case LOCAL_CHANNEL_TYPE_CALC_THERMOMETER:

										saveRemoteAddressData(channel_slot);
										break;


									case LOCAL_CHANNEL_TYPE_GPIO_RELAY:
										{

											z2s_core->setLocalRelayGpioPin(
											  ESPUI.getControl(param_1_text)->getValueInt(), true);

											z2s_core->updateLocalRelayParams();
										}
										break;


									case LOCAL_CHANNEL_TYPE_GPIO_HVAC:
										{

											z2s_core->setLocalHvacGpioPin(
											  ESPUI.getControl(param_1_text)->getValueInt(), true);

											z2s_core->updateLocalHvacParams();
										}
										break;

									case LOCAL_CHANNEL_TYPE_GPIO_BINARY:
										{

											z2s_core->setLocalBinaryGpioPin(
											  ESPUI.getControl(param_1_text)->getValueInt(), true);

											z2s_core->updateLocalBinaryParams();
										}
										break;


									case LOCAL_CHANNEL_TYPE_GPIO_DS18B20:
										{

											uint8_t parsedBytes[8] = { 0 };

											if (hexToBytes(ESPUI.getControl(
											                      param_1_text)
											                 ->getValueCstr(),
											               parsedBytes)) {

												z2s_core->setLocalDS18B20Address(parsedBytes, true);

												z2s_core->updateLocalDs18b20Params();
											}
										}
										break;


									case LOCAL_CHANNEL_TYPE_GPIO_HCSR04:
										{

											long hcsr04_params[6];

											size_t params_found = parseCsvNumbers(
											  ESPUI.getControl(param_1_text)->getValueCstr(),
											  hcsr04_params, 6);

											switch (params_found) {
												case 6:
													z2s_core->setLocalHCSR04MaxOut(hcsr04_params[5]);
													[[fallthrough]];
												case 5:
													z2s_core->setLocalHCSR04MinOut(hcsr04_params[4]);
													[[fallthrough]];
												case 4:
													z2s_core->setLocalHCSR04MaxIn(hcsr04_params[3]);
													[[fallthrough]];
												case 3:
													z2s_core->setLocalHCSR04MinIn(hcsr04_params[2]);
													[[fallthrough]];
												case 2:
													z2s_core->setLocalHCSR04EchoPin(hcsr04_params[1]);
													[[fallthrough]];
												case 1:
													z2s_core->setLocalHCSR04TrigPin(hcsr04_params[0]);
													[[fallthrough]];
													z2s_core->saveChannelData();
													z2s_core->updateLocalHcsr04Params();
												default: break;
											}
										}
										break;
								}
							}
							break;

						case SUPLA_CHANNELTYPE_THERMOMETER:
						case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR:
							{

								saveRemoteAddressData(channel_slot);
							}
							break;


						case SUPLA_CHANNELTYPE_HVAC:

							updateHvacFixedCalibrationTemperature(
							  channel_slot,
							  ESPUI.getControl(param_1_text)->getValueInt());
							break;


						default:
							{

								z2s_core->setChannelUserData1(
								  ESPUI.getControl(param_1_text)->getValueInt());
							}
							break;
					}
				}
				break;

			case GUI_CB_UPDATE_PARAM_2_FLAG:
				{


					switch (z2s_core->getZ2SChannelType()) {


						case SUPLA_CHANNELTYPE_LOCALCHANNEL:
							{

								switch (z2s_core->getZ2SLocalChannelType()) {

									case LOCAL_CHANNEL_TYPE_REMOTE_RELAY:
									case LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER:
									case LOCAL_CHANNEL_TYPE_VIRTUAL_THERM_HYGRO_METER:
									case LOCAL_CHANNEL_TYPE_CALC_THERMOMETER:

										saveRemoteChannelData(channel_slot);
										break;


									case LOCAL_CHANNEL_TYPE_GPIO_RELAY:
										{

											z2s_core->setLocalRelayHighIsOn(
											  ESPUI.getControl(param_2_number)->getValueInt(), true);

											z2s_core->updateLocalRelayParams();
										}
										break;


									case LOCAL_CHANNEL_TYPE_GPIO_HVAC:
										{

											z2s_core->setLocalHvacHighIsOn(
											  ESPUI.getControl(param_2_number)->getValueInt(), true);

											z2s_core->updateLocalHvacParams();
										}
										break;


									case LOCAL_CHANNEL_TYPE_GPIO_BINARY:
										{

											z2s_core->setLocalBinaryInvertLogic(
											  ESPUI.getControl(param_2_number)->getValueInt(), true);

											z2s_core->updateLocalBinaryParams();
										}
										break;


									case LOCAL_CHANNEL_TYPE_GPIO_DS18B20:
										{

											z2s_core->setLocalDS18B20GpioPin(
											  ESPUI.getControl(param_2_number)->getValueInt(), true);

											z2s_core->updateLocalDs18b20Params();
										}
										break;
								}
							}
							break;


						case SUPLA_CHANNELTYPE_THERMOMETER:
						case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR:
							{

								saveRemoteChannelData(channel_slot);
							}
							break;


						default:
							{

								z2s_core->setChannelUserData1(
								  ESPUI.getControl(param_2_number)->getValueInt());
							}
							break;
					}
				}
				break;


			case GUI_CB_UPDATE_PARAM_3_FLAG:
				{


					switch (z2s_core->getZ2SChannelType()) {


						case SUPLA_CHANNELTYPE_LOCALCHANNEL:
							{

								switch (z2s_core->getZ2SLocalChannelType()) {

									case LOCAL_CHANNEL_TYPE_VIRTUAL_THERM_HYGRO_METER:
									case LOCAL_CHANNEL_TYPE_CALC_THERMOMETER:

										saveSourceChannelData(channel_slot);
										break;


									case LOCAL_CHANNEL_TYPE_GPIO_BINARY:
										{

											z2s_core->setLocalBinaryPullUp(
											  ESPUI.getControl(param_3_number)->getValueInt(), true);

											z2s_core->updateLocalBinaryParams();
										}
										break;
								}
							}
							break;
					}
				}
				break;


			case GUI_CB_UPDATE_KEEPALIVE_FLAG:
				{

					z2s_core->setKeepAliveValue(
					  ESPUI.getControl(keepalive_number)->getValueInt());
				}
				break;


			case GUI_CB_UPDATE_TIMEOUT_FLAG:
				{

					z2s_core->setTimeoutValue(
					  ESPUI.getControl(timeout_number)->getValueInt());
				}
				break;


			case GUI_CB_UPDATE_REFRESH_FLAG:
				{

					z2s_core->setRefreshValue(
					  ESPUI.getControl(refresh_number)->getValueInt());
				}
				break;
		}
	}
}

void editChannelFlagsCallback(BasicControl *sender, int type, void *param) {

	if (ESPUI.getControl(channel_selector)->getValueInt() >= 0) {

		uint8_t channel_slot = ESPUI.getControl(channel_selector)->getValueInt();

		auto z2s_core = Z2S_Core::getZ2SCoreByChannelIndex(channel_slot);

		switch ((uint32_t)param) {


			case GUI_CB_DISABLE_CHANNEL_NOTIFICATIONS_FLAG:
				{

					if (type == S_ACTIVE)
						z2s_core->setChannelUserDataFlags(
							USER_DATA_FLAG_DISABLE_NOTIFICATIONS);
					else
						z2s_core->clearChannelUserDataFlags(
						  USER_DATA_FLAG_DISABLE_NOTIFICATIONS);
				}
				break;


			case GUI_CB_TRV_AUTO_TO_SCHEDULE_FLAG:
				{

					if (type == S_ACTIVE)
						z2s_core->setChannelUserDataFlags(
						  USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE);
					else
						z2s_core->clearChannelUserDataFlags(
						  USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE);
				}
				break;


			case GUI_CB_TRV_AUTO_TO_SCHEDULE_MANUAL_FLAG:
				{

					if (type == S_ACTIVE)
						z2s_core->setChannelUserDataFlags(
						  USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE_MANUAL);
					else
						z2s_core->clearChannelUserDataFlags(
						  USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE_MANUAL);
				}
				break;


			case GUI_CB_TRV_FIXED_CALIBRATION_FLAG:
				{

					if (type == S_ACTIVE)
						z2s_core->setChannelUserDataFlags(
						  USER_DATA_FLAG_TRV_FIXED_CORRECTION);
					else {
						z2s_core->clearChannelUserDataFlags(
						  USER_DATA_FLAG_TRV_FIXED_CORRECTION);

						updateHvacFixedCalibrationTemperature(channel_slot, 0);
					}

					addGUICommand(gui_cmd_update_channel_info, -2);
				}
				break;


			case GUI_CB_TRV_COOPERATIVE_CHILDLOCK_FLAG: {

				if (type == S_ACTIVE)
					z2s_core->setChannelUserDataFlags(
						USER_DATA_FLAG_TRV_COOPERATIVE_CHILDLOCK);
				else
					z2s_core->clearChannelUserDataFlags(
						USER_DATA_FLAG_TRV_COOPERATIVE_CHILDLOCK);
			} break;


			case GUI_CB_ENABLE_RESEND_TEMPERATURE_FLAG: {

				if (type == S_ACTIVE) {

					z2s_core->setChannelUserDataFlags(
					  USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE);	
				}
				else {

					z2s_core->setRemoteIPAddress(0);
					z2s_core->setSuplaRemoteChannel(0xFF);

					z2s_core->clearChannelUserDataFlags(
						USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE | 
						USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS);
				}

				addGUICommand(gui_cmd_update_channel_info, -2);
			} break;


			case GUI_CB_SET_SORWNS_ON_START_FLAG:
				{

					if (type == S_ACTIVE)
						z2s_core->setChannelUserDataFlags(
						  USER_DATA_FLAG_SET_SORWNS_ON_START);
					else
						z2s_core->clearChannelUserDataFlags(
						  USER_DATA_FLAG_SET_SORWNS_ON_START);
				}
				break;


			case GUI_CB_SKIP_SUBDEVICE_REGISTRATION_FLAG:
				{

					if (type == S_ACTIVE)
						z2s_core->setChannelUserDataFlags(
						  USER_DATA_FLAG_SKIP_SUBDEVICE_REGISTRATION);
					else
						z2s_core->clearChannelUserDataFlags(
						  USER_DATA_FLAG_SKIP_SUBDEVICE_REGISTRATION);
				}
				break;
		}
	}
}

/*****************************************************************************/

void batteryCallback(BasicControl *sender, int type, void *param) {

  if ((type == B_UP) && 
      (ESPUI.getControl(device_selector)->getValueInt() >= 0)) {

    Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(
      ESPUI.getControl(device_selector)->getValueInt());

    if (device) {

      switch ((uint32_t)param) {

        case GUI_CB_BATTERY_VOLTAGE_MIN_FLAG: {

          device->setBatteryVoltageMin(
            ESPUI.getControl(battery_voltage_min_number)->getValueInt(), 
            false);

          device->saveDeviceData();
        } break;


        case GUI_CB_BATTERY_VOLTAGE_MAX_FLAG: {

          device->setBatteryVoltageMax(
            ESPUI.getControl(battery_voltage_max_number)->getValueInt(), 
            false);

          device->saveDeviceData();
        } break;
      }
    }
  }
}

/*****************************************************************************/

void batterySwitcherCallback(BasicControl *sender, int type, void *param) {

  if (ESPUI.getControl(device_selector)->getValueInt() >= 0) {

    Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(
      ESPUI.getControl(device_selector)->getValueInt());

    if (device) {

      switch ((uint32_t)param) {

        case GUI_CB_DISABLE_VOLTAGE_MSG_FLAG: {

          if (type == S_ACTIVE)
            device->setDeviceUserDataFlags(
              ZBD_USER_DATA_FLAG_DISABLE_BATTERY_VOLTAGE_MSG, false);
          else
            device->clearDeviceUserDataFlags(
              ZBD_USER_DATA_FLAG_DISABLE_BATTERY_VOLTAGE_MSG, false);

          device->saveDeviceData();
        } break;


        case GUI_CB_DISABLE_PERCENTAGE_MSG_FLAG: {

          if (type == S_ACTIVE)
            device->setDeviceUserDataFlags(
              ZBD_USER_DATA_FLAG_DISABLE_BATTERY_PERCENTAGE_MSG, false);
          else
            device->clearDeviceUserDataFlags(
              ZBD_USER_DATA_FLAG_DISABLE_BATTERY_PERCENTAGE_MSG, false);

          device->saveDeviceData();
        } break;
      }
    }
  }
}

/*****************************************************************************/

void datatypeCallback(BasicControl *sender, int type, void *param) {

	int8_t device_attribute_type_selector_value =
	  ESPUI.getControl(
	         clusters_attributes_table[device_attribute_type_selector])
	    ->getValueInt();

	if (device_attribute_type_selector_value >= 0)
		ESPUI.updateNumber(
		  clusters_attributes_table[device_attribute_size_number],
		  zigbee_datatypes[device_attribute_type_selector_value].zigbee_datatype_size);
}

/*****************************************************************************/

void TuyaDatapointIdSelectorCallback(BasicControl *sender, int type, void *param) {

	long Tuya_datapoint_id_selector_value =
	  ESPUI.getControl(
	         Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector])
	    ->getValueInt();
	//log_i("Tuya_datapoint_id_selector_value")
	if (Tuya_datapoint_id_selector_value >= 0) {


		ESPUI.updateNumber(
		  Tuya_devices_tab_controls_table[Tuya_datapoint_id_number],
		  Tuya_datapoints[Tuya_datapoint_id_selector_value].Tuya_datapoint_id);

		ESPUI.updateSelect(
		  Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector],
		  Tuya_datapoints[Tuya_datapoint_id_selector_value].Tuya_datapoint_type);

		//this is bit tricky as it assumes that type value = table index,
		//which should be OK, since we have fixed number of positions
		uint8_t datapoint_type_slot =
		  Tuya_datapoints[Tuya_datapoint_id_selector_value].Tuya_datapoint_type;

		ESPUI.updateNumber(
		  Tuya_devices_tab_controls_table[Tuya_datapoint_length_number],
		  Tuya_datapoint_types[datapoint_type_slot].Tuya_datapoint_type_length);

		working_str =
		  Tuya_datapoints[Tuya_datapoint_id_selector_value].Tuya_datapoint_description;
		ESPUI.updateLabel(
		  Tuya_devices_tab_controls_table[Tuya_datapoint_description_label],
		  working_str);
	} else
		ESPUI.updateSelect(
		  Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector],
		  -1);
}

/*****************************************************************************/

void TuyaDatapointTypeSelectorCallback(BasicControl *sender, int type, void *param) {

	int8_t Tuya_datapoint_type_selector_value =
	  ESPUI.getControl(
	         Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector])
	    ->getValueInt();

	if (Tuya_datapoint_type_selector_value >= 0)
		ESPUI.updateNumber(
		  Tuya_devices_tab_controls_table[Tuya_datapoint_length_number],
		  Tuya_datapoint_types[Tuya_datapoint_type_selector_value].Tuya_datapoint_type_length);
}

/*****************************************************************************/

void attributeCallback(BasicControl *sender, int type, void *param) {

	current_attribute_idx = type;

	if (sender)
		current_attribute_idx = ESPUI.getControl(
		                               clusters_attributes_table[device_attribute_id_selector])
		                          ->getValueInt();

	if (current_attribute_idx >= 0) {

		uint32_t zigbee_datatypes_count = sizeof(zigbee_datatypes) / sizeof(zigbee_datatype_t);

		for (uint8_t datatypes_counter = 0; datatypes_counter < zigbee_datatypes_count; datatypes_counter++) {

			if (zigbee_datatypes[datatypes_counter].zigbee_datatype_id == zigbee_attributes[current_attribute_idx].zigbee_attribute_datatype_id) {

				ESPUI.updateSelect(
				  clusters_attributes_table[device_attribute_type_selector],
				  datatypes_counter);
				ESPUI.updateNumber(
				  clusters_attributes_table[device_attribute_size_number],
				  zigbee_datatypes[datatypes_counter].zigbee_datatype_size);
				break;
			}
		}
		ESPUI.updateNumber(
		  clusters_attributes_table[device_attribute_id_text],
		  zigbee_attributes[current_attribute_idx].zigbee_attribute_id);

		ESPUI.updateNumber(
		  clusters_attributes_table[device_attribute_value_selector], -1);

		ESPUI.updateNumber(
		  clusters_attributes_table[device_attribute_value_text], 0);
	} else {

		ESPUI.updateSelect(
		  clusters_attributes_table[device_attribute_type_selector], -1);
		ESPUI.updateNumber(
		  clusters_attributes_table[device_attribute_size_number], 0);
		ESPUI.updateNumber(
		  clusters_attributes_table[device_attribute_id_text], 0);
		ESPUI.updateNumber(
		  clusters_attributes_table[device_attribute_value_selector], -1);

		ESPUI.updateNumber(
		  clusters_attributes_table[device_attribute_value_text], 0);
	}
	sortAttributeValuesSelectors();
}

/*****************************************************************************/

void clusterCallback(BasicControl *sender, int type, void *param) {

	if (sender->getValueInt() >= -1) {

		addGUICommand(gui_cmd_cluster_callback);
	}
}

/*****************************************************************************/

void valueCallback(BasicControl *sender, int type, void *param) {

	int8_t sender_value = sender->getValueInt();  //max 128 attribute values

	if (sender_value >= 0) {

		int8_t attribute_type_idx = ESPUI.getControl(
		                                   clusters_attributes_table[device_attribute_type_selector])
		                              ->getValueInt();

		uint8_t attribute_type = attribute_type_idx >= 0 ? zigbee_datatypes[attribute_type_idx].zigbee_datatype_id : 0xFF;

		if (attribute_type == ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING) {

			ESPUI.updateNumber(
			  clusters_attributes_table[device_attribute_size_number],
			  strlen(
			    zigbee_attribute_values[sender_value].zigbee_attribute_value_str)
			    / 2);
			ESPUI.updateText(
			  clusters_attributes_table[device_attribute_value_text],
			  zigbee_attribute_values[sender_value].zigbee_attribute_value_str);
		} else
			ESPUI.updateNumber(
			  clusters_attributes_table[device_attribute_value_text],
			  zigbee_attribute_values[sender_value].zigbee_attribute_value);
	} else
		ESPUI.updateNumber(
		  clusters_attributes_table[device_attribute_value_text], 0);
}

/*****************************************************************************/

void advancedDeviceSelectorCallback(
  BasicControl *sender, int type, void *param) {

  char general_purpose_gui_buffer[512] = {};

  int sender_value = type;

  if (sender)
    sender_value = sender->getValueInt();

  Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(sender_value);

  if (!device) {

    updateLabel_C(advanced_device_info_label, three_dots_str);

    if (isSonoffValvePresent)
      enableSonoffValveGUI(false);
    if (isSonoffDualValvePresent)
      enableSonoffDualValveGUI(false);
    if (isTuyaGasDetectorPresent)
      enableTuyaGasDetectorGUI(false);
    if (isMoesAlarmPresent)
      enableMoesAlarmGUI(false);
    return;
  }

  log_i("sender_value %d", sender_value);

  uint32_t desc_id = device->getDeviceDescId();

  if (desc_id == Z2S_DEVICE_DESC_SONOFF_SMART_VALVE)
    enableSonoffValveGUI(true);
  else {
    if (isSonoffValvePresent)
      enableSonoffValveGUI(false);
  }

  if (desc_id == Z2S_DEVICE_DESC_SONOFF_SMART_DUAL_VALVE)
    enableSonoffDualValveGUI(true);
  else {
    if (isSonoffDualValvePresent)
      enableSonoffDualValveGUI(false);
  }

  if (desc_id == Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR)
    enableTuyaGasDetectorGUI(true);
  else {
    if (isTuyaGasDetectorPresent)
      enableTuyaGasDetectorGUI(false);
  }

  if (desc_id == Z2S_DEVICE_DESC_MOES_ALARM)
    enableMoesAlarmGUI(true);
  else {
    if (isMoesAlarmPresent)
      enableMoesAlarmGUI(false);
  }

  sprintf_P(
    general_purpose_gui_buffer, "<b><i>Manufacturer name</i></b> %s <b>| <i>"
  	"model ID</b></i> %s", device->getManufacturerName(),
		device->getModelName());

  updateLabel_P(advanced_device_info_label, general_purpose_gui_buffer);
}
/*****************************************************************************/

void gatewayCallback(BasicControl *sender, int type, void *param) {

	switch ((uint32_t)param) {


		case GUI_CB_ENABLE_GUI_FLAG:
			{

				if (Supla::Storage::ConfigInstance()->setInt8(Z2S_ENABLE_GUI_ON_START_V2,
				                                              ESPUI.getControl(enable_gui_switcher)->getValueInt()))
					Supla::Storage::ConfigInstance()->commit();
			}
			break;


		case GUI_CB_FORCE_CONFIG_FLAG:
			{

				if (Supla::Storage::ConfigInstance()->setUInt8(Z2S_FORCE_CONFIG_ON_START,
				                                               ESPUI.getControl(force_config_switcher)->getValueInt()))
					Supla::Storage::ConfigInstance()->commit();
			}
			break;


		case GUI_CB_GUI_DELAY_FLAG:
			{

				if (Supla::Storage::ConfigInstance()->setInt32(Z2S_GUI_ON_START_DELAY_V2,
				                                               ESPUI.getControl(gui_start_delay_number)->getValueInt()))
					Supla::Storage::ConfigInstance()->commit();
			}
			break;


		case GUI_CB_ACR_TIMEOUT_FLAG:
			{

				int32_t auto_connection_reset_timeout = ESPUI.getControl(
				                                               auto_connection_reset_timeout_number)
				                                          ->getValueInt();

				if (Supla::Storage::ConfigInstance()->setInt32(
				      Z2S_AUTO_CONNECTION_RESET_TIMEOUT,
				      auto_connection_reset_timeout)) {

					Supla::Storage::ConfigInstance()->commit();

					_auto_connection_reset_timeout = auto_connection_reset_timeout;
					SuplaDevice.setAutomaticResetOnConnectionProblem(
					  _auto_connection_reset_timeout);
				}
			}
			break;


		case GUI_CB_SAVE_MDNS_NAME_FLAG:
			{

				if (Supla::Storage::ConfigInstance()->setString(Z2S_GATEWAY_MDNS_LOCAL_NAME,
				                                                ESPUI.getControl(gateway_mdns_name_text)->getValueCstr()))
					Supla::Storage::ConfigInstance()->commit();
			}
			break;


			/*case GUI_CB_REBUILD_CHANNELS_FLAG: {

			if (Supla::Storage::ConfigInstance()->setUInt8(Z2S_REBUILD_CHANNELS_ON_START, 
					ESPUI.getControl(rebuild_Supla_channels_switcher)->getValueInt()))
      	Supla::Storage::ConfigInstance()->commit();
		} break;*/


		case GUI_CB_USE_NEW_AT_FLAG:
			{

				uint8_t use_new_at_model =
				  ESPUI.getControl(use_new_at_model_switcher)->getValueInt();

				if (Supla::Storage::ConfigInstance()->setUInt8(Z2S_USE_NEW_AT_MODEL,
				                                               use_new_at_model))
					Supla::Storage::ConfigInstance()->commit();

				_use_new_at_model = use_new_at_model;
			}
			break;


		case GUI_CB_GUI_RESTART_FLAG:
			{

				SuplaDevice.softRestart();
			}
			break;


		case GUI_CB_GUI_CHECK_GITHUB_OTA_FLAG:

			if (type == B_UP)
				addGUICommand(gui_cmd_check_github_ota);
			break;


		case GUI_CB_GUI_PERFORM_GITHUB_OTA_FLAG:

			if (type == B_UP)
				addGUICommand(gui_cmd_perform_github_ota);
			break;
	}
}

/*****************************************************************************/

void TuyaCustomCmdCallback(BasicControl *sender, int type, void *param) {

	if (type == B_UP)
		addGUICommand(gui_cmd_Tuya_custom_cmd_callback, (int32_t)param);
}

/*****************************************************************************/

void TuyaCustomCmdMain(int32_t custom_flag) {

	char general_purpose_gui_buffer[512] = {};

	if (ESPUI.getControl(Tuya_devices_tab_controls_table[
		Tuya_device_selector])->getValueInt() >= 0) {

		uint8_t Tuya_device_selector_value = ESPUI.getControl(
			Tuya_devices_tab_controls_table[
				Tuya_device_selector])->getValueInt();

		zbg_device_params_t device;
		log_i(
		  "device_selector value %u, param id %d", Tuya_device_selector_value,
		  custom_flag);

		device.endpoint = 1;
		device.cluster_id = TUYA_PRIVATE_CLUSTER_EF00;

		uint8_t cmd_id;

		uint16_t tsn_id = random(0x0000, 0xFFFF);

		Z2S_Device *z2s_device = Z2S_Device::getZ2SDeviceByDeviceIndex(
			Tuya_device_selector_value);

    if (z2s_device) {

      memcpy(
        &device.ieee_addr,
        z2s_device->getDeviceIeeeAddress(),
        sizeof(esp_zb_ieee_addr_t));

      device.short_addr = z2s_device->getDeviceShortAddress();
    }
		else 
			return;

		switch (custom_flag) {

			case GUI_CB_SEND_TUYA_REQUEST_FLAG:
				{

					uint8_t cmd_dp_id = ESPUI.getControl(
						Tuya_devices_tab_controls_table[
							Tuya_datapoint_id_number])->getValueInt();

					if (cmd_dp_id == 0) {

						updateLabel_C(
						  Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label],
						  "Tuya datapoint id must be in range [1...255]!");
						return;
					}

					int8_t cmd_dp_type = ESPUI.getControl(
					  Tuya_devices_tab_controls_table[
							Tuya_datapoint_type_selector])->getValueInt();

					if (cmd_dp_type < 0) {

						updateLabel_C(
						  Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label],
						  "Tuya datapoint type not selected!");
						return;
					}

					uint8_t cmd_dp_length = ESPUI.getControl(
						Tuya_devices_tab_controls_table[
							Tuya_datapoint_length_number])->getValueInt();

					if (cmd_dp_type > 58) {

						updateLabel_C(
						  Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label],
						  "Tuya datapoint length must be in range [1..58]!");
						return;
					}

					int32_t cmd_dp_value = ESPUI.getControl(
					  Tuya_devices_tab_controls_table[
							Tuya_datapoint_value_number])->getValueInt();

					Tuya_custom_cmd_dp = cmd_dp_id;

					current_Tuya_payload_label =
					  Tuya_devices_tab_controls_table[Tuya_device_payload_label];

					uint8_t Tuya_data_cmd = TUYA_REQUEST_CMD;

					if (z2s_device->checkDeviceUserDataFlags(
								ZBD_USER_DATA_FLAG_TUYA_USE_SEND_DATA))
						Tuya_data_cmd = TUYA_SEND_CMD;

					bool result = sendTuyaRequestCmdData(
					  &zbGateway, &device, cmd_dp_id, cmd_dp_type, cmd_dp_value, true,
					  Tuya_data_cmd);

					if (result) {

						if (*zbGateway.getCustomCmdStatusLastResult() == 
								ESP_ZB_ZCL_STATUS_SUCCESS) {

							sprintf_P(
							  general_purpose_gui_buffer, "Tuya custom cluster data request "
								"sent successfully! <br>Datapoint id   = %u(0x%02X)<br>"
							  "Datapoint type = %s(0x%02X)<br>Datapoint length 	= %u(0x%02X)"
							  "<br>Datapoint value = %u(0x%02X)",
							  cmd_dp_id, cmd_dp_id,
							  Tuya_datapoint_types[cmd_dp_type].Tuya_datapoint_type_name,
							  Tuya_datapoint_types[cmd_dp_type].Tuya_datapoint_type_id,
							  cmd_dp_length, cmd_dp_length, cmd_dp_value, cmd_dp_value);

							updateLabel_P(
							  Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label],
							  general_purpose_gui_buffer);
						} else {

							sprintf_P(
							  general_purpose_gui_buffer, "Tuya custom cluster data request "
							                              "failed! <br>Status         = %u(0x%02X)<br>Datapoint id   = "
							                              "%u(0x%02X)<br>Datapoint type = %s(0x%02X)<br>Datapoint length"
							                              " 	= %u(0x%02X)<br>Datapoint value = %u(0x%02X)",
							  *zbGateway.getCustomCmdStatusLastResult(),
							  *zbGateway.getCustomCmdStatusLastResult(),
							  cmd_dp_id, cmd_dp_id,
							  Tuya_datapoint_types[cmd_dp_type].Tuya_datapoint_type_name,
							  Tuya_datapoint_types[cmd_dp_type].Tuya_datapoint_type_id,
							  cmd_dp_length, cmd_dp_length, cmd_dp_value, cmd_dp_value);

							updateLabel_P(
							  Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label],
							  general_purpose_gui_buffer);
							updateLabel_C(
							  Tuya_devices_tab_controls_table[Tuya_device_payload_label],
							  three_dots_str);
						}
					} else {

						updateLabel_C(
						  Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label],
						  device_query_failed_str);
						updateLabel_C(
						  Tuya_devices_tab_controls_table[Tuya_device_payload_label],
						  three_dots_str);
					}
				}
				break;

			case GUI_CB_SEND_TUYA_QUERY_FLAG:
				{

					updateLabel_C(
					  Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label],
					  three_dots_str);
					updateLabel_C(
					  Tuya_devices_tab_controls_table[Tuya_device_payload_label],
					  three_dots_str);

					uint8_t cmd_dp_id = ESPUI.getControl(
					                           Tuya_devices_tab_controls_table[Tuya_datapoint_id_number])
					                      ->getValueInt();

					Tuya_custom_cmd_dp = cmd_dp_id;

					current_Tuya_payload_label =
					  Tuya_devices_tab_controls_table[Tuya_device_payload_label];

					bool result = sendTuyaQueryCmd(&zbGateway, &device, true);

					if (result) {

						if (*zbGateway.getCustomCmdStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS) {

							sprintf_P(
							  general_purpose_gui_buffer,
							  "Tuya custom cluster data query sent successfully!");

							updateLabel_P(
							  Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label],
							  general_purpose_gui_buffer);
						} else {

							sprintf_P(
							  general_purpose_gui_buffer, "Tuya custom cluster data query "
							                              "failed! <br>Status = %u(0x%02X)",
							  *zbGateway.getCustomCmdStatusLastResult(),
							  *zbGateway.getCustomCmdStatusLastResult());

							updateLabel_P(
							  Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label],
							  general_purpose_gui_buffer);
						}
					} else {

						updateLabel_C(
						  Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label],
						  device_query_failed_str);

						updateLabel_C(
						  Tuya_devices_tab_controls_table[Tuya_device_payload_label],
						  three_dots_str);
					}
				}
				break;
		}
	}
}

/*****************************************************************************/

void updateActionSummary(z2s_channel_action_t &new_action) {

	current_action_id = Z2S_findNextActionPosition(0);

	if (current_action_id >= 0) {

		current_action_counter = Z2S_getActionCounter(current_action_id);
		Z2S_loadAction(current_action_id, new_action);
		//sprintfAction(new_action);
		//ESPUI.updateLabel(actions_table_label, working_str);
		addGUICommand(
			gui_cmd_build_actions_table, current_action_counter - 1);
	} 
	else {

		current_action_counter = -1;
		//working_str = "Actions table is empty.";
		//ESPUI.updateLabel(actions_table_label, working_str);
		addGUICommand(gui_cmd_build_actions_table, 0);
	}
}
/*****************************************************************************/

void actionsTableCallback(BasicControl *sender, int type, void *param) {

	if (type == B_UP) {

		z2s_channel_action_t new_action = {};

		working_str = "No actions";

		switch ((uint32_t)param) {


			case GUI_CB_ACTION_FIRST_FLAG:
				{

					int16_t first_action = Z2S_findNextActionPosition(0);
					if (first_action >= 0) {

						current_action_id = first_action;
						current_action_counter = Z2S_getActionCounter(first_action);

						Z2S_loadAction(first_action, new_action);
						//sprintfAction(new_action);
						//ESPUI.updateLabel(actions_table_label, working_str);
						updateActionDetails(new_action);
						sendActionsTableHighlight(
							actions_table_label_2, current_action_counter - 1);
					}
				}
				break;


			case GUI_CB_ACTION_NEXT_FLAG:
				{

					if (current_action_id < 0)
						return;

					int16_t next_action = Z2S_findNextActionPosition(current_action_id + 1);
					if (next_action >= 0) {

						current_action_id = next_action;
						current_action_counter = Z2S_getActionCounter(next_action);

						Z2S_loadAction(next_action, new_action);
						//sprintfAction(new_action);
						//ESPUI.updateLabel(actions_table_label, working_str);
						sendActionsTableHighlight(
							actions_table_label_2, current_action_counter - 1);
						updateActionDetails(new_action);
					}
				}
				break;


			case GUI_CB_ACTION_PREV_FLAG:
				{

					if (current_action_id <= 0)
						return;

					int16_t prev_action = Z2S_findPrevActionPosition(current_action_id - 1);
					if (prev_action >= 0) {

						current_action_id = prev_action;
						current_action_counter = Z2S_getActionCounter(prev_action);

						Z2S_loadAction(prev_action, new_action);
						//sprintfAction(new_action);
						//ESPUI.updateLabel(actions_table_label, working_str);
						sendActionsTableHighlight(
							actions_table_label_2, current_action_counter - 1);
						updateActionDetails(new_action);
					}
				}
				break;


			case GUI_CB_ACTION_LAST_FLAG:
				{

					if (current_action_id < 0)
						return;

					int16_t last_action = Z2S_findPrevActionPosition(Z2S_ACTIONS_MAX_NUMBER);
					if (last_action >= 0) {

						current_action_id = last_action;
						current_action_counter = Z2S_getActionCounter(last_action);

						Z2S_loadAction(last_action, new_action);
						//sprintfAction(new_action);
						//ESPUI.updateLabel(actions_table_label, working_str);
						sendActionsTableHighlight(
							actions_table_label_2, current_action_counter - 1);
						updateActionDetails(new_action);
					}
				}
				break;


			case GUI_CB_ACTION_EDIT_FLAG:
				{

					if ((current_action_gui_state != VIEW_ACTION) || (current_action_id == -1))
						return;

					previous_action_gui_state = current_action_gui_state;
					current_action_gui_state = EDIT_ACTION;
					Z2S_loadAction(current_action_id, new_action);
					enableActionDetails(true);
					updateActionDetails(new_action);
					updateActionButtons();
					enableActionsTable(false);
				}
				break;


			case GUI_CB_ACTION_COPY_FLAG:
				{

					if ((current_action_gui_state != VIEW_ACTION) || (current_action_id == -1))
						return;

					new_action_id = Z2S_findFreeActionIndex();

					if (new_action_id == -1) {

						ESPUI.updateLabel(action_state_label, "Actions table is full - can't add new Z2S Action!");
						return;
					}

					previous_action_gui_state = current_action_gui_state;
					current_action_gui_state = COPY_ACTION;
					Z2S_loadAction(current_action_id, new_action);
					enableActionDetails(true);
					updateActionDetails(new_action);
					updateActionButtons();
					enableActionsTable(false);
				}
				break;


			case GUI_CB_ACTION_NEW_FLAG:
				{

					if (current_action_gui_state != VIEW_ACTION)
						return;

					new_action_id = Z2S_findFreeActionIndex();
					if (new_action_id == -1) {

						ESPUI.updateLabel(
							action_state_label, "Actions table is full - can't add new "
							"Z2S Action!");
						return;
					} 
					else {
						
						previous_action_gui_state = current_action_gui_state;
						current_action_gui_state = NEW_ACTION;
						enableActionDetails(true);
						updateActionButtons();
						updateActionDetails(new_action, true);
						enableActionsTable(false);
					}
				}
				break;

			case GUI_CB_ACTION_SAVE_FLAG:
				{

					if ((current_action_gui_state != NEW_ACTION) && 
							(current_action_gui_state != EDIT_ACTION) && 
							(current_action_gui_state != COPY_ACTION))
						return;

					if (!fillActionDetails(new_action)) {
						
						ESPUI.updateLabel(
							action_state_label, "Required action data is missing!");
						return;
					}

					bool save_result = false;

					const char *save_result_success_str = "Saving Z2S Action: SUCCESS!"
					  "<br>Action has been activated!";
					const char *save_result_enabled_str = "Saving Z2S Action: SUCCESS!"
						"<br>Action has been enabled!";
					const char *save_result_disabled_str = "Saving Z2S Action: SUCCESS!"
					  "<br>Action has been disabled!";
					const char *save_result_saved_str = "Saving Z2S Action: SUCCESS!";
					const char *save_result_failed_str = "Saving Z2S Action: FAILED!";
					const char *save_result_no_changes_str = "No changes detected!";

					const char *save_result_str = save_result_success_str;

					bool send_edit = false;

					previous_action_gui_state = SAVE_ACTION;

					if ((current_action_gui_state == NEW_ACTION) || 
							(current_action_gui_state == COPY_ACTION)) {

						current_action_id = new_action_id;

						save_result = Z2S_saveAction(
						  current_action_id, new_action, CR_SAVE_REBUILD);

						if (!save_result)
							save_result_str = save_result_failed_str;

						current_action_counter = Z2S_getActionCounter(current_action_id);
					} 
					else {

						z2s_channel_action_t old_action = {};
						Z2S_loadAction(current_action_id, old_action);
						ActionCompareResult compare_result = Z2S_compareAction(
						  old_action, new_action);

						switch (compare_result) {


							case CR_SAVE_REBUILD:
								{

									Z2S_removeAction(current_action_id, old_action);
									save_result = Z2S_saveAction(
									  current_action_id, new_action, compare_result);
									if (!save_result)
										save_result_str = save_result_failed_str;

									log_i("Edit action: delete, save, activate");
								}
								break;


							case CR_SAVE_ONLY:
								{

									save_result = Z2S_saveAction(
									  current_action_id, new_action, compare_result);
									if (save_result)
										save_result_str = save_result_saved_str;
									else
										save_result_str = save_result_failed_str;

									log_i("Edit action: only save required");
								}
								break;


							case CR_SAVE_ENABLE:
								{

									save_result = Z2S_saveAction(
									  current_action_id, new_action, compare_result);
									if (save_result)
										save_result_str = save_result_enabled_str;
									else
										save_result_str = save_result_failed_str;

									log_i("Edit action: enabled.");
								}
								break;


							case CR_SAVE_DISABLE:
								{

									save_result = Z2S_saveAction(
									  current_action_id, new_action, compare_result);
									if (save_result)
										save_result_str = save_result_disabled_str;
									else
										save_result_str = save_result_failed_str;

									log_i("Edit action: disabled.");
								}
								break;


							case CR_NO_CHANGES:
								{

									save_result_str = save_result_no_changes_str;

									log_i("Edit action: no changes detected");
								}
								break;
						}
						send_edit = true;
					}

					current_action_gui_state = VIEW_ACTION;

					enableActionDetails(false);
					enableActionControls(true);
					//sprintfAction(new_action);
					//ESPUI.updateLabel(actions_table_label, working_str);
					if (send_edit)
						addGUICommand(
							gui_cmd_edit_actions_table, current_action_counter - 1);
					else
						addGUICommand(
							gui_cmd_build_actions_table, current_action_counter - 1);
					updateActionButtons();

					ESPUI.updateLabel(action_state_label, save_result_str);
				}
				break;

			case GUI_CB_ACTION_CANCEL_FLAG:
				{

					if ((current_action_gui_state != NEW_ACTION) && 
							(current_action_gui_state != EDIT_ACTION) && 
							(current_action_gui_state != COPY_ACTION))
						return;

					previous_action_gui_state = CANCEL_ACTION;
					current_action_gui_state = VIEW_ACTION;
					enableActionDetails(false);
					enableActionControls(true);
					updateActionButtons();
					ESPUI.updateLabel(action_state_label, "Changes cancelled!");
					enableActionsTable(true);
				}
				break;

			case GUI_CB_ACTION_REMOVE_FLAG:
				{

					if ((current_action_gui_state != VIEW_ACTION) || 
							(current_action_id == -1))
						return;

					previous_action_gui_state = REMOVE_ACTION;

					Z2S_loadAction(current_action_id, new_action);

					bool remove_result = Z2S_removeAction(current_action_id, new_action);
					updateActionSummary(new_action);
					current_action_gui_state = VIEW_ACTION;
					enableActionDetails(false);
					enableActionControls(true);
					updateActionButtons();

					if (remove_result)
						ESPUI.updateLabel(
						  action_state_label, "Removing Z2S Action: SUCCESS! <br>"
						  "Action has been deactivated!");
					else
						ESPUI.updateLabel(action_state_label, "Removing Z2S Action: FAILED!");
				}
				break;
		}
	}
}

/*****************************************************************************/

void saveValveProgram(uint8_t device_slot) {

  Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(device_slot);

  if (!device)
    return;

  if ((ESPUI.getControl(valve_program_selector)->getValueInt() > 0) && 
      (ESPUI.getControl(valve_cycles_number)->getValueInt() > 0)) {

    uint8_t program_mode = ESPUI.getControl(
      valve_program_selector)->getValueInt();

    uint8_t valve_cycles = ESPUI.getControl(
      valve_cycles_number)->getValueInt();

    uint32_t value_32 = 0;

    if (program_mode == 1)
      value_32 = ESPUI.getControl(valve_worktime_number)->getValueInt();
    else
      value_32 = ESPUI.getControl(valve_volume_number)->getValueInt();

    device->setSmartValveDataProgram(program_mode, false);
    device->setSmartValveDataCycles(valve_cycles, false);
    device->setSmartValveDataValue(value_32, false);
    device->setSmartValveDataPauseTime(
      ESPUI.getControl(valve_pause_number)->getValueInt(), false);

    if (device->saveDeviceData())
      ESPUI.updateLabel(
        valve_info_label, "Valve program saved successfully.");
    else
      ESPUI.updateLabel(valve_info_label, "Valve program save failed.");

  } else {
    ESPUI.updateLabel(valve_info_label, "No valve program to save");
  }
}

/*****************************************************************************/

void loadValveProgram(uint8_t device_slot) {

  Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(device_slot);

  if (!device)
    return;

  uint8_t valve_cycles = device->getSmartValveDataCycles();

  if (valve_cycles > 0) {

    uint8_t program_mode = device->getSmartValveDataProgram();

    ESPUI.updateNumber(valve_program_selector, program_mode);
    ESPUI.updateNumber(valve_cycles_number, valve_cycles);

    uint32_t value_32 = device->getSmartValveDataValue();

    if (program_mode == 1) {

      ESPUI.updateNumber(valve_worktime_number, value_32);
      ESPUI.updateNumber(valve_volume_number, 0);
    } else {

      ESPUI.updateNumber(valve_volume_number, value_32);
      ESPUI.updateNumber(valve_worktime_number, 0);
    }

    value_32 = device->getSmartValveDataPauseTime();

    ESPUI.updateNumber(valve_pause_number, value_32);

    ESPUI.updateLabel(
      valve_info_label, "Valve program loaded successfully.");

  } else {
    ESPUI.updateLabel(valve_info_label, "No Valve program found.");
  }
}
/*****************************************************************************/

void startValveProgram(uint8_t device_slot) {

  Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(device_slot);

  if (!device)
    return;

  uint16_t attribute_id;

  switch (ESPUI.getControl(valve_program_selector)->getValueInt()) {

    case 1:

      attribute_id = SONOFF_CUSTOM_CLUSTER_TIME_IRRIGATION_CYCLE_ID;
      break;


    case 2:

      attribute_id = SONOFF_CUSTOM_CLUSTER_VOLUME_IRRIGATION_CYCLE_ID;
      break;


    default:

      return;
  }

  uint32_t value_32;
  uint8_t valve_cmd_payload[11] = {};

  valve_cmd_payload[0] = 0x0A;

  valve_cmd_payload[2] = ESPUI.getControl(valve_cycles_number)->getValueInt();

  if (attribute_id == SONOFF_CUSTOM_CLUSTER_TIME_IRRIGATION_CYCLE_ID)
    value_32 = ESPUI.getControl(valve_worktime_number)->getValueInt();
  else
    value_32 = ESPUI.getControl(valve_volume_number)->getValueInt();

  valve_cmd_payload[4] = value_32 >> 16;
  valve_cmd_payload[5] = value_32 >> 8;
  valve_cmd_payload[6] = value_32;

  value_32 = ESPUI.getControl(valve_pause_number)->getValueInt();

  valve_cmd_payload[8] = value_32 >> 16;
  valve_cmd_payload[9] = value_32 >> 8;
  valve_cmd_payload[10] = value_32;

  for (int i = 0; i < 11; i++)
    log_i("valve payload [%u] = 0x%02x", i, valve_cmd_payload[i]);

  if (zbGateway.sendAttributeWrite(
        device->getDeviceShortAddress(), 1, SONOFF_CUSTOM_CLUSTER, 
				attribute_id,ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING, 
				sizeof(valve_cmd_payload), valve_cmd_payload, true)) {

    if (*zbGateway.getWriteAttrStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS)
      ESPUI.updateLabel(valve_info_label, "Valve program start success!");
    else
      ESPUI.updateLabel(valve_info_label, "Valve program start error!");
  } else
    ESPUI.updateLabel(valve_info_label, device_query_failed_str);
}

/*****************************************************************************/

void stopValveProgram(uint8_t device_slot) {

  Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(device_slot);

  if (!device)
    return;

  zbGateway.sendOnOffCmd(device->getDeviceShortAddress(), 1, false);
}

/*****************************************************************************/

void sendValveProgram(uint8_t device_slot, uint32_t flag_id) {

  Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(device_slot);

  if (!device)
    return;

  int8_t channel_sid = SONOFF_SMART_VALVE_RUN_PROGRAM_SID;

  if (flag_id == GUI_CB_SEND_PROGRAM_2_FLAG)
    channel_sid = SONOFF_SMART_VALVE_RUN_PROGRAM_2_SID;

  auto z2s_core = Z2S_findZ2SCore(
    device->getDeviceShortAddress(), ALL_ENDPOINTS,
    SONOFF_CUSTOM_CLUSTER, SUPLA_CHANNELTYPE_RELAY, channel_sid);

  if (!z2s_core) {

    log_i("no Supla channel for Sonoff program run");
    return;
  }

  switch (ESPUI.getControl(valve_program_selector)->getValueInt()) {

    case 1: {

      z2s_core->setSmartValveProgram(1);
      z2s_core->setSmartValveValue(
        ESPUI.getControl(valve_worktime_number)->getValueInt());

      msgZ2SDeviceVirtualRelayValue(
        z2s_core->getZ2SElementPtr(), VRV_S8_ID, 1);

      msgZ2SDeviceVirtualRelayValue(
        z2s_core->getZ2SElementPtr(), VRV_S32_ID,
        ESPUI.getControl(valve_worktime_number)->getValueInt());
    } break;

    case 2: {

      z2s_core->setSmartValveProgram(2);
      z2s_core->setSmartValveValue(
        ESPUI.getControl(valve_volume_number)->getValueInt());

      msgZ2SDeviceVirtualRelayValue(
        z2s_core->getZ2SElementPtr(), VRV_S8_ID, 2);

      msgZ2SDeviceVirtualRelayValue(
        z2s_core->getZ2SElementPtr(), VRV_S32_ID, 
        ESPUI.getControl(valve_volume_number)->getValueInt());
    } break;

    default: {

      z2s_core->setSmartValveProgram(0);
      z2s_core->setSmartValveCycles(0);
      z2s_core->setSmartValveValue(0);

      msgZ2SDeviceVirtualRelayValue(
        z2s_core->getZ2SElementPtr(), VRV_S8_ID, 0);
      return;
    } break;
  }

  z2s_core->setSmartValveCycles(
		ESPUI.getControl(valve_cycles_number)->getValueInt());

	if (z2s_core->setSmartValvePauseTime(
		ESPUI.getControl( valve_pause_number)->getValueInt()))
		ESPUI.updateLabel(
		  valve_info_label, "Valve program send to Supla channel.");

	msgZ2SDeviceVirtualRelayValue(
	  z2s_core->getZ2SElementPtr(), VRV_U8_ID, 
		ESPUI.getControl(valve_cycles_number)->getValueInt());

	msgZ2SDeviceVirtualRelayValue(
	  z2s_core->getZ2SElementPtr(), VRV_U32_ID, 
		ESPUI.getControl(valve_pause_number)->getValueInt());
}

/*****************************************************************************/

void saveDualValveProgram(uint8_t device_slot) {

  Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(device_slot);

  if (!device)
    return;

  if ((ESPUI.getControl(DV_CHANNEL_SELECTOR)->getValueInt() > 0) && 
      (ESPUI.getControl(DV_PROGRAM_SELECTOR)->getValueInt() >= 0)) {

    device->setSmartDualValveDataChannelId(
      ESPUI.getControl(DV_CHANNEL_SELECTOR)->getValueInt(), false);

    device->setSmartDualValveDataProgramId(
      ESPUI.getControl(DV_PROGRAM_SELECTOR)->getValueInt(), false);

    device->setSmartDualValveDataTotalDuration(
      ESPUI.getControl(DV_TOTAL_DURATION)->getValueInt(), false);

    device->setSmartDualValveDataIrrigationDuration(
      ESPUI.getControl(DV_IRRIGATION_DURATION)->getValueInt(), false);

    device->setSmartDualValveDataPauseDuration(
      ESPUI.getControl(DV_PAUSE_TIME)->getValueInt(), false);

    device->setSmartDualValveDataIrrigationVolume(
      ESPUI.getControl(DV_VOLUME)->getValueInt(), false);

    device->setSmartDualValveDataFailSafeDuration(
      ESPUI.getControl(DV_FAILSAFE_TIME)->getValueInt(), false);

    if (device->saveDeviceData())
      ESPUI.updateLabel(DV_INFO_LABEL, "Valve program saved successfully.");
    else
      ESPUI.updateLabel(DV_INFO_LABEL, "Valve program save failed.");

  } else {
    ESPUI.updateLabel(DV_INFO_LABEL, "No valve program to save");
  }
}

/*****************************************************************************/

void loadDualValveProgram(uint8_t device_slot) {

  Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(device_slot);

  if (!device)
    return;

  if (device->getSmartDualValveDataChannelId() > 0) {

    ESPUI.updateNumber(
      DV_CHANNEL_SELECTOR, device->getSmartDualValveDataChannelId());
      
    ESPUI.updateNumber(
      DV_PROGRAM_SELECTOR, device->getSmartDualValveDataProgramId());
      
    ESPUI.updateNumber(
      DV_TOTAL_DURATION, device->getSmartDualValveDataTotalDuration());
      
    ESPUI.updateNumber(
      DV_IRRIGATION_DURATION, 
			device->getSmartDualValveDataIrrigationDuration());
      
    ESPUI.updateNumber(
      DV_PAUSE_TIME, device->getSmartDualValveDataPauseDuration());
      
    ESPUI.updateNumber(
      DV_VOLUME, device->getSmartDualValveDataIrrigationVolume());
      
    ESPUI.updateNumber(
      DV_FAILSAFE_TIME, device->getSmartDualValveDataFailSafeDuration());

    ESPUI.updateLabel(DV_INFO_LABEL, "Valve program loaded successfully.");
    
  } else {
    ESPUI.updateLabel(DV_INFO_LABEL, "No valve program to load");
  }
}

/*****************************************************************************/
/*
void startDualValveProgram(uint8_t device_slot) {

	auto &zb_device = z2s_zb_devices_table[device_slot];

	int16_t endpoint_id = ESPUI.getControl(DV_CHANNEL_SELECTOR)->getValueInt();

	int16_t program_id = ESPUI.getControl(DV_PROGRAM_SELECTOR)->getValueInt();

	if ((endpoint_id < 1) || (program_id < 0)) {

		ESPUI.updateLabel(DV_INFO_LABEL, "Select channel and program!");
		return;
	}


	uint8_t dual_valve_cmd_payload[15] = {};

	dual_valve_cmd_payload[0] = ESP_ZB_ZCL_ATTR_TYPE_U8;
	dual_valve_cmd_payload[1] = 0x0C;

	dual_valve_cmd_payload[3] = program_id;

	uint16_t value_16 = ESPUI.getControl(DV_TOTAL_DURATION)->getValueInt();

	dual_valve_cmd_payload[4] = value_16 >> 8;
	dual_valve_cmd_payload[5] = value_16;

	value_16 = ESPUI.getControl(DV_IRRIGATION_DURATION)->getValueInt();

	dual_valve_cmd_payload[6] = value_16 >> 8;
	dual_valve_cmd_payload[7] = value_16;

	value_16 = ESPUI.getControl(DV_PAUSE_TIME)->getValueInt();

	dual_valve_cmd_payload[8] = value_16 >> 8;
	dual_valve_cmd_payload[9] = value_16;

	dual_valve_cmd_payload[10] = 0x01;  //liters

	value_16 = ESPUI.getControl(DV_VOLUME)->getValueInt();

	dual_valve_cmd_payload[11] = value_16 >> 8;
	dual_valve_cmd_payload[12] = value_16;

	value_16 = ESPUI.getControl(DV_FAILSAFE_TIME)->getValueInt();

	dual_valve_cmd_payload[13] = value_16 >> 8;
	dual_valve_cmd_payload[14] = value_16;

	if (zbGateway.sendAttributeWriteExt(
	      zb_device.short_addr, 1, SONOFF_CUSTOM_CLUSTER,
	      SONOFF_CUSTOM_CLUSTER_MANUAL_DEFAULT_SETTINGS_ID,
	      ESP_ZB_ZCL_ATTR_TYPE_ARRAY, sizeof(dual_valve_cmd_payload),
	      dual_valve_cmd_payload, true)) {

		if (*zbGateway.getWriteAttrStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS) {

			ESPUI.updateLabel(DV_INFO_LABEL, "Valve settings updated! Starting...");
			zbGateway.sendOnOffCmd(zb_device.short_addr, endpoint_id, true);
		} else
			ESPUI.updateLabel(
			  DV_INFO_LABEL, "Valve settings update failed! Try again!");
	} else
		ESPUI.updateLabel(DV_INFO_LABEL, device_query_failed_str);
}
*/

void startDualValveProgram(uint8_t device_slot) {

  Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(device_slot);

  if (!device)
    return;

  int16_t endpoint_id = ESPUI.getControl(DV_CHANNEL_SELECTOR)->getValueInt();

  int16_t program_id = ESPUI.getControl(DV_PROGRAM_SELECTOR)->getValueInt();

  if ((endpoint_id < 1) || (program_id < 0)) {

    ESPUI.updateLabel(DV_INFO_LABEL, "Select channel and program!");
    return;
  }

  uint8_t dual_valve_cmd_payload[15] = {};

  dual_valve_cmd_payload[0] = ESP_ZB_ZCL_ATTR_TYPE_U8;
  dual_valve_cmd_payload[1] = 0x0C;

  dual_valve_cmd_payload[3] = program_id;

  uint16_t value_16 = ESPUI.getControl(DV_TOTAL_DURATION)->getValueInt();

  dual_valve_cmd_payload[4] = value_16 >> 8;
  dual_valve_cmd_payload[5] = value_16;

  value_16 = ESPUI.getControl(DV_IRRIGATION_DURATION)->getValueInt();

  dual_valve_cmd_payload[6] = value_16 >> 8;
  dual_valve_cmd_payload[7] = value_16;

  value_16 = ESPUI.getControl(DV_PAUSE_TIME)->getValueInt();

  dual_valve_cmd_payload[8] = value_16 >> 8;
  dual_valve_cmd_payload[9] = value_16;

  dual_valve_cmd_payload[10] = 0x01;  //liters

  value_16 = ESPUI.getControl(DV_VOLUME)->getValueInt();

  dual_valve_cmd_payload[11] = value_16 >> 8;
  dual_valve_cmd_payload[12] = value_16;

  value_16 = ESPUI.getControl(DV_FAILSAFE_TIME)->getValueInt();

  dual_valve_cmd_payload[13] = value_16 >> 8;
  dual_valve_cmd_payload[14] = value_16;

  if (zbGateway.sendAttributeWriteExt(
        device->getDeviceShortAddress(), 1, SONOFF_CUSTOM_CLUSTER,
        SONOFF_CUSTOM_CLUSTER_MANUAL_DEFAULT_SETTINGS_ID,
        ESP_ZB_ZCL_ATTR_TYPE_ARRAY, sizeof(dual_valve_cmd_payload),
        dual_valve_cmd_payload, true)) {

    if (*zbGateway.getWriteAttrStatusLastResult() == 
        ESP_ZB_ZCL_STATUS_SUCCESS) {

      ESPUI.updateLabel(DV_INFO_LABEL, "Valve settings updated! Starting...");
      
      zbGateway.sendOnOffCmd(
        device->getDeviceShortAddress(), endpoint_id, true);
        
    } else {
      ESPUI.updateLabel(
        DV_INFO_LABEL, "Valve settings update failed! Try again!");
    }
  } else {
    ESPUI.updateLabel(DV_INFO_LABEL, device_query_failed_str);
  }
}

/*****************************************************************************/
/*
void stopDualValveProgram(uint8_t device_slot) {

	auto &zb_device = z2s_zb_devices_table[device_slot];

	zbGateway.sendOnOffCmd(zb_device.short_addr, 1, false);
	//zbGateway.sendOnOffCmd(zb_device.short_addr, 2, false);
}
*/

void stopDualValveProgram(uint8_t device_slot) {

  Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(device_slot);

  if (!device)
    return;

  zbGateway.sendOnOffCmd(device->getDeviceShortAddress(), 1, false);
  //zbGateway.sendOnOffCmd(device->getDeviceShortAddress(), 2, false);
}



/*****************************************************************************/
/*
void sendDualValveProgram(uint8_t device_slot, uint8_t flag_id) {

	int16_t endpoint_id = ESPUI.getControl(DV_CHANNEL_SELECTOR)->getValueInt();

	int16_t program_id = ESPUI.getControl(DV_PROGRAM_SELECTOR)->getValueInt();

	if ((endpoint_id < 1) || (program_id < 0)) {

		ESPUI.updateLabel(DV_INFO_LABEL, "Select channel and program!");
		return;
	}

	int8_t channel_sid = SONOFF_SMART_VALVE_RUN_PROGRAM_SID;

	if (flag_id == GUI_CB_SEND_PROGRAM_2_FLAG)
		channel_sid = SONOFF_SMART_VALVE_RUN_PROGRAM_2_SID;

	auto z2s_core = Z2S_findZ2SCore(
	  z2s_zb_devices_table[device_slot].short_addr, endpoint_id,
	  SONOFF_CUSTOM_CLUSTER, SUPLA_CHANNELTYPE_RELAY, channel_sid);

	if (!z2s_core) {

		log_i("no Supla channel for upload Sonoff dual valve program");
		return;
	}

	bool save_success = false;

	save_success = z2s_core->setSmartDualValveProgramId(program_id);

	save_success &= z2s_core->setSmartDualValveTotalDurationTime(
	  ESPUI.getControl(DV_TOTAL_DURATION)->getValueInt());

	save_success &= z2s_core->setSmartDualValveIrrigationDurationTime(
	  ESPUI.getControl(DV_IRRIGATION_DURATION)->getValueInt());

	save_success &= z2s_core->setSmartDualValveIrrigationPauseTime(
	  ESPUI.getControl(DV_PAUSE_TIME)->getValueInt());

	save_success &= z2s_core->setSmartDualValveIrrigationVolume(
	  ESPUI.getControl(DV_VOLUME)->getValueInt());

	save_success &= z2s_core->setSmartDualValveFailSafeTime(
	  ESPUI.getControl(DV_FAILSAFE_TIME)->getValueInt());

	if (save_success)
		ESPUI.updateLabel(DV_INFO_LABEL, "Program saved in Supla channel");
}
*/

void sendDualValveProgram(uint8_t device_slot, uint8_t flag_id) {

  Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(device_slot);

  if (!device)
    return;

  int16_t endpoint_id = ESPUI.getControl(DV_CHANNEL_SELECTOR)->getValueInt();

  int16_t program_id = ESPUI.getControl(DV_PROGRAM_SELECTOR)->getValueInt();

  if ((endpoint_id < 1) || (program_id < 0)) {

    ESPUI.updateLabel(DV_INFO_LABEL, "Select channel and program!");
    return;
  }

  int8_t channel_sid = SONOFF_SMART_VALVE_RUN_PROGRAM_SID;

  if (flag_id == GUI_CB_SEND_PROGRAM_2_FLAG)
    channel_sid = SONOFF_SMART_VALVE_RUN_PROGRAM_2_SID;

  auto z2s_core = Z2S_findZ2SCore(
    device->getDeviceShortAddress(), endpoint_id,
    SONOFF_CUSTOM_CLUSTER, SUPLA_CHANNELTYPE_RELAY, channel_sid);

  if (!z2s_core) {

    log_i("no Supla channel for upload Sonoff dual valve program");
    return;
  }

  bool save_success = false;

  save_success = z2s_core->setSmartDualValveProgramId(program_id);

  save_success &= z2s_core->setSmartDualValveTotalDurationTime(
    ESPUI.getControl(DV_TOTAL_DURATION)->getValueInt());

  save_success &= z2s_core->setSmartDualValveIrrigationDurationTime(
    ESPUI.getControl(DV_IRRIGATION_DURATION)->getValueInt());

  save_success &= z2s_core->setSmartDualValveIrrigationPauseTime(
    ESPUI.getControl(DV_PAUSE_TIME)->getValueInt());

  save_success &= z2s_core->setSmartDualValveIrrigationVolume(
    ESPUI.getControl(DV_VOLUME)->getValueInt());

  save_success &= z2s_core->setSmartDualValveFailSafeTime(
    ESPUI.getControl(DV_FAILSAFE_TIME)->getValueInt());

  if (save_success)
    ESPUI.updateLabel(DV_INFO_LABEL, "Program saved in Supla channel");
}

/*****************************************************************************/

void valveCallback(BasicControl *sender, int type, void *param) {

	if ((type == B_UP) && (ESPUI.getControl(
		advanced_device_selector)->getValueInt() >= 0)) {

		uint8_t device_slot = ESPUI.getControl(
			advanced_device_selector)->getValueInt();

		log_i(
			"device_selector value %u, param id %d", device_slot, (uint32_t)param);

		Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(device_slot);

    if (!device) 
			return;
      
		uint32_t device_desc_id = device->getDeviceDescId();
    uint16_t short_addr = device->getDeviceShortAddress();

		uint32_t flag_id = (uint32_t)param;

		switch (flag_id) {

			case GUI_CB_SAVE_PROGRAM_FLAG:
				{  //save valve program

					switch (device_desc_id) {

						case Z2S_DEVICE_DESC_SONOFF_SMART_VALVE:

							saveValveProgram(device_slot);
							break;

						case Z2S_DEVICE_DESC_SONOFF_SMART_DUAL_VALVE:

							saveDualValveProgram(device_slot);
							break;
					}
				}
				break;

			case GUI_CB_LOAD_PROGRAM_FLAG:
				{  //load valve program

					switch (device_desc_id) {

						case Z2S_DEVICE_DESC_SONOFF_SMART_VALVE:

							loadValveProgram(device_slot);
							break;

						case Z2S_DEVICE_DESC_SONOFF_SMART_DUAL_VALVE:

							loadDualValveProgram(device_slot);
							break;
					}
				}
				break;

			case GUI_CB_START_PROGRAM_FLAG:
				{  //start valve program

					switch (device_desc_id) {

						case Z2S_DEVICE_DESC_SONOFF_SMART_VALVE:

							startValveProgram(device_slot);
							break;

						case Z2S_DEVICE_DESC_SONOFF_SMART_DUAL_VALVE:

							startDualValveProgram(device_slot);
							break;
					}
				}
				break;

			case GUI_CB_STOP_PROGRAM_FLAG:
				{  //stop valve program

					switch (device_desc_id) {

						case Z2S_DEVICE_DESC_SONOFF_SMART_VALVE:

							stopValveProgram(device_slot);
							break;

						case Z2S_DEVICE_DESC_SONOFF_SMART_DUAL_VALVE:

							stopDualValveProgram(device_slot);
							break;
					}
				}
				break;

			case GUI_CB_SEND_PROGRAM_FLAG:
			case GUI_CB_SEND_PROGRAM_2_FLAG:
				{

					switch (device_desc_id) {

						case Z2S_DEVICE_DESC_SONOFF_SMART_VALVE:

							sendValveProgram(device_slot, flag_id);
							break;

						case Z2S_DEVICE_DESC_SONOFF_SMART_DUAL_VALVE:

							sendDualValveProgram(device_slot, flag_id);
							break;
					}
				}
				break;


			case GUI_CB_SEND_RINGTONE_FLAG:
				{  //write gas detector alarm ringtone

					int16_t gas_alarm_ringtone =
					  ESPUI.getControl(gas_alarm_ringtone_selector)->getValueInt();

					if (gas_alarm_ringtone >= 0) {

						current_Tuya_payload_label = gas_alarm_Tuya_payload_label;

						if (sendTuyaRequestCmdEnum8(
						      short_addr, 1, TUYA_GAS_DETECTOR_RINGTONE_DP,
						      gas_alarm_ringtone, CUSTOM_CMD_SYNC))
							ESPUI.updateLabel(gas_alarm_info_label, "New ringtone sent");
						else
							ESPUI.updateLabel(gas_alarm_info_label, device_query_failed_str);
					} else
						ESPUI.updateLabel(gas_alarm_info_label, "Select ringtone to send.");
				}
				break;

			case GUI_CB_SEND_TIME_FLAG:
				{  //write gas detector alarm duration

					int32_t gas_alarm_time =
					  ESPUI.getControl(gas_alarm_time_number)->getValueInt();

					if (gas_alarm_time >= 0) {

						current_Tuya_payload_label = gas_alarm_Tuya_payload_label;

						if (sendTuyaRequestCmdValue32(
						      short_addr, 1, TUYA_GAS_DETECTOR_ALARM_TIME_DP,
						      gas_alarm_time, CUSTOM_CMD_SYNC))
							ESPUI.updateLabel(gas_alarm_info_label, "New alarm time sent");
						else
							ESPUI.updateLabel(gas_alarm_info_label, device_query_failed_str);
					} else
						ESPUI.updateLabel(
						  gas_alarm_info_label, "Enter alarm duration to send.");
				}
				break;

			case GUI_CB_SELF_TEST_FLAG:
				{  //gas detector self test cmd

					current_Tuya_payload_label = gas_alarm_Tuya_payload_label;

					if (sendTuyaRequestCmdBool(
					      short_addr, 1, TUYA_GAS_DETECTOR_SELF_TEST_RESULT_DP,
					      true, CUSTOM_CMD_SYNC))
						ESPUI.updateLabel(gas_alarm_info_label, "Self-test command sent");
					else
						ESPUI.updateLabel(gas_alarm_info_label, device_query_failed_str);
				}
				break;

			case GUI_CB_SILENCE_FLAG:
				{  //gas detector silence cmd

					current_Tuya_payload_label = gas_alarm_Tuya_payload_label;

					if (sendTuyaRequestCmdBool(
					      short_addr, 1, TUYA_GAS_DETECTOR_SILENCE_DP,
					      true, CUSTOM_CMD_SYNC))
						ESPUI.updateLabel(gas_alarm_info_label, "Silence command sent");
					else
						ESPUI.updateLabel(gas_alarm_info_label, device_query_failed_str);
				}
				break;


			case GUI_CB_SEND_MELODY_FLAG:
				{  //send moes alarm melody

					int16_t moes_alarm_melody = ESPUI.getControl(
					                                   moes_alarm_melody_selector)
					                              ->getValueInt();

					if (moes_alarm_melody >= 0) {

						current_Tuya_payload_label = moes_alarm_Tuya_payload_label;

						if (sendTuyaRequestCmdEnum8(
						      short_addr, 1, MOES_ALARM_MELODY_DP, moes_alarm_melody,
						      CUSTOM_CMD_SYNC))
							ESPUI.updateLabel(moes_alarm_info_label, "New melody sent");
						else
							ESPUI.updateLabel(moes_alarm_info_label, device_query_failed_str);
					} else
						ESPUI.updateLabel(moes_alarm_info_label, "Select melody to send.");
				}
				break;


			case GUI_CB_SEND_VOLUME_FLAG:
				{  //send moes alarm volume

					int16_t moes_alarm_volume = ESPUI.getControl(
					                                   moes_alarm_volume_number)
					                              ->getValueInt();

					if (moes_alarm_volume >= 0) {

						current_Tuya_payload_label = moes_alarm_Tuya_payload_label;

						if (sendTuyaRequestCmdEnum8(
						      short_addr, 1, MOES_ALARM_VOLUME_DP, moes_alarm_volume,
						      CUSTOM_CMD_SYNC))
							ESPUI.updateLabel(moes_alarm_info_label, "New volume sent");
						else
							ESPUI.updateLabel(moes_alarm_info_label, device_query_failed_str);
					} else
						ESPUI.updateLabel(moes_alarm_info_label, "Enter volume to send.");
				}
				break;

			case GUI_CB_SEND_DURATION_FLAG:
				{  //send moes alarm duration

					int32_t moes_alarm_duration = ESPUI.getControl(
					                                     moes_alarm_duration_number)
					                                ->getValueInt();

					if (moes_alarm_duration >= 0) {

						current_Tuya_payload_label = moes_alarm_Tuya_payload_label;

						if (sendTuyaRequestCmdValue32(
						      short_addr, 1, MOES_ALARM_DURATION_DP, moes_alarm_duration,
						      CUSTOM_CMD_SYNC))
							ESPUI.updateLabel(
							  moes_alarm_info_label, "New alarm duration sent");
						else
							ESPUI.updateLabel(moes_alarm_info_label, device_query_failed_str);
					} else
						ESPUI.updateLabel(moes_alarm_info_label, "Enter alarm duration to send.");
				}
				break;
		}
	}
}

/*****************************************************************************/

void TuyaDeviceSelectorCallback(BasicControl *sender, int type, void *param) {

	int32_t device_slot = (sender) ? sender->getValueInt() : type;

	addGUICommand(gui_cmd_Tuya_device_callback, device_slot);
}

/*****************************************************************************/

void TuyaDeviceSelectorCmd(int32_t device_slot) {

	char general_purpose_gui_buffer[512] = {};

	current_Tuya_device_id = device_slot;

	if (device_slot >= 0) {

		Z2S_Device *z2s_device = Z2S_Device::getZ2SDeviceByDeviceIndex(
			device_slot);

		enableTuyaDevicesControls(true);

		sprintf_P(
		  general_purpose_gui_buffer, "<b><i>Manufacturer name</i></b> %s <b>| "
			"<i>model ID</b></i> %s", z2s_device->getManufacturerName(),
		  z2s_device->getModelName());

		updateLabel_P(
		  Tuya_devices_tab_controls_table[Tuya_device_info_label],
		  general_purpose_gui_buffer);
		sortTuyaDatapointsSelectorsMain();


	} else {

		sortTuyaDatapointsSelectorsMain();
		enableTuyaDevicesControls(false);
	}
}

/*****************************************************************************/
/*****************************************************************************/

void addLocalActionHandlerCallback(
  BasicControl *sender, int type, void *param) {

	if (type == B_UP) {

		addGUICommand(gui_cmd_add_logic_gate, (uint32_t)param);
	}
}

/*****************************************************************************/

void addLocalVirtualRelayCallback(
  BasicControl *sender, int type, void *param) {

	if (type == B_UP) {

		addGUICommand(gui_cmd_add_virtual_relay);
	}
}

/*****************************************************************************/

void addLocalChannelCallback(BasicControl *sender, int type, void *param) {

	if (type == B_UP) {

		uint32_t gui_command = (uint32_t)param;

		addGUICommand((gui_commands)gui_command);
	}
}

/*****************************************************************************/

void addLocalVirtualBinaryCallback(
  BasicControl *sender, int type, void *param) {

	if (type == B_UP) {

		addGUICommand(gui_cmd_add_virtual_binary);
	}
}

/*****************************************************************************/

void addLocalRemoteRelayCallback(
  BasicControl *sender, int type, void *param) {

	if (type == B_UP) {

		addGUICommand(gui_cmd_add_remote_relay);
	}
}

/*****************************************************************************/

void addLocalRemoteThermometerCallback(
  BasicControl *sender, int type, void *param) {

	if (type == B_UP) {

		addGUICommand(gui_cmd_add_remote_thermometer);
	}
}

/*****************************************************************************/

void addLocalVirtualHvacCallback(
  BasicControl *sender, int type, void *param) {

	if (type == B_UP) {

		addGUICommand(gui_cmd_add_virtual_hvac);
	}
}

/*****************************************************************************/

void addLocalVirtualThermhygrometerCallback(
  BasicControl *sender, int type, void *param) {

	if (type == B_UP) {

		addGUICommand(gui_cmd_add_virtual_thermhygrometer);
	}
}

/*****************************************************************************/

void addSwitchbotCallback(BasicControl *sender, int type, void *param) {

	if (type == B_UP) {

		switch ((uint32_t)param) {


			case GUI_CB_ADD_SB_1X_FLAG:

				addGUICommand(gui_cmd_add_switchbot_1x);
				break;


			case GUI_CB_ADD_SB_2X_FLAG:

				addGUICommand(gui_cmd_add_switchbot_2x);
				break;
		}
	}
}

/*****************************************************************************/

void saveSwitchbotCallback(BasicControl *sender, int type, void *param) {

	if (type == B_UP) {

		addGUICommand(gui_cmd_save_switchbot_data);
	}
}

/*****************************************************************************/
/*****************************************************************************/

static uint16_t pushover_tab;
static uint16_t pushover_api_token_text;
static uint16_t pushover_user_token_text;
static uint16_t pushover_message_name_text;
static uint16_t pushover_message_body_text;
static uint16_t pushover_message_counter_label;
char pushover_message_counter[16];

/*****************************************************************************/

void updatePushoverMessageCounter() {

	if (Z2S_getPushoverMessagesNumber() > 0)
		snprintf(
		  pushover_message_counter, 16, "Message %03u/%03u", current_message_counter,
		  Z2S_getPushoverMessagesNumber());
	else
		snprintf(
		  pushover_message_counter, 16, "No Pushover messages");
	ESPUI.updateLabel(pushover_message_counter_label, pushover_message_counter);
}

/*****************************************************************************/

void enablePushoverMessageDetails(bool enable) {

	if (!enable) {
	}
	enableControlStyle(pushover_message_subaction_label, enable);
	enableControlStyle(pushover_message_name_text, enable);
	enableControlStyle(pushover_message_body_text, enable);
}

/*****************************************************************************/

void enablePushoverMessageControls(bool enable) {

	enableControlStyle(pushover_message_first_button, enable);
	enableControlStyle(pushover_message_next_button, enable);
	enableControlStyle(pushover_message_prev_button, enable);
	enableControlStyle(pushover_message_last_button, enable);
}

/*****************************************************************************/

void updatePushoverMessageButtons() {

	bool pushover_message_save_button_enable;
	bool pushover_message_new_button_enable;
	bool pushover_message_edit_button_enable;
	bool pushover_message_copy_button_enable;
	bool pushover_message_cancel_button_enable;
	bool pushover_message_remove_button_enable;

	switch (current_pushover_message_gui_state) {


		case VIEW_PUSHOVER_MESSAGE:
			{

				pushover_message_save_button_enable = false;
				pushover_message_cancel_button_enable = false;

				if (current_message_id == -1) {

					pushover_message_edit_button_enable = false;
					pushover_message_remove_button_enable = false;
					pushover_message_copy_button_enable = false;
				} else {

					pushover_message_edit_button_enable = true;
					pushover_message_remove_button_enable = true;
					pushover_message_copy_button_enable = true;
				}

				pushover_message_new_button_enable = true;
			}
			break;


		case NEW_PUSHOVER_MESSAGE:
			{

				pushover_message_save_button_enable = true;
				pushover_message_new_button_enable = false;
				pushover_message_edit_button_enable = false;
				pushover_message_cancel_button_enable = true;
				pushover_message_remove_button_enable = false;
			}
			break;


		case EDIT_PUSHOVER_MESSAGE:
			{

				pushover_message_save_button_enable = true;
				pushover_message_new_button_enable = false;
				pushover_message_edit_button_enable = false;
				pushover_message_cancel_button_enable = true;
				pushover_message_remove_button_enable = false;
			}
			break;


		case COPY_PUSHOVER_MESSAGE:
			{

				pushover_message_save_button_enable = true;
				pushover_message_new_button_enable = false;
				pushover_message_edit_button_enable = false;
				pushover_message_cancel_button_enable = true;
				pushover_message_remove_button_enable = false;
			}
			break;
	}

	enableControlStyle(
	  pushover_message_save_button, pushover_message_save_button_enable);
	enableControlStyle(
	  pushover_message_new_button, pushover_message_new_button_enable);
	enableControlStyle(
	  pushover_message_edit_button, pushover_message_edit_button_enable);
	enableControlStyle(
	  pushover_message_cancel_button, pushover_message_cancel_button_enable);
	enableControlStyle(
	  pushover_message_remove_button, pushover_message_remove_button_enable);
	enableControlStyle(
	  pushover_message_copy_button, pushover_message_remove_button_enable);

	updatePushoverMessageCounter();
}

/*****************************************************************************/

void updatePushoverMessageDetails(
  z2s_pushover_message_t &message, bool empty_message = false) {

	if (current_pushover_message_gui_state == VIEW_PUSHOVER_MESSAGE) {

		if (previous_pushover_message_gui_state != VIEW_PUSHOVER_MESSAGE)
			enablePushoverMessageDetails(false);
		//return;
	}

	if (current_pushover_message_gui_state != VIEW_PUSHOVER_MESSAGE)
		enablePushoverMessageControls(false);

	if (!empty_message)
		log_i(
		  "strlen = %u, mbstrnlen = %u", strlen(message.pushover_message_name),
		  mbstrnlen(message.pushover_message_name, 64));

	working_str = empty_message ? Z2S_findFreePushoverMessageIndex() : message.pushover_message_subaction_id;
	ESPUI.updateLabel(pushover_message_subaction_label, working_str);

	working_str = empty_message ? empty_str : message.pushover_message_name;
	ESPUI.updateText(pushover_message_name_text, working_str);

	working_str = empty_message ? empty_str : message.pushover_message_text;
	ESPUI.updateText(pushover_message_body_text, working_str);
}

/*****************************************************************************/

bool fillPushoverMessageDetails(z2s_pushover_message_t &message) {

	const char *pushover_message_name_str = ESPUI.getControl(
	                                               pushover_message_name_text)
	                                          ->getValueCstr();

	if (strlen(pushover_message_name_str) >= 0) {

		size_t pushover_message_name_size_w = mbstrnlen(
		  pushover_message_name_str, PUSHOVER_MESSAGE_NAME_MAX_SIZE - 1);

		strncpy(
		  message.pushover_message_name, pushover_message_name_str,
		  pushover_message_name_size_w);
		message.pushover_message_name[pushover_message_name_size_w] = '\0';
	} else
		return false;

	const char *pushover_message_body_str = ESPUI.getControl(
	                                               pushover_message_body_text)
	                                          ->getValueCstr();

	if (strlen(pushover_message_body_str) >= 0) {

		size_t pushover_message_body_size_w = mbstrnlen(
		  pushover_message_body_str, PUSHOVER_MESSAGE_TEXT_MAX_SIZE - 1);

		strncpy(
		  message.pushover_message_text, pushover_message_body_str,
		  pushover_message_body_size_w);
		message.pushover_message_text[pushover_message_body_size_w] = '\0';
	} else
		return false;


	long selector_value = ESPUI.getControl(
	                             pushover_message_subaction_label)
	                        ->getValueInt();

	if (selector_value >= 0)
		message.pushover_message_subaction_id = selector_value;
	else
		return false;

	return true;
}

/*****************************************************************************/

void pushoverMessagesCallback(BasicControl *sender, int type, void *param) {

	if (type == B_UP) {

		z2s_pushover_message_t new_message = {};

		switch ((uint32_t)param) {

			case GUI_CB_PUSHOVER_SAVE_DATA_FLAG:
				{

					Z2S_GatewayPreferences.putString(
					  "PUSHOVER_API", ESPUI.getControl(
					                         pushover_api_token_text)
					                    ->getValue());
					Z2S_GatewayPreferences.putString(
					  "PUSHOVER_USER", ESPUI.getControl(
					                          pushover_user_token_text)
					                     ->getValue());
				}
				break;


			case GUI_CB_PUSHOVER_MESSAGE_FIRST_FLAG:
				{

					int16_t first_message = Z2S_findNextPushoverMessagePosition(0);
					if (first_message >= 0) {

						current_message_id = first_message;
						current_message_counter = Z2S_getPushoverMessageCounter(
						  first_message);

						Z2S_loadPushoverMessage(first_message, new_message);
						updatePushoverMessageDetails(new_message);
						updatePushoverMessageCounter();
					}
				}
				break;


			case GUI_CB_PUSHOVER_MESSAGE_NEXT_FLAG:
				{

					if (current_message_id < 0)
						return;

					int16_t next_message = Z2S_findNextPushoverMessagePosition(
					  current_message_id + 1);
					if (next_message >= 0) {

						current_message_id = next_message;
						current_message_counter = Z2S_getPushoverMessageCounter(next_message);

						Z2S_loadPushoverMessage(next_message, new_message);
						updatePushoverMessageDetails(new_message);
						updatePushoverMessageCounter();
					}
				}
				break;


			case GUI_CB_PUSHOVER_MESSAGE_PREV_FLAG:
				{

					if (current_message_id <= 0)
						return;

					int16_t prev_message = Z2S_findPrevPushoverMessagePosition(
					  current_message_id - 1);
					if (prev_message >= 0) {

						current_message_id = prev_message;
						current_message_counter = Z2S_getPushoverMessageCounter(
						  prev_message);

						Z2S_loadPushoverMessage(prev_message, new_message);
						updatePushoverMessageDetails(new_message);
						updatePushoverMessageCounter();
					}
				}
				break;


			case GUI_CB_PUSHOVER_MESSAGE_LAST_FLAG:
				{

					if (current_message_id < 0)
						return;

					int16_t last_message = Z2S_findPrevPushoverMessagePosition(
					  Z2S_PUSHOVER_MESSAGES_MAX_NUMBER);
					if (last_message >= 0) {

						current_message_id = last_message;
						current_message_counter = Z2S_getPushoverMessageCounter(
						  last_message);

						Z2S_loadPushoverMessage(last_message, new_message);
						updatePushoverMessageDetails(new_message);
						updatePushoverMessageCounter();
					}
				}
				break;


			case GUI_CB_PUSHOVER_MESSAGE_EDIT_FLAG:
				{

					if ((current_pushover_message_gui_state != VIEW_PUSHOVER_MESSAGE) || (current_message_id == -1))
						return;

					previous_pushover_message_gui_state =
					  current_pushover_message_gui_state;
					current_pushover_message_gui_state = EDIT_PUSHOVER_MESSAGE;
					Z2S_loadPushoverMessage(current_message_id, new_message);
					enablePushoverMessageDetails(true);
					updatePushoverMessageDetails(new_message);
					updatePushoverMessageButtons();
				}
				break;


			case GUI_CB_PUSHOVER_MESSAGE_COPY_FLAG:
				{

					if ((current_pushover_message_gui_state != VIEW_PUSHOVER_MESSAGE) || (current_message_id == -1))
						return;

					new_message_id = Z2S_findFreePushoverMessageIndex();

					if (new_message_id == -1) {

						ESPUI.updateLabel(
						  pushover_message_state_label,
						  "Pushover messages table is full - can't add new Pushover message!");
						return;
					}

					previous_pushover_message_gui_state =
					  current_pushover_message_gui_state;
					current_pushover_message_gui_state = COPY_PUSHOVER_MESSAGE;
					Z2S_loadPushoverMessage(current_message_id, new_message);
					new_message.pushover_message_subaction_id = new_message_id;
					enablePushoverMessageDetails(true);
					updatePushoverMessageDetails(new_message);
					updatePushoverMessageButtons();
				}
				break;


			case GUI_CB_PUSHOVER_MESSAGE_NEW_FLAG:
				{

					if (current_pushover_message_gui_state != VIEW_PUSHOVER_MESSAGE)
						return;

					new_message_id = Z2S_findFreePushoverMessageIndex();
					if (new_message_id == -1) {

						ESPUI.updateLabel(
						  pushover_message_state_label,
						  "Pushover messages table is full - can't add new Pushover message!");
						return;
					} else {

						previous_pushover_message_gui_state =
						  current_pushover_message_gui_state;
						current_pushover_message_gui_state = NEW_PUSHOVER_MESSAGE;
						enablePushoverMessageDetails(true);
						updatePushoverMessageButtons();
						updatePushoverMessageDetails(new_message, true);
					}
				}
				break;

			case GUI_CB_PUSHOVER_MESSAGE_SAVE_FLAG:
				{

					if ((current_pushover_message_gui_state != NEW_PUSHOVER_MESSAGE) && (current_pushover_message_gui_state != EDIT_PUSHOVER_MESSAGE) && (current_pushover_message_gui_state != COPY_PUSHOVER_MESSAGE))
						return;

					if (!fillPushoverMessageDetails(new_message)) {
						ESPUI.updateLabel(
						  pushover_message_state_label, "Required action data is missing!");
						return;
					}

					previous_pushover_message_gui_state = SAVE_PUSHOVER_MESSAGE;

					if ((current_pushover_message_gui_state == NEW_PUSHOVER_MESSAGE) || (current_pushover_message_gui_state == COPY_PUSHOVER_MESSAGE)) {

						current_message_id = new_message_id;
					}

					bool save_result = Z2S_savePushoverMessage(
					  current_message_id, new_message);

					if ((current_pushover_message_gui_state == NEW_PUSHOVER_MESSAGE) || (current_pushover_message_gui_state == COPY_PUSHOVER_MESSAGE)) {

						current_message_counter = Z2S_getPushoverMessageCounter(
						  current_message_id);
					}

					current_pushover_message_gui_state = VIEW_PUSHOVER_MESSAGE;

					updatePushoverMessageDetails(new_message);
					enablePushoverMessageDetails(false);
					enablePushoverMessageControls(true);
					updatePushoverMessageButtons();

					if (save_result)
						ESPUI.updateLabel(
						  pushover_message_state_label,
						  "Saving Pushover message: SUCCESS!<br>");
					else
						ESPUI.updateLabel(
						  pushover_message_state_label, "Saving Pushover message: FAILED!");
				}
				break;

			case GUI_CB_PUSHOVER_MESSAGE_CANCEL_FLAG:
				{

					if ((current_pushover_message_gui_state != NEW_PUSHOVER_MESSAGE) && (current_pushover_message_gui_state != EDIT_PUSHOVER_MESSAGE) && (current_pushover_message_gui_state != COPY_PUSHOVER_MESSAGE))
						return;

					previous_pushover_message_gui_state = CANCEL_PUSHOVER_MESSAGE;
					current_pushover_message_gui_state = VIEW_PUSHOVER_MESSAGE;

					Z2S_loadPushoverMessage(current_message_id, new_message);
					updatePushoverMessageDetails(new_message);

					enablePushoverMessageDetails(false);
					enablePushoverMessageControls(true);
					updatePushoverMessageButtons();
					ESPUI.updateLabel(pushover_message_state_label, "Changes cancelled!");
				}
				break;

			case GUI_CB_PUSHOVER_MESSAGE_REMOVE_FLAG:
				{

					if ((current_pushover_message_gui_state != VIEW_PUSHOVER_MESSAGE) ||
							(current_message_id == -1))
						return;

					previous_pushover_message_gui_state = REMOVE_PUSHOVER_MESSAGE;

					bool remove_result = Z2S_removePushoverMessage(
					  current_message_id);
					current_message_id = Z2S_findNextPushoverMessagePosition(0);

					if (current_message_id >= 0) {

						current_message_counter = Z2S_getPushoverMessageCounter(
						  current_message_id);
						Z2S_loadPushoverMessage(current_message_id, new_message);
						updatePushoverMessageDetails(new_message);
					} else {

						current_message_counter = -1;
						updatePushoverMessageDetails(new_message, true);
					}
					current_pushover_message_gui_state = VIEW_PUSHOVER_MESSAGE;
					enablePushoverMessageDetails(false);
					enablePushoverMessageControls(true);
					updatePushoverMessageButtons();

					if (remove_result)
						ESPUI.updateLabel(
						  pushover_message_state_label,
						  "Removing Pushover message: SUCCESS!");
					else
						ESPUI.updateLabel(
						  pushover_message_state_label,
						  "Removing Pushover message: FAILED!");
				}
				break;
		}
	}
}

/*****************************************************************************/

void addPushoverMessageCallback(BasicControl *sender, int type, void *param) {
}

/*****************************************************************************/

void buildPushoverTabGUI() {

	//char general_purpose_gui_buffer[768] = {};

	pushover_tab = ESPUI.addControl(
	  Control::Type::Tab, empty_str, "Pushover",
	  Control::Color::Dark, Control::noParent, generalCallback);

	pushover_api_token_text = ESPUI.addControl(
	  Control::Type::Text, "PUSHOVER GENERAL PARAMETERS", emptyString,
	  Control::Color::Dark, pushover_tab, generalCallback);
	ESPUI.getControl(pushover_api_token_text)->reserve(33);

	addClearLabel("Pushover API token", pushover_api_token_text);

	pushover_user_token_text = ESPUI.addControl(
	  Control::Type::Text, empty_str, emptyString, Control::Color::Dark,
	  pushover_api_token_text, generalCallback);
	ESPUI.getControl(pushover_user_token_text)->reserve(33);

	addClearLabel("Pushover user token", pushover_api_token_text);

	ESPUI.updateText(
	  pushover_api_token_text, Z2S_GatewayPreferences.getString(
	                             "PUSHOVER_API", empty_str));
	ESPUI.updateText(
	  pushover_user_token_text, Z2S_GatewayPreferences.getString(
	                              "PUSHOVER_USER", empty_str));

	auto pushover_save_data_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "SAVE",
	  Control::Color::Dark, pushover_api_token_text,
	  pushoverMessagesCallback, (void *)GUI_CB_PUSHOVER_SAVE_DATA_FLAG);


	ESPUI.setPanelWide(pushover_api_token_text, true);

	pushover_message_first_button = ESPUI.addControl(
	  Control::Type::Button, "PUSHOVER MESSAGES", "FIRST",
	  Control::Color::Dark, pushover_tab, pushoverMessagesCallback,
	  (void *)GUI_CB_PUSHOVER_MESSAGE_FIRST_FLAG);

	ESPUI.setPanelWide(pushover_message_first_button, true);

	pushover_message_next_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "NEXT",
	  Control::Color::Dark, pushover_message_first_button,
	  pushoverMessagesCallback, (void *)GUI_CB_PUSHOVER_MESSAGE_NEXT_FLAG);

	pushover_message_prev_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "PREV",
	  Control::Color::Dark, pushover_message_first_button,
	  pushoverMessagesCallback, (void *)GUI_CB_PUSHOVER_MESSAGE_PREV_FLAG);

	pushover_message_last_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "LAST",
	  Control::Color::Dark, pushover_message_first_button,
	  pushoverMessagesCallback, (void *)GUI_CB_PUSHOVER_MESSAGE_LAST_FLAG);

	pushover_message_counter_label = addClearLabel(
	  pushover_message_counter, pushover_message_first_button);

	pushover_message_subaction_label = ESPUI.addControl(
	  Control::Type::Label, "PUSHOVER MESSAGE DETAILS", (long int)0,
	  Control::Color::Dark, pushover_tab, generalMinMaxCallback, (void *)256);

	addClearLabel(
	  "Pushover message subaction ID", pushover_message_subaction_label);

	ESPUI.setPanelWide(pushover_message_subaction_label, true);

	//addEmptyLineLabel(pushover_message_first_button);

	pushover_message_name_text = ESPUI.addControl(
	  Control::Type::Text, empty_str, emptyString, Control::Color::Dark,
	  pushover_message_subaction_label, generalCallback);
	ESPUI.getControl(pushover_user_token_text)->reserve(34);

	addClearLabel(
	  "Pushover message name (maximum 32 characters)",
	  pushover_message_subaction_label);

	pushover_message_body_text = ESPUI.addControl(
	  Control::Type::Text, empty_str, emptyString, Control::Color::Dark,
	  pushover_message_subaction_label, generalCallback);
	ESPUI.getControl(pushover_message_body_text)->reserve(128);

	addClearLabel(
	  "Pushover message (maximum 1024 characters), "
	  "i.e: my message&priority=1 OR my message&title=title&sound=siren OR"
	  "<br>my message&url=https://forum.supla.org&url_title="
	  "Click here to visit Forum Supla",
	  pushover_message_subaction_label);

	pushover_message_new_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "NEW MESSAGE",
	  Control::Color::Dark, pushover_message_subaction_label,
	  pushoverMessagesCallback, (void *)GUI_CB_PUSHOVER_MESSAGE_NEW_FLAG);

	pushover_message_edit_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "EDIT MESSAGE",
	  Control::Color::Dark, pushover_message_subaction_label,
	  pushoverMessagesCallback, (void *)GUI_CB_PUSHOVER_MESSAGE_EDIT_FLAG);

	pushover_message_copy_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "COPY MESSAGE",
	  Control::Color::Dark, pushover_message_subaction_label,
	  pushoverMessagesCallback, (void *)GUI_CB_PUSHOVER_MESSAGE_COPY_FLAG);

	pushover_message_save_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "SAVE MESSAGE",
	  Control::Color::Dark, pushover_message_subaction_label,
	  pushoverMessagesCallback, (void *)GUI_CB_PUSHOVER_MESSAGE_SAVE_FLAG);

	pushover_message_cancel_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "CANCEL CHANGES",
	  Control::Color::Dark, pushover_message_subaction_label,
	  pushoverMessagesCallback, (void *)GUI_CB_PUSHOVER_MESSAGE_CANCEL_FLAG);

	pushover_message_remove_button = ESPUI.addControl(
	  Control::Type::Button, empty_str, "DELETE MESSAGE",
	  Control::Color::Dark, pushover_message_subaction_label,
	  pushoverMessagesCallback, (void *)GUI_CB_PUSHOVER_MESSAGE_REMOVE_FLAG);

	pushover_message_state_label = ESPUI.addControl(
	  Control::Type::Label, empty_str, three_dots_str, Control::Color::Dark,
	  pushover_message_subaction_label);

	z2s_pushover_message_t new_pushover_message = {};

	int16_t first_message = Z2S_findNextPushoverMessagePosition(0);
	if (first_message >= 0) {

		current_message_id = first_message;
		current_message_counter = Z2S_getPushoverMessageCounter(first_message);
		Z2S_loadPushoverMessage(first_message, new_pushover_message);
		updatePushoverMessageDetails(new_pushover_message);
		updatePushoverMessageCounter();
	} else {

		updatePushoverMessageDetails(new_pushover_message, true);
		updatePushoverMessageCounter();
	}

	enablePushoverMessageDetails(false);
	updatePushoverMessageButtons();
}

/*****************************************************************************/
/*****************************************************************************/

void GUI_onTuyaCustomClusterReceive(
  uint8_t command_id, uint16_t payload_size, uint8_t *payload_data) {

	if (current_Tuya_payload_label == 0)
		return;

	char payload_buffer[255] = {};

	payload_buffer[0] = '{';

	char hex_chunk[3];

	hex_chunk[3] = 0x00;

	Tuya_dp_zcl_payload_t *Tuya_dp_zcl_payload =
	  (Tuya_dp_zcl_payload_t *)payload_data;


	int32_t dp_value = getTuyaDPValue(Tuya_dp_zcl_payload);

	if (Tuya_dp_zcl_payload->dp_id != Tuya_custom_cmd_dp)
		return;

	for (uint16_t payload_idx = 0; payload_idx < payload_size; payload_idx++) {

		sprintf(hex_chunk, "%02X", *(payload_data + payload_idx));
		memcpy(payload_buffer + 1 + (payload_idx * 3), hex_chunk, 2);
		payload_buffer[3 + payload_idx * 3] = ',';
	}
	payload_buffer[payload_size * 3] = '}';
	payload_buffer[(payload_size * 3) + 1] = ' ';
	sprintf(
	  payload_buffer + ((payload_size * 3) + 2),
	  "<br>datapoint id 	 = %u(0x%02X)<br>datapoint value = %u(0x%08X)",
	  Tuya_dp_zcl_payload->dp_id, Tuya_dp_zcl_payload->dp_id, dp_value,
	  dp_value);
	updateLabel_P(current_Tuya_payload_label, payload_buffer);
}

/*****************************************************************************/

void GUI_onLastBindingFailure(bool binding_failed) {

	char general_purpose_gui_buffer[512] = {};

	sprintf_P(
	  general_purpose_gui_buffer,
	  "Unknown model %s::%s, no binding is possible",
	  zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
	  zbGateway.getQueryBasicClusterData()->zcl_model_name);

	updateLabel_P(zigbee_last_binding_result_label, general_purpose_gui_buffer);
}

/*****************************************************************************/

void GUI_onZigbeeOpenNetwork(bool is_network_open) {

	if (pairing_mode_switcher < 0xFFFF) {

		ESPUI.updateNumber(pairing_mode_switcher, is_network_open ? 1 : 0);

		if (!is_network_open) {

			force_leave_global_flag = false;
			ESPUI.updateNumber(force_leave_switcher, 0);
		}
	}
}

/*****************************************************************************/

void irDeviceSelectorCallback(BasicControl *sender, int type, void *param) {

	int32_t device_slot = (sender) ? sender->getValueInt() : type;
	addGUICommand(gui_cmd_ir_device_callback, device_slot);
}

/*****************************************************************************/

void irDeviceSelectorCmd(int32_t device_slot) {

	char buf[256] = {};
	current_ir_device_id = device_slot;

	ir_code_learning_gui_active = false;

	if (device_slot >= 0) {
		Z2S_Device *z2s_device = Z2S_Device::getZ2SDeviceByDeviceIndex(device_slot);
		if (z2s_device) {
			snprintf_P(
			  buf, sizeof(buf),
			  "<b><i>Manufacturer name</i></b> %s <b>| <i>model ID</i></b> %s "
			  "<b>| <i>Short addr</i></b> 0x%04X",
			  z2s_device->getManufacturerName(), z2s_device->getModelName(),
			  z2s_device->getDeviceShortAddress());
			updateLabel_P(ir_device_info_label, buf);
		}
	} else {
		updateLabel_C(ir_device_info_label, three_dots_str);
	}

	updateIrCodeButtons();
}

/*****************************************************************************/

void irCodesTableCallback(BasicControl *sender, int type, void *param) {

	if (type != B_UP) return;

	z2s_ir_code_t ir_code = {};

	switch ((uint32_t)param) {

		case GUI_CB_IR_CODE_NEW_FLAG: {
			if (current_ir_gui_state != VIEW_IR_CODE) return;
			new_ir_code_id = irCodesStorage.findFreeIndex();
			if (new_ir_code_id < 0) {
				ESPUI.updateLabel(ir_code_state_label, "IR codes storage is full!");
				return;
			}
			current_ir_gui_state = NEW_IR_CODE;
			enableIrCodeDetails(true);
			if (strlen(getLearnedIrCodeBase64()) > 0) {
				working_str = getLearnedIrCodeBase64();
				ESPUI.updateText(ir_code_base64_text, working_str);
			}
			updateIrCodeButtons();
			enableIrTable(false);
		} break;

		case GUI_CB_IR_CODE_EDIT_FLAG: {
			if (current_ir_gui_state != VIEW_IR_CODE || current_ir_code_id < 0) return;
			if (!irCodesStorage.load(current_ir_code_id, ir_code)) return;

			current_ir_gui_state = EDIT_IR_CODE;
			enableIrCodeDetails(true);
			working_str = ir_code.ir_code_name;
			ESPUI.updateText(ir_code_name_text, working_str);
			working_str = ir_code.ir_code_base64;
			ESPUI.updateText(ir_code_base64_text, working_str);
			updateIrCodeButtons();
			enableIrTable(false);
		} break;

		case GUI_CB_IR_CODE_COPY_FLAG: {
			if (current_ir_gui_state != VIEW_IR_CODE || current_ir_code_id < 0) return;
			new_ir_code_id = irCodesStorage.findFreeIndex();
			if (new_ir_code_id < 0) {
				ESPUI.updateLabel(ir_code_state_label, "IR codes storage is full!");
				return;
			}
			if (!irCodesStorage.load(current_ir_code_id, ir_code)) return;

			current_ir_gui_state = COPY_IR_CODE;
			enableIrCodeDetails(true);
			working_str = ir_code.ir_code_name;
			ESPUI.updateText(ir_code_name_text, working_str);
			working_str = ir_code.ir_code_base64;
			ESPUI.updateText(ir_code_base64_text, working_str);
			updateIrCodeButtons();
			enableIrTable(false);
		} break;

		case GUI_CB_IR_CODE_SAVE_FLAG: {
			if (current_ir_gui_state == VIEW_IR_CODE) return;

			const char *name_str = ESPUI.getControl(ir_code_name_text)->getValueCstr();
			const char *b64_str = ESPUI.getControl(ir_code_base64_text)->getValueCstr();

			if (strlen(name_str) == 0 || strlen(b64_str) == 0) {
				ESPUI.updateLabel(ir_code_state_label, "Name and Base64 code are required!");
				return;
			}

			size_t name_len = mbstrnlen(name_str, IR_CODE_NAME_MAX_SIZE - 1);
			strncpy(ir_code.ir_code_name, name_str, name_len);
			ir_code.ir_code_name[name_len] = '\0';

			strncpy(ir_code.ir_code_base64, b64_str, IR_CODE_BASE64_MAX_SIZE - 1);
			ir_code.ir_code_base64[IR_CODE_BASE64_MAX_SIZE - 1] = '\0';

			bool is_edit = (current_ir_gui_state == EDIT_IR_CODE);
			int16_t target_slot = is_edit ? current_ir_code_id : new_ir_code_id;

			if (irCodesStorage.save(target_slot, ir_code)) {
				current_ir_code_id = target_slot;
				current_ir_code_counter = irCodesStorage.getCounter(current_ir_code_id);
				ESPUI.updateLabel(ir_code_state_label, "IR code saved successfully!");
			} else {
				ESPUI.updateLabel(ir_code_state_label, "Saving IR code FAILED!");
			}

			current_ir_gui_state = VIEW_IR_CODE;
			enableIrCodeDetails(false);

			if (is_edit) {
				addGUICommand(gui_cmd_edit_ir_codes_table, current_ir_code_counter - 1);
			} else {
				addGUICommand(gui_cmd_build_ir_codes_table, current_ir_code_counter - 1);
			}
			updateIrCodeButtons();
		} break;

		case GUI_CB_IR_CODE_CANCEL_FLAG: {
        // 1. If learning is active, cancel the learning state first
        if (ir_code_learning_gui_active) {
          ir_code_learning_gui_active = false;

          if (current_ir_device_id >= 0) {
            addGUICommand(gui_cmd_learn_ir_code, -(current_ir_device_id + 1));
          }

          if (current_ir_gui_state == VIEW_IR_CODE) {
            enableIrTable(true);
          }
          updateIrCodeButtons();
          ESPUI.updateLabel(ir_code_state_label, "IR learning cancelled!");
          return;
        }

        // 2. Otherwise, standard cancel for ADD / EDIT / COPY mode
        if ((current_ir_gui_state != NEW_IR_CODE) &&
            (current_ir_gui_state != EDIT_IR_CODE) &&
            (current_ir_gui_state != COPY_IR_CODE))
          return;

        current_ir_gui_state = VIEW_IR_CODE;
        enableIrCodeDetails(false);
        enableIrTable(true);
        updateIrCodeButtons();
        ESPUI.updateLabel(ir_code_state_label, "Changes cancelled!");
      } break;

		case GUI_CB_IR_CODE_LEARN_FLAG: {
        if (current_ir_device_id < 0 || ir_code_learning_gui_active)
          return;

        if (current_ir_gui_state == VIEW_IR_CODE &&
            irCodesStorage.findFreeIndex() == -1) {
          ESPUI.updateLabel(
            ir_code_state_label,
            "IR codes storage is full - cannot learn new code!");
          return;
        }

        ir_code_learning_gui_active = true;
        enableIrTable(false);
        updateIrCodeButtons();

        ESPUI.updateLabel(
          ir_code_state_label,
          "Waiting for IR signal... Point remote at IR device and press a "
          "button (or click CANCEL to abort).");

        addGUICommand(gui_cmd_learn_ir_code, current_ir_device_id);
      } break;


			case GUI_CB_IR_CODE_SEND_FLAG: {
				
        if (current_ir_device_id < 0 || current_ir_code_id < 0) {
          ESPUI.updateLabel(ir_code_state_label, "Select a device and an IR code first!");
          return;
        }

        ESPUI.updateLabel(ir_code_state_label, "Sending IR code to device...");
        addGUICommand(gui_cmd_send_ir_code, current_ir_code_id);
      } break;


			case GUI_CB_IR_CODE_REMOVE_FLAG: {

        if (current_ir_code_id < 0) {
          ESPUI.updateLabel(ir_code_state_label, "Select an IR code to delete first!");
          return;
        }

				addGUICommand(gui_cmd_delete_ir_code, current_ir_code_id);
        
      } break;
	}
}

/*****************************************************************************/

void GUI_onIrCodeLearned() {

	if (GUIstarted)
		addGUICommand(gui_cmd_ir_code_learned);
}

/*****************************************************************************/

#endif  //USE_SUPLA_WEB_SERVER