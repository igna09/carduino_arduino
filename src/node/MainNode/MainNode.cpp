#include "MainNode.h"

MainNode::MainNode(): CarduinoNode(Node::MAIN.id), I2cNode() {
    NLOGD("MainNode::MainNode called");

    configTemt6000();
    configAht();
}

void MainNode::configAht() {
    // --- Configurazione del sensore AHT ---
    aht_dev.mode = AHT_MODE_NORMAL;
    aht_dev.type = AHT_TYPE_AHT20;

    // // NOTA: Passando la macro I2C_NUM_0 come porta, i2cdev capisce che 
    // // deve cercare un bus già inizializzato su quella porta hardware.
    ESP_ERROR_CHECK(aht_init_desc(&aht_dev, AHT_I2C_ADDRESS_GND, I2C_NUM_0, DEFAULT_I2C_SDA_PIN, DEFAULT_I2C_SCL_PIN));
    ESP_ERROR_CHECK(aht_init(&aht_dev));
    NLOGD("Sensore AHT inizializzato.");
}

void MainNode::configTemt6000() {
    NLOGD("MainNode::configTemt6000 called");

    // 1. Inizializzazione dell'Unità ADC
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = TEMT6000_ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    // 2. Configurazione del Canale ADC
    // Usiamo l'attenuazione massima (12dB) per poter leggere quasi fino a 3.3V
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT, // Risoluzione massima del chip (es. 12 bit)
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, TEMT6000_ADC_CHANNEL, &config));

    // 3. Configurazione della Calibrazione (Opzionale ma altamente consigliata)
    adc_cali_handle_t cali_handle = NULL;
    bool do_calibration = false;
    
    #if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = TEMT6000_ADC_UNIT,
        .chan = TEMT6000_ADC_CHANNEL,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    if (adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle) == ESP_OK) {
        do_calibration = true;
    }
    #endif

    int adc_raw;
    int voltage;

    startRepeatingTask("temt6000_read", 1000, [&]() {
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, TEMT6000_ADC_CHANNEL, &adc_raw));
        
        if (do_calibration) {
            // Converte il valore grezzo in Millivolt reali
            adc_cali_raw_to_voltage(cali_handle, adc_raw, &voltage);
            NLOGD("Valore Grezzo: %d | Tensione: %d mV", adc_raw, voltage);
        } else {
            NLOGD("Valore Grezzo: %d (Calibrazione non disponibile)", adc_raw);
        }
        
        // NLOGD("TEMT6000 light intensity: %d", measurement.lightIntensity);

        // sendMessage(Message(Priority::L.id, Node::BROADCAST.id, EventMulti<float>(EventRegistry::getByName("INTERNAL_LUMINANCE").id, measurement.lightIntensity)));
    });
}

void MainNode::initI2cDevices() {
    // Inizializza eventuali dispositivi I2C aggiuntivi qui
    NLOGD("MainNode: initI2cDevices called");
}