/*
OBDisplay.cpp

See readme for more info.

See https://www.blafusel.de/obd/obd2_kw1281.html for info on OBD KWP1281 protocol.

Ignore compile warnings.
*/

// Arduino/Standard Libraries
#include <Arduino.h>
//  Third party libraries
#include "NewSoftwareSerial.h"

/* --------------------------EDIT THE FOLLOWING TO YOUR LIKING-------------------------------------- */

/* Config */
#define DEBUG 1                  // 1 = enable Serial.print
#define ECU_TIMEOUT 1300         // Most commonly is 1100ms
#define DISPLAY_FRAME_LENGTH 177 // Length of 1 frame in ms
#define DISPLAY_MAX_X 16
#define DISPLAY_MAX_Y 2
uint8_t simulation_mode_active = false;
uint8_t auto_setup = false; // Hold select button to auto initiate connect to:
const bool AUTO_SETUP_SIMULATION_MODE_ACTIVE = false;
const uint8_t AUTO_SETUP_ADDRESS = 0x17;
const uint16_t AUTO_SETUP_BAUD_RATE = 10400;
// const uint8_t AUTO_SETUP_ADDRESS = 0x01
// const uint16_t AUTO_SETUP_BAUD_RATE = 9600;
#define BUTTON_RIGHT(in) (in < 60)
#define BUTTON_UP(in) (in >= 60 && in < 200)
#define BUTTON_DOWN(in) (in >= 200 && in < 400)
#define BUTTON_LEFT(in) (in >= 400 && in < 600)
#define BUTTON_SELECT(in) (in >= 600 && in < 800)

uint8_t pin_rx = 3; // Receive // Black
uint8_t pin_tx = 2; // Transmit // White

/* ECU Addresses. See info.txt in root directory for details on the values of each group. */
const uint8_t ADDR_ENGINE = 0x01;
const uint8_t ADDR_ABS_BRAKES = 0x03;
const uint8_t ADDR_AUTO_HVAC = 0x08;
const uint8_t ADDR_INSTRUMENTS = 0x17;
const uint8_t ADDR_CENTRAL_CONV = 0x46;

/* --------------------------EDIT BELOW ONLY TO FIX STUFF-------------------------------------- */

// Constants
const uint8_t KWP_MODE_ACK = 0;         // Send ack block to keep connection alive
const uint8_t KWP_MODE_READSENSORS = 1; // Read all sensors from the connected ADDR
const uint8_t KWP_MODE_READGROUP = 2;   // Read only specified group from connected ADDR
const char CHAR_YES = 'Y';
const char CHAR_NO = 'N';
char STRING_ERR[] = "ERR";

// Backend
NewSoftwareSerial obd(pin_rx, pin_tx, false); // rx, tx, inverse logic = false
uint32_t connect_time_start = millis();
uint16_t timeout_to_add = 1100; // Wikipedia
uint16_t button_timeout = 222;
uint8_t screen_current = 0;
uint8_t menu_current = 0;
uint8_t kwp_mode = KWP_MODE_READSENSORS;
uint8_t kwp_mode_last = kwp_mode;
uint8_t kwp_group = 1; // Dont go to group 0 its not good.
// ---------------Menu Screen-----------
uint8_t menu_cockpit_screen = 0;
uint8_t menu_cockpit_screen_max = 4;
uint8_t menu_experimental_screen = 0;
uint8_t menu_experimental_screen_max = 64;
uint8_t menu_debug_screen = 0;
uint8_t menu_debug_screen_max = 4;
uint8_t menu_dtc_screen = 0;
uint8_t menu_dtc_screen_max = 9;
uint8_t menu_settings_screen = 0;
uint8_t menu_settings_screen_max = 10;
bool menu_screen_switch = false;
// -------------------------------------
uint32_t endTime = 0;
uint8_t menu_max = 4;
uint8_t menu = 0;
bool menu_switch = false;
uint16_t connection_attempts_counter = 0;
uint64_t button_read_time = 0;

// OBD Connection variables
bool connected = false;
uint16_t baud_rate = 0; // 1200, 2400, 4800, 9600, 10400
uint8_t block_counter = 0;
uint8_t group_current = 1;
bool group_current_updated = false;
uint8_t addr_selected = 0x00; // Selected ECU address to connect to, see ECU Addresses constants
bool com_error = false;

