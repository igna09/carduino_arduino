#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
/* INCLUDE ESP2SOTA LIBRARY */
#include <ESP2SOTA.h>

#include <mcp_can.h>
#include <SPI.h>

#include <TaskScheduler.h>

#include <Adafruit_AHTX0.h>

#include <shared/utils.cpp>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];                        // Array to store serial string

#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(10);                               // Set CS to pin 10

const char* ssid = "ESP2SOTA";
const char* password = "123456789abc";

ESP8266WebServer server(80);

bool otaMode;

Scheduler runner;

Adafruit_AHTX0 aht;

/**
 * convenzione:
 * interi 4 bytes
 * float 4 bytes (parte intera) + 1 byte (parte decimale)
*/

float lux;
void luminanceCallback() {
  float volts = analogRead(A0) * 5.0 / 1024.0;
  float amps = volts / 10000.0; // across 10,000 Ohms
  float microamps = amps * 1000000;
  lux = microamps * 2.0;

  sendIntMessageCanbus(CAN0, 0, floor(lux));
};
Task luminanceTask(500, TASK_FOREVER, &luminanceCallback);

sensors_event_t humidity, temp;
void temperatureCallback() {
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data

  sendfloatMessageCanbus(CAN0, 0, temp.temperature);
};
Task temperatureTask(1000, TASK_FOREVER, &temperatureCallback);

void otaStartup() {
  WiFi.softAP(ssid, password);
  delay(1000);
  IPAddress IP = IPAddress (10, 10, 10, 1);
  IPAddress NMask = IPAddress (255, 255, 255, 0);
  WiFi.softAPConfig(IP, IP, NMask);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  /* INITIALIZE ESP2SOTA LIBRARY */
  ESP2SOTA.begin(&server);
  server.begin();
}

void otaShutdown() {
  server.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
}

void canbusSetup() {
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input

  otaMode = false;
}

void schedulerSetup() {
  runner.addTask(luminanceTask);
  runner.addTask(temperatureTask);
}

void dhtSetup() {
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
}

void handleCanbus() {
  /* YOUR LOOP CODE HERE */
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    while(CAN_MSGAVAIL == CAN0.checkReceive()) {
      CAN0.readMsgBuf(&rxId, &len, rxBuf);
      // print the data
      for (int i = 0; i < len; i++) {
        Serial.print(rxBuf[i]); Serial.print("\t");
      }
      Serial.println();
    }
  }
}

void handleOta() {
  if(otaMode) {
    if(WiFi.getMode() == WIFI_OFF) {
      otaStartup();
    } else {
      /* HANDLE UPDATE REQUESTS */
      server.handleClient();
    }
  } else {
    if(WiFi.getMode() == WIFI_AP) {
      otaShutdown();
    }
  }
}

void readFromCanBus() {
  CAN0.readMsgBuf(&rxId, &len, rxBuf);

  /*for(byte i = 0; i<len; i++) {
    sprintf(msgString, " 0x%.2X", rxBuf[i]);
  }*/
}

int separate (String str, char **p, int size, char *separator) {
  int  n;
  char s [100];

  strcpy (s, str.c_str ());

  *p++ = strtok (s, separator);
  for (n = 1; NULL != (*p++ = strtok (NULL, separator)); n++)
      if (size == n)
          break;

  return n;
}

#define SPTR_SIZE   20
void convertSerialMessagetoArduino(String msg) {
  char   *sPtr [SPTR_SIZE];
  separate(msg, sPtr, SPTR_SIZE, ";");
}

void setup(void) {
  Serial.begin(115200);

  canbusSetup();
  dhtSetup();
  schedulerSetup();
}

void loop(void) {
  handleOta();
  handleCanbus();
  runner.execute();

  //delay(5);
}
