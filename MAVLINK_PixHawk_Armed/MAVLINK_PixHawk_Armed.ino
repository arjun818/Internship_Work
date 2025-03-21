#include "mavlink.h"
#include <Arduino.h>
#include <SoftwareSerial.h>  // Include SoftwareSerial
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
Adafruit_NeoPixel red   = Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_RED, NEO_GRB +   NEO_KHZ800);
Adafruit_NeoPixel blue1 = Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_BLUE1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel blue2 = Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_BLUE2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel green = Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_GREEN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel white = Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_WHITE, NEO_GRB + NEO_KHZ800);

// Create SoftwareSerial object for Pixhawk communication
SoftwareSerial SerialPort(RX_PIN, TX_PIN);  // RX, TX

// MAVLink message and status
mavlink_message_t msg;
mavlink_status_t status;

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
      // Handle heartbeat message
      if (msg.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
        mavlink_heartbeat_t heartbeat;
        mavlink_msg_heartbeat_decode(&msg, &heartbeat);

        // Get the flight mode and armed status
        uint8_t flight_mode = heartbeat.custom_mode;
        String mode = getFlightMode(flight_mode);
        bool is_armed = (heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) != 0;

        // Print flight mode and armed status to serial monitor
        Serial.println("Mode: " + mode);
        Serial.print("Armed: ");
        Serial.println(is_armed ? "YES" : "NO");

        // Control LEDs based on flight mode and armed status
        handleLEDs(flight_mode, is_armed);
      }
    }
  }
  // Handle LED blinking for white LED
  White_Strobing();
}

// Get the flight mode as a string
String getFlightMode(uint8_t custom_mode) {
  switch (custom_mode) {
    case MAV_VTOL_STATE_MC: return "Multicopter";
    case MAV_VTOL_STATE_FW: return "Fixed-Wing";
    default: return "Unknown";
  }
}

// Handle LED display based on flight mode and armed status
void handleLEDs(uint8_t flight_mode, bool is_armed) {
  if (flight_mode == MAV_VTOL_STATE_MC) {
    // Multicopter mode
    if (is_armed) {
      red_led_on();
      blue_led_on();
      green_led_on();
    } else {
      red_led_off();
      blue_led_off();
      green_led_on();
    }
  } else if (flight_mode == MAV_VTOL_STATE_FW) {
    // Fixed-wing mode
    if (is_armed) {
      red_led_off();
      blue_led_off();
      green_led_on();
    } else {
      red_led_on();
      green_led_off();
      blue_led_on();
    }
  }
}

// Control functions for LEDs
void red_led_on() {
  red.fill(red.Color(255, 0, 0));
  red.show();
}

void red_led_off() {
  red.fill(red.Color(0, 0, 0));
  red.show();
}

void blue_led_on() {
  blue1.fill(blue1.Color(0, 0, 255));
  blue1.show();
  blue2.fill(blue2.Color(0, 0, 255));
  blue2.show();
}

void blue_led_off() {
  blue1.fill(blue1.Color(0, 0, 0));
  blue1.show();
  blue2.fill(blue2.Color(0, 0, 0));
  blue2.show();
}

void green_led_on() {
  green.fill(green.Color(0, 255, 0));
  green.show();
}

void green_led_off() {
  green.fill(green.Color(0, 0, 0));
  green.show();
}

void white_led_on() {
  white.fill(white.Color(255, 255, 255));
  white.show();
}

void white_led_off() {
  white.fill(white.Color(0, 0, 0));
  white.show();
}

void White_Strobing() {
  // Blink white LED with a pattern
  for (int i = 0; i < 2; i++) {
    white_led_on();
    delay(100);  // Wait for 100 ms
    white_led_off();
    delay(75);   // Wait for 75 ms
  }
  delay(1000);  // Wait for 1 second before repeating the cycle
}
