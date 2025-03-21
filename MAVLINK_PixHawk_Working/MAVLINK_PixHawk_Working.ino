#include <Adafruit_NeoPixel.h>

/*****Private macros******/
//The digital pin number used
#define NEO_LED_PIN_RED     (int)2  //RED
#define NEO_LED_PIN_BLUE1   (int)3
#define NEO_LED_PIN_BLUE2   (int)4
#define NEO_LED_PIN_GREEN   (int)5
#define NEO_LED_PIN_WHITE   (int)6
//The number of pixels required or the LED count
#define NUM_PIXELS          (int)2

Adafruit_NeoPixel red  = Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_RED,   NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel blue1= Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_BLUE1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel blue2= Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_BLUE2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel green= Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_GREEN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel white= Adafruit_NeoPixel(NUM_PIXELS, NEO_LED_PIN_WHITE, NEO_GRB + NEO_KHZ800);

/****Private function protypes****/
void red_led(void);
void blue_led(void);
void green_led(void);
void white_led_on(void);
void white_led_off(void);
void time_handler(void);

void setup() {
  // put your setup code here, to run once:
  //Initializes the library
  red.begin();
  red_led();
  blue1.begin();
  blue2.begin();
  blue_led();
  green.begin();
  green_led();
  white.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  time_handler();
}

void red_led(void){
  red.setPixelColor(0, red.Color(255,0,0));
  red.setPixelColor(1, red.Color(255,0,0));
  red.show();
}
void blue_led(void){
  blue1.setPixelColor(0, blue1.Color(0,0,255));
  blue1.setPixelColor(1, blue1.Color(0,0,255));
  blue1.show();
  blue2.setPixelColor(0, blue2.Color(0,0,255));
  blue2.setPixelColor(1, blue2.Color(0,0,255));
  blue2.show();
}
void green_led(void){
  green.setPixelColor(0, green.Color(0,255,0));
  green.setPixelColor(1, green.Color(0,255,0));
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

