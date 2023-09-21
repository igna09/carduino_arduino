#include "CarstatusMessage.h"

CarstatusMessage::CarstatusMessage(CanbusMessage canbusMessage) : TypedCanbusMessage((CanbusMessageType*)CanbusMessageType::getValueById(canbusMessage.messageId), id, payload, payloadLength) {
    this->carstatus = (Carstatus*) Carstatus::getValueById(canbusMessage.messageId);
};
