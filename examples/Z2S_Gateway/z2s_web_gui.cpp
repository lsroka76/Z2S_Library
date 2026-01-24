#ifndef USE_SUPLA_WEB_SERVER

#include <ZigbeeGateway.h>

#include "z2s_web_gui.h"
#include "z2s_devices_database.h"
#include "z2s_devices_table.h"
#include "z2s_device_tuya_custom_cluster.h"
#include "z2s_device_virtual_relay.h"
#include "z2s_device_local_action_handler.h"
#include "TuyaDatapoints.h"

#include "z2s_version_info.h"

#include <SuplaDevice.h>
#include <supla/storage/littlefs_config.h>
#include <supla/storage/storage.h>

#include <supla/device/register_device.h>

#include "web_gui_templates.h"

ESPAsyncHTTPUpdateServer updateServer;

extern ZigbeeGateway zbGateway;

extern bool force_leave_global_flag;

extern int8_t  _enable_gui_on_start;
extern uint8_t	_force_config_on_start;
extern int32_t _gui_start_delay;
extern uint8_t _rebuild_Supla_channels_on_start;
extern uint8_t _use_new_at_model;

extern uint8_t _z2s_security_level;

#define MAX_ATTRIBUTE_ID_SELECTOR_OPTIONS 24
#define MAX_ATTRIBUTE_VALUE_SELECTOR_OPTIONS 6

static volatile bool GUIstarted   = false;

//UI handles
uint16_t gateway_general_info;
uint16_t gateway_memory_info;
uint16_t enable_gui_switcher;
uint16_t force_config_switcher;
uint16_t gui_start_delay_number;
//uint16_t gui_start_delay_save_button;
uint16_t gateway_mdns_name_text;
uint16_t rebuild_Supla_channels_switcher;
uint16_t use_new_at_model_switcher;

uint16_t wifi_ssid_text;
uint16_t wifi_pass_text;
uint16_t Supla_server;
uint16_t Supla_email;
uint16_t Supla_skip_certificate_switcher;
uint16_t save_button;
uint16_t save_label;

uint16_t pairing_mode_switcher = 0xFFFF;
uint16_t force_leave_switcher;
uint16_t zigbee_tx_power_text;
//uint16_t zigbee_get_tx_power_button;
//uint16_t zigbee_set_tx_power_button;
uint16_t zigbee_primary_channel_text;
//uint16_t zigbee_get_primary_channel_button;
//uint16_t zigbee_set_primary_channel_button;
uint16_t zigbee_last_binding_result_label;
uint16_t zigbee_installation_code_str;
uint16_t zigbee_installation_code_ieee_address;
uint16_t factory_reset_switcher;
//uint16_t factory_reset_button;
uint16_t factory_reset_label;

uint16_t devicestab;
uint16_t device_selector;
uint16_t device_selector_main_option;
uint16_t zb_device_info_label;
uint16_t device_status_label;
uint16_t device_name_text; 
uint16_t device_name_save_button;

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

uint16_t device_attribute_id_selector_first_option_id = 0xFFFF;
uint16_t device_attribute_id_selector_last_option_id  = 0xFFFF;

uint16_t device_attribute_value_selector_first_option_id = 0xFFFF;
uint16_t device_attribute_value_selector_last_option_id  = 0xFFFF;	

//uint16_t remove_device_button;
uint16_t remove_device_and_channels_button;
uint16_t remove_all_devices_button;

uint16_t channel_selector;
uint16_t channel_selector_first_option_id = 0xFFFF;
uint16_t channel_selector_last_option_id  = 0xFFFF;
uint16_t channel_status_label;
uint16_t zb_channel_info_label; 
uint16_t zb_channel_timings_label;
uint16_t zb_channel_flags_label;
uint16_t zb_channel_params_label;
uint16_t channel_name_text; //, channel_desc_number, channel_sub_id_number;
uint16_t channel_name_save_button;
uint16_t channel_local_function;
uint16_t disable_channel_notifications_switcher;
uint16_t trv_auto_to_schedule_switcher;
uint16_t trv_auto_to_schedule_manual_switcher;
uint16_t trv_fixed_calibration_switcher;
uint16_t trv_cooperative_childlock_switcher;
uint16_t set_sorwns_on_start_switcher;
uint16_t enable_resend_temperature_switcher;
uint16_t param_1_number;
uint16_t param_1_desc_label;
uint16_t param_1_save_button; 
uint16_t param_2_number;
uint16_t param_2_desc_label;
uint16_t param_2_save_button; 
uint16_t keepalive_number;
uint16_t timeout_number;
uint16_t refresh_number;
uint16_t keepalive_save_button; 
uint16_t timeout_save_button; 
uint16_t refresh_save_button;
uint16_t remove_channel_button;
uint16_t remove_all_channels_button;

uint16_t advanced_device_selector;
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
//Tuya_datapoint_value_text,
Tuya_datapoint_description_label,
Tuya_device_info_label,
Tuya_device_cmd_result_label,
Tuya_device_payload_label,
Tuya_device_data_request_button,
Tuya_device_data_query_button,
Tuya_device_last_enum_position
};

uint16_t Tuya_devices_tab_controls_table[Tuya_device_last_enum_position];
uint16_t Tuya_devices_id_selector_first_option_id = 0xFFFF;
uint16_t Tuya_devices_id_selector_last_option_id  = 0xFFFF;

uint16_t table_first_button;
uint16_t table_next_button;
uint16_t table_prev_button;
uint16_t table_last_button;
uint16_t actions_table_label;
uint16_t action_name_text;
uint16_t action_description_text;
uint16_t action_enabled_switcher;
uint16_t action_source_channel_selector 			 = 0xFFFF;
uint16_t action_destination_channel_selector	 = 0xFFFF;
uint16_t action_event_selector;
uint16_t action_action_selector; 
//uint16_t action_condition_selector;
uint16_t action_condition_threshold_1_number;
uint16_t action_condition_threshold_2_number; 
uint16_t action_save_button;
uint16_t action_cancel_button;
uint16_t action_edit_button; 
uint16_t action_new_button; 
uint16_t action_copy_button;
uint16_t action_remove_button;
uint16_t action_state_label;
uint16_t action_source_channel_selector_first_option_id = 0xFFFF;
uint16_t action_source_channel_selector_last_option_id  = 0xFFFF;
uint16_t action_destination_channel_selector_first_option_id = 0xFFFF;
uint16_t action_destination_channel_selector_last_option_id	= 0xFFFF;

//uint8_t	 attribute_id_selector_options_count = 0;
//uint16_t attribute_id_selector_options[MAX_ATTRIBUTE_ID_SELECTOR_OPTIONS];

//uint8_t	 attribute_value_selector_options_count = 0;
//uint16_t attribute_value_selector_options[MAX_ATTRIBUTE_VALUE_SELECTOR_OPTIONS];

volatile bool data_ready = false;

volatile uint8_t gui_command = 0;

volatile uint32_t gui_callback_reentry_number = 0;

volatile uint8_t	remove_all_devices_counter = 2; 


#define GUI_COMMAND_DEVICE_NAME_CHANGE		10
#define GUI_COMMAND_CHANNEL_NAME_CHANGE		11

#define GUI_COMMAND_CLUSTER_ID_CHANGE			34
#define GUI_COMMAND_ATTRIBUTE_ID_CHANGE		35

#define GUI_BUILD_CONTROL_FLAG_GATEWAY 			0x0001
#define GUI_BUILD_CONTROL_FLAG_CREDENTIALS 	0x0002
#define GUI_BUILD_CONTROL_FLAG_ZIGBEE 			0x0004
#define GUI_BUILD_CONTROL_FLAG_DEVICES 			0x0008
#define GUI_BUILD_CONTROL_FLAG_CHANNELS 		0x0010
#define GUI_BUILD_CONTROL_FLAG_ACTIONS 			0x0020
#define GUI_BUILD_CONTROL_FLAG_CA 					0x0040
#define GUI_BUILD_CONTROL_FLAG_AD 					0x0080
#define GUI_BUILD_CONTROL_FLAG_TCC 					0x0100

volatile uint32_t gui_build_control_flags = 0x00;


volatile uint8_t controls_enabled_flags = 0xFF;

#define DEVICES_CONTROLS_ENABLED_FLAG 															0x01
#define CLUSTERS_ATTRIBUTES_CONTROLS_ENABLED_FLAG 									0x02
#define CHANNELS_CONTROLS_ENABLED_FLAG 															0x04
#define TUYA_DEVICES_ENABLED_FLAG																		0x08

volatile uint32_t advanced_device_present_flags = 0x00;

#define SONOFF_VALVE_PRESENT_FLAG																		0x01
#define TUYA_GAS_DETECTOR_PRESENT_FLAG															0x02
#define MOES_ALARM_PRESENT_FLAG																			0x04

bool isSonoffValvePresent 		= false;
bool isTuyaGasDetectorPresent = false;
bool isMoesAlarmPresent				= false;

volatile bool channels_selector_build_required = true;

volatile uint16_t custom_cmd_tsn = 0;
volatile uint8_t 	Tuya_custom_cmd_dp = 0xFF;
volatile uint16_t current_Tuya_payload_label = 0;

volatile int16_t current_action_counter = -1;
volatile int16_t new_action_id = -1;
volatile int16_t current_action_id = -1;

volatile ActionGUIState current_action_gui_state = VIEW_ACTION;
volatile ActionGUIState previous_action_gui_state = VIEW_ACTION;

#define GUI_CB_ENABLE_GUI_FLAG										0x0100
#define GUI_CB_FORCE_CONFIG_FLAG									0x0101
#define GUI_CB_GUI_DELAY_FLAG											0x0102
#define GUI_CB_SAVE_MDNS_NAME_FLAG								0x0103
#define GUI_CB_REBUILD_CHANNELS_FLAG							0x0104
#define GUI_CB_USE_NEW_AT_FLAG										0x0105
#define GUI_CB_GUI_RESTART_FLAG										0x0106


#define GUI_CB_SAVE_FLAG													0x1000
#define GUI_CB_RESTART_FLAG												0x1001

#define GUI_CB_PAIRING_FLAG												0x2001
#define GUI_CB_FORCE_LEAVE_FLAG										0x2002
#define GUI_CB_FACTORY_FLAG												0x2003
#define GUI_CB_GET_TX_FLAG												0x2004
#define GUI_CB_SET_TX_FLAG												0x2005
#define GUI_CB_GET_PC_FLAG												0x2006
#define GUI_CB_SET_PC_FLAG												0x2007
#define GUI_CB_SET_INSTALLATION_CODE_FLAG					0x2008
#define GUI_CB_CLEAR_INSTALLATION_CODES_FLAG			0x2009

#define GUI_CB_BATTERY_VOLTAGE_MIN_FLAG						0x3000
#define GUI_CB_BATTERY_VOLTAGE_MAX_FLAG						0x3001
#define GUI_CB_DISABLE_PERCENTAGE_MSG_FLAG				0x3002
#define GUI_CB_DISABLE_VOLTAGE_MSG_FLAG 					0x3003

#define GUI_CB_UPDATE_DEVICE_NAME_FLAG						0x3005

#define GUI_CB_SWBUILD_FLAG												0x3010
#define GUI_CB_RSSI_FLAG													0x3011

#define GUI_CB_READ_ATTR_FLAG											0x3020
#define GUI_CB_WRITE_ATTR_FLAG										0x3021
#define GUI_CB_CONFIG_REPORT_FLAG									0x3022
#define GUI_CB_READ_CONFIG_FLAG										0x3023
#define GUI_CB_CUSTOM_CMD_FLAG										0x3024

#define GUI_CB_BIND_CLUSTER_FLAG									0x3028
#define GUI_CB_BIND_CLUSTER_2_FLAG								0x3029

#define GUI_CB_SINGLE_FLAG												0x3030
#define GUI_CB_WITH_CHANNELS_FLAG									0x3031
#define GUI_CB_ALL_FLAG														0x3032

#define GUI_CB_UPDATE_CHANNEL_NAME_FLAG						0x4000
#define GUI_CB_UPDATE_CHANNEL_DESC_FLAG						0x4001
#define GUI_CB_UPDATE_CHANNEL_SUB_ID_FLAG					0x4002
#define GUI_CB_UPDATE_CHANNEL_FUNC_FLAG						0x4003
#define GUI_CB_UPDATE_CHANNEL_LOCAL_FUNCTION_FLAG	0x4004
#define GUI_CB_DISABLE_CHANNEL_NOTIFICATIONS_FLAG	0x4005
#define GUI_CB_TRV_AUTO_TO_SCHEDULE_FLAG					0x4006
#define GUI_CB_SET_SORWNS_ON_START_FLAG						0x4007
#define GUI_CB_TRV_FIXED_CALIBRATION_FLAG					0x4008
#define GUI_CB_TRV_AUTO_TO_SCHEDULE_MANUAL_FLAG		0x4009
#define GUI_CB_TRV_COOPERATIVE_CHILDLOCK_FLAG			0x400A
#define GUI_CB_ENABLE_RESEND_TEMPERATURE_FLAG			0x400B

#define GUI_CB_UPDATE_KEEPALIVE_FLAG							0x4020
#define GUI_CB_UPDATE_TIMEOUT_FLAG								0x4021
#define GUI_CB_UPDATE_REFRESH_FLAG								0x4022

#define GUI_CB_UPDATE_PARAM_1_FLAG								0x4030
#define GUI_CB_UPDATE_PARAM_2_FLAG								0x4031

#define GUI_CB_ADD_AND_HANDLER_FLAG								0x4050
#define GUI_CB_ADD_OR_HANDLER_FLAG								0x4051
#define GUI_CB_ADD_XOR_HANDLER_FLAG								0x4052
#define GUI_CB_ADD_NOT_HANDLER_FLAG								0x4053
#define GUI_CB_ADD_NAND_HANDLER_FLAG							0x4054
#define GUI_CB_ADD_NOR_HANDLER_FLAG								0x4055
#define GUI_CB_ADD_AND3_HANDLER_FLAG							0x4056
#define GUI_CB_ADD_OR3_HANDLER_FLAG								0x4057

#define GUI_CB_SAVE_PROGRAM_FLAG									0x5000
#define GUI_CB_LOAD_PROGRAM_FLAG									0x5001
#define GUI_CB_START_PROGRAM_FLAG									0x5002
#define GUI_CB_STOP_PROGRAM_FLAG									0x5003
#define GUI_CB_SEND_PROGRAM_FLAG									0x5004

#define GUI_CB_SELF_TEST_FLAG											0x6000
#define GUI_CB_SILENCE_FLAG												0x6001
#define GUI_CB_SEND_RINGTONE_FLAG									0x6002
#define GUI_CB_SEND_TIME_FLAG											0x6003

#define GUI_CB_SEND_VOLUME_FLAG										0x7000
#define GUI_CB_SEND_MELODY_FLAG										0x7001
#define GUI_CB_SEND_DURATION_FLAG									0x7002

#define GUI_CB_SEND_TUYA_REQUEST_FLAG							0x8000
#define GUI_CB_SEND_TUYA_QUERY_FLAG								0x8001

#define GUI_CB_ACTION_FIRST_FLAG									0x9000
#define GUI_CB_ACTION_NEXT_FLAG										0x9001
#define GUI_CB_ACTION_PREV_FLAG										0x9002
#define GUI_CB_ACTION_LAST_FLAG										0x9003

#define GUI_CB_ACTION_EDIT_FLAG										0x9010
#define GUI_CB_ACTION_NEW_FLAG										0x9011
#define GUI_CB_ACTION_SAVE_FLAG										0x9012
#define GUI_CB_ACTION_CANCEL_FLAG									0x9013
#define GUI_CB_ACTION_REMOVE_FLAG									0x9014
#define GUI_CB_ACTION_COPY_FLAG										0x9015




static constexpr char* three_dots_str PROGMEM = "...";
static constexpr char* empty_str PROGMEM = "";

static constexpr char* device_query_failed_str 								PROGMEM = "Device data query failed - try to wake it up first!";
static constexpr char* device_async_query_str 								PROGMEM = "Device data query sent asynchronously";
static constexpr char* device_query_attr_size_error_str 			PROGMEM = "Error - attribute size has to be in range 0..255";
static constexpr char* Tuya_device_payload_size_error_str 		PROGMEM = "Error - Tuya DP payload size has to be in range 0..58";
static constexpr char* device_query_attr_size_mismatch_str 		PROGMEM = "Error - attribute size and attribute value length mismatch";
static constexpr char* Tuya_device_payload_size_mismatch_str	PROGMEM = "Error - Tuya payload type length and payload value length mismatch";

static constexpr char* factory_reset_enabled_str PROGMEM = 
	"Zigbee stack factory reset enabled";

static constexpr char* factory_reset_disabled_str PROGMEM = 
	"Zigbee stack factory reset disabled";

static constexpr char* zigbee_tx_power_text_str PROGMEM = 
	"Press Read to get current value or enter value between "
	"-24 and 20 and press Update";

static constexpr char* zigbee_primary_channel_text_str PROGMEM = 
	"Press Read to get current value or enter value between "
	"11 and 26 and press Update";

static char general_purpose_gui_buffer[1024] = {};

static constexpr char* disabledstyle PROGMEM = 
	"background-color: #bbb; border-bottom: #999 3px solid;";

const String clearLabelStyle PROGMEM = 
	"background-color: unset; width: 100%;";

const String clearFlagsLabelStyle PROGMEM = 
	"background-color: unset; width: 50%; "
	"vertical-align: text-top;"
//"margin-left: .3rem; margin-right: .3rem; "
	"font-size: 85%; font-style: normal; "
	"font-weight: normal;";

//

static const char* myCustomJS = R"=====(
function myFunction() {
	/*var a = document.createElement('a');
	a.href='/update';
	a.target = '_blank';
	document.body.appendChild(a);
	a.click();*/
	//alert("CLICKED");
	window.location.assign("/update");

};
/*document.addEventListener("click", function(e){
  alert(e.target.id);
	console.log(e.target.getAttribute("id"));
	console.log(e.target.id);

	const target = e.target.closest("#btn31"); 

  if(e.target.id == "#btn31") {
		console.log("#btn31");
    myFunction();
  }
	if(e.target.id == "btn31") {
		console.log("btn31");
    myFunction();
  }
});*/
document.addEventListener("mouseup", function(e){
  //alert(e.target.id);
	console.log(e.target.getAttribute("id"));
	console.log(e.target.id);

	if(e.target.id == "btn31") {
		console.log("btn31");
		e.stopImmediatePropagation();
    myFunction();
  }
});
document.addEventListener("touchend", function(e){
  //alert(e.target.id);
	console.log(e.target.getAttribute("id"));	
	console.log(e.target.id);

	if(e.target.id == "btn31") {
		console.log("btn31");
		e.stopImmediatePropagation();
    myFunction();
  }
});
)=====";

//String switcherLabelStyle = "width: 60px; margin-left: .3rem; margin-right: .3rem; background-color: unset;";
const static String zero_str PROGMEM = "0";
const static String minus_one_str PROGMEM = "-1";
const static String max_int_str PROGMEM = "65535";

static String working_str;

bool isNumber(String& str);

const char* getSuplaChannelTypeName(int32_t channelType);
const char* getSuplaChannelFuncName(int32_t channelType, int32_t channelFunc);
const char* getZ2SDeviceDescName(uint32_t modelID);
const char* getZigbeeDataTypeName(uint8_t dataType);

void buildGatewayTabGUI();
void buildCredentialsGUI();
void buildZigbeeTabGUI();
void buildDevicesTabGUI();
void buildChannelsTabGUI();
void buildAdvancedDevicesTabGUI();
void buildTuyaCustomClusterTabGUI();

void updateChannelInfoLabel(uint8_t label_number);
void updateDeviceInfoLabel();

void gatewayCallback(Control *sender, int type, void *param);
void selectGuiModeCallback(Control *sender, int type);
void enterWifiDetailsCallback(Control *sender, int type, void *param);
void textCallback(Control *sender, int type);
void generalCallback(Control *sender, int type);
void onZigbeeTabCallback(Control *sender, int type);
void onChannelsTabCallback(Control *sender, int type);
void generalMinMaxCallback(Control *sender, int type, void *param);
void endpointCallback(Control *sender, int type);
void switchCallback(Control *sender, int type);
void deviceSelectorCallback(Control *sender, int type);
void clustersattributesdeviceSelectorCallback(Control *sender, int type);
void advancedDeviceSelectorCallback(Control *sender, int type);
void TuyaDeviceSelectorCallback(Control *sender, int type);
void TuyaDatapointIdSelectorCallback(Control *sender, int type);
void TuyaDatapointTypeSelectorCallback(Control *sender, int type);
void enableDeviceControls(bool enable);
void enableClustersAttributesControls(bool enable);
void removeDeviceCallback(Control *sender, int type, void *param);
void channelSelectorCallback(Control *sender, int type);
void enableChannelControls(bool enable);
void enableTuyaDevicesControls(bool enable);
void editDeviceCallback(Control *sender, int type, void *param);
void editChannelCallback(Control *sender, int type, void *param);
void editChannelFlagsCallback(Control *sender, int type, void *param);
void batteryCallback(Control *sender, int type, void *param);
void batterySwitcherCallback(Control *sender, int type, void *param);
void removeChannelCallback(Control *sender, int type);
void removeAllChannelsCallback(Control *sender, int type);
void getClustersAttributesQueryCallback(Control *sender, int type, void *param);
void getZigbeeDeviceQueryCallback(Control *sender, int type, void *param);
void pairingSwitcherCallback(Control *sender, int type, void *param);
void generalZigbeeCallback(Control *sender, int type, void *param);
void datatypeCallback(Control *sender, int type);
void clusterCallback(Control *sender, int type);
void attributeCallback(Control *sender, int type);
void valueCallback(Control *sender, int type);
void valveCallback(Control *sender, int type, void *param);
void TuyaCustomCmdCallback(Control *sender, int type, void *param);
void actionsTableCallback(Control *sender, int type, void *param);
void addLocalActionHandlerCallback(Control *sender, int type, void *param);
void addLocalVirtualRelayCallback(Control *sender, int type);
void addLocalVirtualBinaryCallback(Control *sender, int type);
void addLocalRemoteRelayCallback(Control *sender, int type);
void addLocalRemoteThermometerCallback(Control *sender, int type);
void addLocalVirtualHvacCallback(Control *sender, int type);

void enableControlStyle(uint16_t control_id, bool enable);

void clearAttributeIdSelect();
void clearAttributeValueSelect();
void clusterCallbackCmd();

void rebuildTuyaDevicesDatapointsList(uint8_t Tuya_device_slot);

void fillGatewayGeneralnformation(char *buf);
void fillMemoryUptimeInformation(char *buf);

/*****************************************************************************/

