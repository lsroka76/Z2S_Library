#ifndef USE_SUPLA_WEB_SERVER

#include <ZigbeeGateway.h>

#include "z2s_web_gui.h"
#include "z2s_devices_database.h"
#include "z2s_devices_table.h"

//#include "z2s_version_info.h"
#define Z2S_VERSION "0.8.48-01/07/2025"

#include <SuplaDevice.h>
#include <supla/storage/littlefs_config.h>
#include <supla/storage/storage.h>

#include <supla/device/register_device.h>

ESPAsyncHTTPUpdateServer updateServer;

extern ZigbeeGateway zbGateway;

//UI handles
uint16_t gateway_general_info;
uint16_t gateway_memory_info;

uint16_t wifi_ssid_text, wifi_pass_text, Supla_server, Supla_email;
uint16_t save_button, save_label;

uint16_t pairing_mode_button, pairing_mode_label;
uint16_t zigbee_tx_power_text, zigbee_get_tx_power_button, zigbee_set_tx_power_button;
uint16_t zigbee_primary_channel_text, zigbee_get_primary_channel_button, zigbee_set_primary_channel_button;
uint16_t factory_reset_switch, factory_reset_button, factory_reset_label;

uint16_t deviceselector;
uint16_t zb_device_info_label, zb_device_address_label, device_status_label;

uint16_t getswbuild_button, getrssi_button;
uint16_t swbuildlabel, rssilabel;

uint16_t device_cluster_selector;
uint16_t device_attribute_number;
uint16_t device_read_attribute_label;
uint16_t device_attribute_type_selector;
uint16_t device_attribute_size_number;
uint16_t device_attribute_value_number;
uint16_t remove_device_button, remove_device_and_channels_button, remove_all_devices_button;

uint16_t channelselector;
uint16_t channel_status_label, zb_channel_info_label, zb_channel_info_label_2;
uint16_t keepalive_number, timeout_number, refresh_number;
uint16_t remove_channel_button, remove_all_channels_button;

volatile bool data_ready = false;
volatile bool device_controls_enabled = false;
volatile bool channel_controls_enabled = false;

#define GUI_UPDATE_CMD_NONE 								0x00
#define GUI_UPDATE_CMD_DEVICE_INFO_LABEL		0x10
#define GUI_UPDATE_CMD_CHANNEL_INFO_LABEL_1	0x20
#define GUI_UPDATE_CMD_CHANNEL_INFO_LABEL_2	0x21

volatile bool GUI_update_required = false;
volatile uint8_t GUI_update_cmd = GUI_UPDATE_CMD_NONE;

char save_flag		= 'S';
char restart_flag = 'R';

char swbuild_flag = 'S';
char rssi_flag    = 'R';

char pairing_flag = 'P';
char factory_flag = 'F';

char get_tx_flag = 'T';
char set_tx_flag = 'X';
char get_pc_flag = 'R';
char set_pc_flag = 'S';

char keepalive_flag = 'K';
char timeout_flag = 'T';
char refresh_flag = 'R';

char single_flag = 'S';
char with_channels_flag = 'C';
char all_flag = 'A';

const static char device_query_failed_str[] = "Device data query failed - try to wake it up first!";

const static char factory_reset_enabled_str[] = "Zigbee stack factory reset enabled";
const static char factory_reset_disabled_str[] = "Zigbee stack factory reset disabled";
const static char zigbee_tx_power_text_str[] = "Press Read to get current value or enter value between -24 and 20 and press Update";
const static char zigbee_primary_channel_text_str[] = "Press Read to get current value or enter value between 11 and 26 and press Update";

static char general_purpose_gui_buffer[1024] = {};

typedef struct zigbee_cluster_s {
	const char* zigbee_cluster_name;
	const uint16_t zigbee_cluster_id;
} zigbee_cluster_t;

static const zigbee_cluster_t zigbee_clusters[] = 
	{{.zigbee_cluster_name = "BASIC",.zigbee_cluster_id = 0x0000},
	 {.zigbee_cluster_name = "POWER CFG",.zigbee_cluster_id = 0x0001},
	 {.zigbee_cluster_name = "IDENTIFY",.zigbee_cluster_id = 0x0003},
	 {.zigbee_cluster_name = "GROUPS",.zigbee_cluster_id = 0x0004},
	 {.zigbee_cluster_name = "SCENES",.zigbee_cluster_id = 0x0005},
	 {.zigbee_cluster_name = "ON/OFF",.zigbee_cluster_id = 0x0006},
	 {.zigbee_cluster_name = "LEVEL CONTROL",.zigbee_cluster_id = 0x0008},
	 {.zigbee_cluster_name = "TIME",.zigbee_cluster_id = 0x000A},
	 {.zigbee_cluster_name = "POLL",.zigbee_cluster_id = 0x0020},																								
	 {.zigbee_cluster_name = "ILLUMINANCE",.zigbee_cluster_id = 0x0400},
	 {.zigbee_cluster_name = "TEMPERATURE",.zigbee_cluster_id = 0x0402},
	 {.zigbee_cluster_name = "PRESSURE",.zigbee_cluster_id = 0x0403},
	 {.zigbee_cluster_name = "FLOW",.zigbee_cluster_id = 0x0404},
	 {.zigbee_cluster_name = "HUMIDITY",.zigbee_cluster_id = 0x0405},
	 {.zigbee_cluster_name = "OCCUPANCY",.zigbee_cluster_id = 0x0406},
	 {.zigbee_cluster_name = "ELECTRICAL MEASUREMENT",.zigbee_cluster_id = 0x0B04},
	 {.zigbee_cluster_name = "COLOR CONTROL",.zigbee_cluster_id = 0x0300},
	 {.zigbee_cluster_name = "WINDOW COVERING",.zigbee_cluster_id = 0x0102},
	 {.zigbee_cluster_name = "IAS ZONE",.zigbee_cluster_id = 0x0500},
	 {.zigbee_cluster_name = "IAS WD",.zigbee_cluster_id = 0x0502},
	 {.zigbee_cluster_name = "TUYA E000",.zigbee_cluster_id = 0xE000},
	 {.zigbee_cluster_name = "TUYA E001",.zigbee_cluster_id = 0xE001},
	 {.zigbee_cluster_name = "TUYA EF00",.zigbee_cluster_id = 0xEF00},
	 {.zigbee_cluster_name = "SONOFF FC11",.zigbee_cluster_id = 0xFC11},};

