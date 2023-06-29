#include "src/M5Stack/M5Stack.h"
#include "src/ArduinoJson5/ArduinoJson.h"

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

bool setGridFlag = true; //false;
int setGridTarget = 0;

int x;
int y;
int w;
int h;
int id;


void setup(){
    M5.begin();
    M5.Lcd.setTextFont(4);
    Serial2.begin(115200); 
    Serial.begin(115200, SERIAL_8N1, 36,26); 
}


void decodeJson() {
StaticJsonBuffer<650> jsonBuffer;

JsonObject& root = jsonBuffer.parseObject(inputString);

  // Test if parsing succeeds.
  if (!root.success()) {
    M5.Lcd.clear();
    M5.Lcd.setCursor(0,0);
    M5.Lcd.print("parseObject() failed");
    return;
  }

x = root["x"];
y = root["y"];
w = root["w"];
h = root["h"];
id = root["id"];

//Serial.print("x: "); Serial.println(x);
//Serial.print("y: "); Serial.println(y);
//Serial.print("w: "); Serial.println(w);
//Serial.print("h: "); Serial.println(h);
//Serial.print("ID: "); Serial.println(id);
M5.Lcd.clear();
M5.Lcd.setCursor(0,0);
M5.Lcd.print(id);
 
}

void readSerialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    if (inChar == '\n') {
      stringComplete = true;
    } else {    inputString += inChar; }
    
  }
}

void loop() {
  readSerialEvent();
  
  if (stringComplete) {
    //Serial.println(inputString);
    decodeJson();
    inputString = "";
    stringComplete = false;
  }
}
