#include "src/M5Stack/M5Stack.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

void TaskKR( void *pvParameters ); //Knight Rider

void setup(){
    m5.begin();
    Serial2.begin(115200);  
    // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    TaskKR
    ,  "TaskKR"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
}

void setLedColor(byte i, byte r, byte g, byte b){
    Serial2.write(0xAB);
    Serial2.write(i);
    Serial2.write(r);
    Serial2.write(g);
    Serial2.write(b);
    Serial2.write(0x55);
}

void loop(){
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