bool isNumber(String& str) {

    unsigned int stringLength = str.length();
 
    if (stringLength == 0) {
        return false;
    }
 
    //boolean seenDecimal = false;
 
    for(unsigned int i = 0; i < stringLength; ++i) {
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

const char* getSuplaChannelTypeName(int32_t channelType) {

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

const char* getSuplaChannelFuncName(int32_t channelType, int32_t channelFunc) {

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

const char* getZ2SDeviceDescName(uint32_t modelID)  {

	switch (modelID) {

		case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR:
		case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1:
		case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR:
			return "Temperature and humidity sensor(0x402, 0x405)";
		case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL:
			return "Temperature and humidity sensor(0x20, 0x402, 0x405)";
		case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR:
			return "Temperature and humidity sensor(Tuya 0xEF00)";
		case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10:
			return "Temperature and humidity(x10) sensor(0x402, 0x405)";
		case Z2S_DEVICE_DESC_TEMPHUMIPRESSURE_SENSOR:
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

const char* getZigbeeDataTypeName(uint8_t dataType)  {

	uint32_t zigbee_datatypes_count = sizeof(zigbee_datatypes)/sizeof(zigbee_datatype_t);

	for (uint8_t datatypes_counter = 0; datatypes_counter < zigbee_datatypes_count; datatypes_counter++) {
  
		if (zigbee_datatypes[datatypes_counter].zigbee_datatype_id == dataType)
			return zigbee_datatypes[datatypes_counter].zigbee_datatype_name;
	}
	return PSTR("Unknown data type");
}

/*****************************************************************************/

void fillGatewayGeneralnformation(char *buf) {

	if (buf) {
		
		char Supla_GUID_str[128] = {};
		char extended_pan_id_str[24] = {};
		esp_zb_ieee_addr_t extended_pan_id;

		generateHexString(Supla::RegisterDevice::getGUID(), Supla_GUID_str, SUPLA_GUID_SIZE);
		
		esp_zb_get_extended_pan_id(extended_pan_id);
		ieee_addr_to_str(extended_pan_id_str, extended_pan_id);
		
		snprintf_P(buf, 1024, PSTR("<b><i>Supla firmware:</i></b> <i>%s</i><br><br>"
															 "<b><i>Supla GUID:</i></b> <i>%s</i><br><br>"
															 "<b><i>Z2S Gateway version:</i></b> <i>%s</i><br><br>"
															 "<b><i>Network extended PAN ID:</i></b> <i>%s</i><br><br>"), 
							 Supla::RegisterDevice::getSoftVer(), 
							 Supla_GUID_str, 
							 Z2S_VERSION,
							 extended_pan_id_str);
	
		log_i("Device information %s", buf);

	}
}

/*****************************************************************************/

void fillMemoryUptimeInformation(char *buf) {

	if (buf) {
		time_t local_time_info;
		time(&local_time_info);

		snprintf_P(buf, 1024, PSTR("<b><i>Flash chip real size:</b></i> %u B <b>| <i>Free Sketch Space:</b></i> %u B<br>"
						"<b><i>HeapSize:</b></i> %u B <b>| <i>FreeHeap:</b></i> %u B <b>| <i>"
						"MinimalFreeHeap:</b></i> %u B <b>| <i>MaxAllocHeap:</b></i> %u B<br>"
						"<b><i>uxTaskGetStackHighWaterMark:</b></i> %u B <br>"
						"<b><i>Total PSRAM:</b></i> %u B <b>| <i>Free PSRAM:</b></i> %u B<br><br>"
						"<b><i>Local time:</i></b> %s<b><i>Supla uptime:</i></b> %lu s"), 
						ESP.getFlashChipSize(), ESP.getFreeSketchSpace(), 
						ESP.getHeapSize(), ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getMaxAllocHeap(), 
						uxTaskGetStackHighWaterMark(NULL),
						ESP.getPsramSize(), ESP.getFreePsram(), 
						ctime(&local_time_info),  SuplaDevice.uptime.getUptime());

		log_i("Memory & uptime information %s", buf);
	}
}

/*****************************************************************************/

void enableControlStyle(uint16_t control_id, bool enable) {

	ESPUI.setEnabled(control_id, enable);
	if (enable) {

		working_str = PSTR(";");
		ESPUI.setPanelStyle(control_id, working_str);
	}
	else {

		working_str = disabledstyle;
		ESPUI.setPanelStyle(control_id, working_str);	
	}
}

/*****************************************************************************/

void updateLabel_P(Control::ControlId_t id, const char* value) {

	working_str = value;
	ESPUI.updateLabel(id, working_str);
}

/*****************************************************************************/

void buildGatewayTabGUI() {

	//char buf[1024] = {};
	working_str = PSTR("Gateway");
	auto gatewaytab = ESPUI.addControl(
		Control::Type::Tab, PSTR(empty_str), working_str, Control::Color::Emerald,
		Control::noParent, generalCallback);

	working_str = PSTR(empty_str);
	ESPUI.addControl(
		Control::Type::Separator, PSTR("General information"), working_str, 
		Control::Color::None, gatewaytab);

	fillGatewayGeneralnformation(general_purpose_gui_buffer);

	working_str = general_purpose_gui_buffer;
	gateway_general_info = ESPUI.addControl(
		Control::Type::Label, PSTR("Device information"), working_str, 
		Control::Color::Emerald, gatewaytab);

	ESPUI.setElementStyle(
		gateway_general_info, 
		"color:black;text-align: justify; font-family:tahoma;"
		" font-size: 4 px; font-style: normal; font-weight: normal;");

	ESPUI.setPanelWide(gateway_general_info, true);

	fillMemoryUptimeInformation(general_purpose_gui_buffer);
	
	working_str = PSTR(empty_str);
	ESPUI.addControl(
		Control::Type::Separator, PSTR("Status"), working_str, 
		Control::Color::None, gatewaytab);

	working_str = general_purpose_gui_buffer;
	gateway_memory_info = ESPUI.addControl(
		Control::Type::Label, PSTR("Memory & Uptime"), working_str, 
		Control::Color::Emerald, gatewaytab);

	ESPUI.setElementStyle(
		gateway_memory_info, 
		"color:black;text-align: justify; font-family:tahoma;"
		" font-size: 4 px; font-style: normal; font-weight: normal;");
	ESPUI.setPanelWide(gateway_memory_info, true);

	auto gui_mode_selector = ESPUI.addControl(
		Control::Type::Select, PSTR("Select GUI mode (requires restart)"), 
		zero_str, Control::Color::Emerald, gatewaytab, selectGuiModeCallback);

	for (uint8_t modes_counter = no_gui_mode; 
			 modes_counter < gui_modes_number; modes_counter++) {

  	working_str = modes_counter;
		ESPUI.addControl(
			Control::Type::Option, GUI_MODE_OPTIONS[modes_counter], working_str, 
			Control::Color::None, gui_mode_selector);
	}

	working_str = PSTR(
		"When GUI is disabled on start use 5x BOOT to enable it.");

	ESPUI.setElementStyle(
		ESPUI.addControl(
			Control::Type::Label, PSTR(empty_str), working_str, 
			Control::Color::None, gui_mode_selector), 
		PSTR(clearLabelStyle));

	gui_start_delay_number = ESPUI.addControl(
		Control::Type::Number, PSTR("GUI start delay (s)"), zero_str, 
		Control::Color::Emerald, gatewaytab, generalMinMaxCallback, (void*)3600);

	working_str = PSTR("Save");
	auto gui_start_delay_save_button = ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, gui_start_delay_number, gatewayCallback,
		(void*)GUI_CB_GUI_DELAY_FLAG);

	gateway_mdns_name_text = ESPUI.addControl(
		Control::Type::Text, PSTR("Gateway local mDNS name"), empty_str, 
		Control::Color::Emerald, gatewaytab, generalCallback);

	working_str = PSTR("Save");
	auto gateway_mdns_name_save_button = ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, gateway_mdns_name_text, gatewayCallback, 
		(void*)GUI_CB_SAVE_MDNS_NAME_FLAG);

	working_str = PSTR("(max. 11 characters, no spaces!!!)");
	ESPUI.setElementStyle(
		ESPUI.addControl(
			Control::Type::Label, PSTR(empty_str), working_str, 
			Control::Color::None, gateway_mdns_name_text), 
		PSTR(clearLabelStyle));

	force_config_switcher = ESPUI.addControl(
		Control::Type::Switcher, PSTR("Force config mode on next startup"), 
		zero_str, Control::Color::Emerald, gatewaytab, gatewayCallback,
		(void*)GUI_CB_FORCE_CONFIG_FLAG);

	working_str = PSTR("On next startup gateway will enter config mode.");
	ESPUI.setElementStyle(
		ESPUI.addControl(
			Control::Type::Label, PSTR(empty_str), working_str, 
			Control::Color::None, force_config_switcher), 
		PSTR(clearLabelStyle));

	rebuild_Supla_channels_switcher = ESPUI.addControl(
		Control::Type::Switcher, 
		PSTR("Rebuild missing Supla channels on next startup"), 
		zero_str, Control::Color::Emerald, gatewaytab, gatewayCallback,
		(void*)GUI_CB_REBUILD_CHANNELS_FLAG);

	working_str = PSTR(
		"On next startup gateway will try to rebuild missing Supla channels<br>"
		"using existing Zigbee devices definitions (experimental!)");

	ESPUI.setElementStyle(
		ESPUI.addControl(
			Control::Type::Label, PSTR(empty_str), working_str, 
			Control::Color::None, rebuild_Supla_channels_switcher), 
		PSTR(clearLabelStyle));

	use_new_at_model_switcher = ESPUI.addControl(
		Control::Type::Switcher, 
		PSTR("Use new AT model for smart buttons"), 
		zero_str, Control::Color::Emerald, gatewaytab, gatewayCallback,
		(void*)GUI_CB_USE_NEW_AT_FLAG);

	working_str = PSTR(
		"New smart buttons will add as AT with Cloud and local actions");

	ESPUI.setElementStyle(
		ESPUI.addControl(
			Control::Type::Label, PSTR(empty_str), working_str, 
			Control::Color::None, use_new_at_model_switcher), 
		PSTR(clearLabelStyle));

	/**/

	working_str = PSTR("Restart gateway");
	save_button = ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, gatewaytab, gatewayCallback, 
		(void*)GUI_CB_GUI_RESTART_FLAG);

	working_str = PSTR("FIRMWARE UPDATE");
	auto test_button = ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, gatewaytab);

	//ESPUI.updateNumber(enable_gui_switcher, _enable_gui_on_start);
	working_str = _enable_gui_on_start;
	ESPUI.updateSelect(gui_mode_selector, working_str);
	ESPUI.updateNumber(gui_start_delay_number, _gui_start_delay);
	working_str = GatewayMDNSLocalName;
	ESPUI.updateText(gateway_mdns_name_text, working_str);
	ESPUI.updateNumber(force_config_switcher, _force_config_on_start);
	ESPUI.updateNumber(
		rebuild_Supla_channels_switcher, _rebuild_Supla_channels_on_start);
	ESPUI.updateNumber(use_new_at_model_switcher, _use_new_at_model);
}

/*****************************************************************************/

void buildCredentialsGUI() {

	//char buf[512] = {};
	working_str = PSTR("WiFi & Supla credentials");
	auto wifitab = ESPUI.addControl(
		Control::Type::Tab, PSTR(empty_str), working_str);

	working_str = PSTR(empty_str);
	wifi_ssid_text = ESPUI.addControl(
		Control::Type::Text, PSTR("SSID"), working_str, Control::Color::Emerald, 
		wifitab, textCallback);
	working_str = PSTR("32");
	ESPUI.addControl(
		Control::Type::Max, PSTR(empty_str), working_str, Control::Color::None, 
		wifi_ssid_text);

	working_str = PSTR(empty_str);
	wifi_pass_text = ESPUI.addControl(
		Control::Type::Text, PSTR("Password"), working_str, 
		Control::Color::Emerald, wifitab, textCallback);
	working_str = PSTR("64");
	ESPUI.addControl(
		Control::Type::Max, PSTR(empty_str), working_str, Control::Color::None, 
		wifi_pass_text);
	working_str = PSTR("password");
	ESPUI.setInputType(wifi_pass_text, working_str);

	working_str = PSTR(empty_str);
	Supla_server = ESPUI.addControl(
		Control::Type::Text, PSTR("Supla server"), working_str, 
		Control::Color::Emerald, wifitab, textCallback);
	working_str = PSTR("64");
	ESPUI.addControl(
		Control::Type::Max, PSTR(empty_str), working_str, Control::Color::None, 
		Supla_server);

	working_str = PSTR(empty_str);
	Supla_email = ESPUI.addControl(
		Control::Type::Text, PSTR("Supla email"), working_str, 
		Control::Color::Emerald, wifitab, textCallback);
	working_str = PSTR("64");	
	ESPUI.addControl(
		Control::Type::Max, PSTR(empty_str), working_str, Control::Color::None, 
		Supla_email);

	Supla_skip_certificate_switcher = ESPUI.addControl(
		Control::Type::Switcher, PSTR("Skip CA certificate check"), zero_str, 
		Control::Color::Emerald, wifitab, generalCallback);

	working_str = PSTR("Save");
	save_button = ESPUI.addControl(
		Control::Type::Button, PSTR("Save"), working_str, Control::Color::Emerald, 
		wifitab, enterWifiDetailsCallback,(void*)GUI_CB_SAVE_FLAG);
	working_str = PSTR("Save & Restart");
	auto save_n_restart_button = ESPUI.addControl(
		Control::Type::Button, PSTR("Save & Restart"), working_str, 
		Control::Color::Emerald, save_button, enterWifiDetailsCallback, 
		(void*)GUI_CB_RESTART_FLAG);
	working_str = PSTR("Missing data...");
	save_label = ESPUI.addControl(
		Control::Type::Label, PSTR("Status"), working_str, 
		Control::Color::Wetasphalt, save_button);

	auto cfg = Supla::Storage::ConfigInstance();
  
	if (cfg) {

  	memset(general_purpose_gui_buffer, 0, sizeof(general_purpose_gui_buffer));
  	if (cfg->getWiFiSSID(general_purpose_gui_buffer) && strlen(general_purpose_gui_buffer) > 0)
			ESPUI.updateText(wifi_ssid_text, general_purpose_gui_buffer);
		memset(general_purpose_gui_buffer, 0, sizeof(general_purpose_gui_buffer));
		if (cfg->getSuplaServer(general_purpose_gui_buffer) && strlen(general_purpose_gui_buffer) > 0)
			ESPUI.updateText(Supla_server, general_purpose_gui_buffer);
		memset(general_purpose_gui_buffer, 0, sizeof(general_purpose_gui_buffer));
		if (cfg->getEmail(general_purpose_gui_buffer) && strlen(general_purpose_gui_buffer) > 0)
			ESPUI.updateText(Supla_email, general_purpose_gui_buffer);
		ESPUI.updateNumber(Supla_skip_certificate_switcher, _z2s_security_level == 2 ? 1 : 0);
	}			
}

void buildZigbeeTabGUI() {

	working_str = PSTR("Zigbee settings");
	auto zigbeetab = 
		ESPUI.addControl(Control::Type::Tab, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::Emerald, 
										 Control::noParent, 
										 onZigbeeTabCallback);
	
	working_str = PSTR("Pairing mode");
	pairing_mode_switcher = 
		ESPUI.addControl(Control::Type::Switcher, 
										 PSTR("Pairing mode"), 
									   working_str, 
										 Control::Color::Emerald, 
										 zigbeetab, 
										 pairingSwitcherCallback,
										 (void*)GUI_CB_PAIRING_FLAG);

	working_str = PSTR("enable/disable pairing mode (180 s)");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 pairing_mode_switcher), 
												PSTR(clearLabelStyle));

	working_str = empty_str;
	force_leave_switcher = 
		ESPUI.addControl(Control::Type::Switcher, 
										 PSTR(""), 
									   working_str, 
										 Control::Color::Emerald, 
										 pairing_mode_switcher, 
										 pairingSwitcherCallback,
										 (void*)GUI_CB_FORCE_LEAVE_FLAG);
	working_str = PSTR("Force joined device to leave network and perform full join");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 pairing_mode_switcher), 
												PSTR(clearLabelStyle));


	working_str = zigbee_tx_power_text_str;
	zigbee_tx_power_text = 
		ESPUI.addControl(Control::Type::Text, 
										 PSTR("Zigbee TX power"), 
													working_str, 
												  Control::Color::Emerald, 
													zigbeetab, 
													generalCallback);

	working_str = zigbee_primary_channel_text_str;
	zigbee_primary_channel_text = 
		ESPUI.addControl(Control::Type::Text, 
		PSTR("Zigbee primary channel"), 
		working_str, 
		Control::Color::Emerald, 
		zigbeetab, 
		generalCallback);
	
	working_str = PSTR("Read");
	auto zigbee_get_tx_power_button = 
		ESPUI.addControl(Control::Type::Button, 
									   PSTR(empty_str), 
										 working_str, 
										 Control::Color::Emerald, 
										 zigbee_tx_power_text, 
										 generalZigbeeCallback,
										 (void*)GUI_CB_GET_TX_FLAG);

	auto zigbee_get_primary_channel_button = 
		ESPUI.addControl(Control::Type::Button, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::Emerald, 
										 zigbee_primary_channel_text, 
										 generalZigbeeCallback,
										 (void*)GUI_CB_GET_PC_FLAG);

	working_str = PSTR("Update");
	auto zigbee_set_tx_power_button = 
		ESPUI.addControl(Control::Type::Button, 
	  PSTR(empty_str), 
		working_str, 
		Control::Color::Emerald, 
		zigbee_tx_power_text, 
		generalZigbeeCallback,
		(void*)GUI_CB_SET_TX_FLAG);

	auto zigbee_set_primary_channel_button = 
		ESPUI.addControl(Control::Type::Button, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::Emerald, 
										 zigbee_primary_channel_text, 
										 generalZigbeeCallback,
										 (void*)GUI_CB_SET_PC_FLAG);
	
	zigbee_last_binding_result_label = 
		ESPUI.addControl(Control::Type::Label, 
										 PSTR("Last binding result"), 
										 three_dots_str, 
										 Control::Color::Emerald, 
										 zigbeetab);

	working_str = PSTR("Enter device IEEE address");
	zigbee_installation_code_ieee_address = 
		ESPUI.addControl(Control::Type::Text, 
										 PSTR("Installation code"), 
										 working_str, 
										 Control::Color::Emerald, 
										 zigbeetab,
										 generalCallback);

	working_str = PSTR("Enter device installation code");
	zigbee_installation_code_str = 
		ESPUI.addControl(Control::Type::Text, 
										 empty_str, 
										 working_str, 
										 Control::Color::Emerald, 
										 zigbee_installation_code_ieee_address,
										 generalCallback);
										
	working_str = PSTR("Save installation code for single device");
	auto zigbee_set_installation_code = 
		ESPUI.addControl(Control::Type::Button, 
	  PSTR(empty_str), 
		working_str, 
		Control::Color::Emerald, 
		zigbee_installation_code_ieee_address, 
		generalZigbeeCallback,
		(void*)GUI_CB_SET_INSTALLATION_CODE_FLAG);

	working_str = PSTR("Clear all saved installation codes");
	auto zigbee_clear_installation_codes = 
		ESPUI.addControl(Control::Type::Button, 
	  PSTR(empty_str), 
		working_str, 
		Control::Color::Emerald, 
		zigbee_installation_code_ieee_address, 
		generalZigbeeCallback,
		(void*)GUI_CB_CLEAR_INSTALLATION_CODES_FLAG);


	working_str = PSTR(empty_str);
	ESPUI.addControl(Control::Type::Separator, 
									 PSTR("Zigbee stack factory reset"), 
									 working_str, 
									 Control::Color::None, 
									 zigbeetab);

	factory_reset_switcher = 
		ESPUI.addControl(Control::Type::Switcher, 
										 PSTR("Enable Zigbee stack factory reset"), 
													zero_str, 
													Control::Color::Alizarin, 
													zigbeetab, 
													switchCallback);

	working_str = factory_reset_disabled_str;
	factory_reset_label = 
		ESPUI.addControl(Control::Type::Label, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::Wetasphalt, 
										 factory_reset_switcher);

	working_str = PSTR("FACTORY RESET!");
	auto factory_reset_button = 
		ESPUI.addControl(Control::Type::Button, 
										 PSTR("FACTORY RESET!"), 
										 working_str, 
										 Control::Color::Alizarin, 
										 zigbeetab, 
										 generalZigbeeCallback,
										 (void*)GUI_CB_FACTORY_FLAG); 

	ESPUI.updateNumber(pairing_mode_switcher, 
										 Zigbee.isNetworkOpen() ? 1 : 0);
	ESPUI.updateNumber(force_leave_switcher, 
										 force_leave_global_flag ? 1 : 0);
}

void rebuildDevicesSelector() {

	for (uint8_t devices_counter = 0; 
			 devices_counter < Z2S_ZB_DEVICES_MAX_NUMBER; 
			 devices_counter++) {

    if (z2s_zb_devices_table[devices_counter].record_id > 0) {

			ESPUI.removeControl(
				z2s_zb_devices_table[devices_counter].device_gui_id, false);
		}
	}

	ESPUI.updateControlValue(device_selector, minus_one_str);

	for (uint8_t devices_counter = 0; 
			 devices_counter < Z2S_ZB_DEVICES_MAX_NUMBER; 
			 devices_counter++) {

    if (z2s_zb_devices_table[devices_counter].record_id > 0) {

			working_str = devices_counter;
			z2s_zb_devices_table[devices_counter].device_gui_id =  ESPUI.addControl(
				Control::Type::Option, 
				z2s_zb_devices_table[devices_counter].device_local_name, 
				working_str, Control::Color::None, device_selector);
		}
	}
}

/*****************************************************************************/

void buildDevicesTabGUI() {

	working_str = PSTR("Zigbee devices");
	devicestab = ESPUI.addControl(
		Control::Type::Tab, PSTR(empty_str), working_str);
	
	working_str = -1;
	device_selector = ESPUI.addControl(
		Control::Type::Select, PSTR("Devices"), working_str, 
		Control::Color::Emerald, devicestab, deviceSelectorCallback);

	device_selector_main_option =ESPUI.addControl(
		Control::Type::Option, PSTR("Select Zigbee device..."), working_str, 
		Control::Color::None, device_selector);

	for (uint8_t devices_counter = 0; 
			 devices_counter < Z2S_ZB_DEVICES_MAX_NUMBER; devices_counter++) {

    if (z2s_zb_devices_table[devices_counter].record_id > 0) {

			working_str = devices_counter;
			z2s_zb_devices_table[devices_counter].device_gui_id = ESPUI.addControl(
					Control::Type::Option, 
					z2s_zb_devices_table[devices_counter].device_local_name, 
					working_str, Control::Color::None, device_selector);
		}
	}

	ESPUI.setPanelWide(device_selector, true);

	zb_device_info_label = ESPUI.addControl(Control::Type::Label, 
																					PSTR("Device info"), 
																					three_dots_str, 
																					Control::Color::Emerald, 
																					devicestab); 

	ESPUI.setElementStyle(zb_device_info_label, 
												"color:black;text-align: justify; font-family:tahoma; "
												"font-size: 4 px; font-style: normal; font-weight: normal;");
	
	ESPUI.setPanelWide(zb_device_info_label, true);

	working_str = PSTR(empty_str);
	device_name_text = ESPUI.addControl(Control::Type::Text, 
																			PSTR("Device panel"), 
																			working_str, 
																			 Control::Color::Emerald, 
																			 devicestab, 
																			 generalCallback); 

	working_str = PSTR("&#10023; device name (local) &#10023;");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 device_name_text), 
												PSTR(clearLabelStyle));

	working_str = PSTR("Save");
	device_name_save_button = ESPUI.addControl(Control::Type::Button, 
																						 PSTR(empty_str), 
																						 working_str, 
																						 Control::Color::Emerald, 
																						 device_name_text, 
																				  	 editDeviceCallback, 
																						 (void*)GUI_CB_UPDATE_DEVICE_NAME_FLAG);

	working_str = PSTR("Read Software Build ID");
	getswbuild_button = ESPUI.addControl(Control::Type::Button, 
																			 PSTR("Software Build ID / RSSI"), 
																			 working_str, 
																			 Control::Color::Emerald, 
																			 devicestab, 
																			 getZigbeeDeviceQueryCallback, 
																			 (void*)GUI_CB_SWBUILD_FLAG);

	working_str = PSTR("Read device RSSI");
	getrssi_button = ESPUI.addControl(Control::Type::Button, 
																		PSTR(empty_str), 
																		working_str,
																		Control::Color::Emerald, 
																		getswbuild_button, 
																		getZigbeeDeviceQueryCallback, 
																		(void*)GUI_CB_RSSI_FLAG);

	rssilabel = ESPUI.addControl(Control::Type::Label, 
															 PSTR(empty_str), 
															 three_dots_str, 
															 Control::Color::Emerald, 
															 getswbuild_button);
	
	battery_voltage_min_number = 
		ESPUI.addControl(Control::Type::Number, 
										 PSTR("Battery panel"), 
										 zero_str, 
										 Control::Color::Emerald, 
										 devicestab, 
										 generalMinMaxCallback, 
										 (void*)255);

	/*ESPUI.addControl(
		Control::Type::Min, 
		PSTR(empty_str), 
		zero_str, 
		Control::Color::None, 
		battery_voltage_min_number);

	working_str = 255;
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 working_str, 
									 Control::Color::None, 
									 battery_voltage_min_number);*/

	ESPUI.setElementStyle(battery_voltage_min_number, 
												"color:black; margin: 0% 5%;");

	working_str = PSTR("Save");
	battery_voltage_min_save_button = 
		ESPUI.addControl(Control::Type::Button, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::Emerald, 
										 battery_voltage_min_number, 
										 batteryCallback, 
										 (void*)GUI_CB_BATTERY_VOLTAGE_MIN_FLAG);

	
	battery_voltage_max_number = 
		ESPUI.addControl(Control::Type::Number, 
										 PSTR(empty_str), 
										 zero_str, 
										 Control::Color::Emerald, 
										 battery_voltage_min_number, 
										 generalMinMaxCallback, 
										 (void*)255);

	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 battery_voltage_max_number);*/

	ESPUI.setElementStyle(battery_voltage_max_number, 
												"color:black; margin: 0% 5%;");

	/*working_str = 255;
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 working_str, 
									 Control::Color::None, 
									 battery_voltage_max_number);*/

	working_str = PSTR("Save");
	battery_voltage_max_save_button = 
		ESPUI.addControl(Control::Type::Button, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::Emerald, 
										 battery_voltage_min_number, 
										 batteryCallback, 
										 (void*)GUI_CB_BATTERY_VOLTAGE_MAX_FLAG);

	working_str = "";
	ESPUI.setElementStyle(
		ESPUI.addControl(Control::Type::Label, 
										 PSTR(empty_str), 
										 working_str,
										 Control::Color::None, 
										 battery_voltage_min_number), 
		PSTR(clearLabelStyle));
	
	working_str = PSTR("&#10023; V(min) x10, ie. 28 = 2,8V &#10023;");
	ESPUI.setElementStyle(
		ESPUI.addControl(Control::Type::Label, 
										 PSTR(empty_str), 
										 working_str,
										 Control::Color::None, 
										 battery_voltage_min_number), 
		PSTR(clearFlagsLabelStyle));
	
	working_str = PSTR("&#10023; V(max) x10, ie. 33 = 3,3V &#10023;");
	ESPUI.setElementStyle(
		ESPUI.addControl(Control::Type::Label, 
										 PSTR(empty_str), 
										 working_str,
										 Control::Color::None, 
										 battery_voltage_min_number), 
		PSTR(clearFlagsLabelStyle));
	
	disable_battery_percentage_msg_switcher = 
		ESPUI.addControl(Control::Type::Switcher, 
										 PSTR(empty_str), 
										 zero_str, 
										 Control::Color::Emerald, 
										 battery_voltage_min_number, 
										 batterySwitcherCallback, 
										 (void*)GUI_CB_DISABLE_PERCENTAGE_MSG_FLAG);

	working_str = 
		PSTR("&#10023; <i>DISABLE BATTERY PERCENTAGE MESSAGES</i> &#10023;");

	ESPUI.setElementStyle(
		ESPUI.addControl(Control::Type::Label, 
										 PSTR(empty_str), 
										 working_str,
										 Control::Color::None, 
										 battery_voltage_min_number), 
		PSTR(clearLabelStyle));							

	disable_battery_voltage_msg_switcher = 
		ESPUI.addControl(Control::Type::Switcher, 
										 PSTR(empty_str), 
										 zero_str, 
										 Control::Color::Emerald, 
										 battery_voltage_min_number, 
										 batterySwitcherCallback, 
										 (void*)GUI_CB_DISABLE_VOLTAGE_MSG_FLAG);

	working_str = 
		PSTR("&#10023; <i>DISABLE BATTERY VOLTAGE MESSAGES</i> &#10023;");

	ESPUI.setElementStyle(
		ESPUI.addControl(Control::Type::Label, 
										 PSTR(empty_str), 
										 working_str,
										 Control::Color::None, 
										 battery_voltage_min_number), 
		PSTR(clearLabelStyle));							


	ESPUI.addControl(
		Control::Type::Separator, PSTR(empty_str), working_str, 
		Control::Color::None, devicestab);

	/*working_str = PSTR("Remove device!");
	remove_device_button = ESPUI.addControl(
		Control::Type::Button, PSTR("Remove Zigbee device(s)"), working_str, 
		Control::Color::Alizarin, devicestab, removeDeviceCallback, 
		(void*)GUI_CB_SINGLE_FLAG);*/

	working_str = PSTR("Remove device with channels!");
	remove_device_and_channels_button = ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Alizarin, devicestab, removeDeviceCallback, 
		(void*)GUI_CB_WITH_CHANNELS_FLAG);

	working_str = PSTR("Remove all devices with channels!!!");
	remove_all_devices_button = ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Alizarin, remove_device_and_channels_button, 
		removeDeviceCallback, (void*)GUI_CB_ALL_FLAG); 

	device_status_label = ESPUI.addControl(
		Control::Type::Label, PSTR("Status"), three_dots_str, 
		Control::Color::Alizarin, remove_device_and_channels_button);

	enableDeviceControls(false);
}

/*****************************************************************************/

void rebuildChannelsSelector(
	bool rebuild_channels_list, uint16_t channelstab = 0xFFFF) {

	if (rebuild_channels_list) {

		if (channel_selector_first_option_id < 0xFFFF) {

			for (uint8_t options_id_counter = channel_selector_first_option_id; 
					 options_id_counter <= channel_selector_last_option_id; 
					 options_id_counter++) {

    		if (options_id_counter < 0xFFFF) {
      		ESPUI.removeControl(options_id_counter, false);
			}
		}
	}
		ESPUI.updateControlValue(channel_selector, minus_one_str);
	} else {

		channel_selector = ESPUI.addControl(Control::Type::Select, 
																		  PSTR("Channels"), 
																			minus_one_str, 
																			Control::Color::Emerald, 
																			channelstab, 
																			channelSelectorCallback);

	
		ESPUI.addControl(Control::Type::Option, 
										PSTR("Select Zigbee channel..."), 
										minus_one_str, 
										Control::Color::None, 
										channel_selector);

		
		ESPUI.setPanelWide(channel_selector, true);
	}

	
	uint16_t current_option_id = 0xFFFF;

	channel_selector_first_option_id 	= 0xFFFF;
	channel_selector_last_option_id 	= 0xFFFF;

	for (uint8_t channels_counter = 0; 
			 channels_counter < Z2S_CHANNELS_MAX_NUMBER; 
			 channels_counter++) {

    if (z2s_channels_table[channels_counter].valid_record) {
      
			working_str = channels_counter;
			//z2s_channels_table[channels_counter].gui_control_id  = 
			current_option_id =
				ESPUI.addControl(
					Control::Type::Option, 
					z2s_channels_table[channels_counter].Supla_channel_name, 
					working_str, 
					Control::Color::None, 
					channel_selector);
			
			if (channel_selector_first_option_id == 0xFFFF)
				channel_selector_first_option_id = 
					current_option_id;
			
			log_i("\n\rchannels_counter %u\n\rcurrent_option_id %u"
						"\n\r\channel_selector_first_option_id %u",
						channels_counter,
						current_option_id,
						channel_selector_first_option_id);

			z2s_channels_table[channels_counter].gui_control_id = 
				current_option_id; //for channel name update
		}
	}
	
	channel_selector_last_option_id = current_option_id;
	
	log_i("\n\r\channel_selector_first_option_id %u"
				"\n\rchannel_selector_last_option_id %u",
						channel_selector_first_option_id,
						channel_selector_last_option_id);

	if (rebuild_channels_list)
		channelSelectorCallback(ESPUI.getControl(channel_selector), SL_VALUE);
}

/*****************************************************************************/

void buildChannelsTabGUI() {

	working_str = PSTR("Zigbee channels");
	auto channelstab = ESPUI.addControl(
		Control::Type::Tab, PSTR(empty_str), working_str,Control::Color::Emerald, 
		Control::noParent, onChannelsTabCallback);
  
	rebuildChannelsSelector(false, channelstab);

	zb_channel_info_label = ESPUI.addControl(
		Control::Type::Label, PSTR("Channel info"), three_dots_str, 
		Control::Color::Emerald, channelstab);
	
	ESPUI.setElementStyle(
		zb_channel_info_label, "color:black;text-align: justify;"
		" font-family:tahoma; font-size: "
		"4 px; font-style: normal; font-weight: normal;");
	ESPUI.setPanelWide(zb_channel_info_label, true);

	working_str = PSTR(empty_str);
	channel_name_text = ESPUI.addControl(Control::Type::Text, 
																			 PSTR("Channel description panel"), 
																			 working_str, 
																			 Control::Color::Emerald, 
																			 channelstab, 
																			 generalCallback); 
	working_str = PSTR("&#10023; channel name (local) &#10023;");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				working_str, 
																				Control::Color::None, 
																				channel_name_text), 
												PSTR(clearLabelStyle));
	working_str = PSTR("Save");
	channel_name_save_button = ESPUI.addControl(Control::Type::Button, 
																							PSTR(empty_str), working_str, 
																							Control::Color::Emerald, 
																							channel_name_text, 
																				  		editChannelCallback, 
																							(void*)GUI_CB_UPDATE_CHANNEL_NAME_FLAG);

	channel_local_function = 
		ESPUI.addControl(Control::Type::Select, 
										 PSTR(empty_str), working_str, 
										 Control::Color::Emerald, 
										 channel_name_text, 
										 editChannelCallback, 
										 (void*)GUI_CB_UPDATE_CHANNEL_LOCAL_FUNCTION_FLAG);
		
		ESPUI.addControl(Control::Type::Option, 
										 PSTR("Select channel local function"), 
										 minus_one_str, 
										 Control::Color::None, 
										 channel_local_function);

		for (uint8_t ctf = 1; ctf < 4; ctf++ ) {

			working_str = ctf;
			ESPUI.addControl(Control::Type::Option, 
										 CONNECTED_THERMOMETERS_FUNCTION_NAMES[ctf - 1], 
										 working_str, 
										 Control::Color::None, 
										 channel_local_function);
		}

	
	zb_channel_flags_label = ESPUI.addControl(Control::Type::Label, 
																						PSTR("CHANNEL FLAGS"), 
																					 	PSTR("here you can set channel flags "
																								 "(changes are saved immediately)"), 
																					 	Control::Color::Emerald, 
																					 	channelstab);

	ESPUI.setElementStyle(zb_channel_flags_label, PSTR(clearLabelStyle));
	working_str = PSTR(empty_str);
	disable_channel_notifications_switcher = ESPUI.addControl(Control::Type::Switcher, 
																														PSTR(empty_str), 
																														working_str, 
																														Control::Color::Emerald, 
																													  zb_channel_flags_label, 
																														editChannelFlagsCallback, 
																													  (void*)GUI_CB_DISABLE_CHANNEL_NOTIFICATIONS_FLAG);
	ESPUI.setElementStyle(disable_channel_notifications_switcher, "margin: 0% 15%;");

	set_sorwns_on_start_switcher = ESPUI.addControl(Control::Type::Switcher, 
																									PSTR(empty_str), 
																									zero_str, 
																									Control::Color::Emerald, 
																									zb_channel_flags_label, 
																									editChannelFlagsCallback, 
																									(void*)GUI_CB_SET_SORWNS_ON_START_FLAG);
	ESPUI.setElementStyle(set_sorwns_on_start_switcher, "margin: 0% 15%;");
	
	working_str = PSTR("");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearLabelStyle));

	working_str = PSTR("&#10023; <i>disable single channel notofications</i> &#10023;");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearFlagsLabelStyle));

	working_str = PSTR("&#10023; <i>sensor starts in offline state  &#10023;<br>"
										 "(NO REMOTE WAKEUP SUPPORTED)</i>");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearFlagsLabelStyle));
	
	working_str = PSTR("");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearLabelStyle));

	trv_auto_to_schedule_switcher = ESPUI.addControl(Control::Type::Switcher, 
																									 PSTR(empty_str), 
																									 zero_str, 
																									 Control::Color::Emerald, 
																									 zb_channel_flags_label, 
																									 editChannelFlagsCallback, 
																									 (void*)GUI_CB_TRV_AUTO_TO_SCHEDULE_FLAG);
	ESPUI.setElementStyle(trv_auto_to_schedule_switcher, "margin: 0% 15%;");
	
	trv_fixed_calibration_switcher = ESPUI.addControl(Control::Type::Switcher, 
																									 	PSTR(empty_str), 
																										 zero_str, 
																										 Control::Color::Emerald, 
																										 zb_channel_flags_label, 
																										 editChannelFlagsCallback, 
																									 	(void*)GUI_CB_TRV_FIXED_CALIBRATION_FLAG);
	ESPUI.setElementStyle(trv_fixed_calibration_switcher, "margin: 0% 15%;");

	working_str = PSTR("");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearLabelStyle));

	working_str = PSTR("&#10023; <i>TRV AUTO mode => Supla schedule</i> &#10023;");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearFlagsLabelStyle));

	working_str = PSTR("&#10023; <i>TRV fixed correction value &#10023;<br>"
										 "(ignores external thermometer settings)</i>");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearFlagsLabelStyle));
	
	working_str = PSTR("");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearLabelStyle));

	trv_auto_to_schedule_manual_switcher = 
		ESPUI.addControl(Control::Type::Switcher, 
										 PSTR(empty_str), 
									   zero_str, 
										 Control::Color::Emerald, 
										 zb_channel_flags_label, 
										 editChannelFlagsCallback, 	
										 (void*)GUI_CB_TRV_AUTO_TO_SCHEDULE_MANUAL_FLAG);
	ESPUI.setElementStyle(trv_auto_to_schedule_manual_switcher, 
												"margin: 0% 15%;");
	
	trv_cooperative_childlock_switcher = 
		ESPUI.addControl(Control::Type::Switcher, 
										 PSTR(empty_str), 
									   zero_str, 
										 Control::Color::Emerald, 
										 zb_channel_flags_label, 
										 editChannelFlagsCallback, 	
										 (void*)GUI_CB_TRV_COOPERATIVE_CHILDLOCK_FLAG);
	ESPUI.setElementStyle(trv_cooperative_childlock_switcher, 
												"margin: 0% 15%;");
	

	working_str = PSTR("");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearLabelStyle));

	working_str = PSTR("&#10023; <i>TRV AUTO mode =><br>"
										 "Supla schedule/manual</i> &#10023;");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearFlagsLabelStyle));

	working_str = PSTR("&#10023; <i>TRV cooperative childlock &#10023;");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearFlagsLabelStyle));

	working_str = PSTR("");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearLabelStyle));

	enable_resend_temperature_switcher = 
		ESPUI.addControl(Control::Type::Switcher, 
										 PSTR(empty_str), 
									   zero_str, 
										 Control::Color::Emerald, 
										 zb_channel_flags_label, 
										 editChannelFlagsCallback, 	
										 (void*)GUI_CB_ENABLE_RESEND_TEMPERATURE_FLAG);
	ESPUI.setElementStyle(enable_resend_temperature_switcher, 
												"margin: 0% 15%;");

	working_str = PSTR("");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearLabelStyle));

	working_str = PSTR("&#10023; <i>Enable temperature forwarding<br>"
										 "to remote or local thermometer</i> &#10023;");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 zb_channel_flags_label), 
												PSTR(clearFlagsLabelStyle));

	zb_channel_params_label = ESPUI.addControl(Control::Type::Label, 
																						 PSTR("CHANNEL CUSTOM PARAMETERS"), 
																					 	 PSTR("here you can change channel custom parameters<br>"
																								  "(restart may be required!)"), 
																					 	 Control::Color::Emerald, 
																					 	 channelstab);
	ESPUI.setElementStyle(zb_channel_params_label, PSTR(clearLabelStyle));

	param_1_number = ESPUI.addControl(Control::Type::Text, //Number, 
																		PSTR(empty_str), 
																		zero_str, 
																		Control::Color::Emerald, 
																		zb_channel_params_label, 
																		generalCallback);
	working_str = PSTR("Save");
	param_1_save_button = ESPUI.addControl(Control::Type::Button, 
																				 PSTR(empty_str),
																				 working_str, 
																				 Control::Color::Emerald, 
																				 zb_channel_params_label, 
																				 editChannelCallback, 
																				 (void*)GUI_CB_UPDATE_PARAM_1_FLAG);
	working_str = PSTR("&#10023; PARAM(1) - currently not used &#10023;");
	param_1_desc_label = 
		ESPUI.addControl(Control::Type::Label, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::None, 
										 zb_channel_params_label);
	ESPUI.setElementStyle(param_1_desc_label, PSTR(clearLabelStyle));

	param_2_number = ESPUI.addControl(Control::Type::Number, 
																		PSTR(empty_str), 
																		zero_str, 
																		Control::Color::Emerald, 
																		zb_channel_params_label, 
																		generalCallback);
	working_str = PSTR("Save");
	param_2_save_button = ESPUI.addControl(Control::Type::Button, 
																				 PSTR(empty_str),
																				 working_str, 
																				 Control::Color::Emerald, 
																				 zb_channel_params_label, 
																				 editChannelCallback, 
																				 (void*)GUI_CB_UPDATE_PARAM_2_FLAG);
	working_str = PSTR("&#10023; PARAM(1) - currently not used &#10023;");
	param_2_desc_label = 
		ESPUI.addControl(Control::Type::Label, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::None, 
										 zb_channel_params_label);
	ESPUI.setElementStyle(param_2_desc_label, PSTR(clearLabelStyle));


	zb_channel_timings_label = ESPUI.addControl(
		Control::Type::Label, PSTR("Channel timings panel"), 
		PSTR("Input new value and press Save"), Control::Color::Emerald, 
		channelstab);

	keepalive_number = ESPUI.addControl(
		Control::Type::Number, PSTR(empty_str), zero_str, Control::Color::Emerald, 
		zb_channel_timings_label, generalCallback);
	
	working_str = PSTR("Save");
	keepalive_save_button = ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, zb_channel_timings_label, editChannelCallback, 
		(void*)GUI_CB_UPDATE_KEEPALIVE_FLAG);
	working_str = PSTR("&#10023; keepalive (s) &#10023; turn on delay (s) "
										 "[logical gates] &#10023;");
	ESPUI.setElementStyle(
		ESPUI.addControl(
			Control::Type::Label, PSTR(empty_str), working_str, Control::Color::None, 
			zb_channel_timings_label), 
		PSTR(clearLabelStyle));
	
	timeout_number = ESPUI.addControl(Control::Type::Number, 
																		PSTR(empty_str), 
																		zero_str, 
																		Control::Color::Emerald, 
																		zb_channel_timings_label, 
																		generalCallback);
	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 timeout_number);
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 max_int_str, 
									 Control::Color::None, 
									 timeout_number);*/
	working_str = PSTR("Save");
	timeout_save_button = ESPUI.addControl(Control::Type::Button, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::Emerald, 
																				 zb_channel_timings_label, 
																				 editChannelCallback, 
																				 (void*)GUI_CB_UPDATE_TIMEOUT_FLAG);
	working_str = PSTR("&#10023; timeout (s) &#10023;");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), working_str, 
																				 Control::Color::None, 
																				 zb_channel_timings_label), 
												PSTR(clearLabelStyle));
	
	refresh_number = ESPUI.addControl(Control::Type::Number, 
																		PSTR(empty_str), 
																		zero_str, 
																		Control::Color::Emerald, 
																		zb_channel_timings_label, 
																		generalCallback);
	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 refresh_number);
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 max_int_str, 
									 Control::Color::None, 
									 refresh_number);*/
	working_str = PSTR("Save");
	refresh_save_button = ESPUI.addControl(Control::Type::Button, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::Emerald, 
																				 zb_channel_timings_label, 
																				 editChannelCallback, 
																				 (void*)GUI_CB_UPDATE_REFRESH_FLAG); 
	working_str = PSTR("&#10023; refresh(s) [ElectricityMeter] &#10023;<br>"
										 "&#10023; autoset(s) [VirtualBinary] &#10023<br>;"
										 "&#10023; debounce(ms) [VirtualSceneSwitch] &#10023; <br>"
										 "&#10023; thermometers timeout(s) [Remote Thermometer] &#10023;");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 zb_channel_timings_label), 
												PSTR(clearLabelStyle));

	ESPUI.setPanelWide(zb_channel_timings_label, false);
	
	working_str = PSTR(empty_str);
	ESPUI.addControl(Control::Type::Separator, 
									 PSTR(empty_str), 
									 working_str, 
									 Control::Color::None, 
									 channelstab);

	working_str = PSTR("Remove channel");
	remove_channel_button = ESPUI.addControl(Control::Type::Button, 
																					 PSTR("Remove Supla channel(s)"), 
																					 working_str, 
																					 Control::Color::Alizarin, 
																					 channelstab, 
																					removeChannelCallback);

	working_str = PSTR("Remove all channels!");
	remove_all_channels_button = ESPUI.addControl(Control::Type::Button, 
																								PSTR(empty_str), 
																								working_str, 
																								Control::Color::Alizarin, 
																								remove_channel_button, 
																								removeAllChannelsCallback);

	channel_status_label = ESPUI.addControl(
		Control::Type::Label, PSTR("Status"), three_dots_str, 
		Control::Color::Alizarin, remove_channel_button);

	working_str = PSTR("Add AND gate");
	auto lah_panel = ESPUI.addControl(
		Control::Type::Button, PSTR("Local logic components (experimental!)"), 
		working_str, Control::Color::Emerald, channelstab, 
		addLocalActionHandlerCallback, (void*)GUI_CB_ADD_AND_HANDLER_FLAG);

	working_str = PSTR("Add OR gate");
	ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, lah_panel, addLocalActionHandlerCallback,
		(void*)GUI_CB_ADD_OR_HANDLER_FLAG);

	working_str = PSTR("Add XOR gate");
	ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, lah_panel, addLocalActionHandlerCallback,
		(void*)GUI_CB_ADD_XOR_HANDLER_FLAG);

	working_str = PSTR("Add NOT gate");
	ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, lah_panel, addLocalActionHandlerCallback,
		(void*)GUI_CB_ADD_NOT_HANDLER_FLAG);

	working_str = PSTR("Add NAND gate");
	ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, lah_panel, addLocalActionHandlerCallback,
		(void*)GUI_CB_ADD_NAND_HANDLER_FLAG);

	working_str = PSTR("Add NOR gate");
	ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, lah_panel, addLocalActionHandlerCallback,
		(void*)GUI_CB_ADD_NOR_HANDLER_FLAG);

	working_str = PSTR("Add AND gate (3 inputs)");
	ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, lah_panel, addLocalActionHandlerCallback,
		(void*)GUI_CB_ADD_AND3_HANDLER_FLAG);

	working_str = PSTR("Add OR gate (3 inputs)");
	ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, lah_panel, addLocalActionHandlerCallback,
		(void*)GUI_CB_ADD_OR3_HANDLER_FLAG);

	working_str = PSTR("Add virtual relay");
	ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, lah_panel, addLocalVirtualRelayCallback);

	working_str = PSTR("Add virtual binary");
	ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, lah_panel, addLocalVirtualBinaryCallback);

	working_str = PSTR("Add virtual hvac");
	ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, lah_panel, addLocalVirtualHvacCallback);

	working_str = PSTR("Add remote relay");
	ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, lah_panel, addLocalRemoteRelayCallback);																					

	working_str = PSTR("Add remote thermometer");
	ESPUI.addControl(
		Control::Type::Button, PSTR(empty_str), working_str, 
		Control::Color::Emerald, lah_panel, addLocalRemoteThermometerCallback);																					

	enableChannelControls(false);
}