/* Temporary Measurements for if you want to find out which values show up in your groups in a desired ECU address.
Just uncomment and add the logic in read_sensors(). This can also be done with VCDS or other tools.*/
bool group_side = false; // 0 = display k0 and k1. 1= display k2 and k3
bool group_side_updated = false;
void invert_group_side()
{
  group_side = !group_side;
  group_side_updated = true;
  menu_switch = true;
}
uint8_t k_temp[4] = {0, 0, 0, 0};
bool k_temp_updated = false;
float v_temp[4] = {123.4, 123.4, 123.4, 123.4};
bool v_temp_updated = false;
String unit_temp[4] = {"N/A", "N/A", "N/A", "N/A"};
bool unit_temp_updated = false;
void reset_temp_group_array()
{
  for (uint8_t i = 0; i < 4; i++)
  {
    k_temp[i] = 0;
    v_temp[i] = 123.4;
    unit_temp[i] = "N/A";
  }
}
// DTC error
uint16_t dtc_errors[16] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
bool dtc_errors_updated = false;
uint8_t dtc_status_bytes[16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
bool dtc_status_bytes_updated = false;
void reset_dtc_status_errors_array()
{
  for (uint8_t i = 0; i < 16; i++)
  {
    dtc_errors[i] = (uint16_t)0xFFFF;
    dtc_status_bytes[i] = 0xFF;
  }
}
void reset_dtc_status_errors_array_random()
{
  for (uint8_t i = 0; i < 16; i++)
  {
    dtc_errors[i] = (uint16_t)(i * 1000);
    dtc_status_bytes[i] = i * 10;
  }
}

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

/* Display */
uint32_t display_frame_timestamp = millis();

uint8_t count_digit(int n)
{
  if (n == 0)
    return 1;
  uint8_t count = 0;
  while (n != 0)
  {
    n = n / 10;
    ++count;
  }
  return count;
}

/**
 * Increment block counter. Min: 0, Max: 254.
 * Counts the current block number and is passed in each block.
 * A wrong block counter will result in communication errors.
 */
void increment_block_counter()
{
  if (block_counter >= 255)
  {
    block_counter = 0;
  }
  else
  {
    block_counter++;
  }
}

void increment_group_current()
{
  if (group_current > menu_experimental_screen_max)
  {
    group_current = 1;
  }
  else
  {
    group_current++;
  }
  group_current_updated = true;
}

void decrement_group_current()
{
  if (group_current <= 1)
  {
    group_current = menu_experimental_screen_max;
  }
  else
  {
    group_current--;
  }
  group_current_updated = true;
}

// Display functions
void increment_menu(uint8_t &menu_current, uint8_t &menu_current_max)
{
  if (menu_current >= menu_current_max)
  {
    menu_current = 0;
  }
  else
  {
    menu_current++;
  }
  menu_switch = true;
}
void decrement_menu(uint8_t &menu_current, uint8_t &menu_current_max)
{
  if (menu_current == 0)
  {
    menu_current = menu_current_max;
  }
  else
  {
    menu_current--;
  }
  menu_switch = true;
}

// --------------------------------------------------------------------------------------------------
//                            DISPLAY CODE
// --------------------------------------------------------------------------------------------------

bool engine_rpm_switch = true;
bool vehicle_speed_switch = true;
bool coolant_temp_switch = true;
bool oil_temp_switch = true;
bool oil_level_ok_switch = true;
bool fuel_level_switch = true;
void simulate_values_helper(uint8_t &val, uint8_t amount_to_change, bool &val_switch, bool &val_updated, uint8_t maximum, uint8_t minimum = 0)
{
  if (val_switch)
    val += amount_to_change;
  else
    val -= amount_to_change;

  val_updated = true;

  if (val_switch && val >= maximum)
    val_switch = false;
  else if (!val_switch && val <= minimum)
    val_switch = true;
}
void simulate_values_helper(uint16_t &val, uint8_t amount_to_change, bool &val_switch, bool &val_updated, uint16_t maximum, uint16_t minimum = 0)
{
  if (val_switch)
    val += amount_to_change;
  else
    val -= amount_to_change;

  val_updated = true;

  if (val_switch && val >= maximum)
    val_switch = false;
  else if (!val_switch && val <= minimum)
    val_switch = true;
}
void simulate_values()
{
  increment_block_counter();                                                                  // Simulate some values
  simulate_values_helper(vehicle_speed, 1, vehicle_speed_switch, vehicle_speed_updated, 200); // Vehicle speed
  simulate_values_helper(engine_rpm, 87, engine_rpm_switch, engine_rpm_updated, 7100);        // Engine RPM
  simulate_values_helper(coolant_temp, 1, coolant_temp_switch, coolant_temp_updated, 160);    // Coolant temperature
  simulate_values_helper(oil_temp, 1, oil_temp_switch, oil_temp_updated, 160);                // Oil Temperature
  simulate_values_helper(oil_level_ok, 1, oil_level_ok_switch, oil_level_ok_updated, 8);      // Oil level ok
  simulate_values_helper(fuel_level, 1, fuel_level_switch, fuel_level_updated, 57);           // Fuel
}
void compute_values()
{
  elapsed_seconds_since_start = ((millis() - connect_time_start) / 1000);
  elapsed_seconds_since_start_updated = true;
  elpased_km_since_start = odometer - odometer_start;
  elpased_km_since_start_updated = true;
  fuel_burned_since_start = abs(fuel_level_start - fuel_level);
  fuel_burned_since_start_updated = true;
  fuel_per_100km = (100 / elpased_km_since_start) * fuel_burned_since_start;
  fuel_per_100km_updated = true;
  fuel_per_hour = (3600 / elapsed_seconds_since_start) * fuel_burned_since_start;
  fuel_per_hour_updated = true;
}

// --------------------------------------------------------------------------------------------------
//                            OBD CODE
// --------------------------------------------------------------------------------------------------

void disconnect()
{
  obd.end();
  debug(F("Disconnected. Block counter: "));
  debug(block_counter);
  debug(F(". Connected: "));
  debug(connected);
  debug(F(". Available: "));
  debugln(obd.available());

  block_counter = 0;
  connected = false;
  connect_time_start = 0;
  odometer_start = 0;
  fuel_level_start = 0;
  menu = 0;
  menu_switch = false;
  button_read_time = 0;
  addr_selected = 0x00;
  screen_current = 0;
  menu_current = 0;
}

/**
 * @brief Get number from OBD available, I still dont fully know what this means
 */
int available()
{
  return obd.available();
}

/**
 * @brief Write data to the ECU, wait 5ms before each write to ensure connectivity.
 *
 * @param data The data to send.
 */
void OBD_write(uint8_t data)
{
  // debug(F("->MCU: "));
  // debughexln(data);

  uint8_t to_delay = 5;
  switch (baud_rate)
  {
  case 1200:
  case 2400:
  case 4800:
    to_delay = 15; // For old ECU's
    break;
  case 9600:
    to_delay = 10;
    break;
  default:
    break;
  }

  delay(to_delay);
  obd.write(data);
}

/**
 * @brief Read OBD input from ECU
 *
 * @return uint8_t The incoming byte or -1 if timeout
 */
int16_t OBD_read()
{
  unsigned long timeout = millis() + timeout_to_add;
  while (!obd.available())
  {
    if (millis() >= timeout)
    {
      debugln(F("ERROR: OBD_read() timeout obd.available() = 0."));
      return -1;
    }
  }
  int16_t data = obd.read();
  // debug(F("ECU: "));
  // debughexln(data);

  return data;
}

/**
 * @brief Perform a 5 Baud init sequence to wake up the ECU
 * 5Bd, 7O1
 * @param data Which ECU Address to wake up.
 */
void KWP_5baud_send(uint8_t data)
{
  // // 1 start bit, 7 data bits, 1 parity, 1 stop bit
#define bitcount 10
  byte bits[bitcount];
  byte even = 1;
  byte bit;
  for (int i = 0; i < bitcount; i++)
  {
    bit = 0;
    if (i == 0)
      bit = 0;
    else if (i == 8)
      bit = even; // computes parity bit
    else if (i == 9)
      bit = 1;
    else
    {
      bit = (byte)((data & (1 << (i - 1))) != 0);
      even = even ^ bit;
    }

    debugstrnum(F(" "), bit);

    bits[i] = bit;
  }
  // now send bit stream
  for (int i = 0; i < bitcount + 1; i++)
  {
    if (i != 0)
    {
      // wait 200 ms (=5 baud), adjusted by latency correction
      delay(200);
      if (i == bitcount)
        break;
    }
    if (bits[i] == 1)
    {
      // high
      digitalWrite(pin_tx, HIGH);
    }
    else
    {
      // low
      digitalWrite(pin_tx, LOW);
    }
  }
  obd.flush();
}

/**
 * Helper method to send the addr as 5 baud to the ECU
 */
bool KWP_5baud_init(uint8_t addr)
{
  debug(F("5 baud: (0)"));
  KWP_5baud_send(addr);
  // debugln(F(" (9) END"));
  return true;
}

/**
 * @brief Send a request to the ECU
 *
 * @param s Array where the data is stored
 * @param size The size of the request
 * @return true If no errors occured, will resume
 * @return false If errors occured, will disconnect
 */
bool KWP_send_block(uint8_t *s, int size)
{
  debug(F("Sending "));
  for (uint8_t i = 0; i < size; i++)
  {
    debughex(s[i]);
    debug(F(" "));
  }
  debugln();

  for (uint8_t i = 0; i < size; i++)
  {
    uint8_t data = s[i];
    OBD_write(data);

    if (i < size - 1)
    {
      int16_t complement = OBD_read();
      if (s[2] == 0x06 && s[3] == 0x03 && complement == -1)
        return true; // KWP manual exit
      if (complement != (data ^ 0xFF))
      {
        debugstrnumhex(F("MCU: "), data);
        debugstrnumhex(F(" ECU: "), complement);
        debugln(F("ERROR: invalid complement"));
        return false;
      }
    }
  }
  increment_block_counter();
  return true;
}

/**
 * @brief The default way to keep the ECU awake is to send an Acknowledge Block.
 * Alternatives include Group Readings..
 *
 * @return true No errors
 * @return false Errors, disconnect
 */
bool KWP_send_ACK_block()
{
  debugln(F("Sending ACK block"));
  uint8_t buf[4] = {0x03, block_counter, 0x09, 0x03};
  return (KWP_send_block(buf, 4));
}

/**
 * Sends DTC read block and obtains all DTC errors
 */
bool KWP_send_DTC_read_block()
{
  debugln(F("Sending DTC read block"));
  uint8_t s[32] = {0x03, block_counter, 0x07, 0x03};
  return KWP_send_block(s, 4);
}

/**
 * Sends DTC delete block to clear all DTC errors
 */
bool KWP_send_DTC_delete_block()
{
  debugln(F("Sending DTC delete block"));
  uint8_t s[32] = {0x03, block_counter, 0x05, 0x03};
  return KWP_send_block(s, 4);
}

/**
 * @brief Recieve a response from the ECU
 *
 * @param s Array with stored response data
 * @param maxsize Max size of response to be expected
 * @param size Size of response
 * @param source 1 if this method is called from GroupMeasurements readsensors(group) to determine com errors
 * @return true No errors
 * @return false Errors
 */
bool KWP_receive_block(uint8_t s[], int maxsize, int &size, int source = -1, bool initialization_phase = false)
{
  bool ackeachbyte = false;
  int16_t data = 0;
  int recvcount = 0;
  if (size == 0)
    ackeachbyte = true;

  // debugstrnum(F(" - KWP_receive_block. Size: "), size);
  // debugstrnum(F(". Block counter: "), block_counter);

  if (size > maxsize)
  {
    debugln(F(" - KWPReceiveBlock error: Invalid maxsize"));
    return false;
  }
  unsigned long timeout = millis() + timeout_to_add;
  uint16_t temp_iteration_counter = 0;
  uint8_t temp_0x0F_counter = 0; // For communication errors in startup procedure (1200 baud)
  while ((recvcount == 0) || (recvcount != size))
  {
    while (obd.available())
    {

      data = OBD_read();
      if (data == -1)
      {
        debugln(F(" - KWPReceiveBlock error: (Available=0) or empty buffer!"));
        return false;
      }
      s[recvcount] = data;
      recvcount++;

      /* BAUD 1200 fix, may be useful for other bauds if com errors occur at sync bytes init */
      if ((baud_rate == 1200 || baud_rate == 2400 || baud_rate == 4800) && initialization_phase && (recvcount > maxsize))
      {
        if (data == 0x55)
        {
          temp_0x0F_counter = 0; // Reset
          s[0] = 0x00;
          s[1] = 0x00;
          s[2] = 0x00;
          size = 3;
          recvcount = 1;
          s[0] = 0x55;
          timeout = millis() + timeout_to_add;
        }
        else if (data == 0xFF)
        {
          temp_0x0F_counter = 0; // Skip
        }
        else if (data == 0x0F)
        {
          if (temp_0x0F_counter >= 1) // ACK
          {
            OBD_write(data ^ 0xFF);
            timeout = millis() + timeout_to_add;

            temp_0x0F_counter = 0;
          }
          else // Skip
          {
            temp_0x0F_counter++;
          }
        }
        else
        {
          temp_0x0F_counter = 0; // Skip
        }
        continue;
      }

      if ((size == 0) && (recvcount == 1))
      {
        if (source == 1 && (data != 0x0F || data != 0x03) && obd.available())
        {
          debugln(F(" - KWP_receive_block warn: Communication error occured, check block length!"));
          com_error = true;
          size = 6;
        }
        else
        {
          size = data + 1;
        }
        if (size > maxsize)
        {

          debugln(F(" - KWP_receive_block error: Invalid maxsize2"));
          return false;
        }
      }
      if (com_error)
      {
        if (recvcount == 1)
        {
          ackeachbyte = false;
        }
        else if (recvcount == 3)
        {
          ackeachbyte = true;
        }
        else if (recvcount == 4)
        {
          ackeachbyte = false;
        }
        else if (recvcount == 6)
        {
          ackeachbyte = true;
        }
        continue;
      }
      if ((ackeachbyte) && (recvcount == 2))
      {
        if (data != block_counter)
        {
          if (data == 0x00)
            block_counter = 0; // Reset BC because probably com error occured in init phase. Part of 1200BAUD fix
          else
          {
            debugstrnum(F(" - KWP_receive_block error: Invalid block counter. Expected: "), block_counter);
            debugstrnumln(F(" Is: "), data);
            return false;
          }
        }
      }
      if (((!ackeachbyte) && (recvcount == size)) || ((ackeachbyte) && (recvcount < size)))
      {
        OBD_write(data ^ 0xFF); // send complement ack
      }
      timeout = millis() + timeout_to_add;

      // debugstrnum(F(" - KWP_receive_block: Added timeout. ReceiveCount: "), (uint8_t)recvcount);
      // debug(F(". Processed data: "));
      // debughex(data);
      // debugstrnumln(F(". ACK compl: "), ((!ackeachbyte) && (recvcount == size)) || ((ackeachbyte) && (recvcount < size)));
    }

    if (millis() >= timeout)
    {
      debug(F(" - KWP_receive_block: Timeout overstepped on iteration "));
      debug(temp_iteration_counter);
      debug(F(" with receivecount "));
      debugln(recvcount);

      if (recvcount == 0)
      {
        debugln(F("No connection to ECU! Check wiring."));
      }
      return false;
    }
    temp_iteration_counter++;
  }
  // show data
  // debug(F("IN: size = "));
  // debug(size);
  // debug(F(" data = "));
  // for (uint8_t i = 0; i < size; i++)
  //{
  //  uint8_t data = s[i];
  //  debughex(data);
  //  debug(F(" "));
  //}
  // debugln();

  increment_block_counter();
  return true;
}

/**
 * KWP error encountered procedure
 */
bool KWP_error_block()
{
  uint8_t s[64] = {0x03, block_counter, 0x00, 0x03};
  if (!KWP_send_block(s, 4))
  {
    com_error = false;
    return false;
  }
  block_counter = 0;
  com_error = false;
  int size2 = 0;
  return KWP_receive_block(s, 64, size2);
}

bool KWP_receive_ACK_block()
{
  // --------- Expect response acknowledge ----------------
  uint8_t buf[32];
  int size2 = 0;
  if (!KWP_receive_block(buf, 32, size2))
  {
    return false;
  }
  if (buf[2] != 0x09)
  {
    debug(F(" - Error receiving ACK procedure"));
    return false;
  }
  if (com_error)
  {
    KWP_error_block();
    return false;
  }
  return true;
}

/**
 * @brief Last step in initial ECU startup sequence.
 *
 * @return true no errors
 * @return false errors
 */
bool read_connect_blocks(bool initialization_phase = false)
{
  // debugln(F(" - Readconnectblocks"));

  // String info;
  while (true)
  {
    int size = 0;
    uint8_t s[64];
    if (!(KWP_receive_block(s, 64, size, -1, initialization_phase)))
    {
      // Example debug copypaste
      /*       debugln(F("------"));
            debugln(F(" - Readconnectblocks ERROR in KWP receive block"));
            debugstrnumln(F("initialization_phase ="), initialization_phase);
            debugstrnumln(F("maxsize ="), 64);
            debugstrnumln(F("size ="), size);
            debugstrnumln(F("source ="), -1);
            debugstrnumln(F("ECU_receive_counter ="), ECU_receive_counter);
            debugstrnumln(F("ECU_transmit_counter ="), ECU_transmit_counter);
            debugstrnumln(F("ECU_anomaly_counter ="), ECU_anomaly_counter);
            size_t array_length = sizeof(s) / sizeof(s[0]);
            debugstrnumln(F("s[] length ="), array_length);
            debug(F("s[] = "));
            for (int i = 0; i < array_length; i++)
            {
              if (i < size)
              {
                debug(F(": "));
                debughex(s[i]);
              }
            }
            debugln(F(" "));
            debugln(F("------")); */
      return false;
    }
    if (size == 0)
      return false;
    if (s[2] == 0x09)
      break;
    if (s[2] != 0xF6)
    {
      debug(F(" - Readconnectblocks ERROR: unexpected answer: "));
      debugstrnumhexln(F("should be 0xF6 but is "), s[2]);
      return false;
    }
    // String text = String((char)s);
    // info += text.substring(3, size - 2);
    if (!KWP_send_ACK_block())
      return false;
  }
  // debugstrnum(F("label = "), info);
  return true;
}

/**
 * @brief Perform a measurement group reading and safe the value to the corresponding variable.
 * Each group contains 4 values. Refer to your Label File for further information and order.
 * @param group The group to read
 * @return true no errors
 * @return false errors
 */
bool read_sensors(int group)
{

  for (int i = 0; i <= 3; i++)
  {
    k_temp[i] = 0;
    v_temp[i] = -1;
    unit_temp[i] = "ERR";
  }

  uint8_t s[64];
  s[0] = 0x04;
  s[1] = block_counter;
  s[2] = 0x29;
  s[3] = group;
  s[4] = 0x03;
  if (!KWP_send_block(s, 5))
    return false;
  int size = 0;
  if (!KWP_receive_block(s, 64, size, 1))
  {
    return false;
  }
  if (com_error)
  {
    // Kommunikationsfehler
    uint8_t s[64];
    s[0] = 0x03;
    s[1] = block_counter;
    s[2] = 0x00;
    s[3] = 0x03;
    if (!KWP_send_block(s, 4))
    {
      com_error = false;
      return false;
    }
    block_counter = 0;
    com_error = false;
    int size2 = 0;
    if (!KWP_receive_block(s, 64, size2))
    {
      return false;
    }
  }
  if (s[2] != 0xE7)
  {

    debugln(F("ERROR: invalid answer 0xE7. Full s[]:"));
    for (uint8_t i = 0; i < 16; i++) {
      debugstrnumhex(F("| "), s[i]);
    }
    debugln();
    if (baud_rate != 1200)
    {
      delay(2000);
      // errorData++;
      return false;
    }
    // Baud 1200 trial and error
    if (s[0] != 0x0F) {
      debugln(F("Exiting due to s[0] != 0x0F in group reading in baud 1200"));
      return false;
    }
  }
  int count = (size - 4) / 3;
  debugstrnumln(F("count="), count);
  for (int idx = 0; idx < count; idx++)
  {
    byte k = s[3 + idx * 3];
    byte a = s[3 + idx * 3 + 1];
    byte b = s[3 + idx * 3 + 2];
    String n;
    float v = 0;

    debug(F("type="));
    debug(k);
    debug(F("  a="));
    debug(a);
    debug(F("  b="));
    debug(b);
    debug(F("  text="));

    String t = "";
    String units = "";
    char buf[32];
    switch (k)
    {
    case 1:
      v = 0.2 * a * b;
      units = F("rpm");
      break;
    case 2:
      v = a * 0.002 * b;
      units = F("%%");
      break;
    case 3:
      v = 0.002 * a * b;
      units = F("Deg");
      break;
    case 4:
      v = abs(b - 127) * 0.01 * a;
      units = F("ATDC");
      break;
    case 5:
      v = a * (b - 100) * 0.1;
      units = F("°C");
      break;
    case 6:
      v = 0.001 * a * b;
      units = F("V");
      break;
    case 7:
      v = 0.01 * a * b;
      units = F("km/h");
      break;
    case 8:
      v = 0.1 * a * b;
      units = F(" ");
      break;
    case 9:
      v = (b - 127) * 0.02 * a;
      units = F("Deg");
      break;
    case 10:
      if (b == 0)
        t = F("COLD");
      else
        t = F("WARM");
      break;
    case 11:
      v = 0.0001 * a * (b - 128) + 1;
      units = F(" ");
      break;
    case 12:
      v = 0.001 * a * b;
      units = F("Ohm");
      break;
    case 13:
      v = (b - 127) * 0.001 * a;
      units = F("mm");
      break;
    case 14:
      v = 0.005 * a * b;
      units = F("bar");
      break;
    case 15:
      v = 0.01 * a * b;
      units = F("ms");
      break;
    case 18:
      v = 0.04 * a * b;
      units = F("mbar");
      break;
    case 19:
      v = a * b * 0.01;
      units = F("l");
      break;
    case 20:
      v = a * (b - 128) / 128;
      units = F("%%");
      break;
    case 21:
      v = 0.001 * a * b;
      units = F("V");
      break;
    case 22:
      v = 0.001 * a * b;
      units = F("ms");
      break;
    case 23:
      v = b / 256 * a;
      units = F("%%");
      break;
    case 24:
      v = 0.001 * a * b;
      units = F("A");
      break;
    case 25:
      v = (b * 1.421) + (a / 182);
      units = F("g/s");
      break;
    case 26:
      v = float(b - a);
      units = F("C");
      break;
    case 27:
      v = abs(b - 128) * 0.01 * a;
      units = F("°");
      break;
    case 28:
      v = float(b - a);
      units = F(" ");
      break;
    case 30:
      v = b / 12 * a;
      units = F("Deg k/w");
      break;
    case 31:
      v = b / 2560 * a;
      units = F("°C");
      break;
    case 33:
      v = 100 * b / a;
      units = F("%%");
      break;
    case 34:
      v = (b - 128) * 0.01 * a;
      units = F("kW");
      break;
    case 35:
      v = 0.01 * a * b;
      units = F("l/h");
      break;
    case 36:
      v = ((unsigned long)a) * 2560 + ((unsigned long)b) * 10;
      units = F("km");
      break;
    case 37:
      v = b;
      break; // oil pressure ?!
    // ADP: FIXME!
    /*case 37: switch(b){
             case 0: sprintf(buf, F("ADP OK (%d,%d)"), a,b); t=String(buf); break;
             case 1: sprintf(buf, F("ADP RUN (%d,%d)"), a,b); t=String(buf); break;
             case 0x10: sprintf(buf, F("ADP ERR (%d,%d)"), a,b); t=String(buf); break;
             default: sprintf(buf, F("ADP (%d,%d)"), a,b); t=String(buf); break;
          }*/
    case 38:
      v = (b - 128) * 0.001 * a;
      units = F("Deg k/w");
      break;
    case 39:
      v = b / 256 * a;
      units = F("mg/h");
      break;
    case 40:
      v = b * 0.1 + (25.5 * a) - 400;
      units = F("A");
      break;
    case 41:
      v = b + a * 255;
      units = F("Ah");
      break;
    case 42:
      v = b * 0.1 + (25.5 * a) - 400;
      units = F("Kw");
      break;
    case 43:
      v = b * 0.1 + (25.5 * a);
      units = F("V");
      break;
    case 44:
      sprintf(buf, "%2d:%2d", a, b);
      t = String(buf);
      break;
    case 45:
      v = 0.1 * a * b / 100;
      units = F(" ");
      break;
    case 46:
      v = (a * b - 3200) * 0.0027;
      units = F("Deg k/w");
      break;
    case 47:
      v = (b - 128) * a;
      units = F("ms");
      break;
    case 48:
      v = b + a * 255;
      units = F(" ");
      break;
    case 49:
      v = (b / 4) * a * 0.1;
      units = F("mg/h");
      break;
    case 50:
      v = (b - 128) / (0.01 * a);
      units = F("mbar");
      break;
    case 51:
      v = ((b - 128) / 255) * a;
      units = F("mg/h");
      break;
    case 52:
      v = b * 0.02 * a - a;
      units = F("Nm");
      break;
    case 53:
      v = (b - 128) * 1.4222 + 0.006 * a;
      units = F("g/s");
      break;
    case 54:
      v = a * 256 + b;
      units = F("count");
      break;
    case 55:
      v = a * b / 200;
      units = F("s");
      break;
    case 56:
      v = a * 256 + b;
      units = F("WSC");
      break;
    case 57:
      v = a * 256 + b + 65536;
      units = F("WSC");
      break;
    case 59:
      v = (a * 256 + b) / 32768;
      units = F("g/s");
      break;
    case 60:
      v = (a * 256 + b) * 0.01;
      units = F("sec");
      break;
    case 62:
      v = 0.256 * a * b;
      units = F("S");
      break;
    case 64:
      v = float(a + b);
      units = F("Ohm");
      break;
    case 65:
      v = 0.01 * a * (b - 127);
      units = F("mm");
      break;
    case 66:
      v = (a * b) / 511.12;
      units = F("V");
      break;
    case 67:
      v = (640 * a) + b * 2.5;
      units = F("Deg");
      break;
    case 68:
      v = (256 * a + b) / 7.365;
      units = F("deg/s");
      break;
    case 69:
      v = (256 * a + b) * 0.3254;
      units = F("Bar");
      break;
    case 70:
      v = (256 * a + b) * 0.192;
      units = F("m/s^2");
      break;
    default:
      sprintf(buf, "%2x, %2x      ", a, b);
      break;
    }

    /*
     * Update k_temp and v_temp and unit_temp
     */
    if (k_temp[idx] != k)
    {
      k_temp[idx] = k;
      k_temp_updated = true;
    }
    if (v_temp[idx] != v)
    {
      v_temp[idx] = v;
      v_temp_updated = true;
    }
    if (unit_temp[idx] != units)
    {
      unit_temp[idx] = units;
      unit_temp_updated = true;
    }

    /*
     *  Add here the values from your label file for each address.
     */
    switch (addr_selected)
    {
    case ADDR_INSTRUMENTS:
      switch (group)
      {
      case 1:
        switch (idx)
        {
        case 0:
          // 0.0 km/h Speed
          if (vehicle_speed != (uint16_t)v)
          {
            vehicle_speed = (uint16_t)v;
            vehicle_speed_updated = true;
          }
          break;
        case 1:
          // 0 /min Engine Speed
          if (engine_rpm != (uint16_t)v)
          {
            engine_rpm = (uint16_t)v;
            engine_rpm_updated = true;
          }
          break;
        case 2:
          // Oil Pr. 2 < min (Oil pressure 0.9 bar)
          if (oil_pressure_min != (uint16_t)v)
          {
            oil_pressure_min = (uint16_t)v;
            oil_pressure_min_updated = true;
          }
          break;
        case 3:
          // 21:50 Time
          if (time_ecu != (uint32_t)v)
          {
            time_ecu = (uint32_t)v;
            time_ecu_updated = true;
          }
          break;
        }
        break;
      case 2:
        switch (idx)
        {
        case 0:
          // 101010 Odometer
          if (odometer != (uint32_t)v)
          {
            odometer = (uint32_t)v;
            odometer_updated = true;
          }
          if (millis() - connect_time_start < 10000)
          {
            odometer_start = odometer;
          }
          break;
        case 1:
          // 9 l Fuel level
          if (fuel_level != (uint16_t)v)
          {
            fuel_level = (uint16_t)v;
            fuel_level_updated = true;
          }
          if (millis() - connect_time_start < 10000)
          {
            fuel_level_start = fuel_level;
          }
          break;
        case 2:
          // 93 ohms Fuel Sender Resistance
          if (fuel_sensor_resistance != (uint16_t)v)
          {
            fuel_sensor_resistance = (uint16_t)v;
            fuel_sensor_resistance_updated = true;
          }
          break;
        case 3:
          // 0°C Ambient Temperature
          if (ambient_temp != (uint8_t)v)
          {
            ambient_temp = (uint8_t)v;
            ambient_temp_updated = true;
          }
          break;
        }
        break;
      case 3:
        switch (idx)
        {
        case 0:
          // 12.0°C Coolant temp.
          if (coolant_temp != (uint8_t)v)
          {
            coolant_temp = (uint8_t)v;
            coolant_temp_updated = true;
          }
          break;
        case 1:
          // OK Oil Level (OK/n.OK)
          if (oil_level_ok != (uint8_t)v)
          {
            oil_level_ok = (uint8_t)v;
            oil_level_ok_updated = true;
          }
          break;
        case 2:
          // 11.0°C Oil temp
          if (oil_temp != (uint8_t)v)
          {
            oil_temp = (uint8_t)v;
            oil_temp_updated = true;
          }
          break;
        case 3:
          // N/A
          break;
        }
        break;
      }
      break;
    case ADDR_ENGINE:
      switch (group)
      {
      case 1:
        switch (idx)
        {
        case 0:
          // /min RPM
          if (engine_rpm != (uint16_t)v)
          {
            engine_rpm = (uint16_t)v;
            engine_rpm_updated = true;
          }
          break;
        case 1:
          // 0 /min Engine Speed
          if (temperature_unknown_1 != (uint8_t)v)
          {
            temperature_unknown_1 = (uint8_t)v;
            temperature_unknown_1_updated = true;
          }
          break;
        case 2:
          // Oil Pr. 2 < min (Oil pressure 0.9 bar)

          if (lambda != (int8_t)v)
          {
            lambda = (int8_t)v;
            lambda_updated = true;
          }
          break;
        case 3:
          // 21:50 Time
          String binary_bits_string = String(v);
          for (int i = 0; i < 8; i++)
          {
            if (binary_bits_string.charAt(i) == '1')
            {
              switch (i)
              {
              case 0:
                exhaust_gas_recirculation_error = true;
                break;
              case 1:
                oxygen_sensor_heating_error = true;
                break;
              case 2:
                oxgen_sensor_error = true;
                break;
              case 3:
                air_conditioning_error = true;
                break;
              case 4:
                secondary_air_injection_error = true;
                break;
              case 5:
                evaporative_emissions_error = true;
                break;
              case 6:
                catalyst_heating_error = true;
                break;
              case 7:
                catalytic_converter = true;
                break;
              }
            }
          }
          break;
        }
        break;
      case 3:
        switch (idx)
        {
        case 0:
          break;
        case 1:
          // 9.0 l Fuel level

          if (pressure != (uint16_t)v)
          {
            pressure = (uint16_t)v;
            pressure_updated = true;
          }
          break;
        case 2:
          // 93 ohms Fuel Sender Resistance
          if (tb_angle != (float)v)
          {
            tb_angle = (float)v;
            tb_angle_updated = true;
          }
          break;
        case 3:
          // 0.0°C Ambient Temperature
          if (steering_angle != (float)v)
          {
            steering_angle = (float)v;
            steering_angle_updated = true;
          }
          break;
        }
        break;
      case 4:
        switch (idx)
        {
        case 0:
          break;
        case 1:
          if (voltage != (float)v)
          {
            voltage = (float)v;
            voltage_updated = true;
          }
          break;
        case 2:
          if (temperature_unknown_2 != (uint8_t)v)
          {
            temperature_unknown_2 = (uint8_t)v;
            temperature_unknown_2_updated = true;
          }
          break;
        case 3:
          if (temperature_unknown_3 != (uint8_t)v)
          {
            temperature_unknown_3 = (uint8_t)v;
            temperature_unknown_3_updated = true;
          }
          break;
        }
        break;
      case 6:
        switch (idx)
        {
        case 0:
          break;
        case 1:
          if (engine_load != (uint16_t)v)
          {
            engine_load = (uint16_t)v;
            engine_load_updated = true;
          }
          break;
        case 2:
          break;
        case 3:
          if (lambda_2 != (int8_t)v)
          {
            lambda_2 = (int8_t)v;
            lambda_2_updated = true;
          }
          break;
        }
        break;
      }
      break;
    }
  }
  return true;
}

/**
 * KW1281 procedure to send a simple acknowledge block to keep the connection alive
 */
bool keep_alive()
{
  if (!KWP_send_ACK_block())
    return false;

  return KWP_receive_ACK_block();
}

/**
 * KW1281 procedure to read DTC error codes
 *
 * @return Amount of DTC errors retrieved or -1 if something failed
 */
int8_t read_DTC_codes()
{
  debug(F("Read DTC"));
  if (simulation_mode_active)
  {
    reset_dtc_status_errors_array_random();
    return 0;
  }

  if (!KWP_send_DTC_read_block())
    return -1;

  reset_dtc_status_errors_array();
  uint8_t dtc_counter = 0;
  uint8_t s[64];
  while (true)
  {
    int size = 0;
    if (!KWP_receive_block(s, 64, size))
      return -1;

    // if (com_error)
    //{
    //   // Kommunikationsfehler
    //   KWP_error_block();
    //   return false;
    // }

    if (s[2] == 0x09) // No more DTC error blocks
      break;
    if (s[2] != 0xFC)
    {
      debugln(F("DTC wrong block title"));
      return -1;
    }

    // Extract DTC errors
    int count = (size - 4) / 3;
    for (int i = 0; i < count; i++)
    {
      uint8_t byte_high = s[3 + 3 * i];
      uint8_t byte_low = s[3 + 3 * i + 1];
      uint8_t byte_status = s[3 + 3 * i + 2];

      if (byte_high == 0xFF && byte_low == 0xFF && byte_status == 0x88)
        debugln(F("No DTC codes found")); // return 2;
      else
      {
        uint16_t dtc = (byte_high << 8) + byte_low;
        // if (dtcCounter >= startFrom && dtcCounter - startFrom < amount)
        dtc_errors[dtc_counter] = dtc;
        dtc_status_bytes[dtc_counter] = byte_status;
        dtc_counter++;
      }
    }
    if (!KWP_send_ACK_block())
    {
      debugln(F("DTC read error"));
      debugln(F("Send ACK"));
      return -1;
    }

    /* debug(F("DTC errors: "));
    for (int i = 0; i < count; i++)
    {
      debug(i);
      debug(F(" = "));
      debughex(dtc_errors[i]);
      debug(F(" | "));
    }
    debugln(F(""));
    debug(F("DTC Status bytes: "));
    for (int i = 0; i < count; i++)
    {
      debug(i);
      debug(F(" = "));
      debughex(dtc_status_bytes[i]);
      debug(F(" | "));
    }
    debugln(F("")); */
  }

  return dtc_counter;
}

/**
 * KW1281 procedure to delete DTC error codes
 */
bool delete_DTC_codes()
{

  if (simulation_mode_active)
    return true;

  if (!KWP_send_DTC_delete_block())
    return false;

  int size = 0;
  uint8_t s[64];
  if (!KWP_receive_block(s, 64, size))
    return false;

  if (s[2] != 0x09) // Expect ACK
    return false;

  return true;
}

/**
 * KW1281 exit procedure
 */
bool kwp_exit()
{
  debugln(F("Manual KWP exit.."));
  // Perform KWP end output block
  delay(15);
  uint8_t s[64];
  s[0] = 0x03;
  s[1] = block_counter;
  s[2] = 0x06;
  s[3] = 0x03;
  if (!KWP_send_block(s, 4))
  {
    debugln(F("KWP exit failed"));
    return false;
  }
  else
  {
    debugln(F("KWP exit succesful"));
  }
  return true;
}

bool obd_connect()
{
  debugln(F("Connecting to ECU"));

  block_counter = 0;
  
  debugln(F("Init "));
  obd.begin(baud_rate); // 9600 for 0x01, 10400 for other addresses, 1200 for very old ECU < 1996
  KWP_5baud_init(addr_selected);

  uint8_t response[3] = {0, 0, 0}; // Response should be (0x55, 0x01, 0x8A)base=16 = (85 1 138)base=2
  int response_size = 3;
  if (!KWP_receive_block(response, 3, response_size, -1, true))
  {
    debug(F("Handshake error got "));
    debugstrnumhex(F(" "), response[0]);
    debugstrnumhex(F(" "), response[1]);
    debugstrnumhexln(F(" "), response[2]);

    delay(ECU_TIMEOUT);
    return false;
  }
  if (((((uint8_t)response[0]) != 0x55) || (((uint8_t)response[1]) != 0x01) || (((uint8_t)response[2]) != 0x8A)))
  {
    debug(F("Handshake error got "));
    debugstrnumhex(F(" "), response[0]);
    debugstrnumhex(F(" "), response[1]);
    debugstrnumhexln(F(" "), response[2]);
    delay(ECU_TIMEOUT);
    return false;
  }

  // display_statusbar();

  // debugln(F("KWP_5baud_init DONE"));
  // lcd_print(0, 1, "Read ECU data...");
  // debugln(F("ReadConnectBlocks"));
  if (!read_connect_blocks(false))
  {
    debugln(F(" "));
    debugln(F("------"));
    debugln(F("ECU connection failed"));
    debugln(F("------"));
    return false;
  }
  debugln(F(" "));
  debugln(F("------"));
  debugln(F("ECU connected"));
  debugln(F("------"));

  connected = true;
  return true;
}

/**
 * Perform user setup before connecting to ECU
 */
bool connect()
{
  debugstrnumln(F("Connect attempt: "), connection_attempts_counter);

  // Startup configuration // 0 = false, 1 = true, -1 = undefined for booleans as int8_t
  int8_t userinput_simulation_mode = -1;
  uint16_t userinput_baudrate = 9600;
  uint8_t userinput_baudrate_pointer = 3; // for default 9600
  uint16_t supported_baud_rates_size = 5;
  uint16_t supported_baud_rates[supported_baud_rates_size] = {1200, 2400, 4800, 9600, 10400};
  int8_t userinput_ecu_address = -1; // 1 or 17

  if (connection_attempts_counter > 0)
  {
    userinput_simulation_mode = simulation_mode_active;
  }

  simulation_mode_active = AUTO_SETUP_SIMULATION_MODE_ACTIVE;
  baud_rate = AUTO_SETUP_BAUD_RATE;
  addr_selected = AUTO_SETUP_ADDRESS;

  debugln(F("Saved configuration: "));
  debugstrnumln(F("--- SIMULATION "), simulation_mode_active);
  debugstrnumln(F("--- baud "), baud_rate);
  debugstrnumhexln(F("--- addr "), addr_selected);

  // Connect to ECU
  connection_attempts_counter++;
  if (!simulation_mode_active && !obd_connect())
  {
    disconnect();
    return false;
  }
  connect_time_start = millis();
  menu_switch = true;
  return true;
}

// --------------------------------------------------------------------------------------------------
//                            ARDUINO CODE
// --------------------------------------------------------------------------------------------------

void setup()
{

// For debug USB Serial monitor
#if DEBUG == 1
  Serial.begin(9600);
#endif

  // Pins
  pinMode(pin_tx, OUTPUT); // TX white
  digitalWrite(pin_tx, HIGH);

  reset_dtc_status_errors_array();

  // Check connection
  if (!connect())
    return;
}

void loop()
{
  // Update values
  if (!simulation_mode_active)
  {

    switch (kwp_mode)
    {
    case KWP_MODE_ACK:
      if (!keep_alive())
      {
        disconnect();
        return;
      }
      break;
    case KWP_MODE_READGROUP:
      if (!read_sensors(kwp_group))
      {
        disconnect();
        return;
      }
      break;
    case KWP_MODE_READSENSORS:
      // Read the sensor groups
      for (int i = 1; i <= 3; i++)
      {
        if (!read_sensors(i))
        {
          disconnect();
          return;
        }
      }
      break;
    default:
      debugln(F("Kwp_mode undefined EXIT"));

      break;
    }
  }
  else
  {
    simulate_values();
    delay(222);
  }

  compute_values();

  /*
  // Button input
  bool button_pressed = false;
  if (millis() > endTime)
  {
    // User input, menu selection
    int user_input = analogRead(0);
    if (user_input < 60)
    {
      // Right button
      button_pressed = true;
      increment_menu(menu, menu_max);
    }
    else if (BUTTON_LEFT(user_input))
    {
      // Left button
      button_pressed = true;
      decrement_menu(menu, menu_max);
    }
    else
    {
      switch (menu)
      {
      case 0:
        if (BUTTON_UP(user_input))
        {
          button_pressed = true;
          increment_menu(menu_cockpit_screen, menu_cockpit_screen_max);
        }
        else if (BUTTON_DOWN(user_input))
        {
          button_pressed = true;
          decrement_menu(menu_cockpit_screen, menu_cockpit_screen_max);
        }
        break;
      case 1:
        if (BUTTON_UP(user_input))
        {
          button_pressed = true;
          increment_menu(menu_experimental_screen, menu_experimental_screen_max);
          increment_group_current();
        }
        else if (BUTTON_DOWN(user_input))
        {
          button_pressed = true;
          decrement_menu(menu_experimental_screen, menu_experimental_screen_max);
          decrement_group_current();
        }
        else if (BUTTON_SELECT(user_input))
        {
          button_pressed = true;
          invert_group_side();
        }
        break;
      case 2:
        if (BUTTON_UP(user_input))
        {
          button_pressed = true;
          increment_menu(menu_debug_screen, menu_debug_screen_max);
        }
        else if (BUTTON_DOWN(user_input))
        {
          button_pressed = true;
          decrement_menu(menu_debug_screen, menu_debug_screen_max);
        }
        break;
      case 3:
        if (BUTTON_UP(user_input))
        {
          button_pressed = true;
          increment_menu(menu_dtc_screen, menu_dtc_screen_max);
        }
        else if (BUTTON_DOWN(user_input))
        {
          // Down button
          button_pressed = true;
          decrement_menu(menu_dtc_screen, menu_dtc_screen_max);
        }
        else
        {
          switch (menu_dtc_screen)
          {
          case 0:
            if (BUTTON_SELECT(user_input))
            {
              // Select button = Exit/Reconnect
              int8_t dtc_codes = read_DTC_codes();
              if (dtc_codes == -1)
              {
                LCD_display_error_msg("DTC read error", "Disconnecting...");
                delay(1222);
                disconnect();
                return;
              }
              lcd_clear(0, 1, 16);
              lcd_print(3, 1, "<Success>");
            }
            break;
          case 1:
            if (BUTTON_SELECT(user_input))
            {
              if (!delete_DTC_codes())
              {
                LCD_display_error_msg("DTC delete error", "Disconnecting...");
                delay(1222);
                disconnect();
                return;
              }
              lcd_clear(0, 1, 16);
              lcd_print(3, 1, "<Success>");
              reset_dtc_status_errors_array();
            }
            break;
          default:

            break;
          }
        }
        break;
      case 4:
        if (BUTTON_UP(user_input))
        {
          // Up button
          button_pressed = true;
          increment_menu(menu_settings_screen, menu_settings_screen_max);
        }
        else if (BUTTON_DOWN(user_input))
        {
          // Down button
          button_pressed = true;
          decrement_menu(menu_settings_screen, menu_settings_screen_max);
        }
        else
        {
          switch (menu_settings_screen)
          {
          case 0:
            if (BUTTON_SELECT(user_input))
            {
              // Exit/Reconnect
              kwp_exit();
              disconnect();
              return;
            }
            break;
          case 1:
            if (BUTTON_SELECT(user_input))
            {
              switch (kwp_mode)
              {
              case KWP_MODE_ACK:
                kwp_mode = KWP_MODE_READGROUP;
                break;
              case KWP_MODE_READGROUP:
                kwp_mode = KWP_MODE_READSENSORS;
                break;
              case KWP_MODE_READSENSORS:
                kwp_mode = KWP_MODE_ACK;
                break;
              }

              button_pressed = true;
            }
            break;
          default:
            break;
          }
        }
        break;
      }
    }

    if (button_pressed)
    {
      endTime = millis() + button_timeout;
    }
  }*/
}