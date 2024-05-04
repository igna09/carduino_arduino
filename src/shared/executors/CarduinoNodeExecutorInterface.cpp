#include "CarduinoNodeExecutorInterface.h"

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface(const Category *categoryFilter) {
    // CarduinoNodeExecutorInterface(const Category *categoryFilter, nullptr);
    this->categoryFilter = categoryFilter;
    
    this->filterMessage = false;
    this->messageId = 0;
}

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface(const Category *categoryFilter, uint8_t messageId) {
    this->categoryFilter = categoryFilter;

    this->messageId = messageId;
    this->filterMessage = true;
}
