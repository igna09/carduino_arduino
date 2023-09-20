#include "utils.h"

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

void cloneByteArray (uint8_t *from, uint8_t *to, int len) {
  Serial.print("len -> ");
  Serial.println(len);
  for(int i = 0; i < len; i++) {
    Serial.println(from[i], HEX);
    to[i] = from[i];
  }
}

void convertFloatToByteArray(uint8_t *bytes, float v) {
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

void resetArray(uint8_t *a, int len) {
  for(int i = 0; i < len; i++) {
    a[i] = 0;
  }
}

//TODO: test these functions
int* convertByteArrayToInt(uint8_t *bytes, int size) {
    resetArray(integerByteConverter.array, 4);
    cloneByteArray(bytes, integerByteConverter.array, size);
    int i;
    i = integerByteConverter.value;
    return new int(i);
}

float* convertByteArrayToFloat(uint8_t *bytes, int size) {
    int integerPart, decimalPart;

    integerPart = *convertByteArrayToInt(bytes, size - 1);
    decimalPart = *convertByteArrayToInt(&bytes[4], 1);

    float v = integerPart + decimalPart * pow(10, -2);
    
    return new float(v);
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

String* convertByteArrayToString(uint8_t bytes[], uint8_t len)
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

  return new String(buffer);
}
