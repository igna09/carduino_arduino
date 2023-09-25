#pragma once

// Serial debug
#if DEBUG == 1 // Compile Serial
#define debug(in) Serial.print(in)
#define debughex(in) Serial.print(in, HEX)
#define debugln(in) Serial.println(in)
#define debughexln(in) Serial.println(in, HEX)
#define debugstrnum(str, num) \
  do                          \
  {                           \
    debug(str);               \
    debug(num);               \
  } while (0)
#define debugstrnumln(str, num) \
  do                            \
  {                             \
    debug(str);                 \
    debugln(num);               \
  } while (0)
#define debugstrnumhex(str, num) \
  do                             \
  {                              \
    debug(str);                  \
    debughex(num);               \
  } while (0)
#define debugstrnumhexln(str, num) \
  do                               \
  {                                \
    debug(str);                    \
    debughexln(num);               \
  } while (0)
#else // Do not compile serial to save space
#define debug(in)
#define debughex(in)
#define debugln(in)
#define debughexln(in)
#define debugstrnum(str, num)
#define debugstrnumln(str, num)
#define debugstrnumhex(str, num)
#define debugstrnumhexln(str, num)
#endif

/* Config */
#define DEBUG 1                  // 1 = enable Serial.print
#define ECU_TIMEOUT 1300         // Most commonly is 1100ms
#define DISPLAY_FRAME_LENGTH 177 // Length of 1 frame in ms
#define DISPLAY_MAX_X 16
#define DISPLAY_MAX_Y 2
#define BUTTON_RIGHT(in) (in < 60)
#define BUTTON_UP(in) (in >= 60 && in < 200)
#define BUTTON_DOWN(in) (in >= 200 && in < 400)
#define BUTTON_LEFT(in) (in >= 400 && in < 600)
#define BUTTON_SELECT(in) (in >= 600 && in < 800)

#include <Arduino.h>
#include "../CarduinoNode/CarduinoNode.h"
#include <SoftwareSerial.h>

class KlineCarduinoNode : CarduinoNode {
    public:
        KlineCarduinoNode(int cs, int interruptPin, char *ssid,  char *password);

        void loop();

    private:
        static const EspSoftwareSerial::Config swSerialConfig = EspSoftwareSerial::SWSERIAL_8E1;
        static const SerialConfig hwSerialConfig = ::SERIAL_8E1;

        uint8_t simulation_mode_active = false;
        uint8_t auto_setup = false; // Hold select button to auto initiate connect to:
        const bool AUTO_SETUP_SIMULATION_MODE_ACTIVE = false;
        const uint8_t AUTO_SETUP_ADDRESS = 0x17;
        const uint16_t AUTO_SETUP_BAUD_RATE = 10400;
        // const uint8_t AUTO_SETUP_ADDRESS = 0x01
        // const uint16_t AUTO_SETUP_BAUD_RATE = 9600;
        uint8_t pin_rx = 3; // Receive // Black
        uint8_t pin_tx = 2; // Transmit // White

        /* ECU Addresses. See info.txt in root directory for details on the values of each group. */
        static const uint8_t ADDR_ENGINE = 0x01;
        static const uint8_t ADDR_ABS_BRAKES = 0x03;
        static const uint8_t ADDR_AUTO_HVAC = 0x08;
        static const uint8_t ADDR_INSTRUMENTS = 0x17;
        static const uint8_t ADDR_CENTRAL_CONV = 0x46;

        /* --------------------------EDIT BELOW ONLY TO FIX STUFF-------------------------------------- */

        // Constants
        static const uint8_t KWP_MODE_ACK = 0;         // Send ack block to keep connection alive
        static const uint8_t KWP_MODE_READSENSORS = 1; // Read all sensors from the connected ADDR
        static const uint8_t KWP_MODE_READGROUP = 2;   // Read only specified group from connected ADDR

        // Backend
        SoftwareSerial *obd;
        uint32_t connect_time_start;
        uint16_t timeout_to_add = 1100; // Wikipedia
        uint16_t button_timeout = 222;
        uint8_t screen_current = 0;
        uint8_t menu_current = 0;
        uint8_t kwp_mode = KWP_MODE_READSENSORS;
        uint8_t kwp_mode_last = kwp_mode;
        uint8_t kwp_group = 1; // Dont go to group 0 its not good.

        // OBD Connection variables
        bool connected = false;
        uint16_t baud_rate = 0; // 1200, 2400, 4800, 9600, 10400
        uint8_t block_counter = 0;
        uint8_t group_current = 1;
        bool group_current_updated = false;
        uint8_t addr_selected = 0x00; // Selected ECU address to connect to, see ECU Addresses constants
        bool com_error = false;

        bool group_side = false; // 0 = display k0 and k1. 1= display k2 and k3
        bool group_side_updated = false;

        uint8_t k_temp[4] = {0, 0, 0, 0};
        bool k_temp_updated = false;
        float v_temp[4] = {123.4, 123.4, 123.4, 123.4};
        bool v_temp_updated = false;
        String unit_temp[4] = {"N/A", "N/A", "N/A", "N/A"};
        bool unit_temp_updated = false;