/*****************************************************************************/

void buildClustersAttributesTab() {

	working_str = PSTR("Clusters&Attributes");
	clusters_attributes_table[clusters_attributes_tab] =
		ESPUI.addControl(Control::Type::Tab, 
 										 PSTR(empty_str), 
										 working_str);
	
	working_str = -1;
	clusters_attributes_table[clusters_attributes_device_selector] = 
		ESPUI.addControl(Control::Type::Select, 
										 PSTR("Devices"), 
										 working_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[clusters_attributes_tab], 
										 clustersattributesdeviceSelectorCallback);

	clusters_attributes_table[clusters_attributes_device_info_label] =
		ESPUI.addControl(Control::Type::Label, 
										 PSTR("Device info"), 
										 three_dots_str,	
										 Control::Color::Emerald, 
										 clusters_attributes_table[clusters_attributes_tab]);

	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Select Zigbee device..."), 
									 working_str, 
									 Control::Color::None, 
									 clusters_attributes_table[clusters_attributes_device_selector]);

	for (uint8_t devices_counter = 0; devices_counter < Z2S_ZB_DEVICES_MAX_NUMBER; devices_counter++) 
    if (z2s_zb_devices_table[devices_counter].record_id > 0) {

			working_str = devices_counter;
			ESPUI.addControl(Control::Type::Option, 
											 z2s_zb_devices_table[devices_counter].device_local_name, 
											 working_str, 
											 Control::Color::None, 
											 clusters_attributes_table[clusters_attributes_device_selector]);
		}

	ESPUI.setPanelWide(clusters_attributes_table[clusters_attributes_device_selector], false);

	
	working_str = 1;
	clusters_attributes_table[device_endpoint_number] = 
		ESPUI.addControl(Control::Type::Number, 
										 PSTR("Clusters&Attributes"), 
										 working_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[clusters_attributes_tab], 
										 endpointCallback);

	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 working_str, 
									 Control::Color::None, 
									 clusters_attributes_table[device_endpoint_number]);

	working_str = 255;
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 working_str, 
									 Control::Color::None, 
									 clusters_attributes_table[device_endpoint_number]);*/

	working_str = PSTR("Endpoint id");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), working_str, 
																				 Control::Color::None, 
																				 clusters_attributes_table[device_endpoint_number]), 
												PSTR(clearLabelStyle));

	clusters_attributes_table[device_cluster_selector] = 
		ESPUI.addControl(Control::Type::Select, 
										 PSTR(empty_str), 
										 minus_one_str,
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_endpoint_number], 
										 clusterCallback);

	working_str = PSTR("Cluster id");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 clusters_attributes_table[device_endpoint_number]), 
												PSTR(clearLabelStyle));

	clusters_attributes_table[device_attribute_id_selector] = 
		ESPUI.addControl(Control::Type::Select, 
										 PSTR(empty_str), 
										 minus_one_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_endpoint_number], 
										 attributeCallback);
	
	clusters_attributes_table[device_attribute_id_text] = 
		ESPUI.addControl(Control::Type::Text, 
										 PSTR(empty_str), 
										 zero_str,
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_endpoint_number], 
										 generalCallback);

	working_str = PSTR("Select or enter attribute id/custom command id"
										 "<br>use 0x for hexadecimal values)");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 clusters_attributes_table[device_endpoint_number]), 
												PSTR(clearLabelStyle));

	clusters_attributes_table[device_attribute_type_selector] = 
		ESPUI.addControl(Control::Type::Select, 
										 PSTR(empty_str), 
										 minus_one_str,
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_endpoint_number], 
										 datatypeCallback);
	
	clusters_attributes_table[device_attribute_size_number] = 
		ESPUI.addControl(Control::Type::Number, 
										 PSTR(empty_str), 
										 zero_str,
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_endpoint_number], 
										 generalMinMaxCallback, (void*)255);

	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 clusters_attributes_table[device_attribute_size_number]);
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 max_int_str, 
									 Control::Color::None, 
									 clusters_attributes_table[device_attribute_size_number]);*/

	working_str = PSTR("Select or enter attribute/custom command payload size"
										 "<br>(use 0x for hexadecimal values)");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																	  		 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 clusters_attributes_table[device_endpoint_number]), 
												PSTR(clearLabelStyle));
	
	clusters_attributes_table[device_attribute_value_selector] = 
		ESPUI.addControl(Control::Type::Select, 
										 PSTR(empty_str), 
										 minus_one_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_endpoint_number], 
										 valueCallback);
	
	clusters_attributes_table[device_attribute_value_text] = 
		ESPUI.addControl(Control::Type::Text, 
										 PSTR("Value"), 
										 zero_str,
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_endpoint_number], 
										 generalCallback);

	working_str = PSTR("Select or enter attribute value<br>or custom command payload<br>"
										 "use 0x for hexadecimal values or hexstring without 0x");															 
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 clusters_attributes_table[device_endpoint_number]), 
												PSTR(clearLabelStyle));
	
	clusters_attributes_table[device_config_min_number] =	
		ESPUI.addControl(Control::Type::Number, 
										 PSTR(empty_str), 
										 zero_str,
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_endpoint_number], 
										 generalMinMaxCallback, 
										 (void*)65535);

	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 clusters_attributes_table[device_config_min_number]);
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 max_int_str, 
									 Control::Color::None, 
									 clusters_attributes_table[device_config_min_number]);*/

	working_str = PSTR("Min interval");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 clusters_attributes_table[device_endpoint_number]), 
												PSTR(clearLabelStyle));
	
	clusters_attributes_table[device_config_max_number] =	
		ESPUI.addControl(Control::Type::Number, 
										 PSTR(empty_str), 
										 zero_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_endpoint_number], 
										 generalMinMaxCallback, 
										 (void*)65535);

	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 clusters_attributes_table[device_config_max_number]);
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 max_int_str, 
									 Control::Color::None, 
									 clusters_attributes_table[device_config_max_number]);*/
	working_str = PSTR("Max interval");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 clusters_attributes_table[device_endpoint_number]), 
												PSTR(clearLabelStyle));

	clusters_attributes_table[device_config_delta_number] =	
		ESPUI.addControl(Control::Type::Number, 
										 PSTR(empty_str), 
										 zero_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_endpoint_number], 
										 generalMinMaxCallback, 
										 (void*)65535);

	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 clusters_attributes_table[device_config_delta_number]);
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 max_int_str, 
									 Control::Color::None, 
									 clusters_attributes_table[device_config_delta_number]);*/

	working_str = PSTR("Delta");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 clusters_attributes_table[device_endpoint_number]), 
												PSTR(clearLabelStyle));
	

	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Select Zigbee cluster..."), 
									 minus_one_str, 
									 Control::Color::None, 
									 clusters_attributes_table[device_cluster_selector]);

	//clusters_attributes_table[device_attribute_id_selector_main_option] = 
		ESPUI.addControl(Control::Type::Option, 
										PSTR("Select attribute id..."), 
										minus_one_str, 
										Control::Color::None, 
										clusters_attributes_table[device_attribute_id_selector]);

	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Select attribute type..."), 
									 minus_one_str, 
									 Control::Color::None, 
									 clusters_attributes_table[device_attribute_type_selector]);

	//clusters_attributes_table[device_attribute_value_selector_main_option] = 
		ESPUI.addControl(Control::Type::Option, 
										PSTR("Select attribute value..."), 
										minus_one_str, 
										Control::Color::None, 
										clusters_attributes_table[device_attribute_value_selector]);

	uint32_t zigbee_clusters_count = sizeof(zigbee_clusters)/sizeof(zigbee_cluster_t);

	for (uint8_t clusters_counter = 0; clusters_counter < zigbee_clusters_count; clusters_counter++) {
  
		working_str = zigbee_clusters[clusters_counter].zigbee_cluster_id;
		ESPUI.addControl(Control::Type::Option, 
										 zigbee_clusters[clusters_counter].zigbee_cluster_name, 
										 working_str, 
										 Control::Color::None, 
										 clusters_attributes_table[device_cluster_selector]);
	}

	uint32_t zigbee_datatypes_count = sizeof(zigbee_datatypes)/sizeof(zigbee_datatype_t);

	for (uint8_t datatypes_counter = 0; datatypes_counter < zigbee_datatypes_count; datatypes_counter++) {
  
		working_str = datatypes_counter; 
		ESPUI.addControl(Control::Type::Option, 
										 zigbee_datatypes[datatypes_counter].zigbee_datatype_name, 
										 working_str, 
										 Control::Color::None, 
										 clusters_attributes_table[device_attribute_type_selector]);
	}

	working_str = PSTR("Read attribute");
	clusters_attributes_table[device_read_attribute_button] = 
		ESPUI.addControl(Control::Type::Button, 
										 PSTR("Attribute commands"), 
										 working_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[clusters_attributes_tab], 
										 getClustersAttributesQueryCallback, 
										 (void*)GUI_CB_READ_ATTR_FLAG);

	working_str = PSTR("Read reporting settings");
	clusters_attributes_table[device_read_config_button] = 
		ESPUI.addControl(Control::Type::Button, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_read_attribute_button], 
										 getClustersAttributesQueryCallback, 
										 (void*)GUI_CB_READ_CONFIG_FLAG);

	working_str = PSTR("Write attribute");
	clusters_attributes_table[device_write_attribute_button] = 
		ESPUI.addControl(Control::Type::Button, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_read_attribute_button],
										 getClustersAttributesQueryCallback, 
										 (void*)GUI_CB_WRITE_ATTR_FLAG);

	working_str = PSTR("Configure reporting");
	clusters_attributes_table[device_write_config_button] = 
		ESPUI.addControl(Control::Type::Button, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_read_attribute_button], 
										 getClustersAttributesQueryCallback, 
										 (void*)GUI_CB_CONFIG_REPORT_FLAG); 

	working_str = PSTR("Send custom command");
	clusters_attributes_table[device_custom_command_button] = 
		ESPUI.addControl(Control::Type::Button, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_read_attribute_button], 
										 getClustersAttributesQueryCallback, 
										 (void*)GUI_CB_CUSTOM_CMD_FLAG);

	clusters_attributes_table[manufacturer_code_switcher] = 
		ESPUI.addControl(Control::Type::Switcher, 
										 PSTR("Manufacturer code"), 
										 zero_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[clusters_attributes_tab], 
										 generalCallback);

	working_str = PSTR("&#10023; <i>Use manufacturer code</i> &#10023;");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str,
																				 Control::Color::None, 
																				 clusters_attributes_table[manufacturer_code_switcher]), 
												PSTR(clearLabelStyle));

	clusters_attributes_table[manufacturer_code_selector] = 
		ESPUI.addControl(Control::Type::Select, 
										 PSTR(empty_str), 
										 minus_one_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[manufacturer_code_switcher], 
										 generalCallback);

	working_str = PSTR("Manufacturer code");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 clusters_attributes_table[manufacturer_code_switcher]), 
												PSTR(clearLabelStyle));

	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Select manufacturer code..."), 
									 minus_one_str, 
									 Control::Color::None, 
									 clusters_attributes_table[manufacturer_code_selector]);

	uint32_t zigbee_manufacturer_codes_count = sizeof(zigbee_manufacturer_codes)/
																						 sizeof(zigbee_manufacturer_code_t);

	for (uint8_t manufacturer_codes_counter = 0; 
			 manufacturer_codes_counter < zigbee_manufacturer_codes_count; 
			 manufacturer_codes_counter++) {
  
		working_str = zigbee_manufacturer_codes[manufacturer_codes_counter].manufacturer_code;
		ESPUI.addControl(Control::Type::Option, 
										 zigbee_manufacturer_codes[manufacturer_codes_counter].manufacturer_name,
										 working_str, 
										 Control::Color::None, 
										 clusters_attributes_table[manufacturer_code_selector]);
	}

	clusters_attributes_table[device_async_switcher] = 
		ESPUI.addControl(Control::Type::Switcher, 
		PSTR("Asynchronous command"), 
		zero_str, 
		Control::Color::Emerald, 
		clusters_attributes_table[clusters_attributes_tab], 
		generalCallback);

	clusters_attributes_table[device_read_attribute_label] =
		ESPUI.addControl(Control::Type::Label, 
										 PSTR("Command result"), 
										 three_dots_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[clusters_attributes_tab]);

	clusters_attributes_table[device_Tuya_payload_label] = 
		ESPUI.addControl(Control::Type::Label, 
										 PSTR("Tuya payload"), 
										 three_dots_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[clusters_attributes_tab]);

	working_str = PSTR("Bind cluster (experimental!!!)");
	clusters_attributes_table[device_bind_button] = 
		ESPUI.addControl(Control::Type::Button, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[clusters_attributes_tab],
										 getClustersAttributesQueryCallback, 
										 (void*)GUI_CB_BIND_CLUSTER_FLAG);

	working_str = PSTR("Bind cluster 2(experimental!!!)");
	clusters_attributes_table[device_bind_2_button] = 
		ESPUI.addControl(Control::Type::Button, 
										 PSTR(empty_str), 
										 working_str, 
										 Control::Color::Emerald, 
										 clusters_attributes_table[device_bind_button],
										 getClustersAttributesQueryCallback, 
										 (void*)GUI_CB_BIND_CLUSTER_2_FLAG);

	enableClustersAttributesControls(false);
}

void buildSonoffValveGUI(uint16_t advanced_devices_tab) {

	valve_program_selector = ESPUI.addControl(Control::Type::Select, 
																						"SONOFF SWV VALVE", 
																						minus_one_str, 
																						Control::Color::Emerald, 
																						advanced_devices_tab, 
																						generalCallback);

	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Select program..."), 
									 minus_one_str, 
									 Control::Color::None, 
									 valve_program_selector);
	working_str = 1;
	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Time program..."), 
									 working_str, Control::Color::None, 
									 valve_program_selector);
	working_str = 2;
	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Volume program..."), 
									 working_str, 
									 Control::Color::None, 
									 valve_program_selector);

	valve_cycles_number =	ESPUI.addControl(Control::Type::Number, 
																				 PSTR(empty_str), 
																				 zero_str, 
																				 Control::Color::Emerald, 
																				 valve_program_selector, 
																				 generalMinMaxCallback, 
																				 (void*)100);
	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 valve_cycles_number);
	working_str = 100;
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 working_str, 
									 Control::Color::None, 
									 valve_cycles_number);*/
	working_str = PSTR("Valve cycles count (0-100)");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 valve_program_selector), 
												PSTR(clearLabelStyle));

	valve_worktime_number =	ESPUI.addControl(Control::Type::Number, 
																					 PSTR(empty_str), 
																					 zero_str, 
																					 Control::Color::Emerald, 
																					 
																					 valve_program_selector, 
																					 generalMinMaxCallback, 
																					 (void*)86400);
	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 valve_worktime_number);
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 "86400", 
									 Control::Color::None, 
									 valve_worktime_number);*/
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 "Valve cycle worktime (0 s - 86400 s)", 
																				 Control::Color::None, 
																				 valve_program_selector), 
												PSTR(clearLabelStyle));

	valve_volume_number =	ESPUI.addControl(Control::Type::Number, 
																				 PSTR(empty_str), 
																				 zero_str, 
																				 Control::Color::Emerald, 
																				 valve_program_selector, 
																				 generalMinMaxCallback, 
																				 (void*)6500);
	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 valve_volume_number);
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 "6500", 
									 Control::Color::None, 
									 valve_volume_number);*/
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 "Valve cycle volume (0 L - 6500 L)", 
																				 Control::Color::None, 
																				 valve_program_selector), 
												PSTR(clearLabelStyle));

	valve_pause_number =	ESPUI.addControl(Control::Type::Number, 
																				 PSTR(empty_str), 
																				 zero_str, 
																				 Control::Color::Emerald, 
																				 valve_program_selector, 
																				 generalMinMaxCallback, 
																				 (void*)86400);
	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 valve_pause_number);
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 "86400", 
									 Control::Color::None, 
									 valve_pause_number);*/
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 "Valve cycle pause (0 s - 86400 s)", 
																				 Control::Color::None, 
																				 valve_program_selector), 
												PSTR(clearLabelStyle));

	save_program_button = ESPUI.addControl(Control::Type::Button, 
																				 "Valve programs", 
																				 "Save program", 
																			 	 Control::Color::Emerald, 
																				 valve_program_selector, 
																				 valveCallback, 
																				 (void*)GUI_CB_SAVE_PROGRAM_FLAG);

	load_program_button = ESPUI.addControl(Control::Type::Button, 
																				 "Valve programs", 
																				 "Load program", 
																			 	 Control::Color::Emerald, 
																				 valve_program_selector,
																				 valveCallback, 
																				 (void*)GUI_CB_LOAD_PROGRAM_FLAG);

	start_program_button  = ESPUI.addControl(Control::Type::Button, 
																					 PSTR(empty_str), 
																					 "Start program", 
																				 	 Control::Color::Emerald, 
																					 valve_program_selector, 
																					 valveCallback, 
																					 (void*)GUI_CB_START_PROGRAM_FLAG);

	stop_program_button = ESPUI.addControl(Control::Type::Button, 
																				 PSTR(empty_str), 
																				 "Stop program", 
																				 Control::Color::Emerald, 
																				 valve_program_selector, 
																				 valveCallback, 
																				 (void*)GUI_CB_STOP_PROGRAM_FLAG);

	stop_program_button = ESPUI.addControl(Control::Type::Button, 
																				 PSTR(empty_str), 
																				 "Save program in Supla channel", 
																				 Control::Color::Emerald, 
																				 valve_program_selector, 
																				 valveCallback, 
																				 (void*)GUI_CB_SEND_PROGRAM_FLAG);
																			 
	valve_info_label =  ESPUI.addControl(Control::Type::Label, 
																			 PSTR(empty_str), 
																			 three_dots_str,	
																			 Control::Color::Emerald, 
																			 valve_program_selector);
}

void enableSonoffValveGUI(bool enable) {

	ESPUI.updateSelect(valve_program_selector, minus_one_str);
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
	enableControlStyle(valve_info_label, enable);	
}

void enableTuyaGasDetectorGUI(bool enable) {

	ESPUI.updateSelect(gas_alarm_ringtone_selector, minus_one_str);
	ESPUI.updateNumber(gas_alarm_time_number, 0);
	updateLabel_P(gas_alarm_info_label, three_dots_str);
	updateLabel_P(gas_alarm_Tuya_payload_label, three_dots_str);

	enableControlStyle(gas_alarm_ringtone_selector, enable);
	enableControlStyle(gas_alarm_time_number, enable);
	enableControlStyle(gas_alarm_ringtone_button, enable);
	enableControlStyle(gas_alarm_time_button, enable);
	enableControlStyle(gas_alarm_self_test_button, enable);
	enableControlStyle(gas_alarm_silence_button, enable);
}

void buildTuyaGasDetectorGUI(uint16_t advanced_devices_tab) {

	static const char ringtones[][9] PROGMEM = {"MELODY 1", 
																							"MELODY 2", 
																							"MELODY 3", 
																							"MELODY 4", 
																							"MELODY 5"};

	gas_alarm_ringtone_selector = ESPUI.addControl(Control::Type::Select, 
																								 "GAS DETECTOR", 
																								 minus_one_str, 
																								 Control::Color::Emerald, 
																								 advanced_devices_tab, 
																								 generalCallback);

	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Select alarm ringtone..."), 
									 minus_one_str, 
									 Control::Color::None, 
									 gas_alarm_ringtone_selector);

	for (uint8_t r = 0; r < 5; r++) {

		working_str = r;
		ESPUI.addControl(Control::Type::Option, 
										 ringtones[r], 
										 working_str, 
										 Control::Color::None, 
										 gas_alarm_ringtone_selector);
	}
	
	ESPUI.addControl(Control::Type::Separator,
									 PSTR(empty_str),"", 
									 Control::Color::None, 
									 gas_alarm_ringtone_selector);

	gas_alarm_time_number =	ESPUI.addControl(Control::Type::Number, 
																					 PSTR(empty_str), 
																					 zero_str, 
																					 Control::Color::Emerald, 
																					 gas_alarm_ringtone_selector, 
																					 generalMinMaxCallback, 
																					 (void*)180);
	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 gas_alarm_time_number);
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 "180", 
									 Control::Color::None, 
									 gas_alarm_time_number);*/
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 "Alarm duration (0-180 s)", 
																				 Control::Color::None, 
																				 gas_alarm_ringtone_selector), 
												PSTR(clearLabelStyle));

	gas_alarm_ringtone_button  = ESPUI.addControl(Control::Type::Button, 
																								PSTR(empty_str), 
																								"Send alarm ringtone", 
																				 				Control::Color::Emerald, 
																								gas_alarm_ringtone_selector, 
																								valveCallback, 
																								(void*)GUI_CB_SEND_RINGTONE_FLAG);
	
	gas_alarm_time_button  = ESPUI.addControl(Control::Type::Button, 
																						PSTR(empty_str), 
																						"Send alarm duration", 
																				 		Control::Color::Emerald, 
																						gas_alarm_ringtone_selector, 
																						valveCallback, 
																						(void*)GUI_CB_SEND_TIME_FLAG);

	gas_alarm_self_test_button  = ESPUI.addControl(Control::Type::Button, 
																								 PSTR(empty_str), 
																								 "Self test", 
																				 				 Control::Color::Emerald, 
																								 gas_alarm_ringtone_selector, 
																								 valveCallback, 
																								 (void*)GUI_CB_SELF_TEST_FLAG);

	gas_alarm_silence_button = ESPUI.addControl(Control::Type::Button, 
																							PSTR(empty_str), 
																							"Silence alarm", 
																						 	Control::Color::Emerald, 
																							gas_alarm_ringtone_selector, 
																							valveCallback, 
																							(void*)GUI_CB_SILENCE_FLAG); 
																			 
	gas_alarm_info_label =  ESPUI.addControl(Control::Type::Label, 
																					 PSTR(empty_str), 
																					 three_dots_str,
																					 Control::Color::Emerald, 
																					 gas_alarm_ringtone_selector);

	gas_alarm_Tuya_payload_label = ESPUI.addControl(Control::Type::Label, 
																									PSTR(empty_str), 
																									three_dots_str, 
																									Control::Color::Emerald, 
																									gas_alarm_ringtone_selector);
}

void enableMoesAlarmGUI(bool enable) {

	ESPUI.updateSelect(moes_alarm_melody_selector, minus_one_str);
	ESPUI.updateNumber(moes_alarm_duration_number, 0);
	ESPUI.updateNumber(moes_alarm_volume_number, 0);
	updateLabel_P(moes_alarm_info_label, three_dots_str);
	updateLabel_P(moes_alarm_Tuya_payload_label, three_dots_str);

	enableControlStyle(moes_alarm_melody_selector, enable);
	enableControlStyle(moes_alarm_duration_number, enable);
	enableControlStyle(moes_alarm_volume_number, enable);
	enableControlStyle(moes_alarm_melody_button, enable);
	enableControlStyle(moes_alarm_duration_button, enable);
	enableControlStyle(moes_alarm_volume_button, enable);
}

void buildMoesAlarmGUI(uint16_t advanced_devices_tab) {

	static const char melodies[][10] PROGMEM = {"MELODY 1", 
																							"MELODY 2", 
																							"MELODY 3", 
																							"MELODY 4", 
																							"MELODY 5", 
																							"MELODY 6", 
																							"MELODY 7", 
																							"MELODY 8", 
																							"MELODY 9", 
																							"MELODY 10",
																							"MELODY 11", 
																							"MELODY 12", 
																							"MELODY 13", 
																							"MELODY 14", 
																							"MELODY 15", 
																							"MELODY 16", 
																							"MELODY 17", 
																							"MELODY 18"};

	moes_alarm_melody_selector = ESPUI.addControl(Control::Type::Select, 
																								PSTR("SIREN ALARM"), 
																								minus_one_str, 
																								Control::Color::Emerald, 
																								advanced_devices_tab, 
																								generalCallback);

	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Select alarm melody..."), 
									 minus_one_str, 
									 Control::Color::None, 
									 moes_alarm_melody_selector);

	for (uint8_t m = 0; m < 18; m++) {

		working_str = m;
		ESPUI.addControl(Control::Type::Option, 
										 melodies[m], 
										 working_str, 
										Control::Color::None, 
										moes_alarm_melody_selector);
	}
	
	ESPUI.addControl(Control::Type::Separator,
									 PSTR(empty_str),
									 "", 
									 Control::Color::None, 
									 moes_alarm_melody_selector);

	moes_alarm_duration_number =	ESPUI.addControl(Control::Type::Number, 
																								 PSTR(empty_str), 
																								 zero_str, 
																								 Control::Color::Emerald, 
																								 moes_alarm_melody_selector, 
																								 generalMinMaxCallback, 
																								 (void*)1800);
	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 moes_alarm_duration_number);
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 "1800", 
									 Control::Color::None, 
									 moes_alarm_duration_number);*/
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 "Alarm duration (0 - 1800 s)", 
																				 Control::Color::None, 
																				 moes_alarm_melody_selector), 
												PSTR(clearLabelStyle));
	
	moes_alarm_volume_number =	ESPUI.addControl(Control::Type::Number, 
																							 PSTR(empty_str), 
																							 zero_str, 
																							 Control::Color::Emerald, 
																							 moes_alarm_melody_selector, 
																							 generalMinMaxCallback, 
																							 (void*)2);
	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 moes_alarm_volume_number);
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 "2", 
									 Control::Color::None, 
									 moes_alarm_volume_number);*/
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 "Alarm volume (0 - 2)", 
																				 Control::Color::None, 
																				 moes_alarm_melody_selector), 
												PSTR(clearLabelStyle));

	moes_alarm_melody_button  = ESPUI.addControl(Control::Type::Button, 
																							 PSTR(empty_str), 
																							 "Send alarm melody", 
																				 			Control::Color::Emerald, 
																							moes_alarm_melody_selector, 
																							valveCallback, 
																							(void*)GUI_CB_SEND_MELODY_FLAG); 
	
	moes_alarm_duration_button  = ESPUI.addControl(Control::Type::Button, 
																								 PSTR(empty_str), 
																								 "Send alarm duration", 
																				 				Control::Color::Emerald, 
																								moes_alarm_melody_selector, 
																								valveCallback, 
																								(void*)GUI_CB_SEND_DURATION_FLAG);

	moes_alarm_volume_button  = ESPUI.addControl(Control::Type::Button, 
																							 PSTR(empty_str), 
																							 "Send alarm volume", 
																				 			 Control::Color::Emerald, 
																							 moes_alarm_melody_selector, 
																							 valveCallback, 
																							 (void*)GUI_CB_SEND_VOLUME_FLAG); 

																			 
	moes_alarm_info_label =  ESPUI.addControl(Control::Type::Label, 
																						PSTR(empty_str), 
																						three_dots_str,	
																						Control::Color::Emerald, 
																						moes_alarm_melody_selector);

	moes_alarm_Tuya_payload_label = ESPUI.addControl(Control::Type::Label, 
																									 PSTR(empty_str), 
																									 three_dots_str, 
																									 Control::Color::Emerald, 
																									 moes_alarm_melody_selector);
}

