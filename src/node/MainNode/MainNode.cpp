#include "MainNode.h"

MainNode::MainNode(): CarduinoNode(Node::MAIN.id), I2CNode() {
    NLOGD("MainNode::MainNode called");

    configTemt6000();
}

void MainNode::configTemt6000() {
    NLOGD("MainNode::configTemt6000 called");

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

    startRepeatingTask("aht_read", 1000, [this]() {
        // Lettura AHT
        if (aht_get_data(&aht_dev, &temperature, &humidity) == ESP_OK) {
            ESP_LOGD(TAG, "[AHT] Temp: %.1f°C, Umidità: %.1f%%", temperature, humidity);
        } else {
            ESP_LOGE(TAG, "[AHT] Errore nella lettura dei dati");
        }

        NLOGD(std::format("AHT temperature: {}, humidity: {}", temperature, humidity));

        sendMessage(Message(Priority::L.id, Node::BROADCAST.id, EventMulti<float>(EventRegistry::getByName("INTERNAL_TEMPERATURE").id, temperature)));
        // sendMessage(Message(Priority::L.id, Node::BROADCAST.id, EventMulti<float>(EventRegistry::getByName("INTERNAL_HUMIDITY").id, humidity)));
    });
}

void MainNode::initI2cDevices() {
    // Inizializza eventuali dispositivi I2C aggiuntivi qui
    NLOGD("MainNode: initI2cDevices called");

    // --- Configurazione del sensore AHT ---
    aht_dev.mode = AHT_MODE_NORMAL;
    aht_dev.type = AHT_TYPE_AHT20;

    // NOTA: Passando la macro I2C_NUM_0 come porta, i2cdev capisce che 
    // deve cercare un bus già inizializzato su quella porta hardware.
    ESP_ERROR_CHECK(aht_init_desc(&aht_dev, AHT_I2C_ADDRESS_GND, I2C_PORT, I2C_SDA_PIN, I2C_SCL_PIN));
    ESP_ERROR_CHECK(aht_init(&aht_dev));
    ESP_LOGD(TAG, "Sensore AHT inizializzato.");
}