const String disabledstyle = "background-color: #bbb; border-bottom: #999 3px solid;";
const String clearLabelStyle = "background-color: unset; width: 100%;";

char zigbee_devices_labels[Z2S_ZBDEVICESMAXCOUNT][11] = {};
char zigbee_channels_labels[Z2S_CHANNELMAXCOUNT][13] = {};

bool isNumber(String str);

const char* getSuplaChannelTypeName(int32_t channelType);
const char* getSuplaChannelFuncName(int32_t channelType, int32_t channelFunc);
const char* getZ2SDeviceModelName(uint32_t modelID);

void buildGatewayTabGUI();
void buildCredentialsGUI();
void buildZigbeeTabGUI();
void buildDevicesTabGUI();
void buildChannelsTabGUI();

void updateChannelInfoLabel(uint8_t label_number);
void updateDeviceInfoLabel();

void enterWifiDetailsCallback(Control *sender, int type, void *param);
void textCallback(Control *sender, int type);
void generalCallback(Control *sender, int type);
void switchCallback(Control *sender, int type);
void deviceselectorCallback(Control *sender, int type);
void disableDeviceControls();
void enableDeviceControls();
void removeDeviceCallback(Control *sender, int type, void *param);
void channelselectorCallback(Control *sender, int type);
void disableChannelControls();
void enableChannelControls();
void timingsCallback(Control *sender, int type, void *param);
void removeChannelCallback(Control *sender, int type);
void removeAllChannelsCallback(Control *sender, int type);
void getZigbeeDeviceQueryCallback(Control *sender, int type, void *param);
void generalZigbeeCallback(Control *sender, int type, void *param);

void fillGatewayGeneralnformation(char *buf);
void fillMemoryUptimeInformation(char *buf);

bool isNumber(String str) {

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
	switch (channelFunc) {

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
	}
}

