#include <ZigbeeGateway.h>

#include "z2s_web_gui.h"
#include "z2s_devices_database.h"
#include "z2s_devices_table.h"

#include "z2s_version_info.h"

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
uint16_t remove_device_button, remove_device_and_channels_button, remove_all_devices_button;

uint16_t channelselector;
uint16_t channel_status_label, zb_channel_info_label;
uint16_t keepalive_number, timeout_number, refresh_number;
uint16_t remove_channel_button, remove_all_channels_button;

volatile bool data_ready = false;
volatile bool device_controls_enabled = false;
volatile bool channel_controls_enabled = false;

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

const String disabledstyle = "background-color: #bbb; border-bottom: #999 3px solid;";

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

void fillGatewayGeneralnformation(char *buf) {

	if (buf) {
		
		char guid_buf[128] = {};

		generateHexString(Supla::RegisterDevice::getGUID(), guid_buf, SUPLA_GUID_SIZE);

		snprintf(buf, 1024, "Supla firmware: %s<br><br>Supla GUID:%s<br><br>Z2S Gateway version: %s<br><br>", 
						Supla::RegisterDevice::getSoftVer(), guid_buf, Z2S_VERSION);
	
		log_i("Device information %s", buf);

	}
}

void fillMemoryUptimeInformation(char *buf) {

	if (buf) {
		time_t local_time_info;
		time(&local_time_info);

		snprintf(buf, 1024, "Flash chip real size: %u kB<br>Free Sketch Space: %u kB<br>"
						"Free Heap: %u kB<br>Minimal Free Heap: %u kB<br>HeapSize: %u kB<br>"
						"MaxAllocHeap: %u kB<br><br>Czas lokalny: %sSupla uptime: %lu s", 
						ESP.getFlashChipSize(), ESP.getFreeSketchSpace(), ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getHeapSize(),
						ESP.getMaxAllocHeap(), ctime(&local_time_info),  SuplaDevice.uptime.getUptime());

		log_i("Memory & uptime information %s", buf);
	}
}

void buildGatewayTabGUI() {

	char buf[1024] = {};

	auto gatewaytab = ESPUI.addControl(Tab, "", "Gateway");
	
	fillGatewayGeneralnformation(buf);

	ESPUI.addControl(Separator, "General information", "", None, gatewaytab);
	gateway_general_info = ESPUI.addControl(Label, "Device information", buf, Emerald, gatewaytab);

	fillMemoryUptimeInformation(buf);
	
	ESPUI.addControl(Separator, "Status", "", None, gatewaytab);
	gateway_memory_info = ESPUI.addControl(Label, "Memory & Uptime", buf, Emerald, gatewaytab);
	ESPUI.setElementStyle(gateway_memory_info, "text-align: justify; font-size: 4 px; font-style: normal; font-weight: normal;");
}

void buildCredentialsGUI() {

	char buf[512] = {};

	auto wifitab = ESPUI.addControl(Tab, "", "WiFi & Supla credentials");

	wifi_ssid_text = ESPUI.addControl(Text, "SSID", "", Emerald, wifitab, textCallback);
	ESPUI.addControl(Max, "", "32", None, wifi_ssid_text);

	wifi_pass_text = ESPUI.addControl(Text, "Password", "", Emerald, wifitab, textCallback);
	ESPUI.addControl(Max, "", "64", None, wifi_pass_text);
	ESPUI.setInputType(wifi_pass_text, "password");

	Supla_server = ESPUI.addControl(Text, "Supla server", "", Emerald, wifitab, textCallback);
	ESPUI.addControl(Max, "", "64", None, Supla_server);

	Supla_email = ESPUI.addControl(Text, "Supla email", "", Emerald, wifitab, textCallback);
	ESPUI.addControl(Max, "", "64", None, Supla_email);

	save_button = ESPUI.addControl(Button, "Save", "Save", Emerald, wifitab, enterWifiDetailsCallback,(void*) &save_flag);
	auto save_n_restart_button = ESPUI.addControl(Button, "Save & Restart", "Save & Restart", Emerald, save_button, enterWifiDetailsCallback, &restart_flag);
	save_label = ESPUI.addControl(Label, "Status", "Missing data...", Wetasphalt, save_button);

	auto cfg = Supla::Storage::ConfigInstance();
  
	if (cfg) {

  	memset(buf, 0, sizeof(buf));
  	if (cfg->getWiFiSSID(buf) && strlen(buf) > 0)
			ESPUI.updateText(wifi_ssid_text, buf);
		memset(buf, 0, sizeof(buf));
		if (cfg->getSuplaServer(buf) && strlen(buf) > 0)
			ESPUI.updateText(Supla_server, buf);
		memset(buf, 0, sizeof(buf));
		if (cfg->getEmail(buf) && strlen(buf) > 0)
			ESPUI.updateText(Supla_email, buf);
	}			
}

