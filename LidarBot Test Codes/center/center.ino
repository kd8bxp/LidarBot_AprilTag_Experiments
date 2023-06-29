#include "src/M5Stack/M5Stack.h"
#include "src/ArduinoJson5/ArduinoJson.h"


#define sp 1  // speed of motor 1 is slow probably right for this project, 2 is faster, 3 is faster still

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define gridX 7 //number of squares in the x coridants ROWS
#define gridY 7 //number of squares in the y coridants COLS

//Define the boarder for the grid (AprilTag numbers) Realitive to the front of the LiDarBot
#define topTag 200
#define bottomTag 202
#define rightTag 201
#define leftTag 203

void TaskKR( void *pvParameters ); //Knight Rider

uint8_t forward[6] = {0xAA, sp, sp, sp, sp, 0x55};
uint8_t stop[6] = {0xAA, 0x00, 0x00, 0x00, 0x00, 0x55};
uint8_t backward[6] = {0xAA, -sp, -sp, -sp, -sp, 0x55};
uint8_t slideLeft[6] = {0xAA, -sp, sp, sp, -sp, 0x55};
uint8_t slideRight[6] = {0xAA, sp, -sp, -sp, sp, 0x55};
uint8_t turnLeft[6] = {0xAA, -sp, sp, -sp, sp, 0x55};
uint8_t turnRight[6] = {0xAA, sp, -sp, sp, -sp, 0x55};


//Macro
#define move(x) (Serial2.write(x, sizeof(x))) //function used to move robot call move(forward), move(stop), move(slideLeft) etc


String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

enum directions {unknown, top, right, bottom, left, up, down};

typedef struct {
int x;
int y;
int w;
int h;
int id;
int cx;
int cy;
float rotation;
}camera;

camera tagInfo;
directions robotFacing = unknown; 
directions moveDirection = unknown; //used for calculating move 

bool startMoving = false;
int currentTag;

int grid[gridX][gridY] = {
  {0, 1, 2, 3, 4, 5, 6},
  {7, 8, 9, 10, 11, 12, 13},
  {14, 15, 16, 17, 18, 19, 20},
  {21, 22, 23, 24, 25, 26, 27},
  {28, 29, 30, 31, 32, 33, 34},
  {35, 36, 37, 38, 39, 40, 41},
  {42, 43, 44, 45, 46, 47, 48},  
};

int row;
int col;


//Hard wired to goto location 3,3 for now April Tag ID #24
int targetID = 24;
int targetrow = 3; 
int targetcol = 3;
int target = grid[targetrow][targetcol]; 

