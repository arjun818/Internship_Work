#include <Adafruit_NeoPixel.h>

//User-defined MACROS
#define PWM_PIN             3
#define NEO_LED_PIN         2
#define NUM_PIXELS          2

//User defined global variables
double pulse_var;

//LED objects
Adafruit_NeoPixel white=  Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel blue=   Adafruit_NeoPixel(NUM_PIXELS,  NEO_LED_PIN, NEO_GRB + NEO_KHZ800);

//Private function prototypes
void white_led_strip(void);
void blue_led_strip(void);
void off_led_strip(void);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PWM_PIN, INPUT);
  white.begin();
  blue.begin();
}
void loop() {
  // put your main code here, to run repeatedly:
  pulse_var= pulseIn(PWM_PIN, HIGH);
  Serial.println("PWM values: ");
  Serial.println(pulse_var);

  if(pulse_var>=1470.00 && pulse_var<=1500.00){
    blue_led_strip();

  }else if(pulse_var>=1980.00 && pulse_var<=2000.00){
    white_led_strip();
  }else{
    off_led_strip();
  }
  delay(500);
}
void blue_led_strip(void){
  blue.setPixelColor(0, blue.Color(0,0,255));
  blue.setPixelColor(1, blue.Color(0,0,255));
  blue.show();
}
void white_led_strip(void){
  white.setPixelColor(0, white.Color(255,255,255)); 
  white.setPixelColor(1, white.Color(255,255,255));
  white.show();
}
void off_led_strip(void){
  white.setPixelColor(0, white.Color(0, 0, 0));   
  white.setPixelColor(1, white.Color(0, 0, 0));
  white.show();
}