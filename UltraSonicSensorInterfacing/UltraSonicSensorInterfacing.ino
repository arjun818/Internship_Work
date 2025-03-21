#include <Wire.h>
#include <NewPing.h>
#define _I2C_ADDRESS 0x70
#define _PIN_ULTRASONIC_TRIGGER 8
#define _PIN_ULTRASONIC_ECHO 8
#define _MAX_DISTANCE 300

NewPing oultrasonicSensor(_PIN_ULTRASONIC_TRIGGER, _PIN_ULTRASONIC_ECHO, _MAX_DISTANCE);
uint16_t un16distance;
byte breceived;


void setup() {
  Serial.begin(9600);
  Wire.begin(_I2C_ADDRESS);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}
void loop() {
  un16distance = oultrasonicSensor.ping_cm();
  Serial.print("Distance: ");
  Serial.print(un16distance);
  Serial.println(" cm");
  delay(300);
}
void receiveEvent(int howMany) {
  while (Wire.available() > 0) {
    breceived = Wire.read();
  }
}
void requestEvent() {
  Wire.write(highByte(un16distance));
  Wire.write(lowByte(un16distance));
}
