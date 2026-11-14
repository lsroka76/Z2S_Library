#include <Arduino.h>
#include "StreamString.h"
#include "ESPAsyncHTTPUpdateServer.h"
#include <Ticker.h>

#ifdef ESP32
    #include <Update.h>
    #include <esp_partition.h>
#elif defined(ESP8266)
    #include <flash_hal.h>
    #include <FS.h>
#else
    #error "This library only supports boards with an ESP8266 or ESP32 processor."
#endif

// Weak hooks overridden by GeminiProtocol.cpp when compiled for ESP32-S3 Host
extern "C" __attribute__((weak)) bool z2s_ncp_ota_begin() { return false; }
extern "C" __attribute__((weak)) bool z2s_ncp_ota_write(const uint8_t *data, size_t len) { return false; }
extern "C" __attribute__((weak)) bool z2s_ncp_ota_end() { return false; }

extern "C" __attribute__((weak)) bool z2s_zb_storage_begin() { return false; }
extern "C" __attribute__((weak)) bool z2s_zb_storage_write(const uint8_t *data, size_t len) { return false; }
extern "C" __attribute__((weak)) bool z2s_zb_storage_end() { return false; }

#ifndef ESPASYNCHTTPUPDATESERVER_SerialOutput
    #define ESPASYNCHTTPUPDATESERVER_SerialOutput Serial
#endif

#ifdef ESPASYNCHTTPUPDATESERVER_DEBUG
    #define Log(...) ESPASYNCHTTPUPDATESERVER_SerialOutput.printf(__VA_ARGS__)
#else
    #define Log(...)
#endif

