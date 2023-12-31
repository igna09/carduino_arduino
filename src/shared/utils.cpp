#include "utils.h"

union IntegerByteConverter {
  uint8_t array[4];
  uint32_t value;
} integerByteConverter;

union DecimalByteConverter {
  uint8_t array[1];
  uint32_t value;
} decimalByteConverter;

void cloneByteArray (uint8_t from[], uint8_t to[], int len) {
  for(int i = 0; i < len; i++) {
    to[i] = from[i];
  }
}

uint8_t floatBytes[5];

uint8_t* convertValueToByteArray(float v) {
    convertFloatToByteArray(floatBytes, v);
    return floatBytes;
}

void convertFloatToByteArray(uint8_t *bytes, float v) {
    int integerPart, decimalPart;

    integerPart = floor(v);
    uint8_t integerPartByteArray[4];
	  convertIntegerToByteArray(integerPartByteArray, integerPart);

    resetArray(integerByteConverter.array, 4);
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

uint8_t integerBytes[4];

uint8_t* convertValueToByteArray(int v) {
	convertIntegerToByteArray(integerBytes, v);
	return integerBytes;
}

void convertIntegerToByteArray(uint8_t *bytes, uint32_t v) {
	resetArray(integerByteConverter.array, 4);
	integerByteConverter.value = v;
	for(int i=0; i < 4; i++){
	  bytes[4 - i - 1] = integerByteConverter.array[i]; //reverse the byte order here.
	}
  // printUint8Array("loop", m.payload, m.payloadLength);
}

uint8_t boolBytes[1];

uint8_t* convertValueToByteArray(bool v) {
	convertBoolToByteArray(boolBytes, v);
	return boolBytes;
}

void convertBoolToByteArray(uint8_t *bytes, bool v) {
	bytes[0] = v ? 1 : 0;
}

void resetArray(uint8_t *a, int len) {
  for(int i = 0; i < len; i++) {
    a[i] = 0;
  }
}

//TODO: test these functions
int convertByteArrayToInt(uint8_t bytes[], int size) {
    resetArray(integerByteConverter.array, 4);
    for(int i=0; i < size; i++){
      integerByteConverter.array[size - i - 1] = bytes[i]; //reverse the byte order here.
    }
    return integerByteConverter.value;
}

bool convertByteArrayToBool(uint8_t bytes[], int size) {
    return bytes[0] == 1;
}

float convertByteArrayToFloat(uint8_t *bytes, int size) {
    int integerPart, decimalPart;

    integerPart = convertByteArrayToInt(bytes, size - 1);
    uint8_t decimalArray[] = {bytes[4]};
    decimalPart = convertByteArrayToInt(decimalArray, 1);
    
    return integerPart + decimalPart * pow(10, -2);
}

void pickSubarray( uint8_t A[], uint8_t sub[], int &m, int i, int j ) {
   int ind = 0;
   for( int k = i; k < j; k++, ind++ ) { 
      sub[ ind ] = A[ k ]; 
      m += 1;
   } 
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

void convertSerialMessagetoArduino(String msg) {
  char   *sPtr [SPTR_SIZE];
  separate(msg, sPtr, SPTR_SIZE, ";");
}

String convertByteArrayToString(uint8_t bytes[], uint8_t len)
{
  char buffer[len];

  for (unsigned int i = 0; i < len; i++)
  {
      byte nib1 = (bytes[i] >> 4) & 0x0F;
      byte nib2 = (bytes[i] >> 0) & 0x0F;
      buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
      buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len*2] = '\0';

  return String(buffer);
}

void printFreeHeap(const char label[]) {
    Serial.print(ESP.getFreeHeap());
    Serial.print(" - ");
    Serial.println(label);
}

void printUint8Array(const char label[], uint8_t *array, uint8_t size) {
  Serial.print(label);
  Serial.print(" ");
  for(int i=0; i < size; i++){
      Serial.print(" 0x");
      if(array[i] < 10){
          Serial.print("0");
      }
      Serial.print(array[i], HEX);
  }
  Serial.println();
}

unsigned long elapsed = 0;

void startElapsedTime() {
  elapsed = millis();
};

unsigned long stopElapsedTime() {
  return millis() - elapsed;
};

void printElapsedTime(char *s) {
  unsigned long t = stopElapsedTime();
  Serial.print(s);
  Serial.print(" ");
  Serial.println(t);
};

uint8_t getByteArraySize(int value) {
  return 4;
};

uint8_t getByteArraySize(bool value) {
  return 1;
};

uint8_t getByteArraySize(float value) {
  return 5;
};