void enableTuyaDevicesControls(bool enable) {

	if (enable == (controls_enabled_flags & TUYA_DEVICES_ENABLED_FLAG))
		return;
	
	updateLabel_P(Tuya_devices_tab_controls_table[Tuya_device_info_label], three_dots_str);

	for (uint16_t i = Tuya_datapoint_id_selector; i < Tuya_device_last_enum_position; i++)

		enableControlStyle(Tuya_devices_tab_controls_table[i], enable);

	enable ? controls_enabled_flags |= TUYA_DEVICES_ENABLED_FLAG:
					 controls_enabled_flags &= ~TUYA_DEVICES_ENABLED_FLAG;
}

void rebuildTuyaDevicesDatapointsList(uint8_t Tuya_device_slot) {

	if (Tuya_devices_id_selector_first_option_id < 0xFFFF) {

		for (uint16_t options_id_counter = Tuya_devices_id_selector_first_option_id;
				 options_id_counter <= Tuya_devices_id_selector_last_option_id;
				 options_id_counter++) {

					if (options_id_counter < 0xFFFF)
					ESPUI.removeControl(options_id_counter, false);
		}
	}

	ESPUI.updateControlValue(Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector],
													 minus_one_str);

	if (Tuya_device_slot == 0xFF)
		return;

	uint32_t Tuya_datapoints_desc_number = sizeof(Tuya_datapoints) / 
																		 		 sizeof(Tuya_datapoint_desc_t);
	
	Tuya_devices_id_selector_first_option_id = 0xFFFF;
	Tuya_devices_id_selector_last_option_id		= 0xFFFF;

	uint16_t current_option_id = 0xFFFF;

	uint32_t z2s_device_desc_id = 
		z2s_zb_devices_table[Tuya_device_slot].desc_id;

	for (uint32_t i = 0; i < Tuya_datapoints_desc_number; i++) {
	
		if (Tuya_datapoints[i].z2s_device_desc_id == z2s_device_desc_id) {

			working_str = i;	
			current_option_id =
				ESPUI.addControl(Control::Type::Option, 
													Tuya_datapoints[i].Tuya_datapoint_name, 
													working_str, 
													Control::Color::None, 
													Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector]);

			if (Tuya_devices_id_selector_first_option_id == 0xFFFF)
				Tuya_devices_id_selector_first_option_id = current_option_id;
		}
	}
	
	Tuya_devices_id_selector_last_option_id = current_option_id;
}

void buildTuyaCustomClusterTabGUI() {

	working_str = "Tuya custom cluster (0xEF00) devices";
	auto Tuya_custom_cluster_tab = ESPUI.addControl(Control::Type::Tab, 
																									PSTR(empty_str), 
																									working_str);
	
	Tuya_devices_tab_controls_table[Tuya_device_selector] = 
		ESPUI.addControl(Control::Type::Select, 
										 PSTR("Tuya devices"), 
										 minus_one_str, 
										 Control::Color::Emerald, 
										 Tuya_custom_cluster_tab, 
										 TuyaDeviceSelectorCallback);

	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Select Tuya device..."), 
									 minus_one_str, 
									 Control::Color::None, 
									 Tuya_devices_tab_controls_table[Tuya_device_selector]);

	for (uint8_t devices_counter = 0; devices_counter < Z2S_ZB_DEVICES_MAX_NUMBER; devices_counter++) {

    if ((z2s_zb_devices_table[devices_counter].record_id > 0) && 
				 hasTuyaCustomCluster(z2s_zb_devices_table[devices_counter].desc_id)) {

			working_str = devices_counter;
			ESPUI.addControl(Control::Type::Option, 
											 z2s_zb_devices_table[devices_counter].device_local_name, 
											 working_str, 
											 Control::Color::None, 
											 Tuya_devices_tab_controls_table[Tuya_device_selector]);
		}
	}

	Tuya_devices_tab_controls_table[Tuya_device_info_label]
		 =  ESPUI.addControl(Control::Type::Label, 
												 PSTR("Tuya device"), 
												 three_dots_str,	
												 Control::Color::Emerald, 
											   Tuya_custom_cluster_tab);

	Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector] = 
		ESPUI.addControl(Control::Type::Select, 
										 PSTR("Tuya datapoints"), 
										 minus_one_str, 
										 Control::Color::Emerald, 
										 Tuya_custom_cluster_tab, 
								     TuyaDatapointIdSelectorCallback);

	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Select predefined datapoint..."), 
									 minus_one_str, 
									 Control::Color::None, 
									 Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector]);

	Tuya_devices_tab_controls_table[Tuya_datapoint_id_number] =	
		ESPUI.addControl(Control::Type::Number, 
										 PSTR(empty_str),
										 zero_str, 
										 Control::Color::Emerald, 
										 Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector], 
										 generalMinMaxCallback, (void*)255);

	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 Tuya_devices_tab_controls_table[Tuya_datapoint_id_number]);
	working_str = 255;
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 working_str, 
									 Control::Color::None, 
									 Tuya_devices_tab_controls_table[Tuya_datapoint_id_number]);*/
	working_str = "Datapoint id (1 - 255)";
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector]), 
												PSTR(clearLabelStyle));
	
	Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector] = 
		ESPUI.addControl(Control::Type::Select, 
										 PSTR("Datapoint type"), 
										 minus_one_str, 
										 Control::Color::Emerald, 
										 Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector], 
										 TuyaDatapointTypeSelectorCallback);

	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Select datapoint type..."), 
									 minus_one_str, 
									 Control::Color::None, 
									 Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector]);
	
	for (uint8_t t = 0; t < 6; t++) {

		working_str = Tuya_datapoint_types[t].Tuya_datapoint_type_id;
		ESPUI.addControl(Control::Type::Option, 
										 Tuya_datapoint_types[t].Tuya_datapoint_type_name, 
										 working_str, 
										 Control::Color::None, 
										 Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector]);
	}

	Tuya_devices_tab_controls_table[Tuya_datapoint_length_number] =	
		ESPUI.addControl(Control::Type::Number, 
									   PSTR(empty_str),
										 zero_str, 
										 Control::Color::Emerald, 
										 Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector],
										 generalMinMaxCallback, 
										 (void*)58);

	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 Tuya_devices_tab_controls_table[Tuya_datapoint_length_number]);
	working_str = 58;
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 working_str, 
									 Control::Color::None, 
									 Tuya_devices_tab_controls_table[Tuya_datapoint_length_number]);*/
	working_str = "Datapoint type length (1-58)";
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector]), 
												PSTR(clearLabelStyle));
	
	Tuya_devices_tab_controls_table[Tuya_datapoint_value_number] =	
		ESPUI.addControl(Control::Type::Number, 
									   PSTR(empty_str),
										 zero_str, 
										 Control::Color::Emerald, 
										 Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector],
										 generalCallback);
	working_str = "Datapoint value (numbers only, no range checking)";
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector]), 
												PSTR(clearLabelStyle));
	

	/*Tuya_devices_tab_controls_table[Tuya_datapoint_value_text] = 
		ESPUI.addControl(Control::Type::Text, 
										 PSTR("Datapoint payload"), 
										 zero_str, 
										 Control::Color::Emerald, 
										 Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector], 
										 generalCallback);

	working_str = PSTR("Enter custom command payload - "
										 "use hexstring without 0x <br> ie. "
										 "00120B020004090706FF");															 
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector]), 
												PSTR(clearLabelStyle));
	*/
	Tuya_devices_tab_controls_table[Tuya_datapoint_description_label] =
		ESPUI.addControl(Control::Type::Label, 
										 PSTR("Datapoint description"), 
										 three_dots_str, 
										 Control::Color::Emerald, 
										 Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector]);
	working_str = PSTR("Datapoint acceptable values description.<br>"
										 "Use it to fill datapoint value field.");															 
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector]), 
												PSTR(clearLabelStyle));

	ESPUI.setElementStyle(Tuya_devices_tab_controls_table[Tuya_datapoint_description_label], 
												"text-align: left; font-family:tahoma; font-size: "
												"4 px; font-style: normal; font-weight: normal;");

	Tuya_devices_tab_controls_table[Tuya_device_data_request_button]  = 
		ESPUI.addControl(
			Control::Type::Button, PSTR(empty_str), "Data request", 
			Control::Color::Emerald, Tuya_custom_cluster_tab, 
			TuyaCustomCmdCallback, (void*)GUI_CB_SEND_TUYA_REQUEST_FLAG); 
	
	Tuya_devices_tab_controls_table[Tuya_device_data_query_button]  = 
		ESPUI.addControl(Control::Type::Button, 
										 PSTR(empty_str), 
										 "Data query", 
										 Control::Color::Emerald, 
										 Tuya_devices_tab_controls_table[Tuya_device_data_request_button], 
										 TuyaCustomCmdCallback, 
										 (void*)GUI_CB_SEND_TUYA_QUERY_FLAG);

	Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label] =  
		ESPUI.addControl(Control::Type::Label, 
										 PSTR("Command Result"), 
										 three_dots_str,	
										 Control::Color::Emerald, 
										 Tuya_custom_cluster_tab);

	Tuya_devices_tab_controls_table[Tuya_device_payload_label] =  
		ESPUI.addControl(Control::Type::Label, 
										 PSTR("Command Response payload"), 
										 three_dots_str,	
										 Control::Color::Emerald, 
										 Tuya_custom_cluster_tab);
}

void buildAdvancedDevicesTabGUI() {

	auto advanced_devices_tab = ESPUI.addControl(Control::Type::Tab, 
																							 PSTR(empty_str), 
																							 "Advanced devices");
	
	advanced_device_selector = ESPUI.addControl(Control::Type::Select, 
																							PSTR("Advanced devices"), 
																							minus_one_str, 
																							Control::Color::Emerald, 
																							advanced_devices_tab, 
																							advancedDeviceSelectorCallback);

	ESPUI.addControl(Control::Type::Option, 
									PSTR("Select device..."), 
									minus_one_str, Control::Color::None, 
									advanced_device_selector);

	for (uint8_t devices_counter = 0; devices_counter < Z2S_ZB_DEVICES_MAX_NUMBER; devices_counter++) {

    if ((z2s_zb_devices_table[devices_counter].record_id > 0) && 
		((z2s_zb_devices_table[devices_counter].desc_id == Z2S_DEVICE_DESC_SONOFF_SMART_VALVE) ||
		  (z2s_zb_devices_table[devices_counter].desc_id == Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR) ||
			(z2s_zb_devices_table[devices_counter].desc_id == Z2S_DEVICE_DESC_MOES_ALARM))) {
			
			switch (z2s_zb_devices_table[devices_counter].desc_id) {
				

				case Z2S_DEVICE_DESC_SONOFF_SMART_VALVE:	
					
					isSonoffValvePresent = true; break;


				case Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR:		
					
					isTuyaGasDetectorPresent = true; break;


				case Z2S_DEVICE_DESC_MOES_ALARM:					
				
					isMoesAlarmPresent = true; break;
			}

			working_str = devices_counter;
			ESPUI.addControl(Control::Type::Option, 
											 z2s_zb_devices_table[devices_counter].device_local_name, 
											 working_str, 
											 Control::Color::None, 
											 advanced_device_selector);
		}
	}

	advanced_device_info_label =  ESPUI.addControl(Control::Type::Label, 
																								 PSTR("Advanced Device"), 
																								 three_dots_str,	
																								 Control::Color::Emerald, 
																								 advanced_devices_tab);

//SONOFF VALVE SWV
	if (Z2S_hasZbDevice(Z2S_DEVICE_DESC_SONOFF_SMART_VALVE)) {

		buildSonoffValveGUI(advanced_devices_tab);
		enableSonoffValveGUI(false);
	}

	//GAS DETECTOR
	if (Z2S_hasZbDevice(Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR)) {
		
		buildTuyaGasDetectorGUI(advanced_devices_tab);
		enableTuyaGasDetectorGUI(false);
	}
	//MOES ALARM
	if (Z2S_hasZbDevice(Z2S_DEVICE_DESC_MOES_ALARM)) {

		buildMoesAlarmGUI(advanced_devices_tab);
		enableMoesAlarmGUI(false);
	}
}

const char* getSuplaActionName(Supla::Action action_id) {

	uint16_t actions_number = sizeof(Supla_actions) /
														sizeof(Supla_action_type_t);

	for (uint16_t i = 0; i < actions_number; i++)
		if (Supla_actions[i].Supla_action_id == action_id)
			return Supla_actions[i].Supla_action_name;
	return "Unknown";
}

const char* getSuplaEventName(Supla::Event event_id, bool is_condition) {

	uint16_t events_number = sizeof(Supla_events) / 
													 sizeof(Supla_event_type_t);

	for (uint16_t i = 0; i < events_number; i++)
		
		if ((Supla_events[i].Supla_event_id == event_id) && (Supla_events[i].is_condition == is_condition))	
			return Supla_events[i].Supla_event_name;
	return "Unknown";	
}

const char* getSuplaConditionName(Supla::Conditions condition_id) {

	uint16_t conditions_number = sizeof(Supla_conditions) / 
															 sizeof(Supla_condition_type_t);

	for (uint16_t i = 0; i < conditions_number; i++)

		if (Supla_conditions[i].Supla_condition_id == condition_id)
			return Supla_conditions[i].Supla_condition_name;
	return "Unknown";	
}

void sprintfAction(z2s_channel_action_t &action) {

	if (action.is_condition)
		sprintf(general_purpose_gui_buffer, 
						"<b>Action#:</b> <i>%d</i> <b>of</b> <i>%d</i><br><br>"
						"<b>Action name:</b> <i>%s (%s)</i><br><br>"
						"<b>Condition:</b> <i>{%s}</i><br>"
						"with <b>value(s) = </b> <i>%.2f, %.2f </i><br>"
						"<b>for source channel:</b> <i>[%s]</i><br><br>"
						"<b>Action:</b> <i>{%s}</i> <b><br>"
						"on destination channel:</b> <i>[%s]</i>",
						current_action_counter, Z2S_getActionsNumber(),
						action.action_name, 
						action.is_enabled ? "enabled" : "disabled",
						getSuplaEventName(action.src_Supla_event, action.is_condition),
						action.min_value,
						action.max_value,
						z2s_channels_table[Z2S_findTableSlotByChannelNumber(
								action.src_Supla_channel)].Supla_channel_name, 
						getSuplaActionName(action.dst_Supla_action),
						z2s_channels_table[Z2S_findTableSlotByChannelNumber(
								action.dst_Supla_channel)].Supla_channel_name);
	else
		sprintf(general_purpose_gui_buffer, 
						"<b>Action#:</b> <i>%d</i> <b>of</b> <i>%d</i><br><br>"
						"<b>Action name:</b> <i>%s (%s)</i><br><br>"
						"<b>Event:</b> <i>{%s}</i><br>"
						"<b>from source channel:</b> <i>[%s]</i><br><br>"
						"<b>Action:</b> <i>{%s}</i> <b><br>"
						"on destination channel:</b> <i>[%s]</i>",
						current_action_counter, Z2S_getActionsNumber(),
						action.action_name, 
						action.is_enabled ? "enabled" : "disabled",
						getSuplaEventName(action.src_Supla_event, action.is_condition),
						z2s_channels_table[Z2S_findTableSlotByChannelNumber(
								action.src_Supla_channel)].Supla_channel_name, 
						getSuplaActionName(action.dst_Supla_action),
						z2s_channels_table[Z2S_findTableSlotByChannelNumber(
								action.dst_Supla_channel)].Supla_channel_name);

	working_str = general_purpose_gui_buffer;
}

void enableActionDetails(bool enable) {

	if (!enable) { 

		ESPUI.updateSwitcher(action_enabled_switcher, false);
		working_str = "";
		ESPUI.updateText(action_name_text, working_str);
		ESPUI.updateText(action_description_text, working_str);
		working_str = minus_one_str;
		ESPUI.updateSelect(action_source_channel_selector, working_str);
		ESPUI.updateSelect(action_event_selector, working_str);
		ESPUI.updateSelect(action_destination_channel_selector, working_str);
		ESPUI.updateSelect(action_action_selector, working_str);
		ESPUI.updateNumber(action_condition_threshold_1_number, 0);
		ESPUI.updateNumber(action_condition_threshold_2_number, 0);
		
	}

	enableControlStyle(action_enabled_switcher, enable);
	enableControlStyle(action_name_text, enable);
	enableControlStyle(action_description_text, enable);
	enableControlStyle(action_source_channel_selector, enable);
	enableControlStyle(action_event_selector, enable);
	enableControlStyle(action_destination_channel_selector, enable);
	enableControlStyle(action_action_selector, enable);
	enableControlStyle(action_condition_threshold_1_number, enable);
	enableControlStyle(action_condition_threshold_2_number, enable);
}

void enableActionControls(bool enable) {

	enableControlStyle(table_first_button, enable);
	enableControlStyle(table_next_button, enable);
	enableControlStyle(table_prev_button, enable);
	enableControlStyle(table_last_button, enable);
}

void updateActionButtons() {

	bool action_save_button_enable;
	bool action_new_button_enable;
	bool action_edit_button_enable; 
	bool action_copy_button_enable;
	bool action_cancel_button_enable;
	bool action_remove_button_enable;

	switch (current_action_gui_state) {


		case VIEW_ACTION: {

			action_save_button_enable = false;
			action_cancel_button_enable = false;

			if (current_action_id == -1) {

				action_edit_button_enable  = false;
				action_remove_button_enable = false;
				action_copy_button_enable = false;
			} else {

				action_edit_button_enable  = true;
				action_remove_button_enable = true;
				action_copy_button_enable = true;
			}

			action_new_button_enable  = true;
			
		} break;


		case NEW_ACTION: {

			action_save_button_enable = true;
			action_new_button_enable  = false;
			action_edit_button_enable  = false;
			action_cancel_button_enable = true;
			action_remove_button_enable = false;
		} break;


		case EDIT_ACTION: {

			action_save_button_enable = true;
			action_new_button_enable  = false;
			action_edit_button_enable  = false;
			action_cancel_button_enable = true;
			action_remove_button_enable = false;
		} break;


		case COPY_ACTION: {

			action_save_button_enable = true;
			action_new_button_enable  = false;
			action_edit_button_enable  = false;
			action_cancel_button_enable = true;
			action_remove_button_enable = false;
		} break;
	}

	enableControlStyle(action_save_button, action_save_button_enable);
	enableControlStyle(action_new_button, action_new_button_enable);
	enableControlStyle(action_edit_button, action_edit_button_enable);
	enableControlStyle(action_cancel_button, action_cancel_button_enable);
	enableControlStyle(action_remove_button, action_remove_button_enable);
	enableControlStyle(action_copy_button, action_remove_button_enable);
}

void updateActionDetails(z2s_channel_action_t &action, bool empty_action = false) {

	if (current_action_gui_state == VIEW_ACTION) {

		if (previous_action_gui_state != VIEW_ACTION)
			enableActionDetails(false);
		return;
	}

	if (current_action_gui_state != VIEW_ACTION)
		enableActionControls(false);

	ESPUI.updateSwitcher(action_enabled_switcher, 
		empty_action ? true : action.is_enabled);
	
	working_str = empty_action ? empty_str : action.action_name;
	ESPUI.updateText(action_name_text, working_str);
	
	working_str = empty_action ? empty_str : action.action_description;
	ESPUI.updateText(action_description_text, working_str);
	
	working_str = empty_action ? -1 : action.src_Supla_channel;
	ESPUI.updateSelect(action_source_channel_selector, working_str);

	working_str = empty_action ? 
		-1 : action.is_condition ? 
			(0xFFFF + action.src_Supla_event) : action.src_Supla_event;
	ESPUI.updateSelect(action_event_selector, working_str);

	working_str = empty_action ? -1 : action.dst_Supla_channel;
	ESPUI.updateSelect(action_destination_channel_selector, working_str);

	working_str = empty_action ? -1 : action.dst_Supla_action;
	ESPUI.updateSelect(action_action_selector, working_str);

	working_str = empty_action ? 
		String(0, 2) : String(action.min_value, 2);
	ESPUI.updateText(action_condition_threshold_1_number, working_str);

	working_str = empty_action ? 
		String(0, 2) : String(action.max_value, 2);
	ESPUI.updateText(action_condition_threshold_2_number, working_str);
}

bool fillActionDetails(z2s_channel_action_t &action) {

	action.is_enabled = ESPUI.getControl(action_enabled_switcher)->value.toInt();

	const char *action_name_str = ESPUI.getControl(action_name_text)->value.c_str();
	if (strlen(action_name_str) >= 0)
		strncpy(action.action_name, action_name_str, 32);
	else 
		return false;

	strncpy(action.action_description, ESPUI.getControl(action_description_text)->value.c_str(), 127);

	int32_t selector_value = ESPUI.getControl(action_source_channel_selector)->value.toInt();
	if ( selector_value >= 0)
		action.src_Supla_channel = selector_value;
	else
		return false;
	 
	selector_value = ESPUI.getControl(action_event_selector)->value.toInt();
	if ( selector_value >= 0) {

		if (selector_value >= 0xFFFF) {

			action.is_condition = true;
			action.src_Supla_event = (Supla::Event)(selector_value - 0xFFFF);	
		} else {
			action.is_condition = false;
			action.src_Supla_event = (Supla::Event)selector_value;
		}
	} else
		return false;
	
	selector_value = ESPUI.getControl(action_destination_channel_selector)->value.toInt();
	if ( selector_value >= 0)
		action.dst_Supla_channel = selector_value;
	else
		return false;

	selector_value = ESPUI.getControl(action_action_selector)->value.toInt();
	if ( selector_value >= 0)
		action.dst_Supla_action = (Supla::Action)selector_value;
	else
		return false;

	double threshold_value = 
		ESPUI.getControl(action_condition_threshold_1_number)->value.toDouble();
	action.min_value = threshold_value;
	//else
	//	return false;

	threshold_value = 
		ESPUI.getControl(action_condition_threshold_2_number)->value.toDouble();
	///if ( selector_value >= 0)
	action.max_value = threshold_value;
	//else
	//	return false;

	//action.is_condition = false;

	return true;
}

void buildActionsChannelSelectors(bool rebuild_options, 
																	uint16_t parent_control_id = 0xFFFF) {

	if (rebuild_options) {

		if (action_source_channel_selector_first_option_id < 0xFFFF) {

		
			for (uint8_t options_id_counter = action_source_channel_selector_first_option_id; 
					options_id_counter <= action_source_channel_selector_last_option_id; 
					options_id_counter++) {

				if (options_id_counter < 0xFFFF) {
					ESPUI.removeControl(options_id_counter, false);
				}
			}
		}

		ESPUI.updateControlValue(action_source_channel_selector, 
														 minus_one_str);

		if (action_destination_channel_selector_first_option_id < 0xFFFF) {
	
			for (uint8_t options_id_counter = action_destination_channel_selector_first_option_id; 
					options_id_counter <= action_destination_channel_selector_last_option_id; 
					options_id_counter++) {

				if (options_id_counter < 0xFFFF) {
					ESPUI.removeControl(options_id_counter, false);
				}
			}
		}
		
		ESPUI.updateControlValue(action_destination_channel_selector, 
														 minus_one_str);

	} else { 
		
		action_source_channel_selector = ESPUI.addControl(Control::Type::Select, 
																											PSTR(empty_str), 
																											minus_one_str, 
																											Control::Color::Emerald, 
																											parent_control_id, 
																											generalCallback);

		action_event_selector = ESPUI.addControl(Control::Type::Select,  //this have to be here to keep user friendly layout
																						 PSTR(empty_str), minus_one_str, 
																						 Control::Color::Emerald, 
																						 parent_control_id, 
																						 generalCallback);

		action_destination_channel_selector = ESPUI.addControl(Control::Type::Select, 
																													 PSTR(empty_str), 
																													 minus_one_str,
																													 Control::Color::Emerald, 
																													 parent_control_id, 
																													 generalCallback);

		action_action_selector = ESPUI.addControl(Control::Type::Select, //this have to be here to keep user friendly layout
																							PSTR(empty_str), 
																							minus_one_str, 
																						 	Control::Color::Emerald, 
																							parent_control_id, 
																							generalCallback);

		action_condition_threshold_1_number = ESPUI.addControl(Control::Type::Number, //this have to be here to keep user friendly layout
																													 PSTR(empty_str), 
																													 minus_one_str, 
																						 							 Control::Color::Emerald, 
																													 parent_control_id, 
																													 generalCallback);
		
		working_str = PSTR("&#10023; condition main threshold value (for =, >, <, ≥, ≤) &#10023;"
											 "<br>enter number ie. 21.5 or -8.0");
		ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																					PSTR(empty_str), 
																					working_str, 
																					Control::Color::None, 
																					parent_control_id), 
													PSTR(clearLabelStyle));
		

		action_condition_threshold_2_number = ESPUI.addControl(Control::Type::Number, //this have to be here to keep user friendly layout
																													 PSTR(empty_str), 
																													 minus_one_str, 
																						 							 Control::Color::Emerald, 
																													 parent_control_id, 
																													 generalCallback);

		working_str = PSTR("&#10023; condition secondary threshold value (only for between) &#10023;"
											 "<br>enter number ie. 21.5 or -8.0");
		ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																					PSTR(empty_str), 
																					working_str, 
																					Control::Color::None, 
																					parent_control_id), 
													PSTR(clearLabelStyle));
		


		ESPUI.addControl(Control::Type::Option, 
											PSTR("Select source channel..."), 
											minus_one_str, 
											Control::Color::None, 
											action_source_channel_selector);

	
		ESPUI.addControl(Control::Type::Option, 
										 PSTR("Select destination channel..."), 
										 minus_one_str, 
										 Control::Color::None, 
										 action_destination_channel_selector);
	}

	uint16_t current_option_id 			= 0xFFFF;

	action_source_channel_selector_first_option_id 		 		 = 0xFFFF;
	action_source_channel_selector_last_option_id 			 	 = 0xFFFF;

	for (uint8_t channels_counter = 0; 
			 channels_counter < Z2S_CHANNELS_MAX_NUMBER; 
			 channels_counter++) {

    if (z2s_channels_table[channels_counter].valid_record) {
      
			working_str = z2s_channels_table[channels_counter].Supla_channel; //channels_counter;
			current_option_id = 
				ESPUI.addControl(Control::Type::Option,  
												z2s_channels_table[channels_counter].Supla_channel_name, 
												working_str, 
												Control::Color::None, 
												action_source_channel_selector);
			
			if (action_source_channel_selector_first_option_id == 0xFFFF) 
				action_source_channel_selector_first_option_id = current_option_id;
		}
	}

	action_source_channel_selector_last_option_id = current_option_id;

	current_option_id = 0xFFFF; 

	action_destination_channel_selector_first_option_id 	 = 0xFFFF;
	action_destination_channel_selector_last_option_id  	 = 0xFFFF;


	for (uint8_t channels_counter = 0; channels_counter < Z2S_CHANNELS_MAX_NUMBER; channels_counter++) {
    if (z2s_channels_table[channels_counter].valid_record) {
	
			working_str = z2s_channels_table[channels_counter].Supla_channel; //channels_counter;
			current_option_id = 
				ESPUI.addControl(Control::Type::Option, 
												z2s_channels_table[channels_counter].Supla_channel_name, 
												working_str, 
												Control::Color::None, 
												action_destination_channel_selector);

			if (action_destination_channel_selector_first_option_id == 0xFFFF) 
				action_destination_channel_selector_first_option_id = current_option_id;
		}
	}
	
	action_destination_channel_selector_last_option_id = current_option_id;
}

void buildActionsTabGUI() {

	auto actions_tab = ESPUI.addControl(Control::Type::Tab, 
																			PSTR(empty_str), 
																			"Local actions");
	
	z2s_channel_action_t new_action = {};

	working_str = "FIRST";							
	table_first_button = ESPUI.addControl(Control::Type::Button, 
																				PSTR(empty_str), 
																				working_str, 
																				Control::Color::Emerald, 
																				actions_tab, 
																				actionsTableCallback, 
																				(void*)GUI_CB_ACTION_FIRST_FLAG);
																				
	working_str = "NEXT";							
	table_next_button = ESPUI.addControl(Control::Type::Button,
																			 PSTR(empty_str), 
																			 working_str, 
																		   Control::Color::Emerald, 
																			 table_first_button, 
																			 actionsTableCallback, 
																			 (void*)GUI_CB_ACTION_NEXT_FLAG);

	working_str = "PREV";							
	table_prev_button = ESPUI.addControl(Control::Type::Button, 
																			 PSTR(empty_str), 
																			 working_str, 
																			 Control::Color::Emerald, 
																			 table_first_button, 
																			 actionsTableCallback, 
																			 (void*)GUI_CB_ACTION_PREV_FLAG);

	working_str = "LAST";							
	table_last_button = ESPUI.addControl(Control::Type::Button, 
																			 PSTR(empty_str), 
																			 working_str, 
																			 Control::Color::Emerald, 
																			 table_first_button, 
																			 actionsTableCallback, 
																			 (void*)GUI_CB_ACTION_LAST_FLAG);

	working_str = "No actions";
	
	int16_t first_action = Z2S_findNextActionPosition(0);
  if (first_action >= 0) {

			current_action_id = first_action;
			current_action_counter = Z2S_getActionCounter(first_action);

      Z2S_loadAction(first_action, new_action);
			sprintfAction(new_action);
		}
	
	log_i("table html source length: %u", strlen(general_purpose_gui_buffer));
								
	actions_table_label = ESPUI.addControl(Control::Type::Label, 
																				 PSTR("Table label"), 
																				 working_str, 
									  		 							   Control::Color::Emerald, 
																				 actions_tab);

	ESPUI.setElementStyle(actions_table_label, 
												"text-align: left; font-family:tahoma; font-size: "
												"4 px; font-style: normal; font-weight: normal;");
	ESPUI.setPanelWide(actions_table_label, false);

	action_enabled_switcher = ESPUI.addControl(Control::Type::Switcher, 
																						 PSTR("Action details"), 
																						 zero_str, 
																						 Control::Color::Emerald, 
																						 actions_tab, 
																						 generalCallback);

	working_str = PSTR("&#10023; action enabled &#10023;");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 action_enabled_switcher), 
												PSTR(clearLabelStyle));				

	working_str = PSTR(empty_str);
	action_name_text = ESPUI.addControl(Control::Type::Text, 
																			PSTR(empty_str), 
																			working_str, 
									  		 							Control::Color::Emerald, 
																			action_enabled_switcher, 
																			generalCallback);

	working_str = PSTR("&#10023; action name (max 32 characters) &#10023;");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 action_enabled_switcher), 
												PSTR(clearLabelStyle));
	
	working_str = PSTR(empty_str);
	action_description_text = ESPUI.addControl(Control::Type::Text, 
																						 PSTR(empty_str), 
																						 working_str, 
									  		 							  		 Control::Color::Emerald, 
																						 action_enabled_switcher, 
																						 generalCallback);

	working_str = PSTR("&#10023; action short description (max 127 characters) &#10023;");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 action_enabled_switcher), 
												PSTR(clearLabelStyle));
	
	buildActionsChannelSelectors(false, action_enabled_switcher);

	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Select source event..."), 
									 minus_one_str, 
									 Control::Color::None, 
									 action_event_selector);	

	ESPUI.addControl(Control::Type::Option, 
									 PSTR("Select destination action..."), 
									 minus_one_str, 
									 Control::Color::None, 
									 action_action_selector);

	uint16_t actions_number = sizeof(Supla_actions) /
														sizeof(Supla_action_type_t);

	for (uint16_t actions_counter = 0; actions_counter < actions_number; actions_counter++) {

		working_str = Supla_actions[actions_counter].Supla_action_id;
		ESPUI.addControl(Control::Type::Option, 
										 Supla_actions[actions_counter].Supla_action_name, 
										 working_str, 
										 Control::Color::None, 
										 action_action_selector);
	}

	uint16_t events_number = sizeof(Supla_events) / 
													 sizeof(Supla_event_type_t);

	for (uint16_t events_counter = 0; events_counter < events_number; events_counter++) {

		working_str = 
			Supla_events[events_counter].is_condition ? 
				Supla_events[events_counter].Supla_event_id + 0xFFFF :
				Supla_events[events_counter].Supla_event_id;

		ESPUI.addControl(Control::Type::Option, 
										 Supla_events[events_counter].Supla_event_name, 
										 working_str, 
										 Control::Color::None, 
										 action_event_selector);
	}

	working_str = "EDIT ACTION";							
	action_edit_button = ESPUI.addControl(Control::Type::Button, 
																			  PSTR(empty_str), 
																				working_str, 
																				Control::Color::Emerald, 
																				actions_tab, 
																				actionsTableCallback, 
																				(void*)GUI_CB_ACTION_EDIT_FLAG);

	working_str = "NEW ACTION";							
	action_new_button = ESPUI.addControl(Control::Type::Button, 
																			PSTR(empty_str), 
																			working_str, 
																			Control::Color::Emerald, 
																			action_edit_button, 
																			actionsTableCallback, 
																			(void*)GUI_CB_ACTION_NEW_FLAG);
	
	working_str = "COPY ACTION";							
	action_copy_button = ESPUI.addControl(Control::Type::Button, 
																				PSTR(empty_str), 
																				working_str, 
																				Control::Color::Emerald, 
																				action_edit_button, 
																				actionsTableCallback, 
																				(void*)GUI_CB_ACTION_COPY_FLAG);
																			
	working_str = "SAVE ACTION";							
	action_save_button = ESPUI.addControl(Control::Type::Button, 
																				PSTR(empty_str), 
																				working_str, 
																				Control::Color::Emerald, 
																				action_edit_button, 
																				actionsTableCallback, 
																				(void*)GUI_CB_ACTION_SAVE_FLAG);

	working_str = "CANCEL CHANGES";							
	action_cancel_button = ESPUI.addControl(Control::Type::Button, 
																					PSTR(empty_str), 
																					working_str, 
																			 		Control::Color::Emerald, 
																					action_edit_button, 
																					actionsTableCallback, 
																					(void*)GUI_CB_ACTION_CANCEL_FLAG);

	working_str = "DELETE ACTION";							
	action_remove_button = ESPUI.addControl(Control::Type::Button, 
																					PSTR(empty_str), 
																					working_str, 
																			 		Control::Color::Emerald, 
																					action_edit_button, 
																					actionsTableCallback, 
																					(void*)GUI_CB_ACTION_REMOVE_FLAG);

	working_str = three_dots_str;
	action_state_label = ESPUI.addControl(Control::Type::Label, 
																				PSTR(empty_str), 
																				working_str, 
																			 	Control::Color::Emerald, 
																				action_edit_button);
	
	enableActionDetails(false);
}

