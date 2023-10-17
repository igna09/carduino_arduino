#pragma once

#include "Enum.h"

#define MEDIA_CONTROL_SIZE 4

class MediaControl : public Enum {
    public:
        static const MediaControl VOLUME_UP;
        static const MediaControl VOLUME_DOWN;
        static const MediaControl PLAY_PAUSE;

        MediaControl() : Enum() {};

        uint16_t getMessageId() {
            // TODO: replace this fix
            // uint16_t id = Category::MEDIA_CONTROL.id;
            uint16_t id = 2; // MEDIA_CONTROL id
            id = (id << 8) | this->id;
            return id;
        };

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(MediaControl::values[i]->id == id) {
                    return MediaControl::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(MediaControl::values[i]->name, n) == 0) {
                    return MediaControl::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return MediaControl::index;
        }

        static const Enum** getValues() {
            return MediaControl::values;
        }

        MediaControl(uint8_t id, const char *name) : Enum(id, name) {
            MediaControl::values[MediaControl::index] = this;
            MediaControl::index++;
        };

    private:
        static const Enum* values[];
        static uint8_t index;
};

inline const Enum* MediaControl::values [MEDIA_CONTROL_SIZE] = { 0 };
inline uint8_t MediaControl::index = 0;
inline const MediaControl MediaControl::VOLUME_UP = MediaControl(0x00, "VOLUME_UP");
inline const MediaControl MediaControl::VOLUME_DOWN = MediaControl(0x01, "VOLUME_DOWN");
inline const MediaControl MediaControl::PLAY_PAUSE = MediaControl(0x02, "PLAY_PAUSE");
inline const MediaControl MediaControl::NEXT = MediaControl(0x03, "NEXT");
