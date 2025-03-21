#define INPUT_PIN   2

double read_var;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(INPUT_PIN, INPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:
  read_var= digitalRead(INPUT_PIN);
  Serial.println("Digital values: ");
  Serial.println(read_var);
  delay(500);
}
