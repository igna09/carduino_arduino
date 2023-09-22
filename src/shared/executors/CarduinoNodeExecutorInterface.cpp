#include "CarduinoNodeExecutorInterface.h"

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface(const Category *categoryFilter) {
    Serial.print("CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface ");
    Serial.println(categoryFilter->id);
    this->categoryFilter = categoryFilter;
}
