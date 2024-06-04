#include "Category.h"

#include "shared/CanbusMessage/CarstatusMessage/CarstatusMessage.h" // doesn't work if inserted in header file
#include "shared/CanbusMessage/EventMessage/EventMessage.h" // doesn't work if inserted in header file

//const Enum* Enum::values [] = {&Category::CAR_STATUS, &Category::READ_SETTINGS};
inline const Enum* Category::values [CATEGORY_SIZE] = { 0 };
inline uint8_t Category::index = 0;
inline const Category Category::CAR_STATUS = Category(0x00, "CAR_STATUS", Carstatus::getValues, CarstatusMessage::createSpecializedCopy);
inline const Category Category::READ_SETTING = Category(0x01, "READ_SETTING", Setting::getValues, SettingMessage::createSpecializedCopy, Setting::getValueByName, Setting::getValueById);
inline const Category Category::MEDIA_CONTROL = Category(0x02, "MEDIA_CONTROL", MediaControl::getValues, nullptr);
inline const Category Category::WRITE_SETTING = Category(0x03, "WRITE_SETTING", Setting::getValues, SettingMessage::createSpecializedCopy, Setting::getValueByName, Setting::getValueById);
inline const Category Category::LOG = Category(0x04, "LOG", nullptr, nullptr);
inline const Category Category::ERROR = Category(0x05, "ERROR", nullptr, nullptr);
inline const Category Category::EVENT = Category(0x06, "EVENT", Event::getValues, EventMessage::createSpecializedCopy);
inline const Category Category::GET_SETTINGS = Category(0x07, "GET_SETTINGS", nullptr, [](CanbusMessage *canbusMessage){return new TypedCanbusMessage(&CanbusMessageType::BOOL, canbusMessage);});