const char* getZ2SDeviceModelName(uint32_t modelID)  {

	switch (modelID) {

		case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR:
		case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1:
			return "Temperature and humidity sensor(0x402, 0x405)";
		case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL:
			return "Temperature and humidity sensor(0x20, 0x402, 0x405)";
		case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR:
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
		case Z2S_DEVICE_DESC_ILLUZONE_SENSOR:
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

void fillGatewayGeneralnformation(char *buf) {

	if (buf) {
		
		char guid_buf[128] = {};

		generateHexString(Supla::RegisterDevice::getGUID(), guid_buf, SUPLA_GUID_SIZE);

		snprintf(buf, 1024, "<b><i>Supla firmware:</i></b> %s<br><br><b><i>Supla GUID:</i></b> %s<br><br><b><i>Z2S Gateway version:</i></b> %s<br><br>", 
						Supla::RegisterDevice::getSoftVer(), guid_buf, Z2S_VERSION);
	
		log_i("Device information %s", buf);

	}
}

void fillMemoryUptimeInformation(char *buf) {

	if (buf) {
		time_t local_time_info;
		time(&local_time_info);

		snprintf(buf, 1024, "<b><i>Flash chip real size:</b></i> %u kB <b>| <i>Free Sketch Space:</b></i> %u kB<br>"
						"<b><i>Free Heap:</b></i> %u kB <b>| <i>Minimal Free Heap:</b></i> %u kB <b>| <i>"
						"HeapSize:</b></i> %u kB <b>| <i>MaxAllocHeap:</b></i> %u kB<br><br>"
						"<b><i>Local time:</i></b> %s<b><i>Supla uptime:</i></b> %lu s", 
						ESP.getFlashChipSize(), ESP.getFreeSketchSpace(), ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getHeapSize(),
						ESP.getMaxAllocHeap(), ctime(&local_time_info),  SuplaDevice.uptime.getUptime());

		log_i("Memory & uptime information %s", buf);
	}
}

void buildGatewayTabGUI() {

	//char buf[1024] = {};

	auto gatewaytab = ESPUI.addControl(Control::Type::Tab, "", "Gateway", Control::Color::Emerald, Control::noParent, generalCallback);
	
	fillGatewayGeneralnformation(general_purpose_gui_buffer);

	ESPUI.addControl(Control::Type::Separator, "General information", "", Control::Color::None, gatewaytab);
	gateway_general_info = ESPUI.addControl(Control::Type::Label, "Device information", general_purpose_gui_buffer, 
																					Control::Color::Emerald, gatewaytab);
	ESPUI.setElementStyle(gateway_general_info, "color:black;text-align: justify; font-family:tahoma; font-size: 4 px; font-style: normal; font-weight: normal;");
	ESPUI.setPanelWide(gateway_general_info, true);

	fillMemoryUptimeInformation(general_purpose_gui_buffer);
	
	ESPUI.addControl(Control::Type::Separator, "Status", "", Control::Color::None, gatewaytab);
	gateway_memory_info = ESPUI.addControl(Control::Type::Label, "Memory & Uptime", general_purpose_gui_buffer, 
																				 Control::Color::Emerald, gatewaytab);
	//ESPUI.setElementStyle(gateway_memory_info, "text-align: justify; font-size: 4 px; font-style: normal; font-weight: normal;");
	ESPUI.setElementStyle(gateway_memory_info, "color:black;text-align: justify; font-family:tahoma; font-size: 4 px; font-style: normal; font-weight: normal;");
	ESPUI.setPanelWide(gateway_memory_info, true);

}

void buildCredentialsGUI() {

	//char buf[512] = {};

	auto wifitab = ESPUI.addControl(Control::Type::Tab, "", "WiFi & Supla credentials");

	wifi_ssid_text = ESPUI.addControl(Control::Type::Text, "SSID", "", Control::Color::Emerald, wifitab, textCallback);
	ESPUI.addControl(Control::Type::Max, "", "32", Control::Color::None, wifi_ssid_text);

	wifi_pass_text = ESPUI.addControl(Control::Type::Text, "Password", "", Control::Color::Emerald, wifitab, textCallback);
	ESPUI.addControl(Control::Type::Max, "", "64", Control::Color::None, wifi_pass_text);
	ESPUI.setInputType(wifi_pass_text, "password");

	Supla_server = ESPUI.addControl(Control::Type::Text, "Supla server", "", Control::Color::Emerald, wifitab, textCallback);
	ESPUI.addControl(Control::Type::Max, "", "64", Control::Color::None, Supla_server);

	Supla_email = ESPUI.addControl(Control::Type::Text, "Supla email", "", Control::Color::Emerald, wifitab, textCallback);
	ESPUI.addControl(Control::Type::Max, "", "64", Control::Color::None, Supla_email);

	save_button = ESPUI.addControl(Control::Type::Button, "Save", "Save", Control::Color::Emerald, wifitab, enterWifiDetailsCallback,(void*) &save_flag);
	auto save_n_restart_button = ESPUI.addControl(Control::Type::Button, "Save & Restart", "Save & Restart", Control::Color::Emerald, save_button, enterWifiDetailsCallback, &restart_flag);
	save_label = ESPUI.addControl(Control::Type::Label, "Status", "Missing data...", Control::Color::Wetasphalt, save_button);

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
	}			
}

void buildZigbeeTabGUI() {

	auto zigbeetab = ESPUI.addControl(Control::Type::Tab, "", "Zigbee settings", Control::Color::Emerald, Control::noParent, generalCallback);

	//ESPUI.addControl(Control::Type::Separator, "Zigbee", "", Control::Color::None, zigbeetab);
	auto open_network_button = ESPUI.addControl(Control::Type::Button, "Pairing mode", "Pairing mode", Control::Color::Emerald, zigbeetab, generalZigbeeCallback,(void*) &pairing_flag);

	zigbee_tx_power_text = ESPUI.addControl(Control::Type::Text, "Zigbee TX power", zigbee_tx_power_text_str, Control::Color::Emerald, zigbeetab, generalCallback);

	zigbee_get_tx_power_button = ESPUI.addControl(Control::Type::Button, "Read", "Read", Control::Color::Emerald, zigbee_tx_power_text, generalZigbeeCallback,(void*) &get_tx_flag);
	zigbee_set_tx_power_button = ESPUI.addControl(Control::Type::Button, "Update", "Update", Control::Color::Emerald, zigbee_tx_power_text, generalZigbeeCallback,(void*) &set_tx_flag);

	zigbee_primary_channel_text = ESPUI.addControl(Control::Type::Text, "Zigbee primary channel", zigbee_primary_channel_text_str, Control::Color::Emerald, zigbeetab, generalCallback);

	zigbee_get_primary_channel_button = ESPUI.addControl(Control::Type::Button, "Read", "Read", Control::Color::Emerald, zigbee_primary_channel_text, generalZigbeeCallback,(void*) &get_pc_flag);
	zigbee_set_primary_channel_button = ESPUI.addControl(Control::Type::Button, "Update", "Update", Control::Color::Emerald, zigbee_primary_channel_text, generalZigbeeCallback,(void*) &set_pc_flag);

	ESPUI.addControl(Control::Type::Separator, "Zigbee stack factory reset", "", Control::Color::None, zigbeetab);
	factory_reset_switch = ESPUI.addControl(Control::Type::Switcher, "Enable Zigbee stack factory reset", "0", Control::Color::Alizarin, zigbeetab, switchCallback);
	factory_reset_label = ESPUI.addControl(Control::Type::Label, "", factory_reset_disabled_str, Control::Color::Wetasphalt, factory_reset_switch);
	factory_reset_button = ESPUI.addControl(Control::Type::Button, "FACTORY RESET!", "FACTORY RESET!", Control::Color::Alizarin, zigbeetab, generalZigbeeCallback,(void*) &factory_flag);
}

void buildDevicesTabGUI() {

	auto devicestab = ESPUI.addControl(Control::Type::Tab, "", "Zigbee devices");
	
	deviceselector = ESPUI.addControl(Control::Type::Select, "Devices", String(-1), Control::Color::Emerald, devicestab, deviceselectorCallback);
	ESPUI.addControl(Control::Type::Option, "Select Zigbee device...", String(-1), Control::Color::None, deviceselector);

	for (uint8_t devices_counter = 0; devices_counter < Z2S_ZBDEVICESMAXCOUNT; devices_counter++) 
    if (z2s_zb_devices_table[devices_counter].record_id > 0) {

			sprintf(zigbee_devices_labels[devices_counter], "Device #%02d", devices_counter);
			ESPUI.addControl(Control::Type::Option, zigbee_devices_labels[devices_counter], String(devices_counter), Control::Color::None, deviceselector);
		}

	ESPUI.setPanelWide(deviceselector, true);

	zb_device_info_label = ESPUI.addControl(Control::Type::Label, "Device info", "...", Control::Color::Emerald, devicestab); 
	ESPUI.setElementStyle(zb_device_info_label, "color:black;text-align: justify; font-family:tahoma; font-size: 4 px; font-style: normal; font-weight: normal;");
	
	ESPUI.setPanelWide(zb_device_info_label, true);

	getswbuild_button = ESPUI.addControl(Control::Type::Button, "Software Build ID", "Read", Control::Color::Emerald, devicestab, getZigbeeDeviceQueryCallback, &swbuild_flag);
	swbuildlabel = ESPUI.addControl(Control::Type::Label, "Software Build ID", "...", Control::Color::Emerald, getswbuild_button);

	getrssi_button = ESPUI.addControl(Control::Type::Button, "Device RSSI", "Read", Control::Color::Emerald, devicestab, getZigbeeDeviceQueryCallback, &rssi_flag);
	rssilabel = ESPUI.addControl(Control::Type::Label, "Device Rssi", "...", Control::Color::Emerald, getrssi_button);

	device_cluster_selector = ESPUI.addControl(Control::Type::Select, "Clusters&Attributes", String(-1), Control::Color::Emerald, devicestab, generalCallback);
	device_attribute_number = ESPUI.addControl(Control::Type::Number, "Attribute", "0", Control::Color::Emerald, device_cluster_selector, generalCallback);
	ESPUI.addControl(Control::Type::Min, "", "0", Control::Color::None, device_attribute_number);
	ESPUI.addControl(Control::Type::Max, "", "65536", Control::Color::None, device_attribute_number);
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, "", "Attribute number", Control::Color::None, device_cluster_selector), clearLabelStyle);
	ESPUI.addControl(Control::Type::Option, "Select Zigbee cluster...", String(-1), Control::Color::None, device_cluster_selector);

	uint32_t zigbee_clusters_count = sizeof(zigbee_clusters)/sizeof(zigbee_cluster_t);

	for (uint8_t clusters_counter = 0; clusters_counter < zigbee_clusters_count; clusters_counter++) {
  
		ESPUI.addControl(Control::Type::Option, zigbee_clusters[clusters_counter].zigbee_cluster_name, 
		String(zigbee_clusters[clusters_counter].zigbee_cluster_id), Control::Color::None, device_cluster_selector);
	}

	/*uint16_t device_read_attribute_label;
uint16_t device_attribute_type_selector;
uint16_t device_attribute_size_number;
uint16_t device_attribute_value_number;*/

	ESPUI.addControl(Control::Type::Separator, "", "", Control::Color::None, devicestab);

	remove_device_button = ESPUI.addControl(Control::Type::Button, "Remove Zigbee device(s)", "Remove device!", Control::Color::Alizarin, devicestab, removeDeviceCallback, &single_flag);
	remove_device_and_channels_button = 
		ESPUI.addControl(Control::Type::Button, "", "Remove device with channels!", Control::Color::Alizarin, remove_device_button, removeDeviceCallback, &with_channels_flag);
	remove_all_devices_button = 
		ESPUI.addControl(Control::Type::Button, "", "Remove all devices!", Control::Color::Alizarin, remove_device_button, removeDeviceCallback, &all_flag);
	device_status_label = ESPUI.addControl(Control::Type::Label, "Status", "...", Control::Color::Alizarin, remove_device_button);
}