void setup() {                
  M5.begin();
  M5.Lcd.setTextFont(4);
  Serial.begin(115200, SERIAL_8N1, 36,26);  //camera
  Serial2.begin(115200); //motor & led control

xTaskCreatePinnedToCore(
    TaskKR
    ,  "TaskKR"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

 M5.Lcd.clear();
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
  
if (startMoving == false) { buttonWait(); } //set Target row & col and wait to start
//else if (startMoving == true) { maintainCenter(); }
else if (startMoving == true) { findTag(); }
M5.update();
  
}

void buttonWait() {
  //M5.Lcd.clear();
  M5.Lcd.setCursor(0,0);
  M5.Lcd.println("LidarBot indoor Navigation");
  M5.Lcd.println();
  M5.Lcd.print("Target ID: "); M5.Lcd.println(targetID);
  M5.Lcd.println();
  M5.Lcd.print("Hit 'B' When Ready.");

  if (M5.BtnB.isPressed()) { findnumbers(targetID); startMoving = true; }
  M5.update();
}


void findTag() {
  M5.Lcd.clear();
  M5.Lcd.setCursor(0,0);
  
  readSerialEvent();
 
  if (stringComplete) {
    decodeJson();
    inputString = "";
    stringComplete = false;
  }
  M5.Lcd.print("Target ID: "); M5.Lcd.println(targetID);
  M5.Lcd.print("Seeing ID: "); M5.Lcd.println(tagInfo.id);

  if (tagInfo.id == topTag || tagInfo.id == rightTag || tagInfo.id == bottomTag || tagInfo.id == leftTag) {move(backward); delay(500); move(stop); return;}

  float r = (180 * tagInfo.rotation) / 3.141592653589793; //this is based on a circle of course 
  
  if (int(r) >= 70 && int(r) <= 110 ){robotFacing = top; M5.Lcd.println("Facing Top"); currentTag = tagInfo.id + 7;  } 
  if (int(r) >= 160 && int(r) <= 200 ) {robotFacing = right; M5.Lcd.println("Facing Right"); currentTag = tagInfo.id - 1; }
  if (int(r) >= 250 && int(r) <= 290 ) {robotFacing = bottom; M5.Lcd.println("Facing Bottom"); currentTag = tagInfo.id - 7; }
  if (int(r) >= 340 && int(r) <= 360)  {robotFacing = left; M5.Lcd.println("Facing Left"); currentTag = tagInfo.id + 1;} //left is a special case since it crosses zero/360 degrees
  if (int(r) > 0 && int(r) <= 20)  { robotFacing = left; M5.Lcd.println("Facing Left"); currentTag = tagInfo.id + 1;} //left is a special case since it crosses zero/360 degrees
  M5.Lcd.print("On ID: "); M5.Lcd.println(currentTag);

  //Angle not found within above parameters figure out which way to turn robot, and how far.
  //maybe just move to center? ???
  if (int(r) <= 341 && int(r) >= 291) { M5.Lcd.println("Bottom Left"); move(turnRight); delay(150); move(stop); maintainCenter(); return; }//robot is facing Bottom Left ? currentTag = tagInfo.id - 6 ?
  if (int(r) <= 251 && int(r) >= 201) { M5.Lcd.println("Bottom Right"); move(turnLeft); delay(150); move(stop); maintainCenter(); return; } //robot is facing bottom right? currentTag = tagInfo.id - 8 ?
  if (int(r) <= 161 && int(r) >= 111) { M5.Lcd.println("Top-Right"); move(turnRight); delay(150); move(stop); maintainCenter(); return; } //robot is facing top right? currentTag = tagInfo.id + 6 ?
  if (int(r) <= 71 && int(r) >= 21) { M5.Lcd.println("Top-Left"); move(turnLeft); delay(150); move(stop); maintainCenter(); return;  } //robot is facing top left? currentTag = tagInfo.id + 8 ?

 findnumbers(currentTag); //sets up row and col that the robot needs to get to
 calculatemove();
 


 //Facing TOP
if (robotFacing == top && moveDirection == up) { move(forward); } 
if (robotFacing == top && moveDirection == down) { move(backward); }
if (robotFacing == top && moveDirection == left) { move(slideLeft); }
if (robotFacing == top && moveDirection == right) { move(slideRight); }

//Facing BOTTOM
if (robotFacing == bottom && moveDirection == up) { move(backward);  }
if (robotFacing == bottom && moveDirection == down) { move(forward); }
if (robotFacing == bottom && moveDirection == left) { move(slideRight); }
if (robotFacing == bottom && moveDirection == right) { move(slideLeft); }

//Facing Right
if (robotFacing == right && moveDirection == up) { move(slideLeft); }
if (robotFacing == right && moveDirection == down) { move(slideRight); }
if (robotFacing == right && moveDirection == left) {  move(backward); }
if (robotFacing == right && moveDirection == right) { move(forward); }

//Facing Left
if (robotFacing == left && moveDirection == up) { move(slideRight); }
if (robotFacing == left && moveDirection == down) { move(slideLeft); }
if (robotFacing == left && moveDirection == left) { move(forward); }
if (robotFacing == left && moveDirection == right) { move(backward); }

delay(500);
move(stop);
//maintainCenter();
//tagInfo.id = NULL;

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

void findnumbers(int findnumber) {

  /* at this point there is no error checking, if a code is not
   *  found, it will return a grid of 0,0 (this could cause problems)
   */
   
int a, b;
  for (a = 0; a < gridX; a++) {
    for (b = 0; b < gridY; b++) {
      if (findnumber == grid[a][b]) {
        row = a;
        col = b;
        }
      }
  }
  /*Serial.print(row);
  Serial.print(",");
  Serial.println(col);*/
  M5.Lcd.print("Grid: "); M5.Lcd.print(row); M5.Lcd.print(","); M5.Lcd.println(col);
}

void calculatemove() {

/*This is the logic for the move, but currently only tells
 * a direction and how far the robot should move, it will should
 * recalculate each time it reads a new NFC tag, until target is
 * reached.
 */
//1= UP, 2= right, 3=down, 4=left 
moveDirection = unknown;

  if (targetrow > row) {
    
    M5.Lcd.println("move DOWN ");
    //Serial.print(targetrow - row);
    //Serial.println(" rows.");
    moveDirection = down; //3;
      }
  if (targetrow < row) {
    
    M5.Lcd.println("move UP ");
    //Serial.print (row - targetrow);
    //Serial.println(" rows.");
    moveDirection = up; //1;
  }
  /*if (targetrow == row) {
    Serial.println("We are on the correct row.");   
  }*/
  if (targetcol > col) {
    
    M5.Lcd.println("move Right");
    //Serial.print(targetcol - col);
    //Serial.println(" columns to the right");
    moveDirection = right;
  }
  if (targetcol < col) {
    
    M5.Lcd.println("move Left");
    //Serial.print(col - targetcol);
    //Serial.println(" columns to the left.");
    moveDirection = left; //4;
  }
  /*
  if (targetcol == col) {
    Serial.println("At the target column");
  }*/
  if (targetcol == col && targetrow == row) {
    
    M5.Lcd.println("We are at target.");
    M5.Lcd.clear();
    startMoving = false;
  }
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

void decodeJson() {
StaticJsonBuffer<650> jsonBuffer;

JsonObject& root = jsonBuffer.parseObject(inputString);

  // Test if parsing succeeds.
  if (!root.success()) {
    M5.Lcd.println("parseObject() failed");
    tagInfo.id = NULL;
    tagInfo.cx = NULL;
    tagInfo.cy = NULL;
    tagInfo.rotation = NULL;
    return;
  }

tagInfo.id = root["id"];
tagInfo.cx = root["cx"];
tagInfo.cy = root["cy"];
tagInfo.rotation = root["rotation"];

 
}

void maintainCenter() {
M5.Lcd.clear();
int csp = 1;
//readSerialEvent();
int currentID1 = tagInfo.id;
uint8_t slideCLeft[6] = {0xAA, -csp, csp, csp, -csp, 0x55};
uint8_t slideCRight[6] = {0xAA, csp, -csp, -csp, csp, 0x55};

while(1) {
  move(stop);
  M5.update();

readSerialEvent();
if (stringComplete) {
    decodeJson();
    inputString = "";
    stringComplete = false;
  }

double angle = atan2(tagInfo.cy - tagInfo.y,tagInfo.cx - tagInfo.x) * 180/3.141592653589793;

  if (tagInfo.id) {
 
 M5.Lcd.setCursor(0,0); 
M5.Lcd.print("MC Angle: ");
M5.Lcd.println(angle); //33.0 appears to be center for lidarBot (so make a range)

  //rotation may enter into this as well
if (angle <= 28.0 && angle >= 24.0 && tagInfo.id) {  move(stop);  return; }
if (angle <= 23.9 && tagInfo.id ) { move(slideCRight); delay(50); move(stop);  } 
if (angle >= 28.1 & tagInfo.id ) { move(slideCLeft); delay(50); move(stop);} 

         }
tagInfo.id = NULL;
    }
}
