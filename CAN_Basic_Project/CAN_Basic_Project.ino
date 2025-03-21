#include <SPI.h>
#include <mcp_can.h>
const int spiCSPin = 10;
MCP_CAN CAN(spiCSPin);
void setup() {
    Serial.begin(115200);
    while (CAN_OK != CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ)) {
        Serial.println("CAN BUS init Failed");
        delay(100);
    }
    Serial.println("CAN BUS Shield Init OK!");
    // Set to normal mode for receiving messages
    CAN.setMode(MCP_NORMAL);
}

void loop() {
    unsigned char len = 0;
    unsigned char buf[8];

    // Check if data is available
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
        unsigned long canId;
        CAN.readMsgBuf(&canId, &len, buf);  // Read the message
        Serial.print("Received Message ID: ");
        Serial.println(canId, HEX);
        Serial.print("Data Length: ");
        Serial.println(len);
        Serial.print("Data: ");
        for (int i = 0; i < len; i++) {
            Serial.print(buf[i]);
            Serial.print("\t");
        }
        Serial.println();
    } else {
        Serial.println("No message received");
    }

    delay(100);  // Adjust delay as necessary
}

