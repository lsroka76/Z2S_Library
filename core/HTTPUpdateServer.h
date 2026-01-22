#ifndef __HTTP_UPDATE_SERVER_H
#define __HTTP_UPDATE_SERVER_H

#include <SPIFFS.h>
#include <StreamString.h>
#include <Update.h>
#include <WebServer.h>

static const char serverIndex[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pl">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
  <title>Firmware Update</title>
  <style>
    *, ::before, ::after { box-sizing: border-box; }
    html:focus-within { scroll-behavior: smooth; }
    body, html, h1, h3, p {
      margin: 0;
      padding: 0;
      font-family: HelveticaNeue, "Helvetica Neue", Helvetica, Arial, sans-serif;
    }

    body {
      background: #00d151;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      justify-content: flex-start;
      align-items: center;
      font-size: 14px;
      font-weight: 400;
      color: #fff;
      padding-top: 65px;
    }

    .wrapper {
      width: 100%;
      max-width: 500px;
      padding: 20px 10px;
    }

    .box {
      background: #fff;
      border-radius: 10px;
      padding: 20px;
      box-shadow: 0 5px 6px rgba(0, 0, 0, 0.3);
      margin-bottom: 20px;
    }

    h1 {
      font-weight: 300;
      font-size: 23px;
      margin-bottom: 20px;
    }

    .box h3 {
      margin-top: 0;
      margin-bottom: 15px;
      font-weight: 300;
      font-size: 23px;
      text-align: left;
      color: #000;
    }

    input[type='file'] {
      width: 100%;
      font-size: 1em;
      margin-bottom: 10px;
    }

    .btn-group {
      display: flex;
      flex-direction: column;
      gap: 10px;
    }

    button {
      background: #000;
      width: 100%;
      border: none;
      border-radius: 10px;
      padding: 12px;
      text-transform: uppercase;
      color: #fff;
      font-size: 1.3em;
      cursor: default;
      font-family: inherit;
      box-shadow: 0 5px 6px rgba(0, 0, 0, 0.3);
    }

    button:hover {
      background: #000;
    }

    input[type='file'] {
      width: 100%;
      font-size: 1em;
      margin-bottom: 10px;
      color: #000;
      background-color: #fff;
    }

    label {
      display: block;
      font-weight: 500;
      margin-bottom: 6px;
      color: #00d150;
    }
    
    .file-row {
      display: flex;
      align-items: center;
      gap: 10px;
      margin-bottom: 10px;
    }

    .file-row label {
      margin: 0;
      width: 250px;
      color: #00d150;
      font-weight: 500;
    }

    .file-row input[type='file'] {
      flex-grow: 1;
      margin: 0;
    }

    #logo {
      display: block;
      margin: 0 auto 15px;
      height: 155px;
      width: auto;
      background: #00d151;
    }

    @media screen and (max-width: 530px) {
      .wrapper {
        padding: 20px 6vw;
      }
    }
  </style>
