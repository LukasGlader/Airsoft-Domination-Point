#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <splash.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); //1.3" SH1106 display

/*
  Code for domination game mode bases.

  Circuit:
  - button for blue team attached to pin BLUE from +5v
  - button for yellow team attached to pin YELLOW from +5v
  - button for orange team attached to pin ORANGE from +5v
  - I2C compatible 128x64 display connected to GND, VCC (+5v), SDA, SCL

*/

// memory locations on EEPROM where we should backup score and state in case of power loss.
const int BLUE_SCORE_ADDR = 0;
const int YELLOW_SCORE_ADDR = 31;
const int ORANGE_SCORE_ADDR = 63;
const int LAST_OWNER_ADDR = 95;


const int BLUE = 2; // team ID and pin number
const int YELLOW = 3; // team ID and pin number
const int ORANGE = 4; // team ID and pin number
const int NONE = 255; // team ID


const int REFRESH_RATE = 50; // ms sleep time between main loop iterations
const unsigned long CLEAR_SCORE_TIME = 5000; // how long the buttons should be pressed before we clear the score.
const unsigned long BACKUP_INTERVAL = 60000; // make sure we don't murder the EEPROM by constantly writing to it.
const unsigned long UI_REFRESH_INTERVAL = 1000; // how long to wait (in ms) between screen refresh if no user is interacting with the box.

// Current game stats
unsigned long blueScore = 0;
unsigned long yellowScore = 0;
unsigned long orangeScore = 0;
int currentOwner = NONE;

boolean previouslyAllPressed = false;
unsigned long allPressStart = -1; // start time of reset press

unsigned long lastPointCountTime = millis(); // the last time points were added, needed to get an exact account of how many points to add.
unsigned long lastUiRefresh = millis();
unsigned long lastBackup = millis();

boolean uiDirty = true; // set this flag to 'true' to force the main loop to refresh the screen.

void setup() {
  Serial.begin(9600);
  initPins();
  setupOled();
  loadPreviousScore();
  updateUI();
}

void loop() {
  if (allPressed()) { // game admin wants something
    if (previouslyAllPressed) {
      if (scoreResetTimePassed()) { // game admin has held down the buttons for a long time.
        resetScore();
        setOwner(NONE);
        u8x8.clear(); // clear the entire screen in order to remove any artefact chars. Causes the screen to blink once.
        uiDirty = true;
        updateUI();
        delay(2000); // give the admin some time to release the buttons.
      }
    } else { // this is the first moment we see all buttons being pressed at teh same time.
      allPressStart = millis();
      previouslyAllPressed = true;
    }
  } else {
    if (previouslyAllPressed) { // game admin released an all button press.
      setOwner(NONE);
      previouslyAllPressed = false;
      uiDirty = true;
      updateUI();
      delay(2000); // give the admin some time to release the buttons after the score clearing.
    }

    if (singleButtonPressed()) {
      setOwner(getCurrentPress());
      uiDirty = true;
      updateUI();
    }
  }
  addScore(currentOwner);
  if (millis() - lastBackup > BACKUP_INTERVAL) {
    backupScore();
  }
  updateUI();
  delay(REFRESH_RATE);
}

void updateUI() {
  if (millis() - lastUiRefresh > UI_REFRESH_INTERVAL || uiDirty) {
    updateOled();
    lastUiRefresh = millis();
    uiDirty = false;
  }
}

void setupOled() {
  u8x8.begin();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  u8x8.clear();
}
void updateOled() {
  u8x8.setCursor(0, 0);
  printCurrentOwner();

  u8x8.setCursor(0, 4);
  u8x8.println("Score:");
  printBlueScore();
  printYellowScore();
  printOrangeScore();
  u8x8.display();
}

void printBlueScore() {
  u8x8.print("Blue:  ");
  printScore(blueScore);
}
void printYellowScore() {
  u8x8.print("Yellow:");
  printScore(yellowScore);
}
void printOrangeScore() {
  u8x8.print("Orange:");
  printScore(orangeScore);
}

void printScore(long score) {
  printWithWhitespaceAndColon(getHours(score), false);
  printWithWhitespaceAndColon(getMinutes(score), getHours(score) > 0);
  printWithWhitespace(getSeconds(score), getMinutes(score) > 0 || getHours(score) > 0);
}

