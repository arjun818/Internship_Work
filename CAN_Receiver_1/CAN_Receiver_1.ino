#include <SPI.h>
#include "mcp_can.h"

const int spiCSPin = 10;  // SPI Chip Select pin for MCP2515
const int ledPin = 2;     // Pin connected to LED
boolean ledON = false;    // LED state

MCP_CAN CAN(spiCSPin);

void setup() {
    Serial.begin(115200);  // Initialize serial communication at 115200 baud
    pinMode(ledPin, OUTPUT);  // Set LED pin as output

    // Initialize CAN bus at 500 kbps
    if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
        Serial.println("CAN BUS Init OK!");
    } else {
        Serial.println("CAN BUS Init Failed");
        while (1);  // Stay here if initialization failed
    }
}

void loop() {
    unsigned char len = 0;
    unsigned char buf[8];
    unsigned long canId;

    // Check if a new CAN message is available
    if (CAN.checkReceive() == CAN_MSGAVAIL) {
        // Read message from the buffer
        CAN.readMsgBuf(&canId, &len, buf);

        // Print CAN message details
        Serial.println("-----------------------------");
        Serial.print("Data from ID: 0x");
        Serial.println(canId, HEX);
        
        // Print received data
        Serial.print("Data: ");
        for (int i = 0; i < len; i++) {
            Serial.print(buf[i], HEX);  // Print data as hex
            Serial.print("\t");
        }
        Serial.println();

        // Control LED based on received data
        if (len > 0) {  // Check if there is at least one byte of data
            if (buf[0] == 1) {  // Assuming the first byte is used to toggle LED
                digitalWrite(ledPin, HIGH);  // Turn LED on
                ledON = true;
            } else if (buf[0] == 0) {
                digitalWrite(ledPin, LOW);  // Turn LED off
                ledON = false;
            }
        }
    }
}
