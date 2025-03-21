#include <Adafruit_NeoPixel.h>

//User-defined MACROS
#define NEO_LED_PIN           2
#define NUM_PIXELS            2
#define LDRPIN                A0
#define PWM_PIN               3

int LDRValue = 0;
double pulse_var;

//LED objects
Adafruit_NeoPixel white=  Adafruit_NeoPixel(NUM_PIXELS,  NEO_LED_PIN, NEO_GRB + NEO_KHZ800);


//Private function prototypes
void white_led_strip_on(void);
void white_led_strip_off(void);
void control_led(void);
int readLDRvalue(void);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PWM_PIN, INPUT);
  white.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  LDRValue= readLDRvalue();
  Serial.println("LDR Values: ");
  Serial.println(LDRValue);
  Serial.println("PWM values: ");
  Serial.println(pulse_var);
  control_led();
delay(500);
}

int readLDRvalue(void){
  return analogRead(LDRPIN);
}

void white_led_strip_on(void){
  white.setPixelColor(0, white.Color(255,255,255)); 
  white.setPixelColor(1, white.Color(255,255,255));
  white.show();
}
void white_led_strip_off(void){
  white.setPixelColor(0, white.Color(0, 0, 0));   
  white.setPixelColor(1, white.Color(0, 0, 0));
  white.show();
}
void control_led(void){
  pulse_var= pulseIn(PWM_PIN, HIGH);

  if(pulse_var<=1000.00){
    if(LDRValue<50){
    white_led_strip_on();
    }else{
    white_led_strip_off();
    }
  }else if(pulse_var>=1000.00 && pulse_var<=1500.00){
    if(pulse_var>=1200){
      white_led_strip_off();
    }
  }
}