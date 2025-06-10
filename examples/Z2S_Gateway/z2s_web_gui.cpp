#include "z2s_web_gui.h"

#include <supla/storage/littlefs_config.h>

ESPAsyncHTTPUpdateServer updateServer;

//UI handles
uint16_t wifi_ssid_text, wifi_pass_text, Supla_server, Supla_email;
uint16_t save_button, save_label;
volatile bool data_ready = false;

void enterWifiDetailsCallback(Control *sender, int type);
void textCallback(Control *sender, int type);


void Z2S_buildWebGUI() {

  auto wifitab = ESPUI.addControl(Tab, "", "WiFi Credentials");
	wifi_ssid_text = ESPUI.addControl(Text, "SSID", "", Alizarin, wifitab, textCallback);
	//Note that adding a "Max" control to a text control sets the max length
	ESPUI.addControl(Max, "", "32", None, wifi_ssid_text);
	wifi_pass_text = ESPUI.addControl(Text, "Password", "", Alizarin, wifitab, textCallback);
	ESPUI.addControl(Max, "", "64", None, wifi_pass_text);
	Supla_server = ESPUI.addControl(Text, "Supla server", "", Alizarin, wifitab, textCallback);
	ESPUI.addControl(Max, "", "64", None, Supla_server);
	Supla_email = ESPUI.addControl(Text, "Supla email", "", Alizarin, wifitab, textCallback);
	ESPUI.addControl(Max, "", "64", None, Supla_email);
	save_button = ESPUI.addControl(Button, "Save", "Save", Peterriver, wifitab, enterWifiDetailsCallback);
	save_label = ESPUI.addControl(Label, "Status", "Missing data...", Wetasphalt, save_button);
}

void Z2S_startWebGUI() {

  ESPUI.begin("Z2S GUI INTERFACE");

}

void Z2S_startUpdateServer() {
  updateServer.setup(ESPUI.WebServer(), "admin", "pass");
}

void enterWifiDetailsCallback(Control *sender, int type) {
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
			//SuplaDevice.softRestart();
		}
	}
}

void textCallback(Control *sender, int type) {
	Serial.println(type);
	Serial.println(ESPUI.getControl(wifi_ssid_text)->value);
		Serial.println(ESPUI.getControl(wifi_pass_text)->value);
		Serial.println(ESPUI.getControl(Supla_server)->value);
		Serial.println(ESPUI.getControl(Supla_email)->value);
		
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