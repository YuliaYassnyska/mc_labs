bool isAddress = true;
bool isCommand = false;
byte command;

void setWriteModeRS485() {
  byte port = PORTD;

    PORTD |= 1 << PD1; 
    delay(1);

}


ISR(USART1_TX_vect)
{ 
  PORTD &= ~(1 << PD1);
}

uint8_t crc16(uint8_t const *data, size_t size) {
        uint8_t crc = 0xFF;
        while (size--) {
                crc ^= (*data++)<<8;
                for (unsigned k = 0; k < 8; k++) {
                crc = crc & 1 ? (crc << 1) ^ 0x7 : crc << 1;
                }
        } return crc;
}


void setup() {
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   
  
 
  DDRD |= 1 << PD1;
  PORTD &= ~(1 << PD1);


  Serial.begin(9600);
 
  Serial1.begin(9600, SERIAL_8N1);
  UCSR1B |= (1 << UCSZ12) | (1 << TXCIE1);
}

void loop() {
  if (Serial.available()) {
    byte inByte = Serial.read();
    if (isAddress) {
      setWriteModeRS485();
      UCSR1B |= 1 << TXB81;
      Serial1.write(inByte);
      isAddress = false;
      isCommand = true;
    } else if (isCommand) {
      command = inByte;
      isCommand = false;
      setWriteModeRS485();
      UCSR1B &= ~(1 << TXB81);
      Serial1.write(inByte);
      if (command == 0xB1) isAddress = true;
    } else {
      isAddress = true;
      setWriteModeRS485();
      UCSR1B &= ~(1 << TXB81);
      Serial1.write(inByte);
    }
  }

  if (Serial1.available()) {
    String inString = Serial1.readString();
    Serial.print(inString);
  }
}
