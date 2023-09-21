#pragma once

#include <Arduino.h>
#include "ExecutorInterface.h"
#include "../CanbusMessage/CanbusMessage.h"

class WriteSettingExecutor : public ExecutorInterface {
    public:
        void execute(CanbusMessage message);
};