        // DTC error
        uint16_t dtc_errors[16] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
        bool dtc_errors_updated = false;
        uint8_t dtc_status_bytes[16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        bool dtc_status_bytes_updated = false;

        /* ADDR_INSTRUMENTS measurement group entries, chronologically 0-3 in each group */

        // Group 1
        uint16_t vehicle_speed = 0;
        bool vehicle_speed_updated = false;
        uint16_t engine_rpm = 0;
        bool engine_rpm_updated = false;
        uint16_t oil_pressure_min = 0;
        bool oil_pressure_min_updated = false;
        uint32_t time_ecu = 0;
        bool time_ecu_updated = false;
        // Group 2
        uint32_t odometer = 0;
        bool odometer_updated = false;
        uint32_t odometer_start = odometer;
        uint8_t fuel_level = 0;
        bool fuel_level_updated = false;
        uint8_t fuel_level_start = fuel_level;
        uint16_t fuel_sensor_resistance = 0;
        bool fuel_sensor_resistance_updated = false; // Ohm
        uint8_t ambient_temp = 0;
        bool ambient_temp_updated = false;
        // Group 3 (Only 0-2)
        uint8_t coolant_temp = 0;
        bool coolant_temp_updated = false;
        uint8_t oil_level_ok = 0;
        bool oil_level_ok_updated = false;
        uint8_t oil_temp = 0;
        bool oil_temp_updated = false;
        // ADDR_ENGINE measurement group entries TODO
        // Group 1 (0th is engine rpm)
        uint8_t temperature_unknown_1 = 0; // 1
        bool temperature_unknown_1_updated = false;
        int8_t lambda = 0; // 2
        bool lambda_updated = false;
        bool exhaust_gas_recirculation_error = false; // 3, 8 bit encoding originally
        bool oxygen_sensor_heating_error = false;
        bool oxgen_sensor_error = false;
        bool air_conditioning_error = false;
        bool secondary_air_injection_error = false;
        bool evaporative_emissions_error = false;
        bool catalyst_heating_error = false;
        bool catalytic_converter = false;
        bool error_bits_updated = false;
        String bits_as_string = "        ";
        // Group 3 (Only 1-3 no 0th)
        uint16_t pressure = 0; // mbar
        bool pressure_updated = false;
        float tb_angle = 0;
        bool tb_angle_updated = false;
        float steering_angle = 0;
        bool steering_angle_updated = false;
        // Group 4 (Only 1-3 no 0th)
        float voltage = 0;
        bool voltage_updated = false;
        uint8_t temperature_unknown_2 = 0;
        bool temperature_unknown_2_updated = false;
        uint8_t temperature_unknown_3 = 0;
        bool temperature_unknown_3_updated = false;
        // Group 6 (Only 1 and 3)
        uint16_t engine_load = 0; // 1
        bool engine_load_updated = false;
        int8_t lambda_2 = 0; // 3
        bool lambda_2_updated = false;

        /* Computed Stats */
        uint32_t elapsed_seconds_since_start = 0;
        bool elapsed_seconds_since_start_updated = false;
        uint16_t elpased_km_since_start = 0;
        bool elpased_km_since_start_updated = false;
        uint8_t fuel_burned_since_start = 0;
        bool fuel_burned_since_start_updated = false;
        float fuel_per_100km = 0;
        bool fuel_per_100km_updated = false;
        float fuel_per_hour = 0;
        bool fuel_per_hour_updated = false;

        bool engine_rpm_switch = true;
        bool vehicle_speed_switch = true;
        bool coolant_temp_switch = true;
        bool oil_temp_switch = true;
        bool oil_level_ok_switch = true;
        bool fuel_level_switch = true;

        void invert_group_side();
        void reset_temp_group_array();
        void reset_dtc_status_errors_array();
        void reset_dtc_status_errors_array_random();
        bool read_sensors(int group);
        uint8_t count_digit(int n);
        void increment_block_counter();
        void simulate_values_helper(uint8_t &val, uint8_t amount_to_change, bool &val_switch, bool &val_updated, uint8_t maximum, uint8_t minimum = 0);
        void simulate_values_helper(uint16_t &val, uint8_t amount_to_change, bool &val_switch, bool &val_updated, uint16_t maximum, uint16_t minimum = 0);
        void simulate_values();
        void compute_values();
        void disconnect();
        int available();
        void OBD_write(uint8_t data);
        int16_t OBD_read();
        void KWP_5baud_send(uint8_t data);
        bool KWP_send_block(uint8_t *s, int size);
        bool KWP_send_ACK_block();
        bool KWP_send_DTC_read_block();
        bool KWP_send_DTC_delete_block();
        bool KWP_receive_block(uint8_t s[], int maxsize, int &size, int source = -1, bool initialization_phase = false);
        bool KWP_error_block();
        bool KWP_5baud_init(uint8_t addr);
        bool KWP_receive_ACK_block();
        bool read_connect_blocks(bool initialization_phase = false);
        bool keep_alive();
        int8_t read_DTC_codes();
        bool delete_DTC_codes();
        bool kwp_exit();
        bool obd_connect();
        bool connect();
};