void buildChannelsTabGUI() {

	auto channelstab = ESPUI.addControl(Control::Type::Tab, "", "Zigbee channels");

	channelselector = ESPUI.addControl(Control::Type::Select, "Channels", String(-1), Control::Color::Emerald, channelstab, channelselectorCallback);
	ESPUI.addControl(Control::Type::Option, "Select Zigbee channel...", String(-1), Control::Color::None, channelselector);
	
	ESPUI.setPanelWide(channelselector, true);

	for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) 
    if (z2s_devices_table[devices_counter].valid_record) {
      
			sprintf(zigbee_channels_labels[devices_counter], "Channel #%02d", devices_counter);
			ESPUI.addControl(Control::Type::Option, zigbee_channels_labels[devices_counter], String(devices_counter), Control::Color::None, channelselector);
		}
	zb_channel_info_label = ESPUI.addControl(Control::Type::Label, "Channel info", "...", Control::Color::Emerald, channelstab);
	zb_channel_info_label_2 = ESPUI.addControl(Control::Type::Label, "", "...", Control::Color::Emerald, zb_channel_info_label);
	ESPUI.setElementStyle(zb_channel_info_label, "color:black;text-align: justify; font-family:tahoma; font-size: 4 px; font-style: normal; font-weight: normal;");
	ESPUI.setElementStyle(zb_channel_info_label_2, "color:black;text-align: justify; font-family:tahoma; font-size: 4 px; font-style: normal; font-weight: normal;");
	ESPUI.setPanelWide(zb_channel_info_label, true);


	keepalive_number = ESPUI.addControl(Control::Type::Slider, "Timings panel", "0", Control::Color::Emerald, channelstab, timingsCallback, &keepalive_flag);
	ESPUI.addControl(Control::Type::Min, "", "0", Control::Color::None, keepalive_number);
	ESPUI.addControl(Control::Type::Max, "", "360", Control::Color::None, keepalive_number);
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, "", "keepalive (s)", Control::Color::None, keepalive_number), clearLabelStyle);

	timeout_number = ESPUI.addControl(Control::Type::Slider, "", "0", Control::Color::Emerald, keepalive_number, timingsCallback, &timeout_flag);
	ESPUI.addControl(Control::Type::Min, "", "0", Control::Color::None, timeout_number);
	ESPUI.addControl(Control::Type::Max, "", "360", Control::Color::None, timeout_number);
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, "", "timeout (s)", Control::Color::None, keepalive_number), clearLabelStyle);

	refresh_number = ESPUI.addControl(Control::Type::Slider, "", "0", Control::Color::Emerald, keepalive_number, timingsCallback, &refresh_flag);
	ESPUI.addControl(Control::Type::Min, "", "0", Control::Color::None, refresh_number);
	ESPUI.addControl(Control::Type::Max, "", "360", Control::Color::None, refresh_number);
	ESPUI.setElementStyle(ESPUI.addControl(Control::Type::Label, "", "refresh/autoset (s)", Control::Color::None, keepalive_number), clearLabelStyle);

	ESPUI.addControl(Control::Type::Separator, "", "", Control::Color::None, channelstab);

	remove_channel_button = ESPUI.addControl(Control::Type::Button, "Remove Supla channel(s)", "Remove channel", Control::Color::Alizarin, channelstab, removeChannelCallback);
	remove_all_channels_button = ESPUI.addControl(Control::Type::Button, "", "Remove all channels!", Control::Color::Alizarin, remove_channel_button, removeAllChannelsCallback);
	channel_status_label = ESPUI.addControl(Control::Type::Label, "Status", "...", Control::Color::Alizarin, remove_channel_button);
}

void Z2S_buildWebGUI() {
 
	ESPUI.sliderContinuous = true;
	ESPUI.setVerbosity(Verbosity::VerboseJSON);

	buildGatewayTabGUI();
	buildCredentialsGUI();
	buildZigbeeTabGUI();
	buildDevicesTabGUI();
	buildChannelsTabGUI();
	disableDeviceControls();
	disableChannelControls();
}

