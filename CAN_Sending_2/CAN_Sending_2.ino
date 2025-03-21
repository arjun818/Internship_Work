#include <mcp_can.h>
#include <SPI.h>

const int SPI_CS_PIN = 10; // Pin connected to the CS pin of the MCP2515

MCP_CAN CAN0(SPI_CS_PIN); // Set CS pin for CAN

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Sender");

  // Initialize CAN bus at 500 kbps
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("CAN bus initialized successfully.");
  } else {
    Serial.println("Error initializing CAN bus.");
    while (1);
  }
}

void loop() {
  // Sending a standard CAN message
  byte data[] = {'h', 'e', 'l', 'l', 'o'};
  CAN0.sendMsgBuf(0x12, 0, sizeof(data), data);
  Serial.println("Standard CAN message sent.");

  delay(1000);

  // Sending an extended CAN message
  byte extendedData[] = {'w', 'o', 'r', 'l', 'd'};
  CAN0.sendMsgBuf(0xabcdef, 1, sizeof(extendedData), extendedData);
  Serial.println("Extended CAN message sent.");

  delay(1000);
}