void Z2S_buildWebGUI(gui_modes_t mode, uint32_t gui_custom_flags) {
 
	log_i(" GUI building STARTED...");
	
	ESPUI.sliderContinuous = true;
	//ESPUI.setVerbosity(Verbosity::VerboseJSON);
	ESPUI.captivePortal = false;

	working_str.reserve(1056);

	if (gui_custom_flags)
		gui_build_control_flags = gui_custom_flags;
	else {
		switch (mode) {


			case no_gui_mode:
				
				gui_build_control_flags = 0x0000;
			break;


			case minimal_gui_mode: {

				
				gui_build_control_flags = 
					GUI_BUILD_CONTROL_FLAG_GATEWAY |
					GUI_BUILD_CONTROL_FLAG_CREDENTIALS |
					GUI_BUILD_CONTROL_FLAG_ZIGBEE;
			} break;


			case standard_gui_mode: {

				gui_build_control_flags = 
					GUI_BUILD_CONTROL_FLAG_GATEWAY |
					GUI_BUILD_CONTROL_FLAG_CREDENTIALS |
					GUI_BUILD_CONTROL_FLAG_ZIGBEE |
					GUI_BUILD_CONTROL_FLAG_DEVICES |
					GUI_BUILD_CONTROL_FLAG_CHANNELS |
					GUI_BUILD_CONTROL_FLAG_ACTIONS;
			} break;


			case extended_gui_mode: {

				gui_build_control_flags = 
					GUI_BUILD_CONTROL_FLAG_GATEWAY |
					GUI_BUILD_CONTROL_FLAG_CREDENTIALS |
					GUI_BUILD_CONTROL_FLAG_ZIGBEE |
					GUI_BUILD_CONTROL_FLAG_DEVICES |
					GUI_BUILD_CONTROL_FLAG_CHANNELS |
					GUI_BUILD_CONTROL_FLAG_ACTIONS |
					GUI_BUILD_CONTROL_FLAG_CA;
			} break;


			case full_gui_mode: {

				gui_build_control_flags = 
					GUI_BUILD_CONTROL_FLAG_GATEWAY |
					GUI_BUILD_CONTROL_FLAG_CREDENTIALS |
					GUI_BUILD_CONTROL_FLAG_ZIGBEE |
					GUI_BUILD_CONTROL_FLAG_DEVICES |
					GUI_BUILD_CONTROL_FLAG_CHANNELS |
					GUI_BUILD_CONTROL_FLAG_ACTIONS |
					GUI_BUILD_CONTROL_FLAG_CA |
					GUI_BUILD_CONTROL_FLAG_AD |
					GUI_BUILD_CONTROL_FLAG_TCC;
			} break;


			case developer_gui_mode: {

				gui_build_control_flags = 
					GUI_BUILD_CONTROL_FLAG_GATEWAY |
					GUI_BUILD_CONTROL_FLAG_CA |
					GUI_BUILD_CONTROL_FLAG_AD |
					GUI_BUILD_CONTROL_FLAG_TCC;

			} break;


			case supla_gui_mode: {

				gui_build_control_flags = 
					GUI_BUILD_CONTROL_FLAG_GATEWAY |
					GUI_BUILD_CONTROL_FLAG_CREDENTIALS |
					GUI_BUILD_CONTROL_FLAG_CHANNELS |
					GUI_BUILD_CONTROL_FLAG_ACTIONS;
			} break;


			case gateway_ca_mode: {

				gui_build_control_flags = 
					GUI_BUILD_CONTROL_FLAG_GATEWAY |
					GUI_BUILD_CONTROL_FLAG_CA;
			} break;


			case gateway_tcc_mode: {

				gui_build_control_flags = 
					GUI_BUILD_CONTROL_FLAG_GATEWAY |
					GUI_BUILD_CONTROL_FLAG_TCC;
			} break;


			case gateway_ca_tcc_mode: {

				gui_build_control_flags = 
					GUI_BUILD_CONTROL_FLAG_GATEWAY |
					GUI_BUILD_CONTROL_FLAG_CA |
					GUI_BUILD_CONTROL_FLAG_TCC;
			} break;


			case gateway_devices_channels_mode: {

				gui_build_control_flags = 
					GUI_BUILD_CONTROL_FLAG_GATEWAY |
					GUI_BUILD_CONTROL_FLAG_DEVICES |
					GUI_BUILD_CONTROL_FLAG_CHANNELS;
			} break;

		}
	}

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_GATEWAY)
		buildGatewayTabGUI();

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_CREDENTIALS)
		buildCredentialsGUI();

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_ZIGBEE)
		buildZigbeeTabGUI();

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_DEVICES)
		buildDevicesTabGUI();

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_CHANNELS)
		buildChannelsTabGUI();

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_ACTIONS)
		buildActionsTabGUI();

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_CA)
		buildClustersAttributesTab();

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_AD)
		buildAdvancedDevicesTabGUI();

	if (gui_build_control_flags & GUI_BUILD_CONTROL_FLAG_TCC)
		buildTuyaCustomClusterTabGUI();

	log_i(" ...GUI building FINISHED");
}

void Z2S_reloadWebGUI() {

}

void Z2S_startWebGUIConfig() {

	//char buf[1024] = {};
	
	fillGatewayGeneralnformation(general_purpose_gui_buffer);

	ESPUI.addControl(Control::Type::Separator, 
									 PSTR("General information"), 
									 "", 
									 Control::Color::None);

	gateway_general_info = ESPUI.addControl(Control::Type::Label, 
																					"Device information", 
																					general_purpose_gui_buffer, 
																					Control::Color::Emerald);

	fillMemoryUptimeInformation(general_purpose_gui_buffer);
	
	ESPUI.addControl(Control::Type::Separator, 
									 PSTR("Status"), 
									 "", 
									 Control::Color::None);

	gateway_memory_info = ESPUI.addControl(Control::Type::Label, 
																				 "Memory & Uptime", 
																				 general_purpose_gui_buffer, 
																				 Control::Color::Emerald);
	
	ESPUI.addControl(Control::Type::Separator, 
									 PSTR("Status"), 
									 PSTR(empty_str), 
									 Control::Color::None);

  ESPUI.addControl(Control::Type::Separator, 
									 PSTR("WiFi & Supla credentials"), 
									 "", 
									 Control::Color::None);

	wifi_ssid_text = ESPUI.addControl(Control::Type::Text, 
																		PSTR("SSID"), 
																		"", 
																		Control::Color::Emerald, 
																		Control::noParent, textCallback);

	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 "32", 
									 Control::Color::None,
									  wifi_ssid_text);

	wifi_pass_text = ESPUI.addControl(Control::Type::Text, 
																		"Password", 
																		"", 
																		Control::Color::Emerald, 
																		Control::noParent, textCallback);

	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 "64", 
									 Control::Color::None, 
									 wifi_pass_text);
	ESPUI.setInputType(wifi_pass_text, "password");

	Supla_server = ESPUI.addControl(Control::Type::Text, 
																	PSTR("Supla server"), 
																	"", 
																	Control::Color::Emerald, 
																	Control::noParent, 
																	textCallback);

	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 "64", 
									 Control::Color::None, 
									 Supla_server);

	Supla_email = ESPUI.addControl(Control::Type::Text, 
																 "Supla email",
																 "", 
																 Control::Color::Emerald, 
																 Control::noParent, 
																 textCallback);

	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 "64", 
									 Control::Color::None, 
									 Supla_email);

	Supla_skip_certificate_switcher = ESPUI.addControl(Control::Type::Switcher, 
																										 PSTR("Skip CA certificate check"), 
																										 zero_str,
																										 Control::Color::Emerald, 
																										 Control::noParent, 
																										 generalCallback);

	auto gui_mode_selector = ESPUI.addControl(Control::Type::Select, 
									 												  PSTR("Select GUI mode (requires restart)"), 
																		 				zero_str, 
																		 				Control::Color::Emerald, 
																		 				Control::noParent, 
																		 				selectGuiModeCallback);

	for (uint8_t modes_counter = no_gui_mode; modes_counter < gui_modes_number; modes_counter++) {

  	working_str = modes_counter;
		ESPUI.addControl(Control::Type::Option, 
										 GUI_MODE_OPTIONS[modes_counter], 
										 working_str, 
										 Control::Color::None, 
										 gui_mode_selector);
	}

	working_str = PSTR("When GUI is disabled on start use 5x BOOT to enable it.");
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, 
																				 PSTR(empty_str), 
																				 working_str, 
																				 Control::Color::None, 
																				 gui_mode_selector), 
											  PSTR(clearLabelStyle));


	gui_start_delay_number = ESPUI.addControl(Control::Type::Number, 
																						PSTR("GUI start delay (s)"), 
																						zero_str, 
																						Control::Color::Emerald, 
																						Control::noParent, 
																						generalMinMaxCallback, 
																						(void*)3600);

	/*ESPUI.addControl(Control::Type::Min, 
									 PSTR(empty_str), 
									 zero_str, 
									 Control::Color::None, 
									 gui_start_delay_number);
	
	ESPUI.addControl(Control::Type::Max, 
									 PSTR(empty_str), 
									 max_int_str, 
									 Control::Color::None, 
									 gui_start_delay_number);*/

	working_str = PSTR("Save");
	auto gui_start_delay_save_button = 
		ESPUI.addControl(
			Control::Type::Button, PSTR(empty_str), working_str, 
			Control::Color::Emerald, gui_start_delay_number, 
			gatewayCallback, (void*)GUI_CB_GUI_DELAY_FLAG);

	save_button = ESPUI.addControl(
		Control::Type::Button, PSTR("Save"), "Save", Control::Color::Emerald, 
		Control::noParent, enterWifiDetailsCallback, (void*) GUI_CB_SAVE_FLAG);

	auto save_n_restart_button = ESPUI.addControl(
		Control::Type::Button, PSTR("Save & Restart"), "Save & Restart", 
		Control::Color::Emerald, save_button, enterWifiDetailsCallback, 
		(void*)GUI_CB_RESTART_FLAG);

	save_label = ESPUI.addControl(
		Control::Type::Label, PSTR("Status"), "Missing data...", 
		Control::Color::Emerald, save_button);

	auto cfg = Supla::Storage::ConfigInstance();
  
	if (cfg) {

  	memset(general_purpose_gui_buffer, 0, 
					 sizeof(general_purpose_gui_buffer));
  	if (cfg->getWiFiSSID(general_purpose_gui_buffer) && 
			 strlen(general_purpose_gui_buffer) > 0)
			ESPUI.updateText(
				wifi_ssid_text, general_purpose_gui_buffer);

		memset(
			general_purpose_gui_buffer, 0, sizeof(general_purpose_gui_buffer));
		if (cfg->getSuplaServer(general_purpose_gui_buffer) && 
				strlen(general_purpose_gui_buffer) > 0)
			ESPUI.updateText(
				Supla_server, general_purpose_gui_buffer);

		memset(
			general_purpose_gui_buffer, 0, sizeof(general_purpose_gui_buffer));
		if (cfg->getEmail(general_purpose_gui_buffer) && 
				strlen(general_purpose_gui_buffer) > 0)
			ESPUI.updateText(
				Supla_email, general_purpose_gui_buffer);

		ESPUI.updateNumber(
			Supla_skip_certificate_switcher, _z2s_security_level);
	}

	working_str = _enable_gui_on_start;
	ESPUI.updateSelect(gui_mode_selector, working_str);
	ESPUI.updateNumber(gui_start_delay_number, _gui_start_delay);

	//log_i("%s", myCustomJS);

	ESPUI.begin("ZIGBEE <=> SUPLA CONFIG PAGE");
	GUIstarted = true;
}

void Z2S_startWebGUI() {

  ESPUI.setCustomJS(myCustomJS);
	
	ESPUI.begin("ZIGBEE <=> SUPLA CONTROL PANEL");
	GUIstarted = true;
	handleGatewayEvent(Z2S_SUPLA_EVENT_ON_GUI_STARTED);

}

void Z2S_stopWebGUI() {
	
	if (ESPUI.WebServer())
		ESPUI.WebServer()->end();
	current_Tuya_payload_label = 0;
}

bool Z2S_isGUIStarted() {

	return GUIstarted;
}

void onUpdateBegin(const UpdateType type, int &result) {

	rgbLedWrite(RGB_BUILTIN, 255, 0, 255);  
}

void onUpdateEnd(const UpdateType type, int &result) {

	rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
}

void Z2S_startUpdateServer() {

	if (ESPUI.WebServer()) {
  	updateServer.setup(ESPUI.WebServer());
		updateServer.onUpdateBegin = onUpdateBegin;
		updateServer.onUpdateEnd = onUpdateEnd;
	}
}

void Z2S_updateWebGUI() {

	//char buf[1024] = {};

	fillMemoryUptimeInformation(general_purpose_gui_buffer);
	//working_str = general_purpose_gui_buffer;
	updateLabel_P(gateway_memory_info, 
								general_purpose_gui_buffer);
}

void clusterCallbackCmd() {

	clearAttributeIdSelect();
	clearAttributeValueSelect();

	uint16_t cluster_id = 
		ESPUI.getControl(clusters_attributes_table[device_cluster_selector])->value.toInt();

	ESPUI.updateNumber(clusters_attributes_table[device_attribute_size_number], 0);
	ESPUI.updateNumber(clusters_attributes_table[device_attribute_id_text], 0);
	
	uint32_t zigbee_attributes_count = sizeof(zigbee_attributes) / 
																		 sizeof(zigbee_attribute_t);
	
	device_attribute_id_selector_first_option_id = 0xFFFF;
	device_attribute_id_selector_last_option_id	= 0xFFFF;

	uint16_t current_option_id = 0xFFFF;

	for (uint32_t i = 0; i < zigbee_attributes_count; i++) {
	
		if (zigbee_attributes[i].zigbee_attribute_cluster_id == cluster_id) {

			working_str = i;	
			
			current_option_id =
				ESPUI.addControl(Control::Type::Option, 
													zigbee_attributes[i].zigbee_attribute_name, 
													working_str, 
													Control::Color::None, 
													clusters_attributes_table[device_attribute_id_selector]);

			if (device_attribute_id_selector_first_option_id == 0xFFFF)
				device_attribute_id_selector_first_option_id = current_option_id;
		}
	}
	
	device_attribute_id_selector_last_option_id = current_option_id;
}

void Z2S_loopWebGUI() {


	switch (gui_command) {

		case 34: {

			gui_command = 0;
			clusterCallbackCmd();
		} break;

		case 66: {

			gui_command = 0;
			if (addZ2SDeviceLocalActionHandler(
						LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY, SUPLA_CHANNELFNC_POWERSWITCH)) {

				rebuildChannelsSelector(true);
				buildActionsChannelSelectors(true);
			}
		} break;


		case 67: {

			gui_command = 0;
			if (addZ2SDeviceLocalActionHandler(
						LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY, 
						SUPLA_CHANNELFNC_BINARY_SENSOR)) {

			
				rebuildChannelsSelector(true);
				buildActionsChannelSelectors(true);
			}
		} break;


		case 68: {

			gui_command = 0;
			if (addZ2SDeviceLocalActionHandler(
						LOCAL_CHANNEL_TYPE_REMOTE_RELAY, 
						SUPLA_CHANNELFNC_POWERSWITCH)) {

			
				rebuildChannelsSelector(true);
				buildActionsChannelSelectors(true);
			}
		} break;


		case 69: {

			gui_command = 0;
			if (addZ2SDeviceLocalActionHandler(
						LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER, 
						SUPLA_CHANNELFNC_THERMOMETER)) {

			
				rebuildChannelsSelector(true);
				buildActionsChannelSelectors(true);
			}
		} break;


		case 70: {

			gui_command = 0;
			if (addZ2SDeviceLocalActionHandler(
						LOCAL_CHANNEL_TYPE_VIRTUAL_HVAC, 
						SUPLA_CHANNELFNC_HVAC_THERMOSTAT)) {

				rebuildChannelsSelector(true);
				buildActionsChannelSelectors(true);
			}
		} break;
	}
}

void enterWifiDetailsCallback(Control *sender, int type, void *param) {

	if ((type == B_UP) && data_ready) {

		Serial.println("Saving credentials to Supla Config...");
		Serial.println(ESPUI.getControl(wifi_ssid_text)->value);
		Serial.println(ESPUI.getControl(wifi_pass_text)->value);
		Serial.println(ESPUI.getControl(Supla_server)->value);
		Serial.println(ESPUI.getControl(Supla_email)->value);
		
    auto cfg = Supla::Storage::ConfigInstance();

		if (cfg) {

			cfg->setWiFiSSID(ESPUI.getControl(wifi_ssid_text)->value.c_str());
  		cfg->setWiFiPassword(ESPUI.getControl(wifi_pass_text)->value.c_str());
			cfg->setSuplaServer(ESPUI.getControl(Supla_server)->value.c_str());
		  cfg->setEmail(ESPUI.getControl(Supla_email)->value.c_str());
			cfg->setUInt8(PSTR("security_level"), 
										ESPUI.getControl(Supla_skip_certificate_switcher)->value.toInt() > 0 ? 2 :0);

			cfg->commit();
			if ((uint32_t)param == GUI_CB_RESTART_FLAG) SuplaDevice.softRestart();
		}
	}
}

void textCallback(Control *sender, int type) {
		
		if ((ESPUI.getControl(wifi_ssid_text)->value.length() > 0) &&
				(ESPUI.getControl(wifi_pass_text)->value.length() > 0) &&
				(ESPUI.getControl(Supla_server)->value.length() > 0) &&
				(ESPUI.getControl(Supla_email)->value.length() > 0)) {
			updateLabel_P(save_label, PSTR("Data complete. Press Save"));
			data_ready = true;
		} else {
			updateLabel_P(save_label, PSTR("Data incomplete!"));
			data_ready = false;
		}
}

void generalCallback(Control *sender, int type) {
	
	Serial.print("CB: id(");
	Serial.print(sender->GetId());
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
}

void onZigbeeTabCallback(Control *sender, int type) {
	
	Serial.print("CB: id(");
	Serial.print(sender->GetId());
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);

	ESPUI.updateNumber(pairing_mode_switcher, Zigbee.isNetworkOpen() ? 1 : 0);
	ESPUI.updateNumber(force_leave_switcher, force_leave_global_flag ? 1 : 0);	
}

void onChannelsTabCallback(Control *sender, int type) {
	
	Serial.print("CB: id(");
	Serial.print(sender->GetId());
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
}


void generalMinMaxCallback(Control *sender, int type, void* param) {
	
	Serial.print("CB: id(");
	Serial.print(sender->GetId());
	if (sender->value.toInt() < 0)
		ESPUI.updateNumber(sender->GetId(), 0);
	
	uint32_t max_value = (uint32_t)param;
	
	if (sender->value.toInt() > max_value)
		ESPUI.updateNumber(sender->GetId(), max_value);
}

void endpointCallback(Control *sender, int type) {
	
	Serial.print("CB: id(");
	Serial.print(sender->GetId());
	if (sender->value.toInt() < 1)
		ESPUI.updateNumber(sender->GetId(), 1);
	//uint8_t device_endpoints = z2s_zb_devices_table[ESPUI.getControl(device_selector)->value.toInt()].endpoints_count;
	if (sender->value.toInt() > 255) //device_endpoints)
		ESPUI.updateNumber(sender->GetId(), 255); //device_endpoints);

}

void enableDeviceControls(bool enable) {

	if (enable == (controls_enabled_flags & DEVICES_CONTROLS_ENABLED_FLAG))
		return;

	updateLabel_P(zb_device_info_label, three_dots_str);
	enableControlStyle(zb_device_info_label, enable);

	working_str = PSTR(empty_str);
	ESPUI.updateText(device_name_text, working_str);

	ESPUI.updateNumber(battery_voltage_min_number, 0);
	ESPUI.updateNumber(battery_voltage_max_number, 0);
	
	enableControlStyle(device_name_text, enable);
	enableControlStyle(device_name_save_button, enable);

	enableControlStyle(battery_voltage_min_number, enable);
	enableControlStyle(battery_voltage_min_save_button, enable);
	enableControlStyle(battery_voltage_max_number, enable);
	enableControlStyle(battery_voltage_max_save_button, enable);
		
	updateLabel_P(rssilabel, three_dots_str);
	
	enableControlStyle(getswbuild_button, enable);
	enableControlStyle(getrssi_button, enable);

	//enableControlStyle(remove_device_button, enable);
	enableControlStyle(remove_device_and_channels_button, enable);
	enableControlStyle(remove_all_devices_button, enable);

	enable ? controls_enabled_flags |= DEVICES_CONTROLS_ENABLED_FLAG :
					 controls_enabled_flags &= ~DEVICES_CONTROLS_ENABLED_FLAG;
}

void enableClustersAttributesControls(bool enable) {

	if (enable == (controls_enabled_flags & CLUSTERS_ATTRIBUTES_CONTROLS_ENABLED_FLAG))
		return;

	updateLabel_P(clusters_attributes_table[device_read_attribute_label], three_dots_str);
	updateLabel_P(clusters_attributes_table[device_Tuya_payload_label], three_dots_str);

	ESPUI.updateSelect(clusters_attributes_table[device_cluster_selector], minus_one_str);
	ESPUI.updateSelect(clusters_attributes_table[device_attribute_id_selector], minus_one_str);
	ESPUI.updateSelect(clusters_attributes_table[device_attribute_value_selector], minus_one_str);
	ESPUI.updateSelect(clusters_attributes_table[device_attribute_type_selector], minus_one_str);

	ESPUI.updateNumber(clusters_attributes_table[device_endpoint_number], 1);
	ESPUI.updateNumber(clusters_attributes_table[device_attribute_size_number], 0);
	ESPUI.updateNumber(clusters_attributes_table[device_config_min_number], 0);
	ESPUI.updateNumber(clusters_attributes_table[device_config_max_number], 0);
	ESPUI.updateNumber(clusters_attributes_table[device_config_delta_number], 0);
	
	ESPUI.updateText(clusters_attributes_table[device_attribute_id_text], zero_str);
	
	for (uint8_t i = device_endpoint_number; i < device_last_enum_position; i ++)
		enableControlStyle(clusters_attributes_table[i], enable);
	
	enable ? controls_enabled_flags |= CLUSTERS_ATTRIBUTES_CONTROLS_ENABLED_FLAG :
					 controls_enabled_flags &= ~CLUSTERS_ATTRIBUTES_CONTROLS_ENABLED_FLAG;
}

void updateDeviceInfoLabel() {

	char ieee_addr_str[24] 		= {};

	uint8_t device_slot = ESPUI.getControl(device_selector)->value.toInt();

  sprintf_P(ieee_addr_str, 
						PSTR("%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X"), 
						z2s_zb_devices_table[device_slot].ieee_addr[7],
						z2s_zb_devices_table[device_slot].ieee_addr[6], 
						z2s_zb_devices_table[device_slot].ieee_addr[5], 
						z2s_zb_devices_table[device_slot].ieee_addr[4], 
          	z2s_zb_devices_table[device_slot].ieee_addr[3],
						z2s_zb_devices_table[device_slot].ieee_addr[2],
						z2s_zb_devices_table[device_slot].ieee_addr[1], 
						z2s_zb_devices_table[device_slot].ieee_addr[0]);

	uint8_t battery_percentage = z2s_zb_devices_table[device_slot].battery_percentage >= 0x80 ? 
												  		 z2s_zb_devices_table[device_slot].battery_percentage - 0x80 : 0xFF;

	sprintf_P(general_purpose_gui_buffer,
						PSTR("<b><i><style=color:black>Slot#</b></i> %02u <b>| <i>"
						"Manufacturer name<style=;></i></b> %s "
								"<b>| <i>model ID</b></i> %s <b>| <i>Z2S model</b></i> %s [0x%04X]<br>"
								"<b><i>IEEE address</b></i> %s <b>| <i>Short address</b></i> 0x%04X "
								"<b>| <i>Power source</b></i> 0x%02X<br>"
								"<b><i>Battery percentage</b></i> %u <b>| <i>Last seen (ms)</b></i> %lu "
								"<b>| <i>Gateway unit last seen (ms)</b></i>  %lu <b>| <i>"
								"Gateway unit last RSSI</b></i> %d<br>"
								"<b><i>Device flags</b></i> 0x%08X <b>| <i>ud(1)</b></i> 0x%08X <b>"
								"| <i>ud(2)</b></i> 0x%08X<br>"), 
						device_slot,
						Z2S_getZbDeviceManufacturerName(device_slot),
						Z2S_getZbDeviceModelName(device_slot),
						getZ2SDeviceDescName(z2s_zb_devices_table[device_slot].desc_id), 
						z2s_zb_devices_table[device_slot].desc_id,
						ieee_addr_str,
						z2s_zb_devices_table[device_slot].short_addr,
						z2s_zb_devices_table[device_slot].power_source,
						battery_percentage,
						z2s_zb_devices_table[device_slot].last_seen_ms,
						zbGateway.getZbgDeviceUnitLastSeenMs(z2s_zb_devices_table[device_slot].short_addr),
						zbGateway.getZbgDeviceUnitLastRssi(z2s_zb_devices_table[device_slot].short_addr),
						z2s_zb_devices_table[device_slot].user_data_flags,
						z2s_zb_devices_table[device_slot].user_data_1,
						z2s_zb_devices_table[device_slot].user_data_2);

	log_i("value = %s, length = %u", 
				general_purpose_gui_buffer, 
				strlen(general_purpose_gui_buffer));

	updateLabel_P(zb_device_info_label, general_purpose_gui_buffer);

	working_str = z2s_zb_devices_table[device_slot].device_local_name;
	ESPUI.updateText(device_name_text, working_str);

	ESPUI.updateNumber(battery_voltage_min_number, 
										 z2s_zb_devices_table[device_slot].battery_voltage_min);

	ESPUI.updateNumber(battery_voltage_max_number, 
										 z2s_zb_devices_table[device_slot].battery_voltage_max);
	
	ESPUI.updateNumber(disable_battery_percentage_msg_switcher, 
										 (z2s_zb_devices_table[device_slot].user_data_flags & 
										 	ZBD_USER_DATA_FLAG_DISABLE_BATTERY_PERCENTAGE_MSG) ? 1 : 0);

	ESPUI.updateNumber(disable_battery_voltage_msg_switcher, 
										 (z2s_zb_devices_table[device_slot].user_data_flags & 
										 ZBD_USER_DATA_FLAG_DISABLE_BATTERY_VOLTAGE_MSG) ? 1 : 0);
}

void selectGuiModeCallback(Control *sender, int type) {

	if ((!isNumber(sender->value)) || 
			(sender->value.toInt() < 0) || 
			(sender->value.toInt() >= gui_modes_number)) {

			return;
	} else {

		if (Supla::Storage::ConfigInstance()->setInt8(Z2S_ENABLE_GUI_ON_START_V2, 
					sender->value.toInt()))
			Supla::Storage::ConfigInstance()->commit();
	}
}


void deviceSelectorCallback(Control *sender, int type) {

	if ((!isNumber(sender->value)) || 
			(sender->value.toInt() < 0) || 
			(sender->value.toInt() >= Z2S_ZB_DEVICES_MAX_NUMBER)) {

		enableDeviceControls(false);
		return;
	}
	
	enableDeviceControls(true);

	updateDeviceInfoLabel();
}

void clustersattributesdeviceSelectorCallback(Control *sender, int type) {

	if ((!isNumber(sender->value)) || 
			(sender->value.toInt() < 0) || 
			(sender->value.toInt() >= Z2S_ZB_DEVICES_MAX_NUMBER)) 

		enableClustersAttributesControls(false);
	
	else {
		
		enableClustersAttributesControls(true);

		uint8_t device_slot = sender->value.toInt();

		sprintf_P(general_purpose_gui_buffer,
							PSTR("<b><i>Manufacturer name</i></b> %s "
							"<b>| <i>model ID</b></i> %s"), 
							Z2S_getZbDeviceManufacturerName(device_slot),
							Z2S_getZbDeviceModelName(device_slot));

		updateLabel_P(clusters_attributes_table[clusters_attributes_device_info_label], 
									general_purpose_gui_buffer);
	}

}


void enableChannelControls(bool enable) {

	if (enable == (controls_enabled_flags & CHANNELS_CONTROLS_ENABLED_FLAG))
		return;

	updateLabel_P(zb_channel_info_label, three_dots_str);
	working_str = PSTR(empty_str);
	ESPUI.updateText(channel_name_text, working_str);

	ESPUI.updateNumber(param_1_number, 0);
	ESPUI.updateNumber(param_2_number, 0);
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
	ESPUI.updateSelect(channel_local_function, minus_one_str);
	
	enableControlStyle(channel_name_text, enable);
	enableControlStyle(channel_name_save_button, enable);
	enableControlStyle(disable_channel_notifications_switcher,enable);
	enableControlStyle(trv_auto_to_schedule_switcher, enable);
	enableControlStyle(trv_auto_to_schedule_manual_switcher, enable);
	enableControlStyle(trv_fixed_calibration_switcher, enable);
	enableControlStyle(trv_cooperative_childlock_switcher, enable);
	enableControlStyle(set_sorwns_on_start_switcher, enable);
	enableControlStyle(enable_resend_temperature_switcher, enable);
	enableControlStyle(param_1_number, enable);
	enableControlStyle(param_2_number, enable);
	enableControlStyle(param_1_save_button, enable);
	enableControlStyle(param_2_save_button, enable);
	enableControlStyle(keepalive_number, enable);
	enableControlStyle(keepalive_save_button, enable);
	enableControlStyle(timeout_number, enable);
	enableControlStyle(timeout_save_button, enable);
	enableControlStyle(refresh_number, enable);
	enableControlStyle(refresh_save_button, enable);
	enableControlStyle(remove_channel_button, enable);
	enableControlStyle(remove_all_channels_button, enable);
	enableControlStyle(zb_channel_timings_label, enable);
	enableControlStyle(zb_channel_flags_label, enable);
	enableControlStyle(zb_channel_params_label, enable);
	enableControlStyle(channel_local_function, enable);
	
	enable ? controls_enabled_flags |= CHANNELS_CONTROLS_ENABLED_FLAG:
					 controls_enabled_flags &= ~CHANNELS_CONTROLS_ENABLED_FLAG;
}

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
		
	}
	else {
		
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

	if (flags_mask == 0) 
		enableControlStyle(zb_channel_flags_label, false);
	else
		enableControlStyle(zb_channel_flags_label, true);
}

