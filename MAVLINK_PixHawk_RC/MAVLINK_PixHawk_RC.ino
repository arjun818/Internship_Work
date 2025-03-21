#include "mavlink.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <stdint.h>

// Private macros
#define NEO_LED_PIN_RED   2
#define NEO_LED_PIN_BLUE1 3
#define NEO_LED_PIN_BLUE2 4
#define NEO_LED_PIN_GREEN 5
#define NEO_LED_PIN_WHITE 6
#define NUM_PIXELS 2

#define RX_PIN 10         // Define the pin for receiving data
#define TX_PIN 11         // Define the pin for transmitting data
#define MAVLINK_COMM_0 0  // MAVLink communication channel

#define MAV_VTOL_STATE_MC 3
#define MAV_VTOL_STATE_FW 4

// Initialize NeoPixels
Adafruit_NeoPixel red =   Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_RED, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel blue1 = Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_BLUE1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel blue2 = Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_BLUE2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel green = Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_GREEN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel white = Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_WHITE, NEO_GRB + NEO_KHZ800);

// Create SoftwareSerial object for Pixhawk communication
SoftwareSerial SerialPort(RX_PIN, TX_PIN);  // RX, TX

// MAVLink message and status
mavlink_message_t msg;
mavlink_status_t status;

// Add variables for flight mode and armed status
uint8_t flight_mode = 0;
bool is_armed;

void setup() {
  Serial.begin(57600);      // For serial monitor communication
  SerialPort.begin(57600);  // For Pixhawk communication over SoftwareSerial

  // Initialize NeoPixel libraries
  red.begin();
  blue1.begin();
  blue2.begin();
  green.begin();
  white.begin();
}

void loop() {
  // Check for data available from Pixhawk
  while (SerialPort.available() > 0) {
    uint8_t byte = SerialPort.read();

    // Parse incoming MAVLink message
    if (mavlink_parse_char(MAVLINK_COMM_0, byte, &msg, &status)) {
      
      // Check for RC_CHANNELS message (message ID 65)
      if (msg.msgid == MAVLINK_MSG_ID_RC_CHANNELS_RAW) {
        mavlink_rc_channels_override_t rc_channels;
        mavlink_msg_rc_channels_override_decode(&msg, &rc_channels);

        // Get the value of Channel 8 (typically AUX8)
        uint16_t channel_8_value = rc_channels.chan8_raw;

        // Map the value of Channel 8 to LED behavior
        controlLEDsBasedOnRC(channel_8_value);
      }

      // Check for HEARTBEAT message to get flight mode and armed status
      if (msg.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
        mavlink_heartbeat_t heartbeat;
        mavlink_msg_heartbeat_decode(&msg, &heartbeat);

        // Update flight mode and armed status
        flight_mode = heartbeat.custom_mode;
        is_armed = (heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) != 0;// Check if armed
        //is_armed = (heartbeat.system_status == MAV_STATE_ACTIVE); // Check if armed
      }
    }
  }
  // Handle LEDs based on flight mode and armed status
  handleLEDs(flight_mode, is_armed);
  // Handle LED blinking for white LED
  time_handler();
}
// Control LEDs based on the RC Channel 8 value
void controlLEDsBasedOnRC(uint16_t channel_8_value) {
  if (channel_8_value < 1000) {
    // RC Channel 8 is in the low range (e.g., 1000 or below)
    Serial.println("Red Led ON");
    red_led_on();
    blue_led_off();
    green_led_off();
    white_led_off();
  } else if (channel_8_value >= 1000 && channel_8_value < 1500) {
    // RC Channel 8 is in the mid-low range
    red_led_off();
    Serial.println("Blue Led ON");
    blue_led_on();
    green_led_off();
    white_led_off();
  } else if (channel_8_value >= 1500 && channel_8_value < 2000) {
    // RC Channel 8 is in the mid-high range
    red_led_off();
    blue_led_off();
    Serial.println("Green Led ON");
    green_led_on();
    white_led_off();
  } else {
    // RC Channel 8 is in the high range (e.g., 2000)
    red_led_off();
    blue_led_off();
    green_led_off();
    Serial.println("White Led ON");
    white_led_on();
  }
}

// Handle LED display based on flight mode and armed status
void handleLEDs(uint8_t flight_mode, bool is_armed) {
  if (flight_mode == MAV_VTOL_STATE_MC) {
    // Multicopter mode
    if (is_armed) {
      Serial.println("Red Led ON");
      red_led_on();
      blue_led_on();
      green_led_on();
    } else {
      red_led_off();
      blue_led_off();
      Serial.println("Green Led ON");
      green_led_on();
    }
  } else if (flight_mode == MAV_VTOL_STATE_FW) {
    // Fixed-wing mode
    if (is_armed) {
      red_led_off();
      blue_led_off();
      Serial.println("Green Led ON");
      green_led_on();
    } else {
      red_led_on();
      green_led_off();
      Serial.println("Blue Led ON");
      blue_led_on();
    }
  }
}
void red_led_on(void){
  red.setPixelColor(0, red.Color(255,0,0));
  red.setPixelColor(1, red.Color(255,0,0));
  red.show();
}
void red_led_off(void){
  red.setPixelColor(0, red.Color(0,0,0));
  red.setPixelColor(1, red.Color(0,0,0));
  red.show();
}
void blue_led_on(void){
  blue1.setPixelColor(0, blue1.Color(0,0,255));
  blue1.setPixelColor(1, blue1.Color(0,0,255));
  blue1.show();
  blue2.setPixelColor(0, blue2.Color(0,0,255));
  blue2.setPixelColor(1, blue2.Color(0,0,255));
  blue2.show();
}
void blue_led_off(void){
  blue1.setPixelColor(0, blue1.Color(0,0,0));
  blue1.setPixelColor(1, blue1.Color(0,0,0));
  blue1.show();
  blue2.setPixelColor(0, blue2.Color(0,0,0));
  blue2.setPixelColor(1, blue2.Color(0,0,0));
  blue2.show();
}
void green_led_on(void){
  green.setPixelColor(0, green.Color(0,255,0));
  green.setPixelColor(1, green.Color(0,255,0));
  green.show();
}
void green_led_off(void){
  green.setPixelColor(0, green.Color(0,0,0));
  green.setPixelColor(1, green.Color(0,0,0));
  green.show();
}
void white_led_on(void){
  white.setPixelColor(0, white.Color(255,255,255)); //Turn on white LED
  white.setPixelColor(1, white.Color(255,255,255));
  white.show();
}
void white_led_off(void){
  white.setPixelColor(0, white.Color(0, 0, 0));   // Turn off white LED
  white.setPixelColor(1, white.Color(0, 0, 0));
  white.show();
}
void time_handler(void){
  // Blinking white LED using NeoPixel library
  for(int i=0; i<2; i++){
    white_led_on();
    delay(100);  // Wait for 500 ms
    white_led_off();
    delay(75);  // Wait for 500 ms
  }
delay(1000);
}