void buildZigbeeTabGUI() {

	auto zigbeetab = ESPUI.addControl(Tab, "", "Zigbee settings");

	//ESPUI.addControl(Separator, "Zigbee", "", None, zigbeetab);
	auto open_network_button = ESPUI.addControl(Button, "Pairing mode", "Pairing mode", Emerald, zigbeetab, generalZigbeeCallback,(void*) &pairing_flag);

	zigbee_tx_power_text = ESPUI.addControl(Text, "Zigbee TX power", zigbee_tx_power_text_str, Emerald, zigbeetab, generalCallback);

	zigbee_get_tx_power_button = ESPUI.addControl(Button, "Read", "Read", Emerald, zigbee_tx_power_text, generalZigbeeCallback,(void*) &get_tx_flag);
	zigbee_set_tx_power_button = ESPUI.addControl(Button, "Update", "Update", Emerald, zigbee_tx_power_text, generalZigbeeCallback,(void*) &set_tx_flag);

	zigbee_primary_channel_text = ESPUI.addControl(Text, "Zigbee primary channel", zigbee_primary_channel_text_str, Emerald, zigbeetab, generalCallback);

	zigbee_get_primary_channel_button = ESPUI.addControl(Button, "Read", "Read", Emerald, zigbee_primary_channel_text, generalZigbeeCallback,(void*) &get_pc_flag);
	zigbee_set_primary_channel_button = ESPUI.addControl(Button, "Update", "Update", Emerald, zigbee_primary_channel_text, generalZigbeeCallback,(void*) &set_pc_flag);

	ESPUI.addControl(Separator, "Zigbee stack factory reset", "", None, zigbeetab);
	factory_reset_switch = ESPUI.addControl(Switcher, "Enable Zigbee stack factory reset", "0", Alizarin, zigbeetab, switchCallback);
	factory_reset_label = ESPUI.addControl(Label, "", factory_reset_disabled_str, Wetasphalt, factory_reset_switch);
	factory_reset_button = ESPUI.addControl(Button, "FACTORY RESET!", "FACTORY RESET!", Alizarin, zigbeetab, generalZigbeeCallback,(void*) &factory_flag);
}

void buildDevicesTabGUI() {

	auto devicestab = ESPUI.addControl(Tab, "", "Zigbee devices");
	
	deviceselector = ESPUI.addControl(Select, "Devices", String(-1), Emerald, devicestab, deviceselectorCallback);
	ESPUI.addControl(Option, "Select Zigbee device...", String(-1), None, deviceselector);

	for (uint8_t devices_counter = 0; devices_counter < Z2S_ZBDEVICESMAXCOUNT; devices_counter++) 
    if (z2s_zb_devices_table[devices_counter].record_id > 0) {

			sprintf(zigbee_devices_labels[devices_counter], "Device #%02d", devices_counter);
			ESPUI.addControl(Option, zigbee_devices_labels[devices_counter], String(devices_counter), None, deviceselector);
		}

	zb_device_info_label = ESPUI.addControl(Label, "Device info", "...", Emerald, devicestab); 
	ESPUI.setElementStyle(zb_device_info_label, "text-align: justify; font-size: 4 px; font-style: normal; font-weight: normal;");
	
	getswbuild_button = ESPUI.addControl(Button, "Software Build ID", "Read", Emerald, devicestab, getZigbeeDeviceQueryCallback, &swbuild_flag);
	swbuildlabel = ESPUI.addControl(Label, "Software Build ID", "...", Emerald, getswbuild_button);

	getrssi_button = ESPUI.addControl(Button, "Device RSSI", "Read", Emerald, devicestab, getZigbeeDeviceQueryCallback, &rssi_flag);
	rssilabel = ESPUI.addControl(Label, "Device Rssi", "...", Emerald, getrssi_button);

	ESPUI.addControl(Separator, "", "", None, devicestab);

	remove_device_button = ESPUI.addControl(Button, "Remove Zigbee device(s)", "Remove device!", Alizarin, devicestab, removeDeviceCallback, &single_flag);
	remove_device_and_channels_button = 
		ESPUI.addControl(Button, "", "Remove device with channels!", Alizarin, remove_device_button, removeDeviceCallback, &with_channels_flag);
	remove_all_devices_button = 
		ESPUI.addControl(Button, "", "Remove all devices!", Alizarin, remove_device_button, removeDeviceCallback, &all_flag);
	device_status_label = ESPUI.addControl(Label, "Status", "...", Alizarin, remove_device_button);
}