</head>
<body>
  <div class="wrapper">
    <svg id="logo" version="1.1" viewBox="0 0 200 200" x="0" xml:space="preserve" y="0">
      <path d="M59.3,2.5c18.1,0.6,31.8,8,40.2,23.5c3.1,5.7,4.3,11.9,4.1,18.3c-0.1,3.6-0.7,7.1-1.9,10.6c-0.2,0.7-0.1,1.1,0.6,1.5c12.8,7.7,25.5,15.4,38.3,23c2.9,1.7,5.8,3.4,8.7,5.3c1,0.6,1.6,0.6,2.5-0.1c4.5-3.6,9.8-5.3,15.7-5.4c12.5-0.1,22.9,7.9,25.2,19c1.9,9.2-2.9,19.2-11.8,23.9c-8.4,4.5-16.9,4.5-25.5,0.2c-0.7-0.3-1-0.2-1.5,0.3c-4.8,4.9-9.7,9.8-14.5,14.6c-5.3,5.3-10.6,10.7-15.9,16c-1.8,1.8-3.6,3.7-5.4,5.4c-0.7,0.6-0.6,1,0,1.6c3.6,3.4,5.8,7.5,6.2,12.2c0.7,7.7-2.2,14-8.8,18.5c-12.3,8.6-30.3,3.5-35-10.4c-2.8-8.4,0.6-17.7,8.6-22.8c0.9-0.6,1.1-1,0.8-2c-2-6.2-4.4-12.4-6.6-18.6c-6.3-17.6-12.7-35.1-19-52.7c-0.2-0.7-0.5-1-1.4-0.9c-12.5,0.7-23.6-2.6-33-10.4c-8-6.6-12.9-15-14.2-25c-1.5-11.5,1.7-21.9,9.6-30.7C32.5,8.9,42.2,4.2,53.7,2.7c0.7-0.1,1.5-0.2,2.2-0.2C57,2.4,58.2,2.5,59.3,2.5z M76.5,81c0,0.1,0.1,0.3,0.1,0.6c1.6,6.3,3.2,12.6,4.7,18.9c4.5,17.7,8.9,35.5,13.3,53.2c0.2,0.9,0.6,1.1,1.6,0.9c5.4-1.2,10.7-0.8,15.7,1.6c0.8,0.4,1.2,0.3,1.7-0.4c11.2-12.9,22.5-25.7,33.4-38.7c0.5-0.6,0.4-1,0-1.6c-5.6-7.9-6.1-16.1-1.3-24.5c0.5-0.8,0.3-1.1-0.5-1.6c-9.1-4.7-18.1-9.3-27.2-14c-6.8-3.5-13.5-7-20.3-10.5c-0.7-0.4-1.1-0.3-1.6,0.4c-1.3,1.8-2.7,3.5-4.3,5.1c-4.2,4.2-9.1,7.4-14.7,9.7C76.9,80.3,76.4,80.3,76.5,81z M89,42.6c0.1-2.5-0.4-5.4-1.5-8.1C83,23.1,74.2,16.9,61.7,15.8c-10-0.9-18.6,2.4-25.3,9.7c-8.4,9-9.3,22.4-2.2,32.4c6.8,9.6,19.1,14.2,31.4,11.9C79.2,67.1,89,55.9,89,42.6z M102.1,188.6c0.6,0.1,1.5-0.1,2.4-0.2c9.5-1.4,15.3-10.9,11.6-19.2c-2.6-5.9-9.4-9.6-16.8-8.6c-8.3,1.2-14.1,8.9-12.4,16.6C88.2,183.9,94.4,188.6,102.1,188.6z M167.7,88.5c-1,0-2.1,0.1-3.1,0.3c-9,1.7-14.2,10.6-10.8,18.6c2.9,6.8,11.4,10.3,19,7.8c7.1-2.3,11.1-9.1,9.6-15.9C180.9,93,174.8,88.5,167.7,88.5z"></path>
    </svg>
    <form id="updateForm" method="POST" action="/update" enctype="multipart/form-data">
      <h1>SUPLA</h1>
      <div class="box">
        <h3>Firmware Update</h3>
        <div class="file-row">
          
          <input type="file" accept=".bin,.bin.gz" name="firmware" required>
        </div>
      </div>
      <div class="btn-group">
        <button type="submit">Update</button>
        <button type="button" onclick="window.location.href='/'">BACK TO GUI</button>
      </div>
    </form>
  </div>
</body>
</html>
)rawliteral";

static const char successResponse[] PROGMEM = 
  "<meta http-equiv=\"refresh\" content=\"15;URL=/\">"
  "Update Success! Rebooting...";

class HTTPUpdateServer {
public:
  HTTPUpdateServer(bool serial_debug = false) {
    _serial_output = serial_debug;
    _server = nullptr;
    _updaterError = String();
  }

  void setup(WebServer *server, const String &path = "/update") {
    _server = server;

    // GET - wyświetlenie formularza aktualizacji
    _server->on(path.c_str(), HTTP_GET, [&]() {
      _server->send_P(200, PSTR("text/html"), serverIndex);
    });

    // POST - obsługa uploadu firmware
    _server->on(path.c_str(), HTTP_POST,
      [&]() {
        if (Update.hasError()) {
          _server->send(200, F("text/html"), String(F("Update error: ")) + _updaterError);
        } else {
          _server->client().setNoDelay(true);
          _server->send_P(200, PSTR("text/html"), successResponse);
          delay(100);
          _server->client().stop();
          ESP.restart();
        }
      },
      [&]() {
        HTTPUpload &upload = _server->upload();

        if (upload.status == UPLOAD_FILE_START) {
          _updaterError.clear();
          if (_serial_output) Serial.setDebugOutput(true);

          if (_serial_output) Serial.printf("Update: %s\n", upload.filename.c_str());

          if (upload.name == "filesystem") {
            if (!Update.begin(SPIFFS.totalBytes(), U_SPIFFS)) {
              if (_serial_output) Update.printError(Serial);
            }
          } else {
            uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
            if (!Update.begin(maxSketchSpace, U_FLASH)) {
              _setUpdaterError();
            }
          }
        } else if (upload.status == UPLOAD_FILE_WRITE && _updaterError.isEmpty()) {
          if (_serial_output) Serial.print(".");
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            _setUpdaterError();
          }
        } else if (upload.status == UPLOAD_FILE_END && _updaterError.isEmpty()) {
          if (Update.end(true)) {
            if (_serial_output) Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          } else {
            _setUpdaterError();
          }
          if (_serial_output) Serial.setDebugOutput(false);
        } else if (upload.status == UPLOAD_FILE_ABORTED) {
          Update.end();
          if (_serial_output) Serial.println("Update was aborted");
        }
        delay(0);
      }
    );
  }

private:
  void _setUpdaterError() {
    if (_serial_output) Update.printError(Serial);
    StreamString str;
    Update.printError(str);
    _updaterError = str.c_str();
  }

  bool _serial_output;
  WebServer *_server;
  String _updaterError;
};

#endif