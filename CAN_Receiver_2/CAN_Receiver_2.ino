#include <mcp_can.h>
#include <SPI.h>

// Set the SPI chip select pin for the MCP2515 CAN controller
const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN); // Set CS pin for CAN

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Receiver");

  // Start the CAN bus at 500 kbps
  if (CAN.begin(MCP_ANY, CAN_500KBPS, SPI_CS_PIN) != MCP2515_OK) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {
  // Check if a message has been received
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    // Create buffers to hold CAN message data
    unsigned long canId;
    unsigned char len = 0;
    unsigned char buf[8];

    // Read message from the CAN bus
    CAN.readMsgBuf(&canId, &len, buf);

    // Print the received message
    Serial.print("Received ");
    Serial.print("packet with id 0x");
    Serial.print(canId, HEX);
    Serial.print(" and length ");
    Serial.println(len);

    // Only print packet data if it's not an RTR packet
    for (int i = 0; i < len; i++) {
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}
