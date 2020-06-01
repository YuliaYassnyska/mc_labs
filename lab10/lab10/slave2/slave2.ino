#include <Wire.h>
#define HYT_ADDRESS 0x28

#define BAUD 14400
#define MODE 2
#define TRANSMIT HIGH
#define RECEIVE LOW
#define ID 245
#define MASTER_ID 1

char symbol;
int i = 0;
char* bufferPtr;

int humidity = 12;
int temperature = 16;
int b1, b2, b3, b4, rawHumidity, rawTemperature;

void setup() {
  Wire.begin();
  
  Serial.begin(BAUD);
  pinMode(MODE, OUTPUT);
  digitalWrite(MODE, RECEIVE);
}

void loop() {
  Wire.beginTransmission(HYT_ADDRESS);
  Wire.requestFrom(HYT_ADDRESS, 4);
  
  if(Wire.available() == 4) {
    b1 = Wire.read();
    b2 = Wire.read();
    b3 = Wire.read();
    b4 = Wire.read();
    Wire.endTransmission();
    
    rawHumidity = b1 << 8 | b2;
    // compound bitwise to get 14 bit measurement first two bits are status/stall bit
    rawHumidity =  (rawHumidity &= 0x3FFF);
    humidity = 100.0 / pow(2,14) * rawHumidity;
    
    b4 = (b4 >> 2); // Mask away 2 least significant bits 
    rawTemperature = b3 << 6 | b4;
    temperature = 165.0 / pow(2,14) * rawTemperature - 40;
  }

  i = 0;
  symbol = 0;
  if (Serial.available()) {
    bufferPtr = (char*)malloc(50 * sizeof(char));
    delay(5);
    while (Serial.available()) {
      symbol = Serial.read();
      if (symbol == ';') {
        bufferPtr[i++] = '\0';
        i = 0;
        int c;
        sscanf(bufferPtr, "%d", &c);
        if (c != ID) {
          while (Serial.available()){Serial.read();}
          free(bufferPtr);
          return;
        }
      } else {
        bufferPtr[i++] = symbol;
      }
    }
    if (i > 0){
      bufferPtr[i++] = '\0';
    }
  }
  if (i > 0) {
    if (!strcmp(bufferPtr, "Give")) {
      sendMessage(MASTER_ID);
    }
  }
  free(bufferPtr);
}

void sendMessage(int recevierId) {
  unsigned char str[] = {humidity, temperature};
  char buffer[40];
  digitalWrite(MODE, TRANSMIT);
  long crc = calculateCrcWithTable(str, sizeof(str) / sizeof(char) - 1, 0x00, 0x3D65, false, false, 0xFF, 16);
  delay(60);
  sprintf(buffer, "%d;%d:%lx!%d,%d*", recevierId, ID, crc, humidity, temperature);
  Serial.print(buffer);
  delay(60);
  digitalWrite(MODE, RECEIVE);
}

long calculateCrcWithTable(const char data[], const unsigned long length, unsigned long crc,
        const unsigned long poly, const bool refIn, const bool refOut, const unsigned long xorOut,
        const unsigned long crcBitLength) {
    unsigned long greatestBit = 1 << (crcBitLength - 1);
    unsigned long cutter = (1 << crcBitLength) - 1;
    unsigned long table[256];

    for (unsigned long i = 0; i < 256; i++) {
        unsigned long res = i;

        if (crcBitLength > 8) {
            res <<= (crcBitLength - 8);
        }

        for (unsigned int j = 0; j < 8; j++) {
            if ((res & greatestBit) != 0) {
                res = (res << 1) ^ poly;
            }
            else {
                res <<= 1;
            }
        }

        table[i] = res & cutter;
    }

    unsigned long shift = (crcBitLength - 8 < 0) ? 0 : (crcBitLength - 8);
    for (unsigned long i = 0; i < length; i++) {
        unsigned long temp = data[i];
        if (refIn) {
            temp = reverseBits(temp, 8);
        }
        crc = table[(crc >> shift) ^ temp] ^ (crc << 8);
        crc &= cutter;
    }

    if (refOut) {
        crc = reverseBits(crc, crcBitLength);
    }

    return crc ^ xorOut;
}

unsigned long reverseBits(unsigned long value, const unsigned long size) {
    unsigned long newValue = 0;

    for (unsigned long i = size - 1; i >= 0; i--) {
        newValue |= (value & 1) << i;
        value >>= 1;
    }
    return newValue;
}