void Z2S_startWebGUIConfig() {

	//char buf[1024] = {};
	
	fillGatewayGeneralnformation(general_purpose_gui_buffer);

	ESPUI.addControl(Control::Type::Separator, "General information", "", Control::Color::None);
	gateway_general_info = ESPUI.addControl(Control::Type::Label, "Device information", general_purpose_gui_buffer, Control::Color::Emerald);

	fillMemoryUptimeInformation(general_purpose_gui_buffer);
	
	ESPUI.addControl(Control::Type::Separator, "Status", "", Control::Color::None);
	gateway_memory_info = ESPUI.addControl(Control::Type::Label, "Memory & Uptime", general_purpose_gui_buffer, Control::Color::Emerald);
	//ESPUI.setElementStyle(gateway_memory_info, "text-align: left; font-size: 6 px; font-style: normal; font-weight: normal;");

  ESPUI.addControl(Control::Type::Separator, "WiFi & Supla credentials", "", Control::Color::None);
	wifi_ssid_text = ESPUI.addControl(Control::Type::Text, "SSID", "", Control::Color::Emerald, Control::noParent, textCallback);
	ESPUI.addControl(Control::Type::Max, "", "32", Control::Color::None, wifi_ssid_text);
	wifi_pass_text = ESPUI.addControl(Control::Type::Text, "Password", "", Control::Color::Emerald, Control::noParent, textCallback);
	ESPUI.addControl(Control::Type::Max, "", "64", Control::Color::None, wifi_pass_text);
	ESPUI.setInputType(wifi_pass_text, "password");
	Supla_server = ESPUI.addControl(Control::Type::Text, "Supla server", "", Control::Color::Emerald, Control::noParent, textCallback);
	ESPUI.addControl(Control::Type::Max, "", "64", Control::Color::None, Supla_server);
	Supla_email = ESPUI.addControl(Control::Type::Text, "Supla email", "", Control::Color::Emerald, Control::noParent, textCallback);
	ESPUI.addControl(Control::Type::Max, "", "64", Control::Color::None, Supla_email);
	save_button = ESPUI.addControl(Control::Type::Button, "Save", "Save", Control::Color::Emerald, Control::noParent, enterWifiDetailsCallback,(void*) &save_flag);
	auto save_n_restart_button = ESPUI.addControl(Control::Type::Button, "Save & Restart", "Save & Restart", Control::Color::Emerald, save_button, enterWifiDetailsCallback, &restart_flag);
	save_label = ESPUI.addControl(Control::Type::Label, "Status", "Missing data...", Control::Color::Emerald, save_button);

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
	}

	ESPUI.begin("ZIGBEE <=> SUPLA CONFIG PAGE");
}

void Z2S_startWebGUI() {

  ESPUI.begin("ZIGBEE <=> SUPLA CONTROL PANEL");

}

void Z2S_stopWebGUI() {
  
	if (ESPUI.WebServer())
		ESPUI.WebServer()->end();
}

void Z2S_startUpdateServer() {

	if (ESPUI.WebServer())
  	updateServer.setup(ESPUI.WebServer(), "admin", "pass");
}

void Z2S_updateWebGUI() {

	//char buf[1024] = {};

	fillMemoryUptimeInformation(general_purpose_gui_buffer);

	ESPUI.updateLabel(gateway_memory_info, general_purpose_gui_buffer);
}

void Z2S_loopWebGUI() {

	if (GUI_update_required) {

		switch (GUI_update_cmd) {

			case GUI_UPDATE_CMD_CHANNEL_INFO_LABEL_1: {

				updateChannelInfoLabel(1); 
				GUI_update_cmd = GUI_UPDATE_CMD_CHANNEL_INFO_LABEL_2;
			} break;

			case GUI_UPDATE_CMD_CHANNEL_INFO_LABEL_2: {

				updateChannelInfoLabel(2); 
				GUI_update_cmd = GUI_UPDATE_CMD_NONE;
				GUI_update_required = false;
			} break;

			case GUI_UPDATE_CMD_DEVICE_INFO_LABEL: {

				updateDeviceInfoLabel(); 
				GUI_update_cmd = GUI_UPDATE_CMD_NONE;
				GUI_update_required = false;
			} break;
		}
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

			cfg->commit();
			if (*(char *)param == 'R') SuplaDevice.softRestart();
		}
	}
}

void textCallback(Control *sender, int type) {
	Serial.println(type);
	//Serial.println(ESPUI.getControl(wifi_ssid_text)->value);
//		Serial.println(ESPUI.getControl(wifi_pass_text)->value);
	//	Serial.println(ESPUI.getControl(Supla_server)->value);
//		Serial.println(ESPUI.getControl(Supla_email)->value);
		
		if ((ESPUI.getControl(wifi_ssid_text)->value.length() > 0) &&
				(ESPUI.getControl(wifi_pass_text)->value.length() > 0) &&
				(ESPUI.getControl(Supla_server)->value.length() > 0) &&
				(ESPUI.getControl(Supla_email)->value.length() > 0)) {
			ESPUI.updateLabel(save_label, "Data complete. Press Save");
			data_ready = true;
		} else {
			ESPUI.updateLabel(save_label, "Data incomplete!");
			data_ready = false;
		}
}

void generalCallback(Control *sender, int type) {
	Serial.print("CB: id(");
	//Serial.print(sender->id);
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
}

void disableDeviceControls() {

	ESPUI.updateLabel(zb_device_info_label, "...");
	ESPUI.updateLabel(swbuildlabel, "...");
	ESPUI.updateLabel(rssilabel, "...");
	
	ESPUI.setEnabled(getswbuild_button, false);
	ESPUI.setPanelStyle(getswbuild_button, disabledstyle);
	ESPUI.setEnabled(getrssi_button, false);
	ESPUI.setPanelStyle(getrssi_button, disabledstyle);
	ESPUI.setEnabled(remove_device_button, false);
	ESPUI.setPanelStyle(remove_device_button, disabledstyle);
	ESPUI.setEnabled(remove_device_and_channels_button, false);
	ESPUI.setPanelStyle(remove_device_and_channels_button, disabledstyle);
	ESPUI.setEnabled(remove_all_devices_button, false);
	ESPUI.setPanelStyle(remove_all_devices_button, disabledstyle);	

	device_controls_enabled = false;
}