void buildChannelsTabGUI() {

	auto channelstab = ESPUI.addControl(Tab, "", "Zigbee channels");

	channelselector = ESPUI.addControl(Select, "Channels", String(-1), Emerald, channelstab, channelselectorCallback);
	ESPUI.addControl(Option, "Select Zigbee channel...", String(-1), None, channelselector);
	
	for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) 
    if (z2s_devices_table[devices_counter].valid_record) {
      
			sprintf(zigbee_channels_labels[devices_counter], "Channel #%02d", devices_counter);
			ESPUI.addControl(Option, zigbee_channels_labels[devices_counter], String(devices_counter), None, channelselector);
		}
	zb_channel_info_label = ESPUI.addControl(Label, "Channel info", "...", Emerald, channelstab);
	ESPUI.setElementStyle(zb_channel_info_label, "text-align: justify; font-size: 4 px; font-style: normal; font-weight: normal;");

	String clearLabelStyle = "background-color: unset; width: 100%;";

	keepalive_number = ESPUI.addControl(Slider, "Timings panel", "0", Emerald, channelstab, timingsCallback, &keepalive_flag);
	ESPUI.addControl(Min, "", "0", None, keepalive_number);
	ESPUI.addControl(Max, "", "360", None, keepalive_number);
	ESPUI.setElementStyle(ESPUI.addControl(Label, "", "keepalive (s)", None, keepalive_number), clearLabelStyle);

	timeout_number = ESPUI.addControl(Slider, "", "0", Emerald, keepalive_number, timingsCallback, &timeout_flag);
	ESPUI.addControl(Min, "", "0", None, timeout_number);
	ESPUI.addControl(Max, "", "360", None, timeout_number);
	ESPUI.setElementStyle(ESPUI.addControl(Label, "", "timeout (s)", None, keepalive_number), clearLabelStyle);

	refresh_number = ESPUI.addControl(Slider, "", "0", Emerald, keepalive_number, timingsCallback, &refresh_flag);
	ESPUI.addControl(Min, "", "0", None, refresh_number);
	ESPUI.addControl(Max, "", "360", None, refresh_number);
	ESPUI.setElementStyle(ESPUI.addControl(Label, "", "refresh/autoset (s)", None, keepalive_number), clearLabelStyle);

	ESPUI.addControl(Separator, "", "", None, channelstab);

	remove_channel_button = ESPUI.addControl(Button, "Remove Supla channel(s)", "Remove channel", Alizarin, channelstab, removeChannelCallback);
	remove_all_channels_button = ESPUI.addControl(Button, "", "Remove all channels!", Alizarin, remove_channel_button, removeAllChannelsCallback);
	channel_status_label = ESPUI.addControl(Label, "Status", "...", Alizarin, remove_channel_button);
}

void Z2S_buildWebGUI() {
 
	ESPUI.sliderContinuous = true;

	buildGatewayTabGUI();
	buildCredentialsGUI();
	buildZigbeeTabGUI();
	buildDevicesTabGUI();
	buildChannelsTabGUI();
	disableDeviceControls();
	disableChannelControls();
}

