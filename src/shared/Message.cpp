#include <Arduino.h>

class Message
{
private:
    String action;
    String payload;
public:
    Message(/* args */);
    ~Message();
    String getAction();
    void setAction(String action);
    String getPayload();
    void setPayload(String payload);
};

Message::Message(/* args */)
{
}

Message::~Message()
{
}