void enableChannelParams(uint8_t params_mask) {

	
	if (params_mask & 1) {
		
		enableControlStyle(param_1_number, true);
		enableControlStyle(param_1_save_button, true);
		
	}
	else {
		
		ESPUI.updateNumber(param_1_number, 0);
		working_str = PSTR("&#10023; PARAM(1) - currently not used &#10023;");
		ESPUI.updateText(param_1_desc_label, working_str);
		enableControlStyle(param_1_number, false);
		enableControlStyle(param_1_save_button, false);
	}

	if (params_mask & 2) {
		
		enableControlStyle(param_2_number, true);
		enableControlStyle(param_2_save_button, true);
		
	}
	else {
		
		ESPUI.updateNumber(param_2_number, 0);
		working_str = PSTR("&#10023; PARAM(2) - currently not used &#10023;");
		ESPUI.updateText(param_2_desc_label, working_str);
		enableControlStyle(param_2_number, false);
		enableControlStyle(param_2_save_button, false);
	}

	if (params_mask == 0) 
		enableControlStyle(zb_channel_params_label, false);
	else
		enableControlStyle(zb_channel_params_label, true);
}

void fillRemoteAddressData(uint8_t channel_slot) {

	uint8_t remote_address_type = 
      Z2S_checkChannelFlags(channel_slot, 
                            USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS) ?
      REMOTE_ADDRESS_TYPE_MDNS : REMOTE_ADDRESS_TYPE_IP4;

	
	if (remote_address_type == REMOTE_ADDRESS_TYPE_IP4) {

		IPAddress ip(z2s_channels_table[channel_slot].\
								 remote_channel_data.remote_ip_address);

		ESPUI.updateText(param_1_number, ip.toString());

		ESPUI.updateNumber(
			param_2_number, 
			z2s_channels_table[channel_slot].Supla_remote_channel
		);
	} else if (remote_address_type == REMOTE_ADDRESS_TYPE_MDNS) {

		sprintf(general_purpose_gui_buffer, 
						"mdns://%s",
						z2s_channels_table[channel_slot].\
							remote_channel_data.mDNS_name
					 );
				 
		working_str = general_purpose_gui_buffer;
		ESPUI.updateText(param_1_number, working_str);
				
		ESPUI.updateNumber(
			param_2_number, 
			z2s_channels_table[channel_slot].Supla_remote_channel);
	}
}

/*
working_str = PSTR("&#10023; Enter remote relay IP address or mDNS name &#10023;<br>"
										 "for mDNS use <b><i>mdns://</i></b> prefix ie. mdns://my_gateway");
	ESPUI.updateText(param_1_desc_label, working_str);

	working_str = PSTR("&#10023; Enter remote relay channel # &#10023;");
	ESPUI.updateText(param_2_desc_label, working_str);
*/


void updateChannelInfoLabel(uint8_t label_number) {

	char ieee_addr_str[24] 		= {};

	uint8_t channel_slot = ESPUI.getControl(channel_selector)->value.toInt();
	
  sprintf_P(ieee_addr_str, 
						PSTR("%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X"), 
						z2s_channels_table[channel_slot].ieee_addr[7],
						z2s_channels_table[channel_slot].ieee_addr[6], 
						z2s_channels_table[channel_slot].ieee_addr[5], 
						z2s_channels_table[channel_slot].ieee_addr[4], 
	 	        z2s_channels_table[channel_slot].ieee_addr[3],
						z2s_channels_table[channel_slot].ieee_addr[2],
						z2s_channels_table[channel_slot].ieee_addr[1], 
						z2s_channels_table[channel_slot].ieee_addr[0]);
	
	sprintf_P(general_purpose_gui_buffer,
						PSTR("<meta charset=\"UTF-8\">"
						"<b><i>IEEE address</i></b> %s <b>| <i>Short address</i></b> 0x%04X "
						"<b>| <i>endpoint</i></b> 0x%02X <b>| <i>cluster</i></b> 0x%04X<br>"
						"<b><i>Model id</i></b> %s [0x%04X] <b>| <i>channel</i></b> #%u "
						"<b>| <i>secondary channel</i></b> #%u<br><b><i>Type</b></i> %s "
						"<b>| <i>Function</b></i> %s <b>| <i>Sub id</b></i> %d<br>"
						"<b><i>Channel flags</b></i> 0x%08X <b>| <i>ud(1)</b></i> 0x%08X "
						"<b>| <i>ud(2)</b></i> 0x%08X <b>| <i>ud(3)</b></i> 0x%08X "
						"<b>| <i>ud(4)</b></i> 0x%08X <b>| <i>edt</b></i> 0x%02X<br>"
						"<b><i>ZB device</b></i> %s (%s::%s)<br>"
						"<b><i>GUI id</b></i> %u "),
						ieee_addr_str,
						z2s_channels_table[channel_slot].short_addr,
						z2s_channels_table[channel_slot].endpoint,
        		z2s_channels_table[channel_slot].cluster_id,
						getZ2SDeviceDescName(z2s_channels_table[channel_slot].model_id),
        		z2s_channels_table[channel_slot].model_id,
        		z2s_channels_table[channel_slot].Supla_channel,
        		z2s_channels_table[channel_slot].Supla_secondary_channel,
						getSuplaChannelTypeName(z2s_channels_table[channel_slot].Supla_channel_type),
        		z2s_channels_table[channel_slot].Supla_channel_func > 0 ? 
						getSuplaChannelFuncName(z2s_channels_table[channel_slot].Supla_channel_type, 
																		z2s_channels_table[channel_slot].Supla_channel_func) : "none",
        		z2s_channels_table[channel_slot].sub_id,
						z2s_channels_table[channel_slot].user_data_flags,
						z2s_channels_table[channel_slot].user_data_1,
						z2s_channels_table[channel_slot].user_data_2,
						z2s_channels_table[channel_slot].user_data_3,
						z2s_channels_table[channel_slot].user_data_4,
						z2s_channels_table[channel_slot].extended_data_type,
						Z2S_getZbDeviceLocalName(z2s_channels_table[channel_slot].Zb_device_id),
						(z2s_channels_table[channel_slot].local_channel_type == 0) ?
						Z2S_getZbDeviceManufacturerName(z2s_channels_table[channel_slot].Zb_device_id):
						getZ2SDeviceLocalActionHandlerTypeName(channel_slot),
						(z2s_channels_table[channel_slot].local_channel_type == 0) ?
						Z2S_getZbDeviceModelName(z2s_channels_table[channel_slot].Zb_device_id):
						getZ2SDeviceLocalActionHandlerLogicOperatorName(channel_slot),
						z2s_channels_table[channel_slot].gui_control_id);
	
	updateLabel_P(
		zb_channel_info_label, general_purpose_gui_buffer);
	
	working_str = z2s_channels_table[channel_slot].\
		Supla_channel_name;

	ESPUI.updateText(channel_name_text, working_str);
	
	enableChannelParams(0);

	ESPUI.updateSelect(channel_local_function, minus_one_str);
	enableControlStyle(channel_local_function, false);

	switch (z2s_channels_table[channel_slot].\
						Supla_channel_type) {

		case 0x0000: {

			
			enableChannelFlags(0);

			if (z2s_channels_table[channel_slot].local_channel_type == 
					LOCAL_CHANNEL_TYPE_ACTION_HANDLER) {

				enableChannelTimings(1); //keepalive
				ESPUI.updateNumber(keepalive_number, 	z2s_channels_table[channel_slot].keep_alive_secs);
				//ESPUI.updateNumber(refresh_number, z2s_channels_table[channel_slot].refresh_secs);
	
				//enableChannelFlags(0);
				enableChannelTimings(0);
			}

			if (z2s_channels_table[channel_slot].local_channel_type == 
					LOCAL_CHANNEL_TYPE_REMOTE_RELAY) {

				enableChannelTimings(0);
				enableChannelParams(3);
				fillRemoteAddressData(channel_slot);

				working_str = PSTR(
					"&#10023; Enter remote relay IP address or mDNS name &#10023;<br>"
					"for mDNS use <b><i>mdns://</i></b> prefix ie. mdns://my_gateway");

				ESPUI.updateText(param_1_desc_label, working_str);

				working_str = PSTR("&#10023; Enter remote relay channel # &#10023;");
				ESPUI.updateText(param_2_desc_label, working_str);
			}

			if (z2s_channels_table[channel_slot].local_channel_type == 
					LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER) {

				enableChannelTimings(2+4);

				enableControlStyle(channel_local_function, true);
				working_str = z2s_channels_table[channel_slot].local_channel_func;
				ESPUI.updateSelect(channel_local_function, working_str);

				ESPUI.updateNumber(
					timeout_number, 
					z2s_channels_table[channel_slot].timeout_secs);

				ESPUI.updateNumber(
					refresh_number, 
					z2s_channels_table[channel_slot].refresh_secs);

			}
		} break;
		

		case SUPLA_CHANNELTYPE_BINARYSENSOR: {

			enableChannelTimings(6); //timeout+debounce
			ESPUI.updateNumber(
				timeout_number, 
				z2s_channels_table[channel_slot].timeout_secs);

			ESPUI.updateNumber(
				refresh_number, 
				z2s_channels_table[channel_slot].refresh_secs);
	
			enableChannelFlags(5);
			
			ESPUI.updateNumber(
				disable_channel_notifications_switcher, 
				(z2s_channels_table[channel_slot].user_data_flags & 
				USER_DATA_FLAG_DISABLE_NOTIFICATIONS) ? 1 : 0);

			ESPUI.updateNumber(
				set_sorwns_on_start_switcher, 
				(z2s_channels_table[channel_slot].user_data_flags & 
				USER_DATA_FLAG_SET_SORWNS_ON_START) ? 1 : 0);

			enableChannelParams(1);

			ESPUI.updateNumber(
				param_1_number, 
				z2s_channels_table[channel_slot].\
				rain_intensity_treshold);

			working_str = PSTR(
				"&#10023; Virtual Binary custom param<br>"
				"enter rain intensity threshold<br>"
				"currently unused for other sensors &#10023;");

			ESPUI.updateText(param_1_desc_label, working_str);
		} break;


		case SUPLA_CHANNELTYPE_ACTIONTRIGGER: {

			enableChannelTimings(6); //timeout + debounce 
			ESPUI.updateNumber(timeout_number, 
				z2s_channels_table[channel_slot].timeout_secs);

			ESPUI.updateNumber(refresh_number, 
				z2s_channels_table[channel_slot].refresh_secs);
	
			enableChannelFlags(0); 
		} break;


		case SUPLA_CHANNELTYPE_THERMOMETER:
		case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR:
		case SUPLA_CHANNELTYPE_PRESSURESENSOR: {
	
			enableChannelFlags(4+8);
			enableChannelTimings(2); //timeout only

			ESPUI.updateNumber(
				set_sorwns_on_start_switcher, 
				(z2s_channels_table[channel_slot].user_data_flags & 
				USER_DATA_FLAG_SET_SORWNS_ON_START) ? 1 : 0
			);

			uint8_t enable_resend_temperature_flag = 
				(z2s_channels_table[channel_slot].user_data_flags &
				 USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE) ? 1 : 0;

			ESPUI.updateNumber(
				enable_resend_temperature_switcher, 
				enable_resend_temperature_flag
			); 

			ESPUI.updateNumber(
				timeout_number, z2s_channels_table[channel_slot].timeout_secs);
		
			if (enable_resend_temperature_flag) {
				
				enableChannelParams(3);

				log_i("remote address type = %s",Z2S_checkChannelFlags(
					channel_slot, USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS) ?
      		"MDNS" : "IP4");

				fillRemoteAddressData(channel_slot);

				working_str = PSTR(
					"&#10023; Enter remote thermometer IP address or mDNS name &#10023;<br>"
					"for mDNS use <b><i>mdns://</i></b> prefix ie. mdns://my_gateway<br>"
					"or enter 0.0.0.0 for local temperature forwarding");
				
				ESPUI.updateText(param_1_desc_label, working_str);

				working_str = PSTR(
					"&#10023; Enter destination thermometer channel # &#10023;");
				ESPUI.updateText(param_2_desc_label, working_str);
			} else {

				enableChannelParams(0);
			}
		} break;


		case SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT: {

			enableChannelTimings(0);
			enableChannelFlags(0);
			enableChannelParams(3);
		} break;
		
		
		case SUPLA_CHANNELTYPE_HVAC: {
			
			enableChannelTimings(1 + 2); //keepalive + timeout

			ESPUI.updateNumber(
				keepalive_number, z2s_channels_table[channel_slot].keep_alive_secs);

			ESPUI.updateNumber(
				timeout_number, z2s_channels_table[channel_slot].timeout_secs);
	
			enableChannelFlags(2);
			ESPUI.updateNumber(
				trv_auto_to_schedule_switcher, 
				(z2s_channels_table[channel_slot].user_data_flags &
				 USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE) ? 1 : 0);

			uint8_t fixed_correction_flag = 
				(z2s_channels_table[channel_slot].user_data_flags &
				USER_DATA_FLAG_TRV_FIXED_CORRECTION) ? 1 : 0;

			ESPUI.updateNumber(
				trv_fixed_calibration_switcher, fixed_correction_flag);

			ESPUI.updateNumber(
				trv_auto_to_schedule_manual_switcher, 
				(z2s_channels_table[channel_slot].user_data_flags &
				USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE_MANUAL) ? 1 : 0);

			ESPUI.updateNumber(
				trv_auto_to_schedule_manual_switcher, 
				(z2s_channels_table[channel_slot].user_data_flags &
				USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE_MANUAL) ? 1 : 0);

			ESPUI.updateNumber(
				trv_cooperative_childlock_switcher, 
				(z2s_channels_table[channel_slot].user_data_flags &
				USER_DATA_FLAG_TRV_COOPERATIVE_CHILDLOCK) ? 1 : 0);

			if (fixed_correction_flag) {

				enableChannelParams(1);

				ESPUI.updateNumber(
					param_1_number, 
					z2s_channels_table[channel_slot].hvac_fixed_temperature_correction);

				working_str = PSTR(
					"&#10023; Thermostat custom parameter<br>"
					"enter calibration fixed value (temperature x100)<br>"
					"ie. to set correction to -1 enter -100 &#10023;");
				ESPUI.updateText(param_1_desc_label, working_str);
			} else
				enableChannelParams(0);
		} break;


		case SUPLA_CHANNELTYPE_RGBLEDCONTROLLER: {

			enableChannelTimings(3); //timeout + keepalive

			ESPUI.updateNumber(keepalive_number, 
				z2s_channels_table[channel_slot].keep_alive_secs);

			ESPUI.updateNumber(timeout_number, 
				z2s_channels_table[channel_slot].timeout_secs);

			enableChannelParams(1);

			ESPUI.updateNumber(param_1_number, 
										 		 z2s_channels_table[channel_slot].rgb_color_mode);
			working_str = PSTR("&#10023; RGB custom param<br>"
												 "enter numeric value to select RGB MODE<br>"
												 "ie. to set correction to -1 enter -100 &#10023;");
			ESPUI.updateText(param_1_desc_label, working_str);

			enableChannelFlags(0);
		} break;

		
		case SUPLA_CHANNELTYPE_RELAY:
		case SUPLA_CHANNELTYPE_VALVE_OPENCLOSE:
		case SUPLA_CHANNELTYPE_DIMMER: {

			if (z2s_channels_table[channel_slot].local_channel_type == 0) {

				enableChannelTimings(3); //timeout + keepalive

				ESPUI.updateNumber(keepalive_number, 
					z2s_channels_table[channel_slot].keep_alive_secs);

				ESPUI.updateNumber(timeout_number, 
					z2s_channels_table[channel_slot].timeout_secs);

				enableChannelFlags(0);
			}
		} break;


		case SUPLA_CHANNELTYPE_ELECTRICITY_METER: {

			enableChannelTimings(7); //timeout + keepalive + refresh
			ESPUI.updateNumber(keepalive_number, z2s_channels_table[channel_slot].keep_alive_secs);
			ESPUI.updateNumber(timeout_number, z2s_channels_table[channel_slot].timeout_secs);
			ESPUI.updateNumber(refresh_number, z2s_channels_table[channel_slot].refresh_secs);

			enableChannelFlags(0);
		} break;


		default: {

			enableChannelTimings(0);
			enableChannelFlags(0);
		} break;
	}
}

void channelSelectorCallback(Control *sender, int type) {
	
	log_i("selector value = %u", sender->value.toInt());

	if ((!isNumber(sender->value)) || 
			(sender->value.toInt() < 0) || 
			(sender->value.toInt() >= Z2S_CHANNELS_MAX_NUMBER)) {

		enableChannelControls(false);
		return;
	}
	
	enableChannelControls(true);

	updateChannelInfoLabel(1);
}

void getZigbeeDeviceQueryCallback(Control *sender, int type, void *param) {

	if ((type == B_UP) && (ESPUI.getControl(device_selector)->value.toInt() >= 0)) {

		zbg_device_params_t device;

		log_i("device_selector value %u", 
					ESPUI.getControl(device_selector)->value.toInt());

    device.endpoint = 1; 
    device.cluster_id = 0; 
		memcpy(&device.ieee_addr, 
					 z2s_zb_devices_table[ESPUI.getControl(device_selector)->value.toInt()].ieee_addr,8);
		device.short_addr =
			z2s_zb_devices_table[ESPUI.getControl(device_selector)->value.toInt()].short_addr;


		switch ((uint32_t)param) {
			case GUI_CB_SWBUILD_FLAG: { //software build ID

				if (zbGateway.zbQueryDeviceBasicCluster(&device, true, ESP_ZB_ZCL_ATTR_BASIC_SW_BUILD_ID)) {
					if (strlen(zbGateway.getQueryBasicClusterData()->software_build_ID) > 0) 
						updateLabel_P(rssilabel, zbGateway.getQueryBasicClusterData()->software_build_ID);
				} else
						updateLabel_P(rssilabel, device_query_failed_str);
			} break;
			case GUI_CB_RSSI_FLAG: { //RSSI

				if (zbGateway.sendCustomClusterCmd(
					&device, 0x0003, 0x0000, ESP_ZB_ZCL_ATTR_TYPE_NULL, 0, nullptr, 
					true)) {
					
					sprintf_P(general_purpose_gui_buffer, PSTR("%d"), 
										zbGateway.getZbgDeviceUnitLastRssi(device.short_addr));
					updateLabel_P(rssilabel, general_purpose_gui_buffer);
				}
				else
					updateLabel_P(rssilabel, device_query_failed_str);
			} break;
		}
	}
}

void getClustersAttributesQueryCallback(Control *sender, int type, void *param) {

	if ((type == B_UP) && 
			(ESPUI.getControl(
				clusters_attributes_table[clusters_attributes_device_selector])->value.toInt() >= 0)) {

		zbg_device_params_t device;

		uint16_t c_a_device_selector = clusters_attributes_table[clusters_attributes_device_selector];

		log_i("device_selector value %u", 
					ESPUI.getControl(c_a_device_selector)->value.toInt());

    device.endpoint = 
			ESPUI.getControl(clusters_attributes_table[device_endpoint_number])->value.toInt();

    device.cluster_id = 0; 
    
		memcpy(&device.ieee_addr, 
					 z2s_zb_devices_table[ESPUI.getControl(c_a_device_selector)->value.toInt()].ieee_addr,8);

    device.short_addr = 
			z2s_zb_devices_table[ESPUI.getControl(c_a_device_selector)->value.toInt()].short_addr;

		uint16_t cluster_id = 
			ESPUI.getControl(clusters_attributes_table[device_cluster_selector])->value.toInt();

		uint16_t attribute_id = 
			strtoul(ESPUI.getControl(
								clusters_attributes_table[device_attribute_id_text])->value.c_str(), nullptr,0);

		uint8_t attribute_type_idx = 
			ESPUI.getControl(clusters_attributes_table[device_attribute_type_selector])->value.toInt();

		uint8_t attribute_type = 
			attribute_type_idx < 0xFF ? 
				zigbee_datatypes[attribute_type_idx].zigbee_datatype_id : 
				0xFF;

		bool sync_cmd = 
			ESPUI.getControl(
				clusters_attributes_table[device_async_switcher])->value.toInt() == 0;

		uint8_t manuf_specific = (ESPUI.getControl(
			clusters_attributes_table[manufacturer_code_switcher])->value.toInt() == 0) ? 0 : 1;
		
		uint16_t manuf_code = (ESPUI.getControl(
			clusters_attributes_table[manufacturer_code_selector])->value.toInt() > 0) ? 
			ESPUI.getControl(clusters_attributes_table[manufacturer_code_selector])->value.toInt() : 0;

		switch ((uint32_t)param) {
			

			case GUI_CB_READ_ATTR_FLAG : { //read attribute
					
				bool result = zbGateway.sendAttributeRead(&device, 
																									cluster_id, 
																									attribute_id, 
																									sync_cmd,
																									ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 
																									1, 
																									manuf_specific, 
																									manuf_code);
				if (result) {
					if (*zbGateway.getReadAttrStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS) {

						uint64_t readAttrValue;
						esp_zb_uint48_t readAttrValue48;
						esp_zb_uint24_t readAttrValue24;

						switch (zbGateway.getReadAttrLastResult()->data.size) {


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
						}
					
						sprintf_P(general_purpose_gui_buffer, 
											PSTR("Reading attribute successful!<br>Data value is %llu(0x%llX)"
														"<br>Data type is %s(0x%X)<br>Data size is 0x%X"), 
										readAttrValue, 
										readAttrValue, 
										getZigbeeDataTypeName(zbGateway.getReadAttrLastResult()->data.type), 
										zbGateway.getReadAttrLastResult()->data.type,
										zbGateway.getReadAttrLastResult()->data.size);

						updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
													general_purpose_gui_buffer);
					} else {

						sprintf_P(general_purpose_gui_buffer, 
											PSTR("Reading attribute failed!<br>"
											"Status = %s(0x%02X)<br>"
											"Attribute id = 0x%04X"),
											esp_zb_zcl_status_to_name(*zbGateway.getReadAttrStatusLastResult()),
										*zbGateway.getReadAttrStatusLastResult(),
										zbGateway.getReadAttrLastResult()->id);

						updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
													general_purpose_gui_buffer);
					}
				} else {
					if (sync_cmd)
						updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
													device_query_failed_str);
					else
						updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
													device_async_query_str);
				} 
			} break;

			case GUI_CB_READ_CONFIG_FLAG : { //read attribute config report

				bool result = zbGateway.readClusterReportCfgCmd(
					&device, 
					cluster_id, 
					attribute_id, 
					sync_cmd,
					ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 
					1, 
					manuf_specific, 
					manuf_code);

				if (result) {

					if (zbGateway.getReportConfigRespVariableLastResult()->status == 
							ESP_ZB_ZCL_STATUS_SUCCESS) {

						sprintf_P(general_purpose_gui_buffer, 
											PSTR("Reading attribute config report successful! <br>"
											"Attribute id is 0x%X<br>Data type is %s(0x%X)<br>"
											"Min interval is %u(0x%x)<br>Max interval is %u(0x%X)<br>"
											"Delta is %u(0x%X)"), 
											zbGateway.getReportConfigRespVariableLastResult()->attribute_id,
											getZigbeeDataTypeName(zbGateway.getReportConfigRespVariableLastResult()->client.attr_type),
											zbGateway.getReportConfigRespVariableLastResult()->client.attr_type,
											zbGateway.getReportConfigRespVariableLastResult()->client.min_interval,
											zbGateway.getReportConfigRespVariableLastResult()->client.min_interval,
											zbGateway.getReportConfigRespVariableLastResult()->client.max_interval,
											zbGateway.getReportConfigRespVariableLastResult()->client.max_interval,
											zbGateway.getReportConfigRespVariableLastResult()->client.delta[0],
											zbGateway.getReportConfigRespVariableLastResult()->client.delta[0]);

						updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
													general_purpose_gui_buffer);
					} else {

						sprintf_P(general_purpose_gui_buffer, 
										PSTR("Reading attribute config report failed!<br>"
										"Status = %s(0x%02X)<br>"
										"Attribute id = 0x%04X<br>"),
										esp_zb_zcl_status_to_name(zbGateway.getReportConfigRespVariableLastResult()->status),
										zbGateway.getReportConfigRespVariableLastResult()->status,
										zbGateway.getReportConfigRespVariableLastResult()->status,
										zbGateway.getReportConfigRespVariableLastResult()->attribute_id);

						updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
													general_purpose_gui_buffer);
					}
				} else {
					if (sync_cmd)
						updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
													device_query_failed_str);
					else
						updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
													device_async_query_str);
				}
			} break;

			case GUI_CB_CONFIG_REPORT_FLAG : {	//configure reporting
					
				uint16_t min_interval = ESPUI.getControl(
					clusters_attributes_table[device_config_min_number])->value.toInt();

				uint16_t max_interval = ESPUI.getControl(
					clusters_attributes_table[device_config_max_number])->value.toInt();

				uint16_t delta = ESPUI.getControl(
					clusters_attributes_table[device_config_delta_number])->value.toInt();

				bool result = zbGateway.setClusterReporting(
					&device, 
					cluster_id, 
					attribute_id, 
					attribute_type, 
					min_interval, 
					max_interval, 
					delta, 
					sync_cmd,
					ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 
					1, 
					manuf_specific, 
					manuf_code);
					
				if (result) {
					if (*zbGateway.getConfigReportStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS) {

						sprintf_P(general_purpose_gui_buffer, 
											PSTR("Configure reporting successful! <br>"
											"Attribute id is 0x%X, data type is %s(0x%X)<br>"
											"Min interval is %u(0x%x), max interval is %u(0x%X)<br>"
											"Delta is %u(0x%X)"), 
          	        	attribute_id,
											getZigbeeDataTypeName(attribute_type),
											attribute_type,
											min_interval,
											min_interval,
											max_interval,
											max_interval,
											delta,
											delta);

						updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
													general_purpose_gui_buffer);
					} else {
						sprintf_P(general_purpose_gui_buffer, 
											PSTR("Configure reporting failed! <br>"
											"Status = %s(0x%02X)<br>"
											"Attribute id = 0x%04X<br>"),
											esp_zb_zcl_status_to_name(*zbGateway.getConfigReportStatusLastResult()),
											*zbGateway.getConfigReportStatusLastResult(),
											*zbGateway.getConfigReportStatusLastResult(),
      		          	attribute_id);

						updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
													general_purpose_gui_buffer);
					}
				} else {
					if (sync_cmd)
						updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
													device_query_failed_str);
					else
						updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
													device_async_query_str);
				}
			} break;

			case GUI_CB_WRITE_ATTR_FLAG : {	//write attribute
					
				void *value = nullptr;
				
				uint8_t write_mask;
				uint16_t write_mask_16;
				uint32_t write_mask_32;

				uint8_t *write_attribute_payload = nullptr;

				uint16_t attribute_size = 
					ESPUI.getControl(
							clusters_attributes_table[device_attribute_size_number])->value.toInt();
				
				if (attribute_size > 255) {

					updateLabel_P(
							clusters_attributes_table[device_read_attribute_label], 
							device_query_attr_size_error_str);
					return;
				}

				const char* attribute_value = 
					ESPUI.getControl(
						clusters_attributes_table[device_attribute_value_text])->value.c_str();

				log_i("attribute_value = %s, length = %u", 
							attribute_value, 
							strlen(attribute_value));

      	if ((attribute_type >= ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING) && 
						(attribute_type <= ESP_ZB_ZCL_ATTR_TYPE_BAG))  {
        
					if ((strlen(attribute_value) / 2) != attribute_size) {
						
						updateLabel_P(
							clusters_attributes_table[device_read_attribute_label], 
							device_query_attr_size_mismatch_str);
						return;
					}

        	char byte_str[3];
        	byte_str[2] = '\0';
					write_attribute_payload = (uint8_t*)malloc(attribute_size); //2 by

					if (write_attribute_payload == nullptr) {

						updateLabel_P(
							clusters_attributes_table[device_read_attribute_label], 
							PSTR("Error allocating attribute write buffer!"));
						return;
					}

					memset(write_attribute_payload, 0, attribute_size);
					
        	for (int i = 0; i < attribute_size; i++) {

          	memcpy(byte_str, attribute_value + (i * 2), 2);
          	*(write_attribute_payload + i) = strtoul(byte_str, nullptr, 16); //here hex base must be explicit
						log_i("write_attribute_payload[%u] = %u(0x%02X)", i, 
									*(write_attribute_payload + i), 
									*(write_attribute_payload + i));
        	}
        	
					value = write_attribute_payload;
      	} else {
        	
					switch (attribute_size) {
          	
						case 1: {
            	write_mask = strtoul(attribute_value, nullptr, 0);
            	value = &write_mask; 
          	} break;
          	
						case 2: {
            	write_mask_16 = strtoul(attribute_value, nullptr, 0);
            	value = &write_mask_16; 
          	} break;
          	
						case 4: {
            	write_mask_32 = strtoul(attribute_value, nullptr, 0);
            	value = &write_mask_32; 
          	} break;
        	}
      	}
      	
				if (value) {
					
					bool result = zbGateway.sendAttributeWrite(&device, 
																										 cluster_id,
																										 attribute_id, 
																										 (esp_zb_zcl_attr_type_t)attribute_type, 
																										 attribute_size, 
																										 value, 
																										 sync_cmd, 
																										 manuf_specific, 
																										 manuf_code);
					
					if (result) {
						if (*zbGateway.getWriteAttrStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS) {

						
							sprintf_P(general_purpose_gui_buffer, 
												PSTR("Write attribute successful! <br>"
												"Attribute id   = 0x%04X<br>"
												"Attribute type = %s(0x%02X)<br>"
												"Attribute size = %u(0x%04X)"),
      		            	attribute_id,
												getZigbeeDataTypeName(attribute_type),
												attribute_type,
												attribute_size,
												attribute_size);

							updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
														general_purpose_gui_buffer);
						} else {

							sprintf_P(general_purpose_gui_buffer, 
												PSTR("Write attribute failed! <br>"
												"Status = %s(0x%02X)<br>"
												"Attribute id = 0x%04X<br>"
												"Attribute type = %s(0x%04X)<br>"
												"Attribute size = %u(0x%04X)"),
												esp_zb_zcl_status_to_name(*zbGateway.getWriteAttrStatusLastResult()),
												*zbGateway.getWriteAttrStatusLastResult(),
												*zbGateway.getWriteAttrStatusLastResult(),
      		            	*zbGateway.getWriteAttrAttributeIdLastResult(),
												getZigbeeDataTypeName(attribute_type),
												attribute_type,
												attribute_size,
												attribute_size);

							updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
														general_purpose_gui_buffer);
						}
					} else {
						if (sync_cmd)
							updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
														device_query_failed_str);
						else
							updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
														device_async_query_str);
					}
				}
				if (write_attribute_payload) 
					free(write_attribute_payload);
			} break;

			case GUI_CB_CUSTOM_CMD_FLAG : {	//custom command
					
				uint8_t *custom_cmd_payload = nullptr;

				uint16_t payload_size = 
					ESPUI.getControl(clusters_attributes_table[device_attribute_size_number])->value.toInt();
				
				if (payload_size > 255) {
					updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
												device_query_attr_size_error_str);
					return;
				}

				const char* payload_value = 
					ESPUI.getControl(clusters_attributes_table[device_attribute_value_text])->value.c_str();

				log_i("payload_value = %s, length = %u", 
							payload_value, strlen(payload_value));

      	//if ((attribute_type >= ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING) && (attribute_type <= ESP_ZB_ZCL_ATTR_TYPE_BAG))  {
				if (true) {	
					if (payload_size > 0) {
						
						if ((strlen(payload_value) / 2) != payload_size) {
							updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
														device_query_attr_size_mismatch_str);
							return;
						}

        		char byte_str[3];
        		byte_str[2] = '\0';
						custom_cmd_payload = (uint8_t*)malloc(payload_size); //2 by

						if (custom_cmd_payload == nullptr) {
							updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
														PSTR("Error allocating custom command payload buffer!"));
							return;
						}

						memset(custom_cmd_payload, 0, payload_size);
					
        		for (int i = 0; i < payload_size; i++) {
          		
							memcpy(byte_str, payload_value + (i * 2), 2);
          		*(custom_cmd_payload + i) = strtoul(byte_str, nullptr, 16); //here hex base must be explicit
							log_i("custom_cmd_payload[%u] = %u(0x%02X)", 
										i, *(custom_cmd_payload + i), *(custom_cmd_payload + i));
        		}
					} else
						log_i("Payload size equals 0"); 
      	} else {

        	updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
												PSTR("Invalid data type for custom command!"));
					return;
      	}
      	if (true) {
					
					if (cluster_id == 0xEF00) {

						custom_cmd_tsn = (((uint16_t)(*custom_cmd_payload)) << 16) + (*(custom_cmd_payload + 1));
						Tuya_custom_cmd_dp = *(custom_cmd_payload + 2);
						current_Tuya_payload_label = clusters_attributes_table[device_Tuya_payload_label];
					}

					bool result = zbGateway.sendCustomClusterCmd(&device, 
																											 cluster_id, 
																											 attribute_id, 
																											 (esp_zb_zcl_attr_type_t)attribute_type, 
																											 payload_size, 
																											 custom_cmd_payload, 
																											 sync_cmd,
																											 ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 
																											 1, 
																											 manuf_specific, 
																											 manuf_code);
					
					if (result) {

						if (*zbGateway.getCustomCmdStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS) {

						
							sprintf_P(general_purpose_gui_buffer, 
												PSTR("Custom command successful! <br>"
												"Command id   = 0x%04X<br>"
												"Data type = %s(0x%02X)<br>"
												"Payload size = %u(0x%04X)"),
      		            	attribute_id,
												getZigbeeDataTypeName(attribute_type),
												attribute_type,
												payload_size,
												payload_size);

							updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
														general_purpose_gui_buffer);
						} else {

							sprintf_P(general_purpose_gui_buffer, 
												PSTR("Custom command failed! <br>"
												"Status = %s(0x%02X)"
												"Command id = 0x%04X<br>"
												"Data type = %s(0x%04X)<br>"
												"Payload size = %u(0x#04X)"),
												esp_zb_zcl_status_to_name(*zbGateway.getCustomCmdStatusLastResult()),
												*zbGateway.getCustomCmdStatusLastResult(),
												*zbGateway.getCustomCmdStatusLastResult(),
      		       	     *zbGateway.getCustomCmdRespToCmdLastResult(),
												getZigbeeDataTypeName(attribute_type),
												attribute_type,
												payload_size,
												payload_size);

							updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
														general_purpose_gui_buffer);
						}
					} else {
						if (sync_cmd)
							updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
														device_query_failed_str);
						else
							updateLabel_P(clusters_attributes_table[device_read_attribute_label], 
														device_async_query_str);
					}
				}
				if (custom_cmd_payload) 
					free(custom_cmd_payload);
			} break;


			case GUI_CB_BIND_CLUSTER_FLAG:

				zbGateway.bindDeviceCluster(&device, cluster_id);
			break;


			case GUI_CB_BIND_CLUSTER_2_FLAG:
			
				zbGateway.bindDeviceCluster2(&device, cluster_id);
			break;
		}
	}
}

