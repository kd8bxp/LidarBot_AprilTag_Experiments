#include "src/M5Stack/M5Stack.h"

#define sp 1  // speed of motor 1 is slow probably right for this project, 2 is faster, 3 is faster still

uint8_t forward[6] = {0xAA, sp, sp, sp, sp, 0x55};
uint8_t stop[6] = {0xAA, 0x00, 0x00, 0x00, 0x00, 0x55};
uint8_t backward[6] = {0xAA, -sp, -sp, -sp, -sp, 0x55};
uint8_t slideLeft[6] = {0xAA, -sp, sp, sp, -sp, 0x55};
uint8_t slideRight[6] = {0xAA, sp, -sp, -sp, sp, 0x55};
uint8_t turnLeft[6] = {0xAA, -sp, sp, -sp, sp, 0x55};
uint8_t turnRight[6] = {0xAA, sp, -sp, sp, -sp, 0x55};


#define move(x) (Serial2.write(x, sizeof(x))) //function used to move robot call move(forward), move(stop), move(slideLeft) etc

void setup() {                
m5.begin();
  Serial.begin(115200); //not used in this sketch, will be used for the camera
  Serial2.begin(115200); //motor & led control

  Serial.println("motor run!!");
 /* Serial2.write(slowForwardMessage, sizeof(slowForwardMessage));
  delay(2000);

  Serial2.write(stopMessage, sizeof(stopMessage));
  Serial2.write(slowBackwardMessage, sizeof(slowBackwardMessage));
  delay(2000);
  Serial2.write(stopMessage, sizeof(stopMessage));
  */
 // Serial2.write(slideRight, sizeof(slideRight));
  move (slideLeft);
  delay(2000);
  //Serial2.write(stopMessage, sizeof(stopMessage));
  move (stop); 
}



void loop() {
  

  
}
