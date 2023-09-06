#include <Arduino.h>
#include "./Category.cpp"

class CanbusMessage
{
private:
    unsigned long id;
    String payload;
public:
    CanbusMessage(/* args */) {};
    ~CanbusMessage() {};

    unsigned long getId() {
        return this->id;
    };

    void setId(unsigned long id) {
        this->id = id;
    };
    String getPayload() {
        return this->payload;
    };
    void setPayload(String payload) {
        this->payload = payload;
    };
    Category getCategory() {
        return static_cast<Category>(this->id>>8);
    }
};
