#include "src/M5Stack/M5Stack.h"
uint8_t slowForwardMessage[6] = {0xAA, 1, 1, 1, 1, 0x55};
uint8_t stopMessage[6] = {0xAA, 0x00, 0x00, 0x00, 0x00, 0x55};
uint8_t slowBackwardMessage[6] = {0xAA, -1, -1, -1, -1, 0x55};
uint8_t slideLeft[6] = {0xAA, -1, 1, 1, -1, 0x55};
uint8_t slideRight[6] = {0xAA, 1, -1, -1, 1, 0x55};

void setup() {                
m5.begin();
  Serial.begin(115200);
  Serial2.begin(115200);

  Serial.println("motor run!!");
 /* Serial2.write(slowForwardMessage, sizeof(slowForwardMessage));
  delay(2000);

  Serial2.write(stopMessage, sizeof(stopMessage));
  Serial2.write(slowBackwardMessage, sizeof(slowBackwardMessage));
  delay(2000);
  Serial2.write(stopMessage, sizeof(stopMessage));
  */
  Serial2.write(slideRight, sizeof(slideRight));
  delay(2000);
  Serial2.write(stopMessage, sizeof(stopMessage));
}



void loop() {
  

  
}
