#pragma once

#include <Arduino.h>

union IntegerByteConverter;

union DecimalByteConverter;

extern void cloneByteArray (uint8_t *from, uint8_t *to, int len);

extern void convertFloatToByteArray(uint8_t *bytes, float v);

extern void convertIntegerToByteArray(uint8_t *bytes, uint32_t v);

extern void resetArray(uint8_t *a, int len);

extern int convertByteArrayToInt(uint8_t *bytes, int size);

extern float convertByteArrayToFloat(uint8_t *bytes);

extern void pickSubarray( uint8_t A[], uint8_t sub[], int &m, int i, int j );

extern int separate (String str, char **p, int size, char *separator);

#define SPTR_SIZE   20
extern void convertSerialMessagetoArduino(String msg);
