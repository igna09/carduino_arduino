#include "CarstatusExecutor.h"

void CarstatusExecutor::execute(CarduinoNode *node, CanbusMessage message) {
    CarstatusCanbusMessageTypedInterface* carstatusCanbusMessageTypedInterface = CarstatusCanbusMessageFactory::getCarstatusCanbusMessage(message);
    
}