void enableDeviceControls() {

	ESPUI.setEnabled(getswbuild_button, true);
	ESPUI.setPanelStyle(getswbuild_button, ";");
	ESPUI.setEnabled(getrssi_button, true);
	ESPUI.setPanelStyle(getrssi_button, ";");
	ESPUI.setEnabled(remove_device_button, true);
	ESPUI.setPanelStyle(remove_device_button, ";");
	ESPUI.setEnabled(remove_device_and_channels_button, true);
	ESPUI.setPanelStyle(remove_device_and_channels_button, ";");
	ESPUI.setEnabled(remove_all_devices_button, true);
	ESPUI.setPanelStyle(remove_all_devices_button, ";");	

	device_controls_enabled = true;
}

void updateDeviceInfoLabel() {

	//char device_info_str[512] = {};
	char ieee_addr_str[24] 		= {};

	uint8_t device_slot = ESPUI.getControl(deviceselector)->value.toInt();

  sprintf(ieee_addr_str, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", 
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

	sprintf(general_purpose_gui_buffer,"<b><i><style=color:black>Manufacturer name<style=;></i></b> %s "
					"<b>| <i>model ID</b></i> %s <b>| <i>Z2S model</b></i> %s [0x%04X]<br>"
					"<b><i>IEEE address</b></i> %s <b>| <i>Short address</b></i> 0x%04X <b>| <i>Power source</b></i> 0x%02X<br>"
					"<b><i>Battery percentage</b></i> %u <b>| <i>Last seen (ms)</b></i> %lu "
					"<b>| <i>Gateway unit last seen (ms)</b></i>  %lu <b>| <i>Gateway unit last RSSI</b></i> %d", 
					z2s_zb_devices_table[device_slot].manufacturer_name,
					z2s_zb_devices_table[device_slot].model_name,
					getZ2SDeviceModelName(z2s_zb_devices_table[device_slot].desc_id), z2s_zb_devices_table[device_slot].desc_id,
					ieee_addr_str,
					z2s_zb_devices_table[device_slot].short_addr,
					z2s_zb_devices_table[device_slot].power_source,
					battery_percentage,
					z2s_zb_devices_table[device_slot].last_seen_ms,
					zbGateway.getZbgDeviceUnitLastSeenMs(z2s_zb_devices_table[device_slot].short_addr),
					zbGateway.getZbgDeviceUnitLastRssi(z2s_zb_devices_table[device_slot].short_addr));
	log_i("value = %s, length = %u", general_purpose_gui_buffer, strlen(general_purpose_gui_buffer));
	ESPUI.updateLabel(zb_device_info_label, general_purpose_gui_buffer);
}

void deviceselectorCallback(Control *sender, int type) {

	if ((!isNumber(sender->value)) || (sender->value.toInt() < 0) || (sender->value.toInt() >= Z2S_ZBDEVICESMAXCOUNT)) {

		if (device_controls_enabled)
			disableDeviceControls();
		return;
	}
	
	if (!device_controls_enabled)
		enableDeviceControls();

	//GUI_update_required = true;

	//GUI_update_cmd = GUI_UPDATE_CMD_DEVICE_INFO_LABEL;

	updateDeviceInfoLabel();
}

void disableChannelControls() {

	ESPUI.updateLabel(zb_channel_info_label, "...");
	ESPUI.updateLabel(zb_channel_info_label_2, "...");

	const String disabledstyle = "background-color: #bbb; border-bottom: #999 3px solid;";

	ESPUI.setEnabled(keepalive_number, false);
	ESPUI.setPanelStyle(keepalive_number, disabledstyle);
	ESPUI.setEnabled(timeout_number, false);
	ESPUI.setPanelStyle(timeout_number, disabledstyle);
	ESPUI.setEnabled(refresh_number, false);
	ESPUI.setPanelStyle(refresh_number, disabledstyle);
	ESPUI.setEnabled(remove_channel_button, false);
	ESPUI.setPanelStyle(remove_channel_button, disabledstyle);
	ESPUI.setEnabled(remove_all_channels_button, false);
	ESPUI.setPanelStyle(remove_all_channels_button, disabledstyle);	

	channel_controls_enabled = false;
}

void enableChannelControls() {

	//ESPUI.updateLabel(zb_channel_info_label, "");

	ESPUI.setEnabled(keepalive_number, true);
	ESPUI.setPanelStyle(keepalive_number, ";");
	ESPUI.setEnabled(timeout_number, true);
	ESPUI.setPanelStyle(timeout_number, ";");
	ESPUI.setEnabled(refresh_number, true);
	ESPUI.setPanelStyle(refresh_number, ";");
	ESPUI.setEnabled(remove_channel_button, true);
	ESPUI.setPanelStyle(remove_channel_button, ";");
	ESPUI.setEnabled(remove_all_channels_button, true);
	ESPUI.setPanelStyle(remove_all_channels_button, ";");	

	channel_controls_enabled = true;
}

void updateChannelInfoLabel(uint8_t label_number) {

	char ieee_addr_str[24] 		= {};

	uint8_t channel_slot = ESPUI.getControl(channelselector)->value.toInt();
	
  sprintf(ieee_addr_str, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", 
					z2s_devices_table[channel_slot].ieee_addr[7],
					z2s_devices_table[channel_slot].ieee_addr[6], 
					z2s_devices_table[channel_slot].ieee_addr[5], 
					z2s_devices_table[channel_slot].ieee_addr[4], 
          z2s_devices_table[channel_slot].ieee_addr[3],
					z2s_devices_table[channel_slot].ieee_addr[2],
					z2s_devices_table[channel_slot].ieee_addr[1], 
					z2s_devices_table[channel_slot].ieee_addr[0]);
	
	sprintf(general_purpose_gui_buffer,
					"<meta charset=\"UTF-8\"><b><i>Channel name:</i></b> %s<br>"
					"<b><i>IEEE address</i></b> %s <b>| <i>Short address</i></b> 0x%04X <b>| <i>endpoint</i></b> 0x%02X <b>| <i>cluster</i></b> 0x%04X<br>"
					"<b><i>Model id</i></b> %s [0x%04X] <b>| <i>channel</i></b> #%u <b>| <i>secondary channel</i></b> #%u<br>"
					"<b><i>Type</b></i> %s <b>| <i>Function</b></i> %s <b>| <i>Sub id</b></i> %d<br>"
					"<b><i>ZB device</b></i> %s::%s",
					strlen(z2s_devices_table[channel_slot].Supla_channel_name) > 0 ? z2s_devices_table[channel_slot].Supla_channel_name : "---",
					ieee_addr_str,
					z2s_devices_table[channel_slot].short_addr,
					z2s_devices_table[channel_slot].endpoint,
        	z2s_devices_table[channel_slot].cluster_id,
					getZ2SDeviceModelName(z2s_devices_table[channel_slot].model_id),
        	z2s_devices_table[channel_slot].model_id,
        	z2s_devices_table[channel_slot].Supla_channel,
        	z2s_devices_table[channel_slot].Supla_secondary_channel,
					getSuplaChannelTypeName(z2s_devices_table[channel_slot].Supla_channel_type),
        	z2s_devices_table[channel_slot].Supla_channel_func > 0 ? 
						getSuplaChannelFuncName(z2s_devices_table[channel_slot].Supla_channel_type, z2s_devices_table[channel_slot].Supla_channel_func) : "none",
        	z2s_devices_table[channel_slot].sub_id,
					z2s_zb_devices_table[z2s_devices_table[channel_slot].ZB_device_id].manufacturer_name,
					z2s_zb_devices_table[z2s_devices_table[channel_slot].ZB_device_id].model_name);
	
	log_i("Up2HERE!, value = %s, length = %u", general_purpose_gui_buffer, strlen(general_purpose_gui_buffer));
	//if (label_number == 1)
	ESPUI.updateLabel(zb_channel_info_label, general_purpose_gui_buffer);
	//delay(200);
	/*sprintf(channel_info_str_2, 
				"<meta charset=\"UTF-8\"><b><i>Data flags</b></i> %lu <b>| <i>user data(1)</b></i> %lu <b>| <i>user data(2)</b></i> %lu <b>| <i>user data(3)</b></i> %lu" 
					"<b>| <i>user data(4)</b></i> %lu <b>| <i>data counter</b></i> %llu<br>"
					"<b><i>ZB device</b></i> %s::%s",
        	z2s_devices_table[channel_slot].user_data_flags,
        	z2s_devices_table[channel_slot].user_data_1,
        	z2s_devices_table[channel_slot].user_data_2,
        	z2s_devices_table[channel_slot].user_data_3,
        	z2s_devices_table[channel_slot].user_data_4,
					z2s_devices_table[channel_slot].data_counter,
					z2s_zb_devices_table[z2s_devices_table[channel_slot].ZB_device_id].manufacturer_name,
					z2s_zb_devices_table[z2s_devices_table[channel_slot].ZB_device_id].model_name);

	log_i("Up2HERE!, value = %s, length = %u", channel_info_str_2, strlen(channel_info_str_2));
	//ESPUI.updateLabel(zb_channel_info_label, channel_info_str);
	if (label_number == 2)
		ESPUI.updateLabel(zb_channel_info_label_2, channel_info_str_2);*/
	ESPUI.updateNumber(keepalive_number, z2s_devices_table[channel_slot].keep_alive_secs);
	ESPUI.updateNumber(timeout_number, z2s_devices_table[channel_slot].timeout_secs);
	ESPUI.updateNumber(refresh_number, z2s_devices_table[channel_slot].refresh_secs);
}

void channelselectorCallback(Control *sender, int type) {
	
	if (GUI_update_required) return;

	if ((!isNumber(sender->value)) || (sender->value.toInt() < 0) || (sender->value.toInt() >= Z2S_CHANNELMAXCOUNT)) {

		if (channel_controls_enabled)
			disableChannelControls();
		return;
	}
	
	if (!channel_controls_enabled)
		enableChannelControls();

	//GUI_update_required = true;

	//GUI_update_cmd = GUI_UPDATE_CMD_CHANNEL_INFO_LABEL_2;

	updateChannelInfoLabel(1);
}

void getZigbeeDeviceQueryCallback(Control *sender, int type, void *param) {

	if ((type == B_UP) && (ESPUI.getControl(deviceselector)->value.toInt() >= 0)) {

		zbg_device_params_t device;
		log_i("deviceselector value %u", ESPUI.getControl(deviceselector)->value.toInt());
    device.endpoint = 1; //z2s_devices_table[channel_number_slot].endpoint;
    device.cluster_id = 0; //z2s_devices_table[channel_number_slot].cluster_id;
    memcpy(&device.ieee_addr, z2s_zb_devices_table[ESPUI.getControl(deviceselector)->value.toInt()].ieee_addr,8);
    device.short_addr = z2s_zb_devices_table[ESPUI.getControl(deviceselector)->value.toInt()].short_addr;

		switch (*(char *)param) {
			case 'S': {
				if (zbGateway.zbQueryDeviceBasicCluster(&device, true, ESP_ZB_ZCL_ATTR_BASIC_SW_BUILD_ID)) {
					if (strlen(zbGateway.getQueryBasicClusterData()->software_build_ID) > 0) 
						ESPUI.updateLabel(swbuildlabel, zbGateway.getQueryBasicClusterData()->software_build_ID);
				} else
						ESPUI.updateLabel(swbuildlabel, device_query_failed_str);
			} break;
			case 'R': {
				if (zbGateway.sendCustomClusterCmd(&device, 0x0003, 0x0000, ESP_ZB_ZCL_ATTR_TYPE_NULL, 0, nullptr, true))
					ESPUI.updateLabel(rssilabel, String(zbGateway.getZbgDeviceUnitLastRssi(device.short_addr)));
				else
					ESPUI.updateLabel(rssilabel, device_query_failed_str);
			} break;
		}
	}
}

void removeDeviceCallback(Control *sender, int type, void *param) {

	if ((type == B_UP) && (ESPUI.getControl(deviceselector)->value.toInt() >= 0)) {

		uint8_t device_slot = ESPUI.getControl(deviceselector)->value.toInt();

		//char status_line[128];
		bool restart_required = false;

		switch (*(char *)param) {

			case 'S' : {		
				
				z2s_zb_devices_table[device_slot].record_id = 0;
				if (Z2S_saveZBDevicesTable()) {
					sprintf(general_purpose_gui_buffer, "Device #%02u removed! Restarting...", device_slot);
					restart_required = true;
				} else
					sprintf(general_purpose_gui_buffer, "Device #%02u removal failed! Error saving ZB devices table!", device_slot);
			} break;

			case 'C' : {	

				if (Z2S_removeZBDeviceWithAllChannels(device_slot)) {

					sprintf(general_purpose_gui_buffer, "Device #%02u and it's all channels removed! Restarting...", device_slot);
					restart_required = true;
				} else
					sprintf(general_purpose_gui_buffer, "Device #%02u or some of it's channels removal failed! Error saving one of devices table!", device_slot);
			} break;

			case 'A': {

				if (Z2S_clearZBDevicesTable()) {
					sprintf(general_purpose_gui_buffer, "All devices removed! Restarting...");
					restart_required = true;
				} else
					sprintf(general_purpose_gui_buffer, "Devices removal failed! Error saving ZB devices table!");
			} break;
		}
    ESPUI.updateLabel(device_status_label, general_purpose_gui_buffer);
    if (restart_required) 
			SuplaDevice.scheduleSoftRestart(1000);
	}
}

void removeChannelCallback(Control *sender, int type) {

	if ((type == B_UP) && (ESPUI.getControl(channelselector)->value.toInt() >= 0)) {

		uint8_t channel_slot = ESPUI.getControl(channelselector)->value.toInt();

		z2s_devices_table[channel_slot].valid_record = false;
      
		if (Z2S_saveDevicesTable()) {
			//char status_line[128];
			sprintf(general_purpose_gui_buffer, "Channel # %02u removed. Restarting...", channel_slot);
      ESPUI.updateLabel(channel_status_label, general_purpose_gui_buffer);
      SuplaDevice.scheduleSoftRestart(1000);
		}
	}
}

void removeAllChannelsCallback(Control *sender, int type) {

	if (type == B_UP) {
		
		if (Z2S_clearDevicesTable()) {
      ESPUI.updateLabel(channel_status_label, "All channels removed!. Restarting...");
      SuplaDevice.scheduleSoftRestart(1000);
		}
	}
}

void generalZigbeeCallback(Control *sender, int type, void *param){

	if (type == B_UP) {

		switch (*(char *)param) {

			case 'P' : {		
				Zigbee.openNetwork(180); 
			} break;

			case 'F' : {	
				if (ESPUI.getControl(factory_reset_switch)->value.toInt() > 0)
					Zigbee.factoryReset(); 
			} break;

			case 'T' : { 
				int8_t zb_tx_power;
    		esp_zb_get_tx_power(&zb_tx_power);
				log_i("get tx power %d", zb_tx_power);
				ESPUI.updateText(zigbee_tx_power_text, String(zb_tx_power));
			} break;

			case 'X' : {
				if (isNumber(ESPUI.getControl(zigbee_tx_power_text)->value) &&
						(ESPUI.getControl(zigbee_tx_power_text)->value.toInt() >= -24) &&
						(ESPUI.getControl(zigbee_tx_power_text)->value.toInt() <= 20))
							esp_zb_set_tx_power(ESPUI.getControl(zigbee_tx_power_text)->value.toInt());
				else
					ESPUI.updateText(zigbee_tx_power_text, zigbee_tx_power_text_str);

			} break;

			case 'R' : {
				uint32_t zb_primary_channel = esp_zb_get_primary_network_channel_set();
    		for (uint8_t i = 11; i <= 26; i++) {
      		if (zb_primary_channel & (1 << i))
						ESPUI.updateText(zigbee_primary_channel_text, String(i));
    		}		
			} break;

			case 'S' : {
					
					if (isNumber(ESPUI.getControl(zigbee_primary_channel_text)->value) &&
						(ESPUI.getControl(zigbee_primary_channel_text)->value.toInt() >= 11) &&
						(ESPUI.getControl(zigbee_primary_channel_text)->value.toInt() <= 26)) {
							uint32_t zb_primary_channel = ESPUI.getControl(zigbee_primary_channel_text)->value.toInt();

							if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_ZIGBEE_PRIMARY_CHANNEL, (1 << zb_primary_channel))) {
        				ESPUI.updateText(zigbee_primary_channel_text, "New Zigbee primary channel write success! Restarting...");
   				     	Supla::Storage::ConfigInstance()->commit();
        				SuplaDevice.scheduleSoftRestart(1000);
							}
						}
				else
					ESPUI.updateText(zigbee_primary_channel_text, zigbee_primary_channel_text_str);
			} break;
		}
	}
}

void switchCallback(Control *sender, int type) {

	if (sender->value.toInt() == 1)
		ESPUI.updateLabel(factory_reset_label, factory_reset_enabled_str);
	else
		ESPUI.updateLabel(factory_reset_label, factory_reset_disabled_str);
}

void timingsCallback(Control *sender, int type, void *param) {

	if (ESPUI.getControl(channelselector)->value.toInt() >= 0) {

		uint8_t channel_slot = ESPUI.getControl(channelselector)->value.toInt();

		switch (*(char *)param) {

				case 'K' : {		
					updateTimeout(channel_slot,0, 1, sender->value.toInt());
				} break;

				case 'T' : {		
					updateTimeout(channel_slot,0, 2, sender->value.toInt());
				} break;

				case 'R' : {		
					updateTimeout(channel_slot,0, 2, sender->value.toInt());
				} break;	
		}
	}
}

#endif //USE_SUPLA_WEB_SERVER