/*
   prints the given numerical value with a length of 3 characters (ending with either a colon or whitespace).
*/
void printWithWhitespaceAndColon(int val, boolean padZero) {
  if (val == 0) {
    if (padZero) {
      u8x8.print("00:");
    } else {
      u8x8.print("   ");
    }
  } else if (val < 10) {
    if (padZero) {
      u8x8.print("0");
    } else {
      u8x8.print(" ");
    }
    u8x8.print(val);
    u8x8.print(":");
  } else {
    u8x8.print(val);
    u8x8.print(":");
  }
}

/*
  prints the given 0-99 numerical value left padded with spaces.
  NOTE: this function terminates the row.
*/
void printWithWhitespace(int val, boolean padZero) {
  if (val == 0) {
    if (padZero) {
      u8x8.println("00");
    } else {
      u8x8.println("  ");
    }
  } else if (val < 10) {
    if (padZero) {
      u8x8.print("0");
    } else {
      u8x8.print(" ");
    }
    u8x8.println(val);
  } else {
    u8x8.println(val);
  }
}

void printCurrentOwner() {
  u8x8.println("Owner:");
  if (currentOwner == BLUE) {
    u8x8.println("Team Blue     ");
  } else if (currentOwner == YELLOW) {
    u8x8.print("Team Yellow");
  } else if (currentOwner == ORANGE) {
    u8x8.print("Team Orange");
  } else {
    u8x8.println("            "); // effectively clears the line
  }
}

int getSeconds(long ms) {
  return (ms / 1000) % 60;
}
int getMinutes(long ms) {
  return ((ms / 1000) / 60) % 60;
}
int getHours(long ms) {
  return ((ms / 1000)) / 3600 ;
}

void addScore(int team) {
  if (currentOwner == BLUE) {
    blueScore += millis() - lastPointCountTime;
  } else if (currentOwner == YELLOW) {
    yellowScore += millis() - lastPointCountTime;
  } else if (currentOwner == ORANGE) {
    orangeScore += millis() - lastPointCountTime;
  }
  lastPointCountTime = millis();
}

void resetScore() {
  blueScore = 0;
  yellowScore = 0;
  orangeScore = 0;
  uiDirty = true;
}

boolean scoreResetTimePassed() {
  return millis() - allPressStart > CLEAR_SCORE_TIME;
}

/*
   Returns the button that is currently being pressed. Note that this method
   assumes that zero or one buttons are being pressed, if two or more buttons are
   pressed the result is an arbitrary button.
*/
int getCurrentPress() {
  if (isPressed(BLUE)) {
    return BLUE;
  } else if (isPressed(YELLOW)) {
    return YELLOW;
  } else if (isPressed(ORANGE)) {
    return ORANGE;
  } else {
    return NONE;
  }
}

void initPins () {
  pinMode(BLUE, INPUT_PULLUP);
  pinMode(YELLOW, INPUT_PULLUP);
  pinMode(ORANGE, INPUT_PULLUP);
}

void setOwner(int newOwner) {
  if (currentOwner != newOwner) {
    currentOwner = newOwner;
    backupScore(); // incorrect backup state of owner is really bad.
  }
}

boolean allPressed() {
  return pressCount() == 3;
}

boolean nonePressed() {
  return pressCount() == 0;
}

boolean singleButtonPressed() {
  return pressCount() == 1;
}

int pressCount() {
  int result = 0;
  if (isPressed(BLUE)) {
    result = result + 1;
  }
  if (isPressed(YELLOW)) {
    result = result + 1;
  }
  if (isPressed(ORANGE)) {
    result = result + 1;
  }
  return result;
}

boolean isPressed(int owner) {
  return digitalRead(owner) == LOW;
}

void loadPreviousScore() {
  blueScore = loadLong(BLUE_SCORE_ADDR);
  yellowScore = loadLong(YELLOW_SCORE_ADDR);
  orangeScore = loadLong(ORANGE_SCORE_ADDR);
  currentOwner = EEPROM.read(LAST_OWNER_ADDR);
  lastBackup = millis();
}

void backupScore() {
  storeLong(blueScore, BLUE_SCORE_ADDR);
  storeLong(yellowScore, YELLOW_SCORE_ADDR);
  storeLong(orangeScore, ORANGE_SCORE_ADDR);
  EEPROM.update(LAST_OWNER_ADDR, currentOwner);
  lastBackup = millis();
}

long loadLong(long addr) {
  long four = EEPROM.read(addr);
  long three = EEPROM.read(addr + 1);
  long two = EEPROM.read(addr + 2);
  long one = EEPROM.read(addr + 3);

  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void storeLong(unsigned long value, long addr) {
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  EEPROM.update(addr, four);
  EEPROM.update(addr + 1, three);
  EEPROM.update(addr + 2, two);
  EEPROM.update(addr + 3, one);
}
