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
int cx;
int cy;
float rotation;

int startingTag; //starting tag
int currentTag;

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
    x = NULL; 
    y = NULL;
    w = NULL;
    h = NULL;
    id = NULL;
    cx = NULL;
    cy = NULL;
    rotation = NULL;

    return;
  }

x = root["x"];
y = root["y"];
w = root["w"];
h = root["h"];
id = root["id"];
cx = root["cx"];
cy = root["cy"];
rotation = root["rotation"];

//Serial.print("x: "); Serial.println(x);
//Serial.print("y: "); Serial.println(y);
//Serial.print("w: "); Serial.println(w);
//Serial.print("h: "); Serial.println(h);
//Serial.print("ID: "); Serial.println(id);
M5.Lcd.clear();
M5.Lcd.setCursor(0,0);
M5.Lcd.print("Seeing ID: ");
M5.Lcd.println(id);

float r = (180 * rotation) / 3.141592653589793; //this is based on a circle of course 
  //Serial.println(r);

if (int(r) >= 70 && int(r) <= 110 ){ M5.Lcd.println("Facing Top"); currentTag = id + 7;  } 
if (int(r) >= 160 && int(r) <= 200 ) {M5.Lcd.println("Facing Right"); currentTag = id - 1; }
if (int(r) >= 250 && int(r) <= 290 ) { M5.Lcd.println("Facing Bottom"); currentTag = id - 7; }
if (int(r) >= 340 && int(r) <= 360)  { M5.Lcd.println("Facing Left"); currentTag = id + 1;} //left is a special case since it crosses zero/360 degrees
if (int(r) > 0 && int(r) <= 20)  { M5.Lcd.println("Facing Left"); currentTag = id + 1;} //left is a special case since it crosses zero/360 degrees
M5.Lcd.print("On ID: "); M5.Lcd.println(startingTag);

//Angle not found within above parameters figure out which way to turn robot, and how far.
//maybe just move to center? ???
if (int(r) <= 341 && int(r) >= 291) { M5.Lcd.println("Bottom Left");; }//robot is facing Bottom Left ? 
if (int(r) <= 251 && int(r) >= 201) { M5.Lcd.println("Bottom Right");;} //robot is facing bottom right?
if (int(r) <= 161 && int(r) >= 111) { M5.Lcd.println("Top-Right");;} //robot is facing top right?
if (int(r) <= 71 && int(r) >= 21) { M5.Lcd.println("Top-Left?");;} //robot is facing top left?
 
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
