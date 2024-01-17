#include "Logger.h"

Logger::Logger() {};

void Logger::setupLogger(ESP8266WebServer *server, bool logOnServer, bool logOnSerial) {
    this->_logOnSerial = logOnSerial;
    this->_logOnServer = logOnServer;
    if(this->_logOnServer) {
        this->_webSocketsServer = new WebSocketsServer(81);
        this->_webServer = server;
        this->_webSocketsServer->begin();
        this->_webSocketsServer->onEvent([&](uint8_t num, WStype_t type, uint8_t * payload, size_t length){
            this->onWebSocketEvent(num, type, payload, length);
        });
        
        static const char PROGMEM INDEX_HTML[] = R"rawliteral(
            <!DOCTYPE html>
            <html>
            <head>
            <meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
            <title>ESP8266 WebSocket Demo</title>
            <style>
            "body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }"
            </style>
            <script>
            var websock;
            function start() {
            websock = new WebSocket('ws://' + window.location.hostname + ':81/');
            websock.onopen = function(evt) { console.log('websock open'); };
            websock.onclose = function(evt) { console.log('websock close'); };
            websock.onerror = function(evt) { console.log(evt); };
            websock.onmessage = function(evt) {
                console.log(evt);
                var e = document.getElementById('ledstatus');
                if (evt.data === 'ledon') {
                e.style.color = 'red';
                }
                else if (evt.data === 'ledoff') {
                e.style.color = 'black';
                }
                else {
                console.log('unknown event');
                }
            };
            }
            function buttonclick(e) {
            websock.send(e.id);
            }
            </script>
            </head>
            <body onload="javascript:start();">
            <h1>ESP8266 WebSocket Demo</h1>
            <div id="ledstatus"><b>LED</b></div>
            <button id="ledon"  type="button" onclick="buttonclick(this);">On</button> 
            <button id="ledoff" type="button" onclick="buttonclick(this);">Off</button>
            </body>
            </html>
            )rawliteral";

        this->_webServer->on("/logger", HTTP_GET, [&](){
            this->_webServer->send_P(200, PSTR("text/html"), INDEX_HTML);
            // TODO: send webapp homepage
        });

        // server->on("/logger/logs", HTTP_GET, [&](){
        //       _server->send_P(200, PSTR("text/html"), serverIndex);
        //     TODO: send list of logs
        // });
    }
};

void Logger::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  Serial.printf("webSocketEvent(%d, %d, ...)\r\n", num, type);
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\r\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = this->_webSocketsServer->remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // Send the current LED status
        // if (LEDStatus) {
        //   this->_webSocketsServer->sendTXT(num, LEDON, strlen(LEDON));
        // }
        // else {
        //   this->_webSocketsServer->sendTXT(num, LEDOFF, strlen(LEDOFF));
        // }
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\r\n", num, payload);

    //   if (strcmp(LEDON, (const char *)payload) == 0) {
    //     // writeLED(true);
    //   }
    //   else if (strcmp(LEDOFF, (const char *)payload) == 0) {
    //     // writeLED(false);
    //   }
    //   else {
    //     Serial.println("Unknown command");
    //   }
      // send data to all connected clients
      this->_webSocketsServer->broadcastTXT(payload, length);
      break;
    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\r\n", num, length);
      hexdump(payload, length);

      // echo data back to browser
      this->_webSocketsServer->sendBIN(num, payload, length);
      break;
    default:
      Serial.printf("Invalid WStype [%d]\r\n", type);
      break;
  }
}

void Logger::logOnServer(const char c[]) {
    this->_webSocketsServer->broadcastTXT(c);
}

void Logger::printlnWrapper(const String &s) {
    if(_logOnSerial) Serial.println(s);
    if(_logOnServer) logOnServer(s.c_str() + '\n');
}

void Logger::printlnWrapper(const char c[]) {
    if(_logOnSerial) Serial.println(c);
    if(_logOnServer) logOnServer(c + '\n');
}

void Logger::printlnWrapper(char c) {
    if(_logOnSerial) Serial.println(c);
}

void Logger::printlnWrapper(unsigned char b, int base) {
    if(_logOnSerial) Serial.println(b, base);
}

void Logger::printlnWrapper(int num, int base) {
    if(_logOnSerial) Serial.println(num, base);
}

void Logger::printlnWrapper(unsigned int num, int base) {
    if(_logOnSerial) Serial.println(num, base);
}

void Logger::printlnWrapper(long num, int base) {
    if(_logOnSerial) Serial.println(num, base);
}

void Logger::printlnWrapper(unsigned long num, int base) {
    if(_logOnSerial) Serial.println(num, base);
}

void Logger::printlnWrapper(long long num, int base) {
    if(_logOnSerial) Serial.println(num, base);
}

void Logger::printlnWrapper(unsigned long long num, int base) {
    if(_logOnSerial) Serial.println(num, base);
}

void Logger::printlnWrapper(double num, int digits) {
    if(_logOnSerial) Serial.println(num, digits);
}

void Logger::printWrapper(const String &s) {
    if(_logOnSerial) Serial.print(s);
    if(_logOnServer) logOnServer(s.c_str());
}

void Logger::printWrapper(const char c[]) {
    if(_logOnSerial) Serial.print(c);
    if(_logOnServer) logOnServer(c);
}

void Logger::printWrapper(char c) {
    if(_logOnSerial) Serial.print(c);
}

void Logger::printWrapper(unsigned char b, int base) {
    if(_logOnSerial) Serial.print(b, base);
}

void Logger::printWrapper(int num, int base) {
    if(_logOnSerial) Serial.print(num, base);
}

void Logger::printWrapper(unsigned int num, int base) {
    if(_logOnSerial) Serial.print(num, base);
}

void Logger::printWrapper(long num, int base) {
    if(_logOnSerial) Serial.print(num, base);
}

void Logger::printWrapper(unsigned long num, int base) {
    if(_logOnSerial) Serial.print(num, base);
}

void Logger::printWrapper(long long num, int base) {
    if(_logOnSerial) Serial.print(num, base);
}

void Logger::printWrapper(unsigned long long num, int base) {
    if(_logOnSerial) Serial.print(num, base);
}

void Logger::printWrapper(double num, int digits) {
    if(_logOnSerial) Serial.print(num, digits);
}
