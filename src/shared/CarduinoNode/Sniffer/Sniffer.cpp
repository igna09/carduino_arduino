#include "Sniffer.h"

/**
 * TODO: 
 * 	TEST taskscheduler che riporta la resistenza a 0 dopo tot di ms (da recuperare da main node)
 * 	TEST portare la registrazione dei pulsanti da main node a qui
 * 	durante la registrazione dei pulsanti mandare messaggio alla radio (non visibile perché siamo su app radio swc, BLE_PAIRING_CODE)
 * 	TEST aggiungere buzzer
 * 	TEST far suonare buzzer durante il cambio pulsanti registrazione
 * 	TEST aggiungere mapping MediaControl => resistance value
 * 	TEST far partire registrazione dei pulsanti solo quando viene premuto il pulsante
 * 	to send string messages to radio use ids: on adroid app store a json file mapping id to message
 */

Sniffer::Sniffer(uint8_t id, uint8_t cs, uint8_t interruptPin, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, false, false, true) {
    this->restoreSettings();

	this->canExecutor->addExecutor(new AllCanEvent());
};

void Sniffer::loop() {
	CarduinoNode::loop();
};