static const char UPDATE_CENTER_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Z2S Update Center</title>
<style>
:root{--bg:#181b20;--card:#252a32;--text:#f1f5f9;--muted:#cbd5e1;--accent:#3b82f6;--emerald:#10b981;--alizarin:#e74c3c;--border:#374151}
*{box-sizing:border-box}
body{margin:0;font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;background:var(--bg);color:var(--text);font-size:13px;line-height:1.45}
header{background:#111317;border-bottom:1px solid var(--border);padding:12px 16px}
.hdr-inner{max-width:780px;margin:0 auto;display:flex;justify-content:space-between;align-items:center}
header h1{margin:0;font-size:16px;font-weight:700;letter-spacing:.5px;color:#fff}
main{max-width:780px;margin:18px auto;padding:0 16px;display:flex;flex-direction:column;gap:16px}
.panel{background:var(--card);border:1px solid var(--border);border-bottom:3px solid #111317;border-radius:6px;padding:16px;box-shadow:0 3px 6px rgba(0,0,0,.25)}
.panel.highlight{border-color:var(--accent)}
.panel-hdr{font-size:14px;font-weight:700;color:#fff;background:rgba(0,0,0,.22);padding:8px 12px;margin:-16px -16px 14px -16px;border-radius:5px 5px 0 0;border-bottom:1px solid var(--border);display:flex;justify-content:space-between;align-items:center}
.badge{font-size:11px;padding:2px 8px;border-radius:10px;background:#1e293b;color:#93c5fd;border:1px solid #3b82f6}
.desc{color:var(--muted);font-size:12px;margin:0 0 12px 0}
.row{display:flex;flex-wrap:wrap;gap:10px;align-items:center}
input[type=file]{background:#14171c;color:var(--text);border:1px solid #4b5563;border-radius:4px;padding:6px 10px;font-size:12px;flex:1 1 240px}
select{background:#14171c;color:#fff;border:1px solid #4b5563;border-radius:4px;padding:6px 10px;font-size:12.5px;height:35px}
.btn{background:#3b4252;color:#fff;border:1px solid #4c566a;border-bottom:3px solid #2e3440;border-radius:4px;padding:0 16px;height:35px;font-size:12px;font-weight:600;cursor:pointer;transition:all .12s;display:inline-flex;align-items:center;justify-content:center;text-decoration:none}
.btn:hover{background:#434c5e;border-color:#60a5fa}
.btn:disabled{opacity:.5;cursor:not-allowed}
.btn.primary{background:#2563eb;border-color:#3b82f6;border-bottom-color:#1d4ed8}
.btn.primary:hover{background:#3b82f6}
.btn.emerald{background:#059669;border-color:#10b981;border-bottom-color:#065f46}
.btn.emerald:hover{background:#10b981}
.prog-wrap{display:none;background:#14171c;border:1px solid var(--border);border-radius:4px;margin-top:12px;overflow:hidden;height:22px}
.prog-bar{width:0%;height:100%;background:var(--accent);color:#fff;font-size:11px;font-weight:700;display:flex;align-items:center;justify-content:center;transition:width .15s}
.status{margin-top:10px;font-size:12.5px;font-weight:600;min-height:18px}
.status.ok{color:var(--emerald)}
.status.err{color:var(--alizarin)}
</style>
</head>
<body>
<header>
  <div class="hdr-inner">
    <h1>ZIGBEE &lt;=&gt; SUPLA UPDATE CENTER</h1>
    <button class="btn" onclick="location.href='/'">&#8592; Back to Control Panel</button>
  </div>
</header>
<main>
  <section class="panel" id="sec-fw">
    <div class="panel-hdr">
      <span>1. Gateway Firmware, C6 Co-Processor &amp; Filesystem Update</span>
      <span class="badge">ESP32 OTA</span>
    </div>
    <p class="desc">Select a compiled gateway firmware image (<b>.bin</b>), C6 NCP co-processor image (<b>Z2S_C6_NCP*.bin</b>), or filesystem image and click <b>Start Gateway Update</b>. The gateway will reboot automatically after the update.</p>
    <div class="row">
            <select id="fw-type">
        <option value="firmware">Gateway Firmware (.bin)</option>
        <option value="ncp_firmware">ZigBee C6 Co-Processor NCP (.bin)</option>
        <option value="zb_storage">ZigBee Storage Partition (zb_storage .bin 16KB/64KB)</option>
        <option value="filesystem">Gateway Filesystem (.bin)</option>
      </select>
      <input type="file" id="fw-file" accept=".bin,.bin.gz">
      <button class="btn primary" id="fw-btn" onclick="uploadGateway()">Start Gateway Update</button>
    </div>
    <div class="prog-wrap" id="fw-prog-wrap"><div class="prog-bar" id="fw-prog-bar">0%</div></div>
    <div class="status" id="fw-status"></div>
  </section>

  <section class="panel" id="sec-zb">
    <div class="panel-hdr">
      <span>2. ZigBee Device OTA File Upload</span>
      <span class="badge">LittleFS /zigbee_ota</span>
    </div>
    <p class="desc">Upload a ZigBee device OTA firmware file (<b>.ota</b> / <b>.zigbee</b>) to the gateway's local storage. Once uploaded, you can trigger the OTA upgrade in the <b>ZigBee devices</b> tab.</p>
    <div class="row">
      <input type="file" id="zb-file" accept=".ota,.zigbee,.bin,*/*">
      <button class="btn emerald" id="zb-btn" onclick="uploadZigbeeOta()">Upload ZigBee OTA File</button>
    </div>
    <div class="prog-wrap" id="zb-prog-wrap"><div class="prog-bar" id="zb-prog-bar">0%</div></div>
    <div class="status" id="zb-status"></div>
  </section>
</main>
<script>
if(location.hash==="#zigbee"){
  const el=document.getElementById("sec-zb");
  if(el){el.classList.add("highlight");el.scrollIntoView({behavior:"smooth"});}
}

function runUpload(url,fileInputId,fieldName,btnId,wrapId,barId,statusId,onSuccess){
  const fileEl=document.getElementById(fileInputId);
  const btn=document.getElementById(btnId);
  const wrap=document.getElementById(wrapId);
  const bar=document.getElementById(barId);
  const st=document.getElementById(statusId);
  if(!fileEl.files||!fileEl.files[0]){
    st.className="status err";
    st.textContent="Please select a file first!";
    return;
  }
  const fd=new FormData();
  fd.append(fieldName,fileEl.files[0],fileEl.files[0].name);
  btn.disabled=true;
  wrap.style.display="block";
  bar.style.width="0%";
  bar.textContent="0%";
  st.className="status";
  st.textContent="Uploading...";

  const xhr=new XMLHttpRequest();
  xhr.upload.addEventListener("progress",function(e){
    if(e.lengthComputable){
      const p=Math.round((e.loaded/e.total)*100)+"%";
      bar.style.width=p;
      bar.textContent=p;
    }
  });
  xhr.onreadystatechange=function(){
    if(xhr.readyState===4){
      btn.disabled=false;
      if(xhr.status===200&&!xhr.responseText.includes("Update error")&&!xhr.responseText.includes("aborted")){
        st.className="status ok";
        onSuccess(st,xhr.responseText);
      }else{
        st.className="status err";
        st.textContent="Upload failed: "+(xhr.responseText||("HTTP "+xhr.status));
      }
    }
  };
  xhr.open("POST",url,true);
  xhr.send(fd);
}

function uploadGateway(){
  const type=document.getElementById("fw-type").value;
  runUpload("/update?name="+encodeURIComponent(type),"fw-file",type,"fw-btn","fw-prog-wrap","fw-prog-bar","fw-status",function(st){
    let sec=15;
    st.textContent=`Update successful! Rebooting gateway (${sec}s)...`;
    const timer=setInterval(function(){
      sec--;
      if(sec<=0){clearInterval(timer);location.href="/";}
      else st.textContent=`Update successful! Rebooting gateway (${sec}s)...`;
    },1000);
  });
}

function uploadZigbeeOta(){
  runUpload("/upload_zigbee_ota","zb-file","data","zb-btn","zb-prog-wrap","zb-prog-bar","zb-status",function(st,resp){
    st.textContent="ZigBee OTA file uploaded ("+resp+")! Ready for OTA in ZigBee devices tab.";
  });
}
</script>
</body>
</html>)rawliteral";

static const char successResponse[] PROGMEM =
    R"(<meta content="15;URL=/" http-equiv="refresh">Update Success! Rebooting...)";

Ticker restartTimer;

ESPAsyncHTTPUpdateServer::ESPAsyncHTTPUpdateServer()
{
    _server = NULL;
    _username = emptyString;
    _password = emptyString;
    _authenticated = false;
    _updateType = UpdateType::FIRMWARE;
    _updateResult = UpdateResult::UPDATE_OK;
}

void ESPAsyncHTTPUpdateServer::setup(AsyncWebServer *server, const String &path, const String &username, const String &password)
{
    _server = server;
    _username = username;
    _password = password;

    _server->on(path.c_str(), HTTP_GET, [&](AsyncWebServerRequest *request) {
        if (_username.length() > 0 && _password.length() > 0) {
            if (!request->authenticate(_username.c_str(), _password.c_str())) {
                return request->requestAuthentication();
            }
        }
        request->send_P(200, "text/html", UPDATE_CENTER_HTML);
    });

    _server->on(path.c_str(), HTTP_OPTIONS, [&](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "y");
        response->addHeader("Access-Control-Allow-Headers", "*");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);

        _authenticated = (_username.length() == 0 || _password.length() == 0 ||
                          request->authenticate(_username.c_str(), _password.c_str()));
    });

    _server->on(
        path.c_str(), HTTP_POST,
        [&](AsyncWebServerRequest *request) {
            if (!_authenticated) {
                return request->requestAuthentication();
            }

            if (_updateResult != UpdateResult::UPDATE_OK) {
                String errMsg = (_updaterError.length() > 0)
                    ? (String(F("Update error: ")) + _updaterError)
                    : String(F("Update aborted by server."));
                AsyncWebServerResponse *response = request->beginResponse(200, "text/html", errMsg);
                response->addHeader("Access-Control-Allow-Headers", "*");
                response->addHeader("Access-Control-Allow-Origin", "*");
                response->addHeader("Connection", "close");
                request->send(response);
            } else {
                request->send_P(200, "text/html", successResponse);
                Log("Rebooting...\n");
                restartTimer.once_ms(1000, []() { ESP.restart(); });
            }
        },
        [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
            if (!index) {
                const AsyncWebParameter *pName = request->getParam("name");
                if (pName && strcmp(pName->value().c_str(), "filesystem") == 0) {
                    _updateType = UpdateType::FILE_SYSTEM;
                } else if ((pName && strcmp(pName->value().c_str(), "ncp_firmware") == 0) ||
                           filename.startsWith("Z2S_C6_NCP")) {
                    _updateType = UpdateType::NCP_FIRMWARE;
                } else {
                    _updateType = UpdateType::FIRMWARE;
                }

                _updaterError.clear();

                _authenticated = (_username.length() == 0 || _password.length() == 0 ||
                                  request->authenticate(_username.c_str(), _password.c_str()));
                if (!_authenticated) {
                    Log("Unauthenticated Update\n");
                    return;
                }

                _updateResult = UpdateResult::UPDATE_OK;
                if (onUpdateBegin) {
                    onUpdateBegin(_updateType, _updateResult);
                    if (_updateResult != UpdateResult::UPDATE_OK) {
                        Log("Update aborted by server: %d\n", _updateResult);
                        if (onUpdateEnd) onUpdateEnd(_updateType, _updateResult);
                        return;
                    }
                }

                Log("Update: %s (type=%d)\n", filename.c_str(), (int)_updateType);

                if (_updateType == UpdateType::NCP_FIRMWARE) {
                    if (!z2s_ncp_ota_begin()) {
                        _updaterError = "C6 NCP OTA begin failed";
                        _updateResult = UpdateResult::UPDATE_ERROR;
                    }
                } else if (_updateType == UpdateType::FILE_SYSTEM) {
                    size_t fsSize = UPDATE_SIZE_UNKNOWN;
                    const esp_partition_t *part = esp_partition_find_first(
                        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, nullptr);
                    if (part) fsSize = part->size;
                    if (!Update.begin(fsSize, U_SPIFFS)) {
                        _setUpdaterError();
                    }
                } else {
                    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
                    if (!Update.begin(maxSketchSpace, U_FLASH)) {
                        _setUpdaterError();
                    }
                }
            }

            if (_authenticated && len && _updateResult == UpdateResult::UPDATE_OK) {
                if (_updateType == UpdateType::NCP_FIRMWARE) {
                    if (!z2s_ncp_ota_write(data, len)) {
                        _updaterError = "C6 NCP OTA write failed";
                        _updateResult = UpdateResult::UPDATE_ERROR;
                    }
                } else {
                    if (Update.write(data, len) != len) {
                        _setUpdaterError();
                    }
                }
            }

            if (_authenticated && final && _updateResult == UpdateResult::UPDATE_OK) {
                if (_updateType == UpdateType::NCP_FIRMWARE) {
                    if (z2s_ncp_ota_end()) {
                        Log("C6 NCP Update Success.\n");
                        _updateResult = UpdateResult::UPDATE_OK;
                        if (onUpdateEnd) onUpdateEnd(_updateType, _updateResult);
                    } else {
                        _updaterError = "C6 NCP OTA end failed";
                        _updateResult = UpdateResult::UPDATE_ERROR;
                    }
                } else {
                    if (Update.end(true)) {
                        Log("Update Success.\n");
                        _updateResult = UpdateResult::UPDATE_OK;
                        if (onUpdateEnd) onUpdateEnd(_updateType, _updateResult);
                    } else {
                        _setUpdaterError();
                    }
                }
            }
        });
}

void ESPAsyncHTTPUpdateServer::_setUpdaterError()
{
#ifdef ESPASYNCHTTPUPDATESERVER_DEBUG
    Update.printError(ESPASYNCHTTPUPDATESERVER_SerialOutput);
#endif
    StreamString str;
    Update.printError(str);
    _updaterError = str.c_str();

    _updateResult = UpdateResult::UPDATE_ERROR;
    if (onUpdateEnd) {
        onUpdateEnd(_updateType, _updateResult);
    }
}
