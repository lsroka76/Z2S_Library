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
uint16_t zb_device_info_label, zb_device_address_label;
uint16_t deviceselector, channelselector;
uint16_t swbuildlabel;
volatile bool data_ready = false;

char save_flag		= 'S';
char restart_flag = 'R';

char zigbee_devices_labels[Z2S_ZBDEVICESMAXCOUNT][11] = {};
char zigbee_channels_labels[Z2S_CHANNELMAXCOUNT][13] = {};

void enterWifiDetailsCallback(Control *sender, int type, void *param);
void textCallback(Control *sender, int type);
void generalCallback(Control *sender, int type);
void deviceselectorCallback(Control *sender, int type);
void channelselectorCallback(Control *sender, int type);
void getswbuildCallback(Control *sender, int type);

void fillGatewayGeneralnformation(char *buf);
void fillMemoryUptimeInformation(char *buf);

void fillGatewayGeneralnformation(char *buf) {

	if (buf) {
		
		char guid_buf[128] = {};

		generateHexString(Supla::RegisterDevice::getGUID(), guid_buf, SUPLA_GUID_SIZE);

		snprintf(buf, 1024, "Supla firmware: %s<br>Supla GUID:%s<br>Z2S Gateway version: %s<br>", 
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


void Z2S_buildWebGUI() {

	char buf[1024] = {};

	auto gatewaytab = ESPUI.addControl(Tab, "", "Gateway");
	
	fillGatewayGeneralnformation(buf);

	ESPUI.addControl(Separator, "General information", "", None, gatewaytab);
	gateway_general_info = ESPUI.addControl(Label, "Device information", buf, Emerald, gatewaytab);

	fillMemoryUptimeInformation(buf);
	
	ESPUI.addControl(Separator, "Status", "", None, gatewaytab);
	gateway_memory_info = ESPUI.addControl(Label, "Memory & Uptime", buf, Emerald, gatewaytab);
	//ESPUI.setElementStyle(gateway_memory_info, "text-align: left; font-size: 6 px; font-style: normal; font-weight: normal;");

  auto wifitab = ESPUI.addControl(Tab, "", "WiFi & Supla credentials");
	wifi_ssid_text = ESPUI.addControl(Text, "SSID", "", Emerald, wifitab, textCallback);
	//Note that adding a "Max" control to a text control sets the max length
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

	//----------------------devices-------------------------------------------//

	auto devicestab = ESPUI.addControl(Tab, "", "Zigbee devices");
	
	deviceselector = ESPUI.addControl(Select, "Devices", "None", Emerald, devicestab, deviceselectorCallback);
	ESPUI.addControl(Option, "Select Zigbee device...", String(-1), None, deviceselector);

	for (uint8_t devices_counter = 0; devices_counter < Z2S_ZBDEVICESMAXCOUNT; devices_counter++) 
    if (z2s_zb_devices_table[devices_counter].record_id > 0) {

			sprintf(zigbee_devices_labels[devices_counter], "Device #%d", devices_counter);
			ESPUI.addControl(Option, zigbee_devices_labels[devices_counter], String(devices_counter), None, deviceselector);
		}
	zb_device_info_label = ESPUI.addControl(Label, "Device info", "...", Emerald, devicestab); 
	
	auto getswbuild_button = ESPUI.addControl(Button, "GetSwBuild", "GetSwBuild", Emerald, devicestab, getswbuildCallback);
	swbuildlabel = ESPUI.addControl(Label, "Software Build ID", "...", Emerald, devicestab);

	auto channelstab = ESPUI.addControl(Tab, "", "Zigbee channels");

	channelselector = ESPUI.addControl(Select, "Channels", "None", Emerald, channelstab, channelselectorCallback);
	ESPUI.addControl(Option, "Select Zigbee channel...", String(-1), None, channelselector);
	
	for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) 
    if (z2s_devices_table[devices_counter].valid_record) {
      
			sprintf(zigbee_channels_labels[devices_counter], "Channel #%d", devices_counter);
			ESPUI.addControl(Option, zigbee_channels_labels[devices_counter], String(devices_counter), None, channelselector);
		}
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

void Z2S_startUpdateServer() {
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

void deviceselectorCallback(Control *sender, int type) {

	if (sender->value.toInt() == -1) return;
	
	char device_info_str[256] = {};
	char ieee_addr_str[24] 		= {};

  sprintf(ieee_addr_str, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", 
					z2s_zb_devices_table[sender->value.toInt()].ieee_addr[7],
					z2s_zb_devices_table[sender->value.toInt()].ieee_addr[6], 
					z2s_zb_devices_table[sender->value.toInt()].ieee_addr[5], 
					z2s_zb_devices_table[sender->value.toInt()].ieee_addr[4], 
          z2s_zb_devices_table[sender->value.toInt()].ieee_addr[3],
					z2s_zb_devices_table[sender->value.toInt()].ieee_addr[2],
					z2s_zb_devices_table[sender->value.toInt()].ieee_addr[1], 
					z2s_zb_devices_table[sender->value.toInt()].ieee_addr[0]);

	uint8_t battery_percentage = z2s_zb_devices_table[sender->value.toInt()].battery_percentage >= 0x80 ? 
												  		 z2s_zb_devices_table[sender->value.toInt()].battery_percentage - 0x80 : 0xFF;

	sprintf(device_info_str,"Manufacturer name: %s<br>"
					"Model ID: %s<br>"
					"IEEE address %s<br>"
					"Short address 0x%04X<br>"
					"Power source %d<br>"
					"Battery percentage %u<br>"
					"Last seen (ms) %lu<br>", 
					z2s_zb_devices_table[sender->value.toInt()].manufacturer_name,
					z2s_zb_devices_table[sender->value.toInt()].model_name,
					ieee_addr_str,
					z2s_zb_devices_table[sender->value.toInt()].short_addr,
					z2s_zb_devices_table[sender->value.toInt()].power_source,
					battery_percentage,
					z2s_zb_devices_table[sender->value.toInt()].last_seen_ms);


	ESPUI.updateLabel(zb_device_info_label, device_info_str);
	//ESPUI.updateLabel(zb_device_address_label, z2s_zb_devices_table[sender->value.toInt()].);
	Serial.print("CB: id(");
	Serial.print(sender->id);
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
}

void channelselectorCallback(Control *sender, int type) {
	Serial.print("CB: id(");
	Serial.print(sender->id);
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
}

void getswbuildCallback(Control *sender, int type){
	if (ESPUI.getControl(deviceselector)->value.toInt() >= 0) {

		zbg_device_params_t device;
		log_i("deviceselector value %u", ESPUI.getControl(deviceselector)->value.toInt());
    device.endpoint = 1; //z2s_devices_table[channel_number_slot].endpoint;
    device.cluster_id = 0; //z2s_devices_table[channel_number_slot].cluster_id;
    memcpy(&device.ieee_addr, z2s_zb_devices_table[ESPUI.getControl(deviceselector)->value.toInt()].ieee_addr,8);
    device.short_addr = z2s_zb_devices_table[ESPUI.getControl(deviceselector)->value.toInt()].short_addr;

		if (zbGateway.zbQueryDeviceBasicCluster(&device, true, ESP_ZB_ZCL_ATTR_BASIC_SW_BUILD_ID))
			if (strlen(zbGateway.getQueryBasicClusterData()->software_build_ID) > 0)
				ESPUI.updateLabel(swbuildlabel, zbGateway.getQueryBasicClusterData()->software_build_ID);
	}
}
