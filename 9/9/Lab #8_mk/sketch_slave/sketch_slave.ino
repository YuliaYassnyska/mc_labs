bool isAddress = true;
byte address;
bool isCommand = false;
byte command;
byte data;

void setWriteModeRS485() {
  PORTD |= 1 << PD2; 
  delay(1);
}

void sendString(char* data, unsigned int len){
   int i = 0;
   int packetsAmount = len%5 ? len/5+1 : len/5;
   Serial.write((byte)packetsAmount);
   while(len >= 5){
     Serial.write(data+(5*i), 5);
     Serial.write('\n');
     unsigned int crcInt = crc8_rohc(data+(5*i), 5);
     Serial.println(crcInt);
     len = len - 5;
     i++;
   }
   if(len%5){
     Serial.write((data+(5*i)), len);
     Serial.write('\n');
     unsigned int crcInt = crc8_rohc(data+(5*i), len);
     Serial.println(crcInt);
   }
}

void sendString(char* data, unsigned int len, int byteToChange, byte toggleMask){
   byteToChange++;
   int i = 0;
   int packetsAmount = len%5 ? len/5+1 : len/5;
   int toggleByteCycle = (byteToChange%5 ? byteToChange/5+1 : byteToChange/5)-1;
   Serial.write((byte)packetsAmount);
   while(len >= 5){
     unsigned int crcInt = crc8_rohc(data+(5*i), 5);
     if(i == toggleByteCycle) *(data+byteToChange-1) ^= toggleMask;
     Serial.write(data+(5*i), 5);
     Serial.write('\n');
     Serial.println(crcInt);
     len = len - 5;
     i++;
   }
   if(len%5){
     Serial.write((data+(5*i)), len);
     Serial.write('\n');
     unsigned int crcInt = crc8_rohc(data+(5*i), len);
     Serial.println(crcInt);
   }
}

ISR(USART_TX_vect)
{ 
  PORTD &= ~(1 << PD2); 
}

uint8_t crc8_rohc(uint8_t const *data, size_t size) {
        uint8_t crc = 0xFF;
        while (size--) {
                crc ^= *data++;
                for (unsigned k = 0; k < 8; k++) crc = crc & 1 ? (crc >> 1) ^ (0x17<<3) : crc >> 1;
        } return crc;
}

void setup() {
  delay(1000);

  DDRB = 0xFF;
  PORTB = 0x00;

  DDRD = 0b00000111;
  PORTD = 0b11111000;

  Serial.begin(9600, SERIAL_8N1);
  UCSR0B |= (1 << UCSZ02) | (1 << TXCIE0);
  UCSR0A |= (1 << MPCM0);

  delay(1);

  
  address = 1;
  PORTB = address;
}

void loop() {
  data = (~PIND);
  data = data >> 3;

  if (Serial.available()) {
    byte inByte = Serial.read();
    if (isAddress) {
      if (address == inByte) {
        isAddress = false;
        isCommand = true;
        
        UCSR0A &= ~(1 << MPCM0);
      }
    } else if (isCommand) {
      command = inByte;
      isCommand = false;
      if (command == 0xA1) {

      } else if (command == 0xB1) {
        isAddress = true;
        setWriteModeRS485();
          unsigned int crcInt = crc8_rohc("Yasnyska Yulia", sizeof("Yasnyska Yulia")-1);
          char surname[] = "Yasnyska Yulia";
          sendString(surname, 15);
      } else if (command == 0xC1) {
        isAddress = true;
        setWriteModeRS485();
          unsigned int crcInt = crc8_rohc("Yasnyska Yulia", sizeof("Yasnyska Yulia")-1);
          char surname[] = "Yasnyska Yulia";
          sendString(surname, 15, 0, 0b1);
      } else if (command == 0xD1) {
        isAddress = true;
        setWriteModeRS485();
          unsigned int crcInt = crc8_rohc("Yasnyska Yulia", sizeof("Yasnyska Yulia")-1);
          char surname[] = "Yasnyska Yulia";
          sendString(surname, 15, 8, 0b111);
      }
    } else {
      if (command == 0xA1) {
        isAddress = true;
        PORTB = inByte;
      }
    }
  }
}