void removeDeviceCallback(Control *sender, int type, void *param) {

	if ((type == B_UP) && (ESPUI.getControl(device_selector)->value.toInt() >= 0)) {

		uint8_t device_slot = ESPUI.getControl(device_selector)->value.toInt();

		//char status_line[128];
		bool restart_required = false;

		switch ((uint32_t)param) {

			case GUI_CB_SINGLE_FLAG : {		
				
				z2s_zb_devices_table[device_slot].record_id = 0;
				if (Z2S_saveZbDevicesTable()) {

					sprintf_P(
						general_purpose_gui_buffer, 
						PSTR("Device #%02u removed! Restarting..."), device_slot);

					restart_required = true;
				} else
					sprintf_P(
						general_purpose_gui_buffer, 
						PSTR("Device #%02u removal failed! Error saving ZB devices table!"), 
						device_slot);
			} break;


			case GUI_CB_WITH_CHANNELS_FLAG : {	

				if (Z2S_removeZbDeviceWithAllChannels(device_slot, true)) {

					sprintf_P(
						general_purpose_gui_buffer, 
						PSTR("Device #%02u and it's all channels removed! Restarting..."), 
						device_slot);
					restart_required = true;
				} else
					sprintf_P(
						general_purpose_gui_buffer, 
						PSTR("Device #%02u or some of it's channels removal failed! "
						"Error saving one of devices table!"), device_slot);
			} break;


			case GUI_CB_ALL_FLAG: {

				//if (Z2S_clearZbDevicesTable()) {
				if (remove_all_devices_counter == 0) {

					if (Z2S_removeAllZbDeviceWithAllChannels()) {
						sprintf_P(
							general_purpose_gui_buffer, 
							PSTR("All devices with channels removed! Restarting..."));
							restart_required = true;
					} else
						sprintf_P(
							general_purpose_gui_buffer, 
							PSTR("Devices removal failed! Error saving ZB devices table!"));
				} else {

						sprintf_P(
							general_purpose_gui_buffer, 
							PSTR("Press this button %s more to remove all devices"),
							(remove_all_devices_counter == 2) ? "twice" : "once");
							remove_all_devices_counter--;
				}
			} break;
		}

    updateLabel_P(device_status_label, general_purpose_gui_buffer);

    if (restart_required) 
			SuplaDevice.scheduleSoftRestart(1000);
	}
}

void removeChannelCallback(Control *sender, int type) {

	if ((type == B_UP) && 
			(ESPUI.getControl(channel_selector)->value.toInt() >= 0)) {

		uint8_t channel_slot = ESPUI.getControl(channel_selector)->value.toInt();

    bool is_zigbee_channel = 
			(z2s_channels_table[channel_slot].local_channel_type == 0);

		if (Z2S_removeChannel(channel_slot, true)) {

			if (is_zigbee_channel) {
				
				sprintf_P(
					general_purpose_gui_buffer, 
					PSTR("Channel # %02u with all actions removed. Restarting..."),
					channel_slot);

      	updateLabel_P(channel_status_label, general_purpose_gui_buffer);
      	SuplaDevice.scheduleSoftRestart(1000);
			} else {
				
				sprintf_P(
					general_purpose_gui_buffer, 
					PSTR("Channel # %02u with all actions removed."), channel_slot);

      	updateLabel_P(channel_status_label, general_purpose_gui_buffer);
				
			}
		}
	}
}

void removeAllChannelsCallback(Control *sender, int type) {

	if (type == B_UP) {
		
		if (Z2S_removeAllChannels()) {

      updateLabel_P(
				channel_status_label, 
				PSTR("All channels and actions removed!. Restarting..."));
      SuplaDevice.scheduleSoftRestart(1000);
		}
	}
}

void pairingSwitcherCallback(Control *sender, int type, void *param){


	switch ((uint32_t)param) {

		case GUI_CB_PAIRING_FLAG : {		//open network

			if (type == S_ACTIVE) 
				Zigbee.openNetwork(180); 
			else {
    if (Zigbee.isNetworkOpen())
			Zigbee.openNetwork(0);//ESPUI.updateNumber(pairing_mode_switcher, 1);
			}
		} break;


		case GUI_CB_FORCE_LEAVE_FLAG : {		

			if (type == S_ACTIVE) 
				force_leave_global_flag = true; 
			else 
				force_leave_global_flag = false;
		} break;
	}
}

void generalZigbeeCallback(Control *sender, int type, void *param){

	if (type == B_UP) {

		switch ((uint32_t)param) {

			case GUI_CB_PAIRING_FLAG : {		//open network

				Zigbee.openNetwork(180); 
			} break;

			case GUI_CB_FACTORY_FLAG : {	//factory reset
			
				if (ESPUI.getControl(factory_reset_switcher)->value.toInt() > 0)
					Zigbee.factoryReset(); 
			} break;

			case GUI_CB_GET_TX_FLAG : { //read TX power

				int8_t zb_tx_power;
    		esp_zb_get_tx_power(&zb_tx_power);
				log_i("get tx power %d", zb_tx_power);
				working_str = zb_tx_power;
				ESPUI.updateText(zigbee_tx_power_text, working_str);
			} break;

			case GUI_CB_SET_TX_FLAG: {//update TX power

				if (isNumber(ESPUI.getControl(zigbee_tx_power_text)->value) &&
						(ESPUI.getControl(zigbee_tx_power_text)->value.toInt() >= -24) &&
						(ESPUI.getControl(zigbee_tx_power_text)->value.toInt() <= 20))
					esp_zb_set_tx_power(
						ESPUI.getControl(zigbee_tx_power_text)->value.toInt());
				else
					ESPUI.updateText(zigbee_tx_power_text, zigbee_tx_power_text_str);
			} break;

			case GUI_CB_GET_PC_FLAG : { //read primary channel

				uint32_t zb_primary_channel = 
					esp_zb_get_primary_network_channel_set();

    		for (uint8_t i = 11; i <= 26; i++) {
      		if (zb_primary_channel & (1 << i)) {

						working_str = i;
						ESPUI.updateText(zigbee_primary_channel_text, working_str);
					}
    		}		
			} break;

			case GUI_CB_SET_PC_FLAG : { //write primary channel
					
					if (isNumber(
								ESPUI.getControl(zigbee_primary_channel_text)->value) &&
						(ESPUI.getControl(
								zigbee_primary_channel_text)->value.toInt() >= 11) &&
						(ESPUI.getControl(
								zigbee_primary_channel_text)->value.toInt() <= 26)) {

							uint32_t zb_primary_channel = 
								ESPUI.getControl(zigbee_primary_channel_text)->value.toInt();

							if (Supla::Storage::ConfigInstance()->setUInt32(
										Z2S_ZIGBEE_PRIMARY_CHANNEL, (1 << zb_primary_channel))) {

        				ESPUI.updateText(zigbee_primary_channel_text, 
									"New Zigbee primary channel write success! Restarting...");
   				     	Supla::Storage::ConfigInstance()->commit();
        				SuplaDevice.scheduleSoftRestart(1000);
							}
						}
				else
					ESPUI.updateText(zigbee_primary_channel_text, 
													 zigbee_primary_channel_text_str);
			} break;

			case GUI_CB_SET_INSTALLATION_CODE_FLAG: {

				if (ESPUI.getControl(
							zigbee_installation_code_ieee_address)->value.length() != 16) {
				
				  working_str = 
						PSTR("Enter device IEEE address (16 characters), "
								 "ie. 56AB23BE90004560");
					ESPUI.updateText(zigbee_installation_code_ieee_address, working_str);
					return;			
				}
				
				if (ESPUI.getControl(
							zigbee_installation_code_str)->value.length() != 36) {
				
				  working_str = 
						PSTR("Enter device installation code (36 characters), "
								 "ie. BC2356...");
					ESPUI.updateText(zigbee_installation_code_str, working_str);
					return;
				}	
				uint8_t device_ieee_addr[8];
				uint8_t device_installation_code[18];

				for (uint8_t i = 0; i < 8; i++) {

					working_str = 
						ESPUI.getControl(
							zigbee_installation_code_ieee_address)->value.substring(2*i, 2+ 2*i);

					device_ieee_addr[7 - i] = strtoul(working_str.c_str(), nullptr, 16);
					log_i("device_ieee_addr[%u] = %02X", 7 - i, device_ieee_addr[7 - i]);
				}

				for (uint8_t i = 0; i < 18; i++) {

					working_str = 
						ESPUI.getControl(
							zigbee_installation_code_str)->value.substring(
								2*i, 2+ 2*i);
					
					device_installation_code[i] = strtoul(working_str.c_str(), nullptr, 16);
					log_i("device_installation_code[%u] = %02X", 
								i, device_installation_code[i]);
				}

				esp_zb_secur_ic_add(device_ieee_addr, 
                            ESP_ZB_IC_TYPE_128, 
                            device_installation_code);

			} break;

			case GUI_CB_CLEAR_INSTALLATION_CODES_FLAG:

				esp_zb_secur_ic_remove_all_req();
			break;
		}
	}
}

void switchCallback(Control *sender, int type) {

	if (sender->value.toInt() == 1)
		updateLabel_P(factory_reset_label, factory_reset_enabled_str);
	else
		updateLabel_P(factory_reset_label, factory_reset_disabled_str);
}

void editDeviceCallback(Control *sender, int type, void *param) {

	if ((type == B_UP) && (ESPUI.getControl(device_selector)->value.toInt() >= 0)) {

		uint8_t device_slot = ESPUI.getControl(device_selector)->value.toInt();

		switch ((uint32_t)param) {

			case GUI_CB_UPDATE_DEVICE_NAME_FLAG : {

				strncpy(z2s_zb_devices_table[device_slot].device_local_name, 
								ESPUI.getControl(device_name_text)->value.c_str(), 32);

				z2s_zb_devices_table[device_slot].device_local_name[32] = '\0';
				z2s_zb_devices_table[device_slot].user_data_flags &= 
					~ZBD_USER_DATA_FLAG_SUBDEVICE_REGISTERED;
					
				if (Z2S_saveZbDevicesTable()) {

					//rebuildDevicesSelector();
					/*log_i("%d, %s", z2s_zb_devices_table[device_slot].device_gui_id, 
								z2s_zb_devices_table[device_slot].device_local_name);*/
					ESPUI.updateControlLabel(z2s_zb_devices_table[device_slot].device_gui_id, 
																	 z2s_zb_devices_table[device_slot].device_local_name);
					ESPUI.jsonDom(0);
				}
			} break;
		}
	}
}

uint32_t saveRemoteAddressData(uint8_t channel_slot) {

	working_str = ESPUI.getControl(param_1_number)->value;

	int8_t prefix_pos = working_str.indexOf("mdns://");

	if (prefix_pos >= 0) {

		Z2S_setChannelFlags(channel_slot, 
												USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS,
												false);

		memcpy(\
			z2s_channels_table[channel_slot].\
				remote_channel_data.mDNS_name,\
				working_str.c_str() + 7, 11); // cut "mdns://" before save
			
		z2s_channels_table[channel_slot].\
			remote_channel_data.mDNS_name[11] = '\0';
	} else {

		IPAddress ip;
		ip.fromString(working_str);

		Z2S_clearChannelFlags(channel_slot, 
													USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS,
													false);
			
		z2s_channels_table[channel_slot].\
			remote_channel_data.remote_ip_address = ip;
	}
		
	if (Z2S_saveChannelsTable()) {

		log_i("remote channel mDNS/IP address saved successfuly!");
				//		z2s_channels_table[channel_slot].user_data_1);
	}

	return
    Z2S_checkChannelFlags(channel_slot, 
                          USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS) ?
    REMOTE_ADDRESS_TYPE_MDNS : REMOTE_ADDRESS_TYPE_IP4;

}

uint8_t	saveRemoteChannelData(uint8_t channel_slot){

	uint8_t remote_Supla_channel = 
		ESPUI.getControl(param_2_number)->value.toInt();

		z2s_channels_table[channel_slot].Supla_remote_channel =\
				remote_Supla_channel;								

	if (Z2S_saveChannelsTable()) {

		log_i("remote_Supla_channel saved successfuly to %lu", 
					remote_Supla_channel);
	}
	return 
		remote_Supla_channel;
}

void editChannelCallback(Control *sender, int type, void *param) {

	log_i("type = %u, param %lu", type, (uint32_t)param);

	if (((type == B_UP) || (type == S_VALUE)) && 
			(ESPUI.getControl(channel_selector)->value.toInt() >= 0)) {
		
		if (gui_callback_reentry_number > 0) return;
		else gui_callback_reentry_number++;

		uint8_t channel_slot = 
			ESPUI.getControl(channel_selector)->value.toInt();

		switch ((uint32_t)param) {

			case GUI_CB_UPDATE_CHANNEL_NAME_FLAG : {	

				strncpy(z2s_channels_table[channel_slot].Supla_channel_name, 
								ESPUI.getControl(channel_name_text)->value.c_str(), 32);

				z2s_channels_table[channel_slot].Supla_channel_name[32] = '\0';

				if (Z2S_saveChannelsTable()) {

					log_i(
						"%d, %s", 
						z2s_channels_table[channel_slot].gui_control_id, 
						z2s_channels_table[channel_slot].Supla_channel_name);

					ESPUI.updateControlLabel(
						z2s_channels_table[channel_slot].gui_control_id, 
						z2s_channels_table[channel_slot].Supla_channel_name);
					
					ESPUI.jsonDom(0);
				}
			} break;


			case GUI_CB_UPDATE_CHANNEL_LOCAL_FUNCTION_FLAG: {

				int16_t connected_thermometers_function = 
					ESPUI.getControl(channel_local_function)->value.toInt();
				
				if (connected_thermometers_function > 0)
					setRemoteThermometerFunction(channel_slot, 
																			 connected_thermometers_function);
			} break;


			case GUI_CB_UPDATE_PARAM_1_FLAG : {	


				switch (z2s_channels_table[channel_slot].Supla_channel_type) {


					case 0x0000: {

						if (z2s_channels_table[channel_slot].local_channel_type ==
								LOCAL_CHANNEL_TYPE_REMOTE_RELAY) {

							if (saveRemoteAddressData(channel_slot) ==
									REMOTE_ADDRESS_TYPE_MDNS)
								updateRemoteRelayMDNSName(
									channel_slot,
									z2s_channels_table[channel_slot].\
									remote_channel_data.mDNS_name);
							else
								updateRemoteRelayIPAddress(
									channel_slot,
									z2s_channels_table[channel_slot].\
										remote_channel_data.remote_ip_address);
						}
					} break;

					
					case SUPLA_CHANNELTYPE_THERMOMETER:
					case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR: {

						saveRemoteAddressData(channel_slot);
					} break;


					case SUPLA_CHANNELTYPE_HVAC:

						updateHvacFixedCalibrationTemperature(
							channel_slot,
							ESPUI.getControl(param_1_number)->value.toInt());
					break;


					default: {

						z2s_channels_table[channel_slot].user_data_1 = 
							ESPUI.getControl(param_1_number)->value.toInt();
						
						if (Z2S_saveChannelsTable()) {

							log_i("channel user data 1 updated successfuly to %lu", 
										z2s_channels_table[channel_slot].user_data_1);
						}
					} break;
				}
			} break;

			case GUI_CB_UPDATE_PARAM_2_FLAG : {	


				switch (z2s_channels_table[channel_slot].Supla_channel_type) {


					case 0x0000: {

						updateRemoteRelaySuplaChannel(channel_slot,
																					saveRemoteChannelData(channel_slot));
					} break;


					case SUPLA_CHANNELTYPE_THERMOMETER:
					case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR: {

						saveRemoteChannelData(channel_slot);
					} break;


					default: {

						z2s_channels_table[channel_slot].user_data_2 = 
							ESPUI.getControl(param_2_number)->value.toInt();
						
						if (Z2S_saveChannelsTable()) {

							log_i("channel user data 2 updated successfuly to %lu", 
										z2s_channels_table[channel_slot].user_data_2);
						}
					} break;
				}
			} break;
			
			case GUI_CB_UPDATE_KEEPALIVE_FLAG : {	

				updateTimeout(
					channel_slot, 0, 1, 
					ESPUI.getControl(keepalive_number)->value.toInt());
			} break;


			case GUI_CB_UPDATE_TIMEOUT_FLAG : {		

				updateTimeout(channel_slot, 0, 2, 
											ESPUI.getControl(timeout_number)->value.toInt());
			} break;


			case GUI_CB_UPDATE_REFRESH_FLAG : {		

				updateTimeout(channel_slot, 0, 4, 
											ESPUI.getControl(refresh_number)->value.toInt());
			} break;	
		}
		gui_callback_reentry_number--;
	}
}

void editChannelFlagsCallback(Control *sender, int type, void *param) {

	if (ESPUI.getControl(channel_selector)->value.toInt() >= 0) {

		uint8_t channel_slot = ESPUI.getControl(channel_selector)->value.toInt();

		switch ((uint32_t)param) {


				case GUI_CB_DISABLE_CHANNEL_NOTIFICATIONS_FLAG: {

						if (type == S_ACTIVE)
							Z2S_setChannelFlags(channel_slot, 
																	USER_DATA_FLAG_DISABLE_NOTIFICATIONS);
						else
							Z2S_clearChannelFlags(channel_slot, 
																		USER_DATA_FLAG_DISABLE_NOTIFICATIONS);
				} break;


				case GUI_CB_TRV_AUTO_TO_SCHEDULE_FLAG: {

					if (type == S_ACTIVE)
							Z2S_setChannelFlags(channel_slot, 
																	USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE);
						else
							Z2S_clearChannelFlags(channel_slot, 
																		USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE);
				} break;


				case GUI_CB_TRV_AUTO_TO_SCHEDULE_MANUAL_FLAG: {

					if (type == S_ACTIVE)
							Z2S_setChannelFlags(channel_slot, 
																	USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE_MANUAL);
						else
							Z2S_clearChannelFlags(channel_slot, 
																		USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE_MANUAL);
				} break;


				case GUI_CB_TRV_FIXED_CALIBRATION_FLAG: {

					if (type == S_ACTIVE)
							Z2S_setChannelFlags(channel_slot, 
																	USER_DATA_FLAG_TRV_FIXED_CORRECTION);
						else {
							Z2S_clearChannelFlags(channel_slot, 
																		USER_DATA_FLAG_TRV_FIXED_CORRECTION);

							updateHvacFixedCalibrationTemperature(channel_slot, 0);
						}

						updateChannelInfoLabel(1);
				} break;


				case GUI_CB_TRV_COOPERATIVE_CHILDLOCK_FLAG: {

					if (type == S_ACTIVE)
							Z2S_setChannelFlags(channel_slot, 
																	USER_DATA_FLAG_TRV_COOPERATIVE_CHILDLOCK);
						else
							Z2S_clearChannelFlags(channel_slot, 
																		USER_DATA_FLAG_TRV_COOPERATIVE_CHILDLOCK);
				} break;


				case GUI_CB_ENABLE_RESEND_TEMPERATURE_FLAG: {

					if (type == S_ACTIVE)
							Z2S_setChannelFlags(channel_slot, 
																	USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE);
						else {

							z2s_channels_table[channel_slot].\
								remote_channel_data.remote_ip_address = 0;

							z2s_channels_table[channel_slot].Supla_remote_channel = 0xFF;

							Z2S_clearChannelFlags(channel_slot, 
																		USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE |
																		USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS);			
						}	
					
					updateChannelInfoLabel(1);
				} break;


				case GUI_CB_SET_SORWNS_ON_START_FLAG: {

					if (type == S_ACTIVE)
							Z2S_setChannelFlags(channel_slot, 
																	USER_DATA_FLAG_SET_SORWNS_ON_START);
						else
							Z2S_clearChannelFlags(channel_slot, 
																		USER_DATA_FLAG_SET_SORWNS_ON_START);
				} break;	
		}
	}
}

void batteryCallback(Control *sender, int type, void *param) {

	if ((type == B_UP) && 
			(ESPUI.getControl(device_selector)->value.toInt() >= 0)) {

		uint8_t channel_slot = ESPUI.getControl(device_selector)->value.toInt();

		switch ((uint32_t)param) {


			case GUI_CB_BATTERY_VOLTAGE_MIN_FLAG : {		

				z2s_zb_devices_table[channel_slot].battery_voltage_min = 
					ESPUI.getControl(battery_voltage_min_number)->value.toInt();
				
				Z2S_saveZbDevicesTable();
			} break;


			case GUI_CB_BATTERY_VOLTAGE_MAX_FLAG : {		

				z2s_zb_devices_table[channel_slot].battery_voltage_max = 
					ESPUI.getControl(battery_voltage_max_number)->value.toInt();

				Z2S_saveZbDevicesTable();
			} break;	
		}
	}
}

void batterySwitcherCallback(Control *sender, int type, void *param) {

	if (ESPUI.getControl(device_selector)->value.toInt() >= 0) {

		uint8_t device_slot = ESPUI.getControl(device_selector)->value.toInt();

		switch ((uint32_t)param) {


			case GUI_CB_DISABLE_VOLTAGE_MSG_FLAG : {		

				if (type == S_ACTIVE)
					Z2S_setZbDeviceFlags(device_slot, 
															 ZBD_USER_DATA_FLAG_DISABLE_BATTERY_VOLTAGE_MSG);
				else
					Z2S_clearZbDeviceFlags(device_slot, 
																 ZBD_USER_DATA_FLAG_DISABLE_BATTERY_VOLTAGE_MSG);
			} break;


			case GUI_CB_DISABLE_PERCENTAGE_MSG_FLAG : {	

				if (type == S_ACTIVE)
					Z2S_setZbDeviceFlags(device_slot, 
															 ZBD_USER_DATA_FLAG_DISABLE_BATTERY_PERCENTAGE_MSG);
				else
					Z2S_clearZbDeviceFlags(device_slot, 
																 ZBD_USER_DATA_FLAG_DISABLE_BATTERY_PERCENTAGE_MSG);
			} break;
		}
	}
}

void datatypeCallback(Control *sender, int type) {

	int8_t device_attribute_type_selector_value =  
		ESPUI.getControl(
			clusters_attributes_table[device_attribute_type_selector])->value.toInt();

	if (device_attribute_type_selector_value >= 0)
		ESPUI.updateNumber(
			clusters_attributes_table[device_attribute_size_number], 
			zigbee_datatypes[device_attribute_type_selector_value].zigbee_datatype_size);
}

void TuyaDatapointIdSelectorCallback(Control *sender, int type) {

	int8_t Tuya_datapoint_id_selector_value = 
		ESPUI.getControl(
			Tuya_devices_tab_controls_table[Tuya_datapoint_id_selector])->value.toInt();

	if (Tuya_datapoint_id_selector_value >= 0) {

		
		ESPUI.updateNumber(
			Tuya_devices_tab_controls_table[Tuya_datapoint_id_number], 
			Tuya_datapoints[Tuya_datapoint_id_selector_value].Tuya_datapoint_id);
		
		working_str = 
			Tuya_datapoints[Tuya_datapoint_id_selector_value].Tuya_datapoint_type;
		ESPUI.updateSelect(
			Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector], 
			working_str);
		
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
	}
	else
		ESPUI.updateSelect(
			Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector],
			minus_one_str);
}


void TuyaDatapointTypeSelectorCallback(Control *sender, int type) {

	int8_t Tuya_datapoint_type_selector_value = 
		ESPUI.getControl(
			Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector])->value.toInt();

	if (Tuya_datapoint_type_selector_value >= 0)
		ESPUI.updateNumber(
			Tuya_devices_tab_controls_table[Tuya_datapoint_length_number], 
			Tuya_datapoint_types[Tuya_datapoint_type_selector_value].Tuya_datapoint_type_length);
}

void clearAttributeIdSelect() {

	if (device_attribute_id_selector_first_option_id < 0xFFFF) {

		for (uint16_t options_id_counter = device_attribute_id_selector_first_option_id; 
				options_id_counter <= device_attribute_id_selector_last_option_id; 
				options_id_counter++) {

				if (options_id_counter < 0xFFFF)
					ESPUI.removeControl(options_id_counter, false);		
		}
	}

	ESPUI.updateControlValue(clusters_attributes_table[device_attribute_id_selector], 
													 minus_one_str);
	ESPUI.updateControlValue(clusters_attributes_table[device_attribute_type_selector], 
													 minus_one_str);
}

void clearAttributeValueSelect() {

	if (device_attribute_value_selector_first_option_id < 0xFFFF) {

		for (uint16_t options_id_counter = device_attribute_value_selector_first_option_id; 
				options_id_counter <= device_attribute_value_selector_last_option_id; 
				options_id_counter++) {

				if (options_id_counter < 0xFFFF)
					ESPUI.removeControl(options_id_counter, false);		
		}
	}

	ESPUI.updateControlValue(clusters_attributes_table[device_attribute_value_selector], 
													 minus_one_str);
	ESPUI.updateNumber(clusters_attributes_table[device_attribute_value_text], 0);
}

void attributeCallback (Control *sender, int type) {

	if (ESPUI.getControl(clusters_attributes_table[device_attribute_id_selector])->value.toInt() >= 0) {

		uint16_t zigbee_attributes_idx = 
			ESPUI.getControl(clusters_attributes_table[device_attribute_id_selector])->value.toInt();

		uint32_t zigbee_datatypes_count = sizeof(zigbee_datatypes) / 
																			sizeof(zigbee_datatype_t);

		for (uint8_t datatypes_counter = 0; 
				 datatypes_counter < zigbee_datatypes_count; 
				 datatypes_counter++) {
  
			if (zigbee_datatypes[datatypes_counter].zigbee_datatype_id == 
					zigbee_attributes[zigbee_attributes_idx].zigbee_attribute_datatype_id) {

				working_str = datatypes_counter;
				ESPUI.updateSelect(clusters_attributes_table[device_attribute_type_selector], working_str);
				ESPUI.updateNumber(clusters_attributes_table[device_attribute_size_number], 
													 zigbee_datatypes[datatypes_counter].zigbee_datatype_size);
				break;
			}
		}
		ESPUI.updateNumber(clusters_attributes_table[device_attribute_id_text], 
											 zigbee_attributes[zigbee_attributes_idx].zigbee_attribute_id);

		clearAttributeValueSelect();

	
		uint32_t zigbee_attribute_values_count = 
			sizeof(zigbee_attribute_values)/sizeof(zigbee_attribute_value_t);

		uint16_t cluster_id = 
			ESPUI.getControl(clusters_attributes_table[device_cluster_selector])->value.toInt();

		device_attribute_value_selector_first_option_id = 0xFFFF;
		device_attribute_value_selector_last_option_id		= 0xFFFF;

		uint16_t current_option_id = 0xFFFF;

		for (uint32_t i = 0; i < zigbee_attribute_values_count; i++) {

			if ((zigbee_attribute_values[i].zigbee_cluster_id == cluster_id) && 
					(zigbee_attribute_values[i].zigbee_attribute_id == 
					 zigbee_attributes[zigbee_attributes_idx].zigbee_attribute_id)) {

				working_str = i;
				current_option_id = 
					ESPUI.addControl(Control::Type::Option, 
													zigbee_attribute_values[i].zigbee_attribute_value_name, 
													working_str, 
													Control::Color::None, 
													clusters_attributes_table[device_attribute_value_selector]);

				if (device_attribute_value_selector_first_option_id == 0xFFFF)
					device_attribute_value_selector_first_option_id = current_option_id;
			}
		}
		
		device_attribute_value_selector_last_option_id = current_option_id;
	}
}
	
void clusterCallback (Control *sender, int type) {

	if (sender->value.toInt() >= 0) {
		
		gui_command = 34;
	}
}

void valueCallback(Control *sender, int type) {
	
	int8_t sender_value = sender->value.toInt(); //max 128 attribute values

	if (sender_value >= 0) {
	
		ESPUI.updateNumber(clusters_attributes_table[device_attribute_value_text], 
											 zigbee_attribute_values[sender_value].zigbee_attribute_value);
	}
}

void advancedDeviceSelectorCallback(Control *sender, int type) {

	log_i("value = %s(%u)", sender->value.c_str(), sender->value.toInt());
	
	if ((!isNumber(sender->value)) || 
			(sender->value.toInt() < 0) || 
			(sender->value.toInt() >= Z2S_ZB_DEVICES_MAX_NUMBER)) {

		updateLabel_P(advanced_device_info_label, three_dots_str);

		if (isSonoffValvePresent)
			enableSonoffValveGUI(false);
		if (isTuyaGasDetectorPresent)
			enableTuyaGasDetectorGUI(false);
		if (isMoesAlarmPresent)
			enableMoesAlarmGUI(false);
		return;
	}
	
	uint8_t device_slot = sender->value.toInt();

	if (z2s_zb_devices_table[device_slot].desc_id == Z2S_DEVICE_DESC_SONOFF_SMART_VALVE)
		enableSonoffValveGUI(true);
	else {
		if (isSonoffValvePresent)
			enableSonoffValveGUI(false);
	}
	
	if (z2s_zb_devices_table[device_slot].desc_id == Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR)
		enableTuyaGasDetectorGUI(true);
	else {
		if (isSonoffValvePresent)
			enableTuyaGasDetectorGUI(false);
	}
	if (z2s_zb_devices_table[device_slot].desc_id == Z2S_DEVICE_DESC_MOES_ALARM)
		enableMoesAlarmGUI(true);
	else {
		if (isMoesAlarmPresent)
			enableMoesAlarmGUI(false);
	}

	sprintf_P(general_purpose_gui_buffer,PSTR("<b><i>Manufacturer name</i></b> %s "
					"<b>| <i>model ID</b></i> %s"), 
					Z2S_getZbDeviceManufacturerName(device_slot),
					Z2S_getZbDeviceModelName(device_slot));

	updateLabel_P(advanced_device_info_label, general_purpose_gui_buffer);
}

