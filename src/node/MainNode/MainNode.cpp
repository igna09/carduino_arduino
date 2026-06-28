#include "MainNode.h"

MainNode::MainNode(): CarduinoNode(Node::MAIN.id), I2cNode() {
    NLOGD("MainNode::MainNode called");

    _canExecutor.addExecutor(new BootExecutor());

    // configTemt6000();
    // configAht();

    // Chiamiamo l'inizializzazione dei dispositivi adesso che l'oggetto è pronto!
    initI2cDevices();

    this->enable();

    // xTaskCreate([](void* pvParameters) {
    //     auto* self = static_cast<CarduinoNode*>(pvParameters);
    //     const uint32_t periodo_ms = 5000; // 5 secondi

    //     while (true) {
    //         uint32_t now = self->syncedMillis();
            
    //         // 1. Calcola matematicamente il prossimo multiplo tondo di 5000 ms
    //         uint32_t prossimo_multiplo = ((now / periodo_ms) + 1) * periodo_ms;
            
    //         // 2. Calcola quanti millisecondi mancano esattamente a quel momento
    //         uint32_t ms_da_attendere = prossimo_multiplo - now;
            
    //         // 3. Metti in pausa il task per il tempo calcolato
    //         vTaskDelay(pdMS_TO_TICKS(ms_da_attendere));
            
    //         // --- Esecuzione della tua Lambda / Log ---
    //         std::cout << "[SYNC TASK] Svegliato a syncedMillis: " << self->syncedMillis() << std::endl;
    //     }
    // }, "synced_task", 4096, this, 5, NULL);
}

void MainNode::configAht() {
    // Azzera completamente la struttura per ripulire la memoria da valori residui/spazzatura
    memset(&aht_dev, 0, sizeof(aht_dev));

    // --- Configurazione del sensore AHT ---
    aht_dev.mode = AHT_MODE_NORMAL;
    aht_dev.type = AHT_TYPE_AHT20;

    // Ora i2cdev troverà il puntatore del mutex a NULL e lo allocherà correttamente in RAM
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
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT, 
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, TEMT6000_ADC_CHANNEL, &config));

    // 3. Configurazione della Calibrazione
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

    // MODIFICA QUI: 
    // 1. Cambiamo [&] in [adc_handle, cali_handle, do_calibration] per copiare i descrittori per valore.
    //    In questo modo rimarranno salvati persistentemente all'interno dell'oggetto della lambda.
    startRepeatingTask("temt6000_read", 1000, [adc_handle, cali_handle, do_calibration]() {
        // 2. Spostiamo le variabili di supporto qui dentro, così vengono allocate ad ogni ciclo
        int adc_raw = 0;
        int voltage = 0;

        // Ora adc_handle è una copia valida e non memoria corrotta dello stack passato
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, TEMT6000_ADC_CHANNEL, &adc_raw));
        
        if (do_calibration) {
            adc_cali_raw_to_voltage(cali_handle, adc_raw, &voltage);
            NLOGD("Valore Grezzo: %d | Tensione: %d mV", adc_raw, voltage);
        } else {
            NLOGD("Valore Grezzo: %d (Calibrazione non disponibile)", adc_raw);
        }
    });
}

void MainNode::initI2cDevices() {
    // Inizializza eventuali dispositivi I2C aggiuntivi qui
    NLOGD("MainNode: initI2cDevices called");
}

void MainNode::enable() {
    NLOGD("MainNode::enable called");
    CarduinoNode::enable();

    Message m(Priority::L.id, Node::BROADCAST.id, EventRegistry::createById(EV_GET_HELLOS));
    sendMessage(m);
}