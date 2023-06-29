#include "src/M5Stack/M5Stack.h"

#define sp 1  // speed of motor 1 is slow probably right for this project, 2 is faster, 3 is faster still
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

void TaskKR( void *pvParameters ); //Knight Rider

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

xTaskCreatePinnedToCore(
    TaskKR
    ,  "TaskKR"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  Serial.println("motor run!!");
 /* Serial2.write(slowForwardMessage, sizeof(slowForwardMessage));
  delay(2000);

  Serial2.write(stopMessage, sizeof(stopMessage));
  Serial2.write(slowBackwardMessage, sizeof(slowBackwardMessage));
  delay(2000);
  Serial2.write(stopMessage, sizeof(stopMessage));
  */
 // Serial2.write(slideRight, sizeof(slideRight));
 delay(2000);
 
  move (slideLeft);
  delay(2000);
  //Serial2.write(stopMessage, sizeof(stopMessage));
  move (stop); 
}

void setLedColor(byte i, byte r, byte g, byte b){
    Serial2.write(0xAB);
    Serial2.write(i);
    Serial2.write(r);
    Serial2.write(g);
    Serial2.write(b);
    Serial2.write(0x55);
}

void loop() {
  

  
}

void TaskKR(void *pvParameters) {  
  (void) pvParameters;
  while(1) {
  for (int i = 0; i < 7; i++)
  {
    setLedColor(i, 20, 0, 0);
    delay(15);
    setLedColor(i, 0, 0, 0);
    delay(15);
  }
  for (int i = 7; i > 0; i--)
  {
    setLedColor(i, 20, 0, 0);
    delay(15);
    setLedColor(i, 0, 0, 0);
    delay(15);
  }
  vTaskDelay(10 / portTICK_PERIOD_MS);  
  }
}
