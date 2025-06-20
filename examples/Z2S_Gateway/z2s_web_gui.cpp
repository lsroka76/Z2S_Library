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
uint16_t zb_device_info_label, zb_device_address_label, zb_channel_info_label;
uint16_t channel_status_label;
uint16_t deviceselector, channelselector;
uint16_t swbuildlabel, rssilabel;
uint16_t remove_channel_button;

volatile bool data_ready = false;


char save_flag		= 'S';
char restart_flag = 'R';

char swbuild_flag = 'S';
char rssi_flag    = 'R';

const static char device_query_failed_str[] = "Device data query failed - try to wake it up first!";

char zigbee_devices_labels[Z2S_ZBDEVICESMAXCOUNT][11] = {};
char zigbee_channels_labels[Z2S_CHANNELMAXCOUNT][13] = {};

void buildGatewayTabGUI();
void buildCredentialsGUI();
void buildDevicesTabGUI();
void buildChannelsTabGUI();

void enterWifiDetailsCallback(Control *sender, int type, void *param);
void textCallback(Control *sender, int type);
void generalCallback(Control *sender, int type);
void deviceselectorCallback(Control *sender, int type);
void channelselectorCallback(Control *sender, int type);
void removeChannelCallback(Control *sender, int type);
void getZigbeeDeviceQueryCallback(Control *sender, int type, void *param);

void fillGatewayGeneralnformation(char *buf);
void fillMemoryUptimeInformation(char *buf);

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

void buildDevicesTabGUI() {

	auto devicestab = ESPUI.addControl(Tab, "", "Zigbee devices");
	
	deviceselector = ESPUI.addControl(Select, "Devices", "None", Emerald, devicestab, deviceselectorCallback);
	ESPUI.addControl(Option, "Select Zigbee device...", String(-1), None, deviceselector);

	for (uint8_t devices_counter = 0; devices_counter < Z2S_ZBDEVICESMAXCOUNT; devices_counter++) 
    if (z2s_zb_devices_table[devices_counter].record_id > 0) {

			sprintf(zigbee_devices_labels[devices_counter], "Device #%02d", devices_counter);
			ESPUI.addControl(Option, zigbee_devices_labels[devices_counter], String(devices_counter), None, deviceselector);
		}

	zb_device_info_label = ESPUI.addControl(Label, "Device info", "...", Emerald, devicestab); 
	ESPUI.setElementStyle(zb_device_info_label, "text-align: justify; font-size: 4 px; font-style: normal; font-weight: normal;");
	
	auto getswbuild_button = ESPUI.addControl(Button, "Software Build ID", "Read", Emerald, devicestab, getZigbeeDeviceQueryCallback, &swbuild_flag);
	swbuildlabel = ESPUI.addControl(Label, "Software Build ID", "...", Emerald, getswbuild_button);

	auto getrssi_button = ESPUI.addControl(Button, "Device RSSI", "Read", Emerald, devicestab, getZigbeeDeviceQueryCallback, &rssi_flag);
	rssilabel = ESPUI.addControl(Label, "Device Rssi", "...", Emerald, getrssi_button);
}

void buildChannelsTabGUI() {

	auto channelstab = ESPUI.addControl(Tab, "", "Zigbee channels");

	channelselector = ESPUI.addControl(Select, "Channels", "None", Emerald, channelstab, channelselectorCallback);
	ESPUI.addControl(Option, "Select Zigbee channel...", String(-1), None, channelselector);
	
	for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) 
    if (z2s_devices_table[devices_counter].valid_record) {
      
			sprintf(zigbee_channels_labels[devices_counter], "Channel #%02d", devices_counter);
			ESPUI.addControl(Option, zigbee_channels_labels[devices_counter], String(devices_counter), None, channelselector);
		}
	zb_channel_info_label = ESPUI.addControl(Label, "Channel info", "...", Emerald, channelstab);
	ESPUI.setElementStyle(zb_channel_info_label, "text-align: justify; font-size: 4 px; font-style: normal; font-weight: normal;");

	remove_channel_button = ESPUI.addControl(Button, "Remove channel", "Remove channel", Emerald, channelstab, removeChannelCallback);
	channel_status_label = ESPUI.addControl(Label, "Status", "...", Emerald, remove_channel_button);
}

void Z2S_buildWebGUI() {
 
	buildGatewayTabGUI();
	buildCredentialsGUI();
	buildDevicesTabGUI();
	buildChannelsTabGUI();
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

void deviceselectorCallback(Control *sender, int type) {

	if (sender->value.toInt() == -1) return;
	
	char device_info_str[256] = {};
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
					"IEEE address %s<br>"
					"Short address 0x%04X<br><br>"
					"Power source %d<br><br>"
					"Battery percentage %u<br>"
					"Last seen (ms) %lu<br>", 
					z2s_zb_devices_table[device_slot].manufacturer_name,
					z2s_zb_devices_table[device_slot].model_name,
					ieee_addr_str,
					z2s_zb_devices_table[device_slot].short_addr,
					z2s_zb_devices_table[device_slot].power_source,
					battery_percentage,
					z2s_zb_devices_table[device_slot].last_seen_ms);

	ESPUI.updateLabel(zb_device_info_label, device_info_str);
	//ESPUI.updateLabel(zb_device_address_label, z2s_zb_devices_table[sender->value.toInt()].);
}

void channelselectorCallback(Control *sender, int type) {
	
	if (sender->value.toInt() == -1) return;
	
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
					"type: %u, function: %u, sub id: %d, data flags: %lu <br>"
					"user data(1): %lu, user data(2): %lu<br>"
					"user data(3): %lu, user data(4): %lu<br>"
					"data counter: %llu<br>"
					"keep alive: %lu, timeout: %lu, refresh: %lu<br>"
					"ZB device: %u",
					z2s_devices_table[channel_slot].Supla_channel_name,
					ieee_addr_str,
					z2s_devices_table[channel_slot].short_addr,
					z2s_devices_table[channel_slot].endpoint,
        	z2s_devices_table[channel_slot].cluster_id,
        	z2s_devices_table[channel_slot].model_id,
        	z2s_devices_table[channel_slot].Supla_channel,
        	z2s_devices_table[channel_slot].Supla_secondary_channel,
        	z2s_devices_table[channel_slot].Supla_channel_type,
        	z2s_devices_table[channel_slot].Supla_channel_func,
        	z2s_devices_table[channel_slot].sub_id,
        	z2s_devices_table[channel_slot].user_data_flags,
        	z2s_devices_table[channel_slot].user_data_1,
        	z2s_devices_table[channel_slot].user_data_2,
        	z2s_devices_table[channel_slot].user_data_3,
        	z2s_devices_table[channel_slot].user_data_4,
					z2s_devices_table[channel_slot].data_counter,
        	z2s_devices_table[channel_slot].keep_alive_secs,
        	z2s_devices_table[channel_slot].timeout_secs,
        	z2s_devices_table[channel_slot].refresh_secs,
        	z2s_devices_table[channel_slot].ZB_device_id);


	ESPUI.updateLabel(zb_channel_info_label, channel_info_str);
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