void Z2S_startWebGUIConfig() {

	char buf[1024] = {};
	
	fillGatewayGeneralnformation(buf);

	ESPUI.addControl(Separator, "General information", "", None);
	gateway_general_info = ESPUI.addControl(Label, "Device information", buf, Emerald);

	fillMemoryUptimeInformation(buf);
	
	ESPUI.addControl(Separator, "Status", "", None);
	gateway_memory_info = ESPUI.addControl(Label, "Memory & Uptime", buf, Emerald);
	//ESPUI.setElementStyle(gateway_memory_info, "text-align: left; font-size: 6 px; font-style: normal; font-weight: normal;");

  ESPUI.addControl(Separator, "WiFi & Supla credentials", "", None);
	wifi_ssid_text = ESPUI.addControl(Text, "SSID", "", Emerald, Control::noParent, textCallback);
	ESPUI.addControl(Max, "", "32", None, wifi_ssid_text);
	wifi_pass_text = ESPUI.addControl(Text, "Password", "", Emerald, Control::noParent, textCallback);
	ESPUI.addControl(Max, "", "64", None, wifi_pass_text);
	ESPUI.setInputType(wifi_pass_text, "password");
	Supla_server = ESPUI.addControl(Text, "Supla server", "", Emerald, Control::noParent, textCallback);
	ESPUI.addControl(Max, "", "64", None, Supla_server);
	Supla_email = ESPUI.addControl(Text, "Supla email", "", Emerald, Control::noParent, textCallback);
	ESPUI.addControl(Max, "", "64", None, Supla_email);
	save_button = ESPUI.addControl(Button, "Save", "Save", Emerald, Control::noParent, enterWifiDetailsCallback,(void*) &save_flag);
	auto save_n_restart_button = ESPUI.addControl(Button, "Save & Restart", "Save & Restart", Emerald, save_button, enterWifiDetailsCallback, &restart_flag);
	save_label = ESPUI.addControl(Label, "Status", "Missing data...", Emerald, save_button);

	auto cfg = Supla::Storage::ConfigInstance();
  
	if (cfg) {

  	memset(buf, 0, sizeof(buf));
  	if (cfg->getWiFiSSID(buf) && strlen(buf) > 0)
			ESPUI.updateText(wifi_ssid_text, buf);
		memset(buf, 0, sizeof(buf));
		if (cfg->getSuplaServer(buf) && strlen(buf) > 0)
			ESPUI.updateText(Supla_server, buf);
		memset(buf, 0, sizeof(buf));
		if (cfg->getEmail(buf) && strlen(buf) > 0)
			ESPUI.updateText(Supla_email, buf);
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

	char buf[1024] = {};

	fillMemoryUptimeInformation(buf);

	ESPUI.updateLabel(gateway_memory_info, buf);
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
	Serial.print(sender->id);
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


void deviceselectorCallback(Control *sender, int type) {

	if ((!isNumber(sender->value)) || (sender->value.toInt() < 0) || (sender->value.toInt() >= Z2S_ZBDEVICESMAXCOUNT)) {

		if (device_controls_enabled)
			disableDeviceControls();
		return;
	}
	
	if (!device_controls_enabled)
		enableDeviceControls();

	char device_info_str[512] = {};
	char ieee_addr_str[24] 		= {};

	uint8_t device_slot = sender->value.toInt();

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

	sprintf(device_info_str,"Manufacturer name: %s<br>"
					"Model ID: %s<br><br>"
					"Z2S model: %s(%ul)"
					"IEEE address %s<br>"
					"Short address 0x%04X<br><br>"
					"Power source %d<br><br>"
					"Battery percentage %u<br>"
					"Last seen (ms) %lu<br>"
					"Gateway unit last seen (ms)  %lu<br>"
					"Gateway unit last RSSI %d", 
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

	ESPUI.updateLabel(zb_device_info_label, device_info_str);
	//ESPUI.updateLabel(zb_device_address_label, z2s_zb_devices_table[sender->value.toInt()].);
}

void disableChannelControls() {

	ESPUI.updateLabel(zb_channel_info_label, "");

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


void channelselectorCallback(Control *sender, int type) {
	
	if ((!isNumber(sender->value)) || (sender->value.toInt() < 0) || (sender->value.toInt() >= Z2S_CHANNELMAXCOUNT)) {

		if (channel_controls_enabled)
			disableChannelControls();
		return;
	}
	
	if (!channel_controls_enabled)
		enableChannelControls();

	char channel_info_str[1024] = {};
	char ieee_addr_str[24] 		= {};

	uint8_t channel_slot = sender->value.toInt();

  sprintf(ieee_addr_str, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", 
					z2s_devices_table[channel_slot].ieee_addr[7],
					z2s_devices_table[channel_slot].ieee_addr[6], 
					z2s_devices_table[channel_slot].ieee_addr[5], 
					z2s_devices_table[channel_slot].ieee_addr[4], 
          z2s_devices_table[channel_slot].ieee_addr[3],
					z2s_devices_table[channel_slot].ieee_addr[2],
					z2s_devices_table[channel_slot].ieee_addr[1], 
					z2s_devices_table[channel_slot].ieee_addr[0]);

	sprintf(channel_info_str,
					"Channel name: %s<br><br>"
					"IEEE address: %s<br>"
					"Short address: 0x%04X, endpoint: 0x%02X, cluster: 0x%04X<br><br>"
					"Model id: 0x%04X, channel: #%u, secondary channel: #%u<br>"
					"Type: %s<br>"
					"Function: %s<br>"
					"Sub id: %d, data flags: %lu <br>"
					"User data(1) = %lu, user data(2) = %lu<br>"
					"user data(3) = %lu, user data(4) = %lu<br>"
					"data counter = %llu<br>"
					//"keepalive = %lu, timeout = %lu, refresh = %lu<br>"
					"ZB device: %u",
					z2s_devices_table[channel_slot].Supla_channel_name,
					ieee_addr_str,
					z2s_devices_table[channel_slot].short_addr,
					z2s_devices_table[channel_slot].endpoint,
        	z2s_devices_table[channel_slot].cluster_id,
        	z2s_devices_table[channel_slot].model_id,
        	z2s_devices_table[channel_slot].Supla_channel,
        	z2s_devices_table[channel_slot].Supla_secondary_channel,
        	getSuplaChannelTypeName(z2s_devices_table[channel_slot].Supla_channel_type),
        	getSuplaChannelFuncName(z2s_devices_table[channel_slot].Supla_channel_type, z2s_devices_table[channel_slot].Supla_channel_func),
        	z2s_devices_table[channel_slot].sub_id,
        	z2s_devices_table[channel_slot].user_data_flags,
        	z2s_devices_table[channel_slot].user_data_1,
        	z2s_devices_table[channel_slot].user_data_2,
        	z2s_devices_table[channel_slot].user_data_3,
        	z2s_devices_table[channel_slot].user_data_4,
					z2s_devices_table[channel_slot].data_counter,
        	//z2s_devices_table[channel_slot].keep_alive_secs,
        	//z2s_devices_table[channel_slot].timeout_secs,
        	//z2s_devices_table[channel_slot].refresh_secs,
        	z2s_devices_table[channel_slot].ZB_device_id);


	ESPUI.updateLabel(zb_channel_info_label, channel_info_str);
	ESPUI.updateNumber(keepalive_number, z2s_devices_table[channel_slot].keep_alive_secs);
	ESPUI.updateNumber(timeout_number, z2s_devices_table[channel_slot].timeout_secs);
	ESPUI.updateNumber(refresh_number, z2s_devices_table[channel_slot].refresh_secs);
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

		char status_line[128];
		bool restart_required = false;

		switch (*(char *)param) {

			case 'S' : {		
				
				z2s_zb_devices_table[device_slot].record_id = 0;
				if (Z2S_saveZBDevicesTable()) {
					sprintf(status_line, "Device #%02u removed! Restarting...", device_slot);
					restart_required = true;
				} else
					sprintf(status_line, "Device #%02u removal failed! Error saving ZB devices table!", device_slot);
			} break;

			case 'C' : {	

				if (Z2S_removeZBDeviceWithAllChannels(device_slot)) {

					sprintf(status_line, "Device #%02u and it's all channels removed! Restarting...", device_slot);
					restart_required = true;
				} else
					sprintf(status_line, "Device #%02u or some of it's channels removal failed! Error saving one of devices table!", device_slot);
			} break;

			case 'A': {

				if (Z2S_clearZBDevicesTable()) {
					sprintf(status_line, "All devices removed! Restarting...");
					restart_required = true;
				} else
					sprintf(status_line, "Devices removal failed! Error saving ZB devices table!");
			} break;
		}
    ESPUI.updateLabel(device_status_label, status_line);
    if (restart_required) 
			SuplaDevice.scheduleSoftRestart(1000);
	}
}

void removeChannelCallback(Control *sender, int type) {

	if ((type == B_UP) && (ESPUI.getControl(channelselector)->value.toInt() >= 0)) {

		uint8_t channel_slot = ESPUI.getControl(channelselector)->value.toInt();

		z2s_devices_table[channel_slot].valid_record = false;
      
		if (Z2S_saveDevicesTable()) {
			char status_line[128];
			sprintf(status_line, "Channel # %02u removed. Restarting...", channel_slot);
      ESPUI.updateLabel(channel_status_label, status_line);
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