#include <Arduino.h>

#include <mcp_can.h>
#include <SPI.h>

#include "./CanbusMessage.cpp"
#include "./Category.cpp"

union IntegerByteConverter {
  int size = 4;
  uint8_t array[4];
  uint32_t value;
} integerByteConverter;

union DecimalByteConverter {
  int size = 1;
  uint8_t array[1];
  uint32_t value;
} decimalByteConverter;

uint8_t* convertFloatToByteArray(uint8_t *bytes, float v) {
    int integerPart, decimalPart;

    integerPart = floor(v);
    integerByteConverter.value = integerPart;
    uint8_t integerPartByteArray[4];
    cloneByteArray(integerByteConverter.array, integerPartByteArray, 4);

    decimalPart = v * pow(10,2) - integerPart * pow(10,2);
    decimalByteConverter.value = decimalPart;
    uint8_t decimalPartByteArray[1];
    cloneByteArray(decimalByteConverter.array, decimalPartByteArray, 1);

    bytes[0] = integerPartByteArray[0];
    bytes[1] = integerPartByteArray[1];
    bytes[2] = integerPartByteArray[2];
    bytes[3] = integerPartByteArray[3];
    bytes[4] = decimalPartByteArray[0];
}

void convertIntegerToByteArray(uint8_t *bytes, uint32_t v) {
    integerByteConverter.value = v;
    cloneByteArray(integerByteConverter.array, bytes, integerByteConverter.size);
}

void cloneByteArray (uint8_t *from, uint8_t *to, int len) {
  Serial.print("len -> ");
  Serial.println(len);
  for(int i = 0; i < len; i++) {
    Serial.println(from[i], HEX);
    to[i] = from[i];
  }
}

void sendFloatMessageCanbus(MCP_CAN can, unsigned long id, float v) {
  uint8_t buf[5];
  convertFloatToByteArray(buf, v);
  sendMessageCanBus(can, 0, 5, buf);
}

void sendIntMessageCanbus(MCP_CAN can, unsigned long id, uint32_t v) {
  uint8_t buf[4];
  convertIntegerToByteArray(buf, v);
  sendMessageCanBus(can, 0, 4, buf);
}

void sendMessageCanBus(MCP_CAN can, unsigned long messageId, int len, byte *buf) {
  byte sndStat = can.sendMsgBuf(0x100, 0, len, buf);
  if(sndStat == CAN_OK){
    Serial.println("Message Sent Successfully!");
  } else {
    Serial.println("Error Sending Message...");
  }
}

#define IDNAME(name) #name
String convertCanbusMessageToString(CanbusMessage canbusMessage) {
    String message = "";
    message += "CANBUS;";
    message += CATEGORY_STRING[canbusMessage.getCategory()];
    message += ";";
    message += canbusMessage.getPayload();
    return message;
}