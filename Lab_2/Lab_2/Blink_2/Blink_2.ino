int leds_1[8] = {42, 49, 43, 48, 44, 47, 45, 46};
int leds_2[8] = {42, 43, 44, 45, 46, 47, 48, 49};

void setup(){
  int i;
  Serial.begin(9600);
  pinMode (27, INPUT_PULLUP);
  pinMode (29, INPUT_PULLUP);
  pinMode(42, OUTPUT);
  for (i = 0; i < 8; i++) {
  digitalWrite(leds_1[i], LOW);
  }
  for (i = 0; i < 8; i++) {
    pinMode (leds_1[i], OUTPUT);
  }
  for (i = 0; i < 8; i++) {
  digitalWrite(leds_2[i], LOW);
  }
  for (i = 0; i < 8; i++) {
    pinMode (leds_2[i], OUTPUT);
  }
}

void loop() {
  int i;
  // read from port 0:
  if (Serial.available()) {
    int inByte = Serial.read();
    if (inByte == 0xA1) {
    for (i = 0; i < 8; i++) {
    digitalWrite (leds_1[i], HIGH);
    delay (100);
    digitalWrite (leds_1[i], LOW);
    delay (75);
     }
    } else if (inByte == 0xB1) {
     for (i = 0; i < 8; i++) {
    digitalWrite (leds_2[i], HIGH);
    delay (100);
    digitalWrite (leds_2[i], LOW);
    delay (75);
     }
    }
 }
 
  // write to port 0:
  if(digitalRead(27)==LOW){
    Serial.write(0xB1);
    delay(10);
  }
   if(digitalRead(29)==LOW){
    Serial.write(0xB1);
    delay(10);
  }
}
