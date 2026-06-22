#include "MainNode.h"

MainNode::MainNode(): CarduinoNode(Node::MAIN.id) {
    NLOGD("MainNode::MainNode start");

    configTemt6000();

    NLOGD("MainNode::MainNode end");
}

void MainNode::configTemt6000() {
    if (TEMT6000_OK != temt6000__Init(&temtDevice, TEMT6000_ADC_UNIT, TEMT6000_ADC_CHANNEL)) {
        NLOGE("Failed to initialize TEMT6000 sensor");
        return;
    }

    startRepeatingTask("temt6000_read", 1000, [this]() {
        TEMT6000_measurement_data_t measurement;
        if (TEMT6000_OK != temt6000__ReadLightIntensity(&temtDevice, TEMT6000_SAMPLES_NUM, &measurement.lightIntensity))
        {
            NLOGE("Failed to read from TEMT6000 sensor");
            return;
        }
        
        NLOGD(std::format("TEMT6000 light intensity: {}", measurement.lightIntensity));

        sendMessage(Message(Priority::L.id, Node::BROADCAST.id, EventMulti<float>(EventRegistry::getByName("INTERNAL_LUMINANCE").id, measurement.lightIntensity)));
    });
}