void gatewayCallback(Control *sender, int type, void *param) {

	switch ((uint32_t)param) {


		case GUI_CB_ENABLE_GUI_FLAG: { 

			if (Supla::Storage::ConfigInstance()->setInt8(Z2S_ENABLE_GUI_ON_START_V2, 
					ESPUI.getControl(enable_gui_switcher)->value.toInt()))
      	Supla::Storage::ConfigInstance()->commit();
		} break;


		case GUI_CB_FORCE_CONFIG_FLAG: { 

			if (Supla::Storage::ConfigInstance()->setUInt8(Z2S_FORCE_CONFIG_ON_START, 
					ESPUI.getControl(force_config_switcher)->value.toInt()))
      	Supla::Storage::ConfigInstance()->commit();
		} break;


		case GUI_CB_GUI_DELAY_FLAG: {

			if (Supla::Storage::ConfigInstance()->setInt32(Z2S_GUI_ON_START_DELAY_V2, 
					ESPUI.getControl(gui_start_delay_number)->value.toInt()))
      	Supla::Storage::ConfigInstance()->commit();
		} break;


		case GUI_CB_SAVE_MDNS_NAME_FLAG: {

			if (Supla::Storage::ConfigInstance()->setString(Z2S_GATEWAY_MDNS_LOCAL_NAME, 
					ESPUI.getControl(gateway_mdns_name_text)->value.c_str()))
      	Supla::Storage::ConfigInstance()->commit();
		} break;


		case GUI_CB_REBUILD_CHANNELS_FLAG: {

			if (Supla::Storage::ConfigInstance()->setUInt8(Z2S_REBUILD_CHANNELS_ON_START, 
					ESPUI.getControl(rebuild_Supla_channels_switcher)->value.toInt()))
      	Supla::Storage::ConfigInstance()->commit();
		} break;


		case GUI_CB_USE_NEW_AT_FLAG: {

			uint8_t use_new_at_model = 
				ESPUI.getControl(use_new_at_model_switcher)->value.toInt();

			if (Supla::Storage::ConfigInstance()->setUInt8(Z2S_USE_NEW_AT_MODEL, 
					use_new_at_model))
      	Supla::Storage::ConfigInstance()->commit();

			_use_new_at_model = use_new_at_model;
		} break;


		case GUI_CB_GUI_RESTART_FLAG: {

			SuplaDevice.softRestart();
		} break;
	}
}

void TuyaCustomCmdCallback(Control *sender, int type, void *param) {

	if ((type == B_UP) && 
			(ESPUI.getControl(Tuya_devices_tab_controls_table[Tuya_device_selector])->value.toInt() >= 0)) {

		uint8_t Tuya_device_selector_value = 
			ESPUI.getControl(Tuya_devices_tab_controls_table[Tuya_device_selector])->value.toInt();

		zbg_device_params_t device;
		log_i("device_selector value %u, param id %d", Tuya_device_selector_value, (uint32_t)param);
    
		device.endpoint = 1;
    device.cluster_id = TUYA_PRIVATE_CLUSTER_EF00;

		uint8_t cmd_id;

		uint16_t tsn_id = random(0x0000, 0xFFFF);;

    memcpy(
			&device.ieee_addr, 
			z2s_zb_devices_table[Tuya_device_selector_value].ieee_addr, 
			sizeof(esp_zb_ieee_addr_t));
    
		device.short_addr = 
			z2s_zb_devices_table[Tuya_device_selector_value].short_addr;

		switch ((uint32_t)param) {
			
			case GUI_CB_SEND_TUYA_REQUEST_FLAG: {

				uint8_t cmd_dp_id = ESPUI.getControl(
						Tuya_devices_tab_controls_table[Tuya_datapoint_id_number])->value.toInt();

				if (cmd_dp_id == 0) {

					updateLabel_P(
						Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label], 
						PSTR("Tuya datapoint id must be in range [1...255]!"));
					return;
				}

				int8_t cmd_dp_type = ESPUI.getControl(
					Tuya_devices_tab_controls_table[Tuya_datapoint_type_selector])->value.toInt();

				if (cmd_dp_type < 0) {

					updateLabel_P(
						Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label], 
						PSTR("Tuya datapoint type not selected!"));
					return;
				}

				uint8_t cmd_dp_length = ESPUI.getControl(
					Tuya_devices_tab_controls_table[Tuya_datapoint_length_number])->value.toInt();

				if (cmd_dp_type > 58) {

					updateLabel_P(
						Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label], 
						PSTR("Tuya datapoint length must be in range [1..58]!"));
					return;
				}

				int32_t cmd_dp_value = ESPUI.getControl(
					Tuya_devices_tab_controls_table[Tuya_datapoint_value_number])->value.toInt();

				Tuya_custom_cmd_dp = cmd_dp_id;

				current_Tuya_payload_label = 
					Tuya_devices_tab_controls_table[Tuya_device_payload_label];

				uint8_t Tuya_data_cmd = TUYA_REQUEST_CMD;

				if (Z2S_checkZbDeviceFlags(
					Tuya_device_selector_value, 
					ZBD_USER_DATA_FLAG_TUYA_USE_SEND_DATA))
					Tuya_data_cmd = TUYA_SEND_CMD;

				bool result = sendTuyaRequestCmdData(
					&zbGateway, &device, cmd_dp_id, cmd_dp_type, cmd_dp_value, true, 
					Tuya_data_cmd);

				if (result) {

					if (*zbGateway.getCustomCmdStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS) {
					
						sprintf_P(general_purpose_gui_buffer, 
											PSTR("Tuya custom cluster data request sent successfully! <br>"
														"Datapoint id   = %u(0x%02X)<br>"
														"Datapoint type = %s(0x%02X)<br>"
														"Datapoint length 	= %u(0x%02X)<br>"
														"Datapoint value = %u(0x%02X)"),
											cmd_dp_id,
											cmd_dp_id,
											Tuya_datapoint_types[cmd_dp_type].Tuya_datapoint_type_name,
											Tuya_datapoint_types[cmd_dp_type].Tuya_datapoint_type_id,
											cmd_dp_length,
											cmd_dp_length,
											cmd_dp_value,
											cmd_dp_value);
											
						updateLabel_P(Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label], 
													general_purpose_gui_buffer);
					} else {

						sprintf_P(general_purpose_gui_buffer, 
											PSTR("Tuya custom cluster data request failed! <br>"
														"Status         = %u(0x%02X)<br>"
														"Datapoint id   = %u(0x%02X)<br>"
														"Datapoint type = %s(0x%02X)<br>"
														"Datapoint length 	= %u(0x%02X)<br>"
														"Datapoint value = %u(0x%02X)"),
											*zbGateway.getCustomCmdStatusLastResult(),
											*zbGateway.getCustomCmdStatusLastResult(),
											cmd_dp_id,
											cmd_dp_id,
											Tuya_datapoint_types[cmd_dp_type].Tuya_datapoint_type_name,
											Tuya_datapoint_types[cmd_dp_type].Tuya_datapoint_type_id,
											cmd_dp_length,
											cmd_dp_length,
											cmd_dp_value,
											cmd_dp_value);
						updateLabel_P(Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label], 
													general_purpose_gui_buffer);
						updateLabel_P(Tuya_devices_tab_controls_table[Tuya_device_payload_label], 
													three_dots_str);
					}
				} else {

						updateLabel_P(Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label], 
													device_query_failed_str);
						updateLabel_P(Tuya_devices_tab_controls_table[Tuya_device_payload_label], 
													three_dots_str);
				}
			} break;

			case GUI_CB_SEND_TUYA_QUERY_FLAG: {

				updateLabel_P(Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label], 
											three_dots_str);
				updateLabel_P(Tuya_devices_tab_controls_table[Tuya_device_payload_label], 
											three_dots_str);
				
				uint8_t cmd_dp_id = ESPUI.getControl(
					Tuya_devices_tab_controls_table[Tuya_datapoint_id_number])->value.toInt();
				
				Tuya_custom_cmd_dp = cmd_dp_id;
				
				current_Tuya_payload_label = 
					Tuya_devices_tab_controls_table[Tuya_device_payload_label];

				bool result = sendTuyaQueryCmd(&zbGateway, &device, true);

				if (result) {

						if (*zbGateway.getCustomCmdStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS) {
						
							sprintf_P(
								general_purpose_gui_buffer, 
								PSTR("Tuya custom cluster data query sent successfully!"));

							updateLabel_P(
								Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label], 
								general_purpose_gui_buffer);
						} else {

							sprintf_P(
								general_purpose_gui_buffer, 
								PSTR("Tuya custom cluster data query failed! <br>"
								"Status = %u(0x%02X)"),
								*zbGateway.getCustomCmdStatusLastResult(),
								*zbGateway.getCustomCmdStatusLastResult());

							updateLabel_P(
								Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label], 
								general_purpose_gui_buffer);
						}
				} else {

					updateLabel_P(
						Tuya_devices_tab_controls_table[Tuya_device_cmd_result_label], 
						device_query_failed_str);
						
					updateLabel_P(
						Tuya_devices_tab_controls_table[Tuya_device_payload_label], 
						three_dots_str);										
				}
			} break; 
		}
	}
}

void actionsTableCallback(Control *sender, int type, void *param) {

	if (type == B_UP) {

		z2s_channel_action_t new_action = {};
		
		working_str = "No actions";

		switch ((uint32_t)param) {


			case GUI_CB_ACTION_FIRST_FLAG: {

				int16_t first_action = Z2S_findNextActionPosition(0);
  			if (first_action >= 0) {

					current_action_id = first_action;
					current_action_counter = Z2S_getActionCounter(first_action);

      		Z2S_loadAction(first_action, new_action);
					sprintfAction(new_action);
					ESPUI.updateLabel(actions_table_label, working_str);
					updateActionDetails(new_action);
				}
			} break;


			case GUI_CB_ACTION_NEXT_FLAG: {

				if (current_action_id < 0)
					return;

				int16_t next_action = Z2S_findNextActionPosition(current_action_id + 1);
  			if (next_action >= 0) {

					current_action_id = next_action;
					current_action_counter = Z2S_getActionCounter(next_action);

      		Z2S_loadAction(next_action, new_action);
					sprintfAction(new_action);
					ESPUI.updateLabel(actions_table_label, working_str);
					updateActionDetails(new_action);
				}				
			} break;


			case GUI_CB_ACTION_PREV_FLAG: {

				if (current_action_id <= 0)
					return;

				int16_t prev_action = Z2S_findPrevActionPosition(current_action_id - 1);
  			if (prev_action >= 0) {

					current_action_id = prev_action;
					current_action_counter = Z2S_getActionCounter(prev_action);
				
      		Z2S_loadAction(prev_action, new_action);
					sprintfAction(new_action);
					ESPUI.updateLabel(actions_table_label, working_str);
					updateActionDetails(new_action);
				}
			} break;


			case GUI_CB_ACTION_LAST_FLAG: {

				if (current_action_id < 0)
					return;

				int16_t last_action = Z2S_findPrevActionPosition(Z2S_ACTIONS_MAX_NUMBER);
  			if (last_action >= 0) {

					current_action_id = last_action;
					current_action_counter = Z2S_getActionCounter(last_action);

      		Z2S_loadAction(last_action, new_action);
					sprintfAction(new_action);
					ESPUI.updateLabel(actions_table_label, working_str);
					updateActionDetails(new_action);
				}
			} break;


			case GUI_CB_ACTION_EDIT_FLAG: {

				if ((current_action_gui_state != VIEW_ACTION) || (current_action_id == -1))
					return;

				previous_action_gui_state = current_action_gui_state;
				current_action_gui_state = EDIT_ACTION;
				Z2S_loadAction(current_action_id, new_action);
				enableActionDetails(true);
				updateActionDetails(new_action);
				updateActionButtons();
			} break;


			case GUI_CB_ACTION_COPY_FLAG: {

				if ((current_action_gui_state != VIEW_ACTION) || (current_action_id == -1))
					return;

				new_action_id = Z2S_findFreeActionIndex();
				
				if (new_action_id == -1) {
					
					updateLabel_P(action_state_label, "Actions table is full - can't add new Z2S Action!");
					return;
				}
				
				previous_action_gui_state = current_action_gui_state;
				current_action_gui_state = COPY_ACTION;
				Z2S_loadAction(current_action_id, new_action);
				enableActionDetails(true);
				updateActionDetails(new_action);
				updateActionButtons();
			} break;


			case GUI_CB_ACTION_NEW_FLAG: {

				if (current_action_gui_state != VIEW_ACTION)
					return;
				
				new_action_id = Z2S_findFreeActionIndex();
				if (new_action_id == -1) {
					
					updateLabel_P(action_state_label, "Actions table is full - can't add new Z2S Action!");
					return;
				} else {
					previous_action_gui_state = current_action_gui_state;
					current_action_gui_state = NEW_ACTION;
					enableActionDetails(true);
					updateActionButtons();
					updateActionDetails(new_action, true);
				}
			} break;

			case GUI_CB_ACTION_SAVE_FLAG: {

				if ((current_action_gui_state != NEW_ACTION) && 
						(current_action_gui_state != EDIT_ACTION) && 
						(current_action_gui_state != COPY_ACTION))
					return;
				
				if (!fillActionDetails(new_action)) {
					updateLabel_P(action_state_label, "Required action data is missing!");
					return;
				}

				previous_action_gui_state = SAVE_ACTION;
				
				if ((current_action_gui_state == NEW_ACTION) ||
				 		(current_action_gui_state == COPY_ACTION)) {
					
					current_action_id = new_action_id;
				}

				bool save_result = Z2S_saveAction(current_action_id, new_action);

				if ((current_action_gui_state == NEW_ACTION) ||
						(current_action_gui_state == COPY_ACTION)) {
					
					current_action_counter = Z2S_getActionCounter(current_action_id);
				}

				current_action_gui_state = VIEW_ACTION;

				enableActionDetails(false);
				enableActionControls(true);
				sprintfAction(new_action);
				ESPUI.updateLabel(actions_table_label, working_str);
				updateActionButtons();

				if (save_result)
					updateLabel_P(action_state_label, "Saving Z2S Action: SUCCESS!<br>"
																						"Restart gateway to activate action!");
				else
					updateLabel_P(action_state_label, "Saving Z2S Action: FAILED!");
			} break;

			case GUI_CB_ACTION_CANCEL_FLAG: {

				if ((current_action_gui_state != NEW_ACTION) && 
						(current_action_gui_state != EDIT_ACTION) &&
						(current_action_gui_state != COPY_ACTION))
					return;

				previous_action_gui_state = CANCEL_ACTION;
				current_action_gui_state = VIEW_ACTION;
				enableActionDetails(false);				
				enableActionControls(true);
				updateActionButtons();
				updateLabel_P(action_state_label, "Changes cancelled!");
			} break;

			case GUI_CB_ACTION_REMOVE_FLAG: {

				if ((current_action_gui_state != VIEW_ACTION) || (current_action_id == -1))
					return;

				previous_action_gui_state = REMOVE_ACTION;

				bool remove_result = Z2S_removeAction(current_action_id);
				current_action_id = Z2S_findNextActionPosition(0);

				if (current_action_id >= 0) {

					current_action_counter = Z2S_getActionCounter(current_action_id);
					Z2S_loadAction(current_action_id, new_action);
					sprintfAction(new_action);
					ESPUI.updateLabel(actions_table_label, working_str);
				}
				else {

					current_action_counter = -1;
					working_str = "Actions table is empty.";
					ESPUI.updateLabel(actions_table_label, working_str);
				}
				current_action_gui_state = VIEW_ACTION;
				enableActionDetails(false);
				enableActionControls(true);
				updateActionButtons();

				if (remove_result)
					updateLabel_P(action_state_label, "Removing Z2S Action: SUCCESS! <br>"
																						"Action still active in Supla Device - "
																						"restart gateway to deactivate it!");
				else
					updateLabel_P(action_state_label, "Removing Z2S Action: FAILED!");
			} break;
		}
	}
}

void valveCallback(Control *sender, int type, void *param) {

	if ((type == B_UP) && (ESPUI.getControl(advanced_device_selector)->value.toInt() >= 0)) {

		uint8_t device_slot = ESPUI.getControl(advanced_device_selector)->value.toInt();

		zbg_device_params_t device;
		log_i("device_selector value %u, param id %d", device_slot, (uint32_t)param);
    device.endpoint = 1;
    device.cluster_id = SONOFF_CUSTOM_CLUSTER; 
    memcpy(&device.ieee_addr, z2s_zb_devices_table[device_slot].ieee_addr,8);
    device.short_addr = z2s_zb_devices_table[device_slot].short_addr;

		uint16_t attribute_id;

		uint32_t value_32;

		switch ((uint32_t)param) {
			
			case GUI_CB_SAVE_PROGRAM_FLAG: { //save valve program

				if ((ESPUI.getControl(valve_program_selector)->value.toInt() > 0) &&
						(ESPUI.getControl(valve_cycles_number)->value.toInt() > 0)) {

					uint8_t program_mode = 
						ESPUI.getControl(valve_program_selector)->value.toInt();

					uint8_t valve_cycles = 
						ESPUI.getControl(valve_cycles_number)->value.toInt();

					if (program_mode == 1) 
						
						value_32 = ESPUI.getControl(valve_worktime_number)->value.toInt();
						
					else {
						
						value_32 = ESPUI.getControl(valve_volume_number)->value.toInt();
					}
					
					z2s_zb_devices_table[device_slot].smart_valve_data.program = program_mode;
					z2s_zb_devices_table[device_slot].smart_valve_data.cycles = valve_cycles;
					z2s_zb_devices_table[device_slot].smart_valve_data.value = value_32;
					z2s_zb_devices_table[device_slot].smart_valve_data.pause_time = 
						ESPUI.getControl(valve_pause_number)->value.toInt();
					
					if (Z2S_saveZbDevicesTable()) 
        		
						updateLabel_P(valve_info_label, PSTR("Valve program saved successfully."));
					else
						
						updateLabel_P(valve_info_label, PSTR("Valve program save failed."));
				} else
				
				updateLabel_P(valve_info_label, PSTR("No valve program to save"));
			} break;

			case GUI_CB_LOAD_PROGRAM_FLAG: { //load valve program

				uint8_t valve_cycles = z2s_zb_devices_table[device_slot].smart_valve_data.cycles; 
				
				if (valve_cycles > 0) {

					uint8_t program_mode = z2s_zb_devices_table[device_slot].smart_valve_data.program; 

					ESPUI.updateNumber(valve_program_selector, program_mode);
					ESPUI.updateNumber(valve_cycles_number, valve_cycles); // & 0x7F);

					value_32 = z2s_zb_devices_table[device_slot].smart_valve_data.value; 
					
					if (program_mode == 1) {
						ESPUI.updateNumber(valve_worktime_number, value_32);
						ESPUI.updateNumber(valve_volume_number, 0);
					}
					else {
						ESPUI.updateNumber(valve_volume_number, value_32);
						ESPUI.updateNumber(valve_worktime_number, 0);
					}

					value_32 = z2s_zb_devices_table[device_slot].smart_valve_data.pause_time; 

					ESPUI.updateNumber(valve_pause_number, value_32);

        	updateLabel_P(valve_info_label, PSTR("Valve program loaded successfully."));	
				} else
					
					updateLabel_P(valve_info_label, PSTR("No Valve program found."));
			} break;
			
			case GUI_CB_START_PROGRAM_FLAG: { //start valve program

				switch (ESPUI.getControl(valve_program_selector)->value.toInt()) {

					case 1: attribute_id = SONOFF_CUSTOM_CLUSTER_TIME_IRRIGATION_CYCLE_ID; break;

					case 2: attribute_id = SONOFF_CUSTOM_CLUSTER_VOLUME_IRRIGATION_CYCLE_ID; break;

					default: return;
				}
		
				uint8_t valve_cmd_payload[11] = {0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

				valve_cmd_payload[2] = ESPUI.getControl(valve_cycles_number)->value.toInt();
		
				if (attribute_id == 0x5008) 
					value_32 = ESPUI.getControl(valve_worktime_number)->value.toInt();
				else 
					value_32 = ESPUI.getControl(valve_volume_number)->value.toInt();

				valve_cmd_payload[6] = value_32 & 0xFF;
				valve_cmd_payload[5] = (value_32 & 0xFF00) >> 8;
				valve_cmd_payload[4] = (value_32 & 0xFF0000) >> 16;
		
				value_32 = ESPUI.getControl(valve_pause_number)->value.toInt();
				valve_cmd_payload[10] = value_32 & 0xFF;
				valve_cmd_payload[9] = (value_32 & 0xFF00) >> 8;
				valve_cmd_payload[8] = (value_32 & 0xFF0000) >> 16;
		
				for (int i = 0; i < 11; i++)
					log_i("valve payload [%u] = 0x%02x", i,valve_cmd_payload[i]);

				if (zbGateway.sendAttributeWrite(&device, 
																				 SONOFF_CUSTOM_CLUSTER, 
																				 attribute_id, 
																				 ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING, 
																				 11, 
																				 &valve_cmd_payload, 
																				 true)) {

					if (*zbGateway.getWriteAttrStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS)
						
						updateLabel_P(valve_info_label, PSTR("Valve program start success!"));
					else
						
						updateLabel_P(valve_info_label, PSTR("Valve program start error!"));
				} else
					
					updateLabel_P(valve_info_label, device_query_failed_str);
			} break;

			case GUI_CB_STOP_PROGRAM_FLAG: { //stop valve program

				zbGateway.sendOnOffCmd(&device, false);
			} break;

			case GUI_CB_SEND_PROGRAM_FLAG: {

				int16_t channel_number_slot = Z2S_findChannelNumberSlot(z2s_zb_devices_table[device_slot].ieee_addr, 
																																-1, 
																																SONOFF_CUSTOM_CLUSTER, 
																																SUPLA_CHANNELTYPE_RELAY, 
																																SONOFF_SMART_VALVE_RUN_PROGRAM_SID);
  
  			if (channel_number_slot < 0) {
					
					log_i("no Supla channel for Sonoff program run");
					return;
				}

				switch (ESPUI.getControl(valve_program_selector)->value.toInt()) {

					case 1: {
						
						z2s_channels_table[channel_number_slot].smart_valve_data.program = 1;
						z2s_channels_table[channel_number_slot].smart_valve_data.value = 
							ESPUI.getControl(valve_worktime_number)->value.toInt();

						msgZ2SDeviceVirtualRelayValue(channel_number_slot, 
																					VRV_S8_ID, 
																					1);

						msgZ2SDeviceVirtualRelayValue(channel_number_slot, 
																					VRV_S32_ID,
																					ESPUI.getControl(valve_worktime_number)->value.toInt());
					} break;

					case 2: {
						
						z2s_channels_table[channel_number_slot].smart_valve_data.program = 2;
						z2s_channels_table[channel_number_slot].smart_valve_data.value = 
							ESPUI.getControl(valve_volume_number)->value.toInt();

						msgZ2SDeviceVirtualRelayValue(channel_number_slot, 
																					VRV_S8_ID, 
																					2);

						msgZ2SDeviceVirtualRelayValue(channel_number_slot, 
																					VRV_S32_ID,
																					ESPUI.getControl(valve_volume_number)->value.toInt());
					} break;

					default: {
						z2s_channels_table[channel_number_slot].smart_valve_data.program = 0;
						z2s_channels_table[channel_number_slot].smart_valve_data.cycles = 0;
						z2s_channels_table[channel_number_slot].smart_valve_data.value = 0;

						msgZ2SDeviceVirtualRelayValue(channel_number_slot, 
																					VRV_S8_ID, 
																					0);

						Z2S_saveChannelsTable();
						return;
					} break;
				}

				z2s_channels_table[channel_number_slot].smart_valve_data.cycles = 
					ESPUI.getControl(valve_cycles_number)->value.toInt();

				z2s_channels_table[channel_number_slot].smart_valve_data.pause_time = 
				ESPUI.getControl(valve_pause_number)->value.toInt();

				Z2S_saveChannelsTable();

				msgZ2SDeviceVirtualRelayValue(channel_number_slot, 
																			VRV_U8_ID,
																			ESPUI.getControl(valve_cycles_number)->value.toInt());
																			
				msgZ2SDeviceVirtualRelayValue(channel_number_slot, 
																			VRV_U32_ID, 
																			ESPUI.getControl(valve_pause_number)->value.toInt());
			} break;

			case GUI_CB_SEND_RINGTONE_FLAG: { //write gas detector alarm ringtone

				int16_t gas_alarm_ringtone = 
					ESPUI.getControl(gas_alarm_ringtone_selector)->value.toInt();
				
				if (gas_alarm_ringtone >= 0) {

					current_Tuya_payload_label = gas_alarm_Tuya_payload_label;
					
					if (sendTuyaRequestCmdEnum8(&zbGateway, 
																			&device, 
																			TUYA_GAS_DETECTOR_RINGTONE_DP,
																			gas_alarm_ringtone, 
																			CUSTOM_CMD_SYNC))

						updateLabel_P(gas_alarm_info_label, PSTR("New ringtone sent"));
					else
					
						updateLabel_P(gas_alarm_info_label, device_query_failed_str);
				} else
					
					updateLabel_P(gas_alarm_info_label, PSTR("Select ringtone to send."));
			} break;

			case GUI_CB_SEND_TIME_FLAG: { //write gas detector alarm duration

				int32_t gas_alarm_time = 
					ESPUI.getControl(gas_alarm_time_number)->value.toInt();
				
				if (gas_alarm_time >= 0) {

					current_Tuya_payload_label = gas_alarm_Tuya_payload_label;
					
					if (sendTuyaRequestCmdValue32(&zbGateway, 
																				&device, 
																				TUYA_GAS_DETECTOR_ALARM_TIME_DP,
																				gas_alarm_time, 
																				CUSTOM_CMD_SYNC))

					updateLabel_P(gas_alarm_info_label, PSTR("New alarm time sent"));
				else

					updateLabel_P(gas_alarm_info_label, device_query_failed_str);
				} else
					
					updateLabel_P(gas_alarm_info_label, PSTR("Enter alarm duration to send."));
			} break;

			case GUI_CB_SELF_TEST_FLAG: { //gas detector self test cmd

				current_Tuya_payload_label = gas_alarm_Tuya_payload_label;

				if (sendTuyaRequestCmdBool(&zbGateway, 
																	 &device, 
																	 TUYA_GAS_DETECTOR_SELF_TEST_RESULT_DP,
																	 true, //ON
																	 CUSTOM_CMD_SYNC))

					updateLabel_P(gas_alarm_info_label, PSTR("Self-test command sent"));
				else

					updateLabel_P(gas_alarm_info_label, device_query_failed_str);

			} break;

			case GUI_CB_SILENCE_FLAG: { //gas detector silence cmd

				current_Tuya_payload_label = gas_alarm_Tuya_payload_label;

				if (sendTuyaRequestCmdBool(&zbGateway, 
																	 &device, 
																	 TUYA_GAS_DETECTOR_SILENCE_DP,
																	 true, //ON
																	 CUSTOM_CMD_SYNC))

					updateLabel_P(gas_alarm_info_label, PSTR("Silence command sent"));
				else

					updateLabel_P(gas_alarm_info_label, device_query_failed_str);
			} break;


			case GUI_CB_SEND_MELODY_FLAG: { //send moes alarm melody
 
				int16_t moes_alarm_melody = 
					ESPUI.getControl(moes_alarm_melody_selector)->value.toInt();
				
				if (moes_alarm_melody >= 0) {

					current_Tuya_payload_label = moes_alarm_Tuya_payload_label;
					
					if (sendTuyaRequestCmdEnum8(&zbGateway, 
																			&device, 
																			MOES_ALARM_MELODY_DP,
																			moes_alarm_melody, 
																			CUSTOM_CMD_SYNC))

						updateLabel_P(moes_alarm_info_label, PSTR("New melody sent"));
					else
					
						updateLabel_P(moes_alarm_info_label, device_query_failed_str);
				} else
						
					updateLabel_P(moes_alarm_info_label, PSTR("Select melody to send."));
			} break;


			case GUI_CB_SEND_VOLUME_FLAG: { //send moes alarm volume

				int16_t moes_alarm_volume = 
					ESPUI.getControl(moes_alarm_volume_number)->value.toInt();
				
				if (moes_alarm_volume >= 0) {

					current_Tuya_payload_label = moes_alarm_Tuya_payload_label;
					
					if (sendTuyaRequestCmdEnum8(&zbGateway, 
																			&device, 
																			MOES_ALARM_VOLUME_DP,
																			moes_alarm_volume, 
																			CUSTOM_CMD_SYNC))

						updateLabel_P(moes_alarm_info_label, PSTR("New volume sent"));
					else
					
						updateLabel_P(moes_alarm_info_label, device_query_failed_str);
				} else
					
					updateLabel_P(moes_alarm_info_label, PSTR("Enter volume to send."));
			} break;

			case GUI_CB_SEND_DURATION_FLAG: { //send moes alarm duration

				int32_t moes_alarm_duration = 
					ESPUI.getControl(moes_alarm_duration_number)->value.toInt();
				
				if (moes_alarm_duration >= 0) {

					current_Tuya_payload_label = moes_alarm_Tuya_payload_label;
					
					if (sendTuyaRequestCmdValue32(&zbGateway, 
																				&device, 
																				MOES_ALARM_DURATION_DP,
																				moes_alarm_duration, 
																				CUSTOM_CMD_SYNC))

					updateLabel_P(moes_alarm_info_label, PSTR("New alarm duration sent"));
					else
						
						updateLabel_P(moes_alarm_info_label, device_query_failed_str);
				} else
					
					updateLabel_P(moes_alarm_info_label, PSTR("Enter alarm duration to send."));
			} break; 
		}
	}
}

void TuyaDeviceSelectorCallback(Control *sender, int type) {

	int16_t device_slot = sender->value.toInt();

	if (device_slot >=0) {

		enableTuyaDevicesControls(true);

		sprintf_P(general_purpose_gui_buffer,
							PSTR("<b><i>Manufacturer name</i></b> %s "
							"<b>| <i>model ID</b></i> %s"), 
							Z2S_getZbDeviceManufacturerName(device_slot),
							Z2S_getZbDeviceModelName(device_slot));

		updateLabel_P(Tuya_devices_tab_controls_table[Tuya_device_info_label], 
									general_purpose_gui_buffer);
		
		rebuildTuyaDevicesDatapointsList(device_slot);

		
	} else {

		//rebuildTuyaDevicesDatapointsList(0xFF);
		enableTuyaDevicesControls(false);	
	}
}

void addLocalActionHandlerCallback(Control *sender, int type, void *param) {

	if (type == B_UP) {

		uint8_t logic_operator = PIN_LOGIC_OPERATOR_NONE;

		switch ((uint32_t)param) {


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
		}

		if (addZ2SDeviceLocalActionHandler(LOCAL_CHANNEL_TYPE_ACTION_HANDLER, 
																			 SUPLA_CHANNELFNC_NONE,
																			 logic_operator)) {
			delay(200);																
			rebuildChannelsSelector(true);
			buildActionsChannelSelectors(true);
		}
	}
}

void addLocalVirtualRelayCallback(Control *sender, int type) {

	if (type == B_UP) {

		gui_command = 66;
	}
}

void addLocalVirtualBinaryCallback(Control *sender, int type) {

	if (type == B_UP) {

		gui_command = 67;
	}
}

void addLocalRemoteRelayCallback(Control *sender, int type) {

	if (type == B_UP) {

		gui_command = 68;
	}
}

void addLocalRemoteThermometerCallback(Control *sender, int type) {

	if (type == B_UP) {

		gui_command = 69;
	}
}

void addLocalVirtualHvacCallback(Control *sender, int type) {

	if (type == B_UP) {

		gui_command = 70;
	}
}

void GUI_onTuyaCustomClusterReceive(
	uint8_t command_id, uint16_t payload_size, uint8_t * payload_data){

	if (current_Tuya_payload_label == 0)
		return;
	
	char payload_buffer[255] = {};

	payload_buffer[0] = '{';
	
	char hex_chunk[3];
	
	hex_chunk[3] = 0x00;

	Tuya_dp_zcl_payload_t *Tuya_dp_zcl_payload = (Tuya_dp_zcl_payload_t *)payload_data;

	
	//uint16_t	tsn_id = (((uint16_t)(*payload_data)) << 16) + (*(payload_data + 1));

	//uint8_t dp_id = *(payload_data + 2);

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
	sprintf(payload_buffer + ((payload_size * 3) + 2), 
					"<br>datapoint id 	 = %u(0x%02X)"
					"<br>datapoint value = %u(0x%08X)",
					Tuya_dp_zcl_payload->dp_id,
					Tuya_dp_zcl_payload->dp_id,
					dp_value,
					dp_value);
	updateLabel_P(current_Tuya_payload_label, payload_buffer);	
}

void GUI_onLastBindingFailure(bool binding_failed) {
	
	sprintf_P(
		general_purpose_gui_buffer, 
		PSTR("Unknown model %s::%s, no binding is possible"), 
		zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
    zbGateway.getQueryBasicClusterData()->zcl_model_name);

	updateLabel_P(zigbee_last_binding_result_label, general_purpose_gui_buffer);
}

void GUI_onZigbeeOpenNetwork(bool is_network_open) {

	if (pairing_mode_switcher < 0xFFFF)
		ESPUI.updateNumber(pairing_mode_switcher, is_network_open ? 1 : 0);
}
#endif //USE_SUPLA_WEB_SERVER
