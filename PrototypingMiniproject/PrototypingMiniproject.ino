#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define START 3
#define NEXT 2
#define BACK 5
#define END 4

#define NUM_BUTTONS 8
#define NUM_KEYS 4

int buttons[NUM_BUTTONS] = {6, 7, 8, 9, 10, 11, 12, 13};
int keys[NUM_KEYS] = {START, NEXT, BACK, END};

LiquidCrystal_I2C lcd(0x27, 16, 2);

int generatedNumbers[NUM_KEYS];
int userInput[NUM_KEYS];
int inputIndex = 0;

enum State {IDLE, GENERATE, USER_INPUT, CHECK, WAIT};
State currentState = IDLE;

void setup() {
  randomSeed(analogRead(0));
  lcd.init();
  lcd.backlight();
  for (int i = 0; i < NUM_KEYS; i++) {
    pinMode(keys[i], INPUT_PULLUP);
  }
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }
  lcd.print("Press START to begin");
}

void loop() {
  switch (currentState) {
    case IDLE:
      if (digitalRead(START) == LOW) {
        currentState = GENERATE;
      }
      break;
    case GENERATE:
      generateNumbers();
      lcd.clear();
      displayNumbers();
      delay(3000);
      lcd.clear();
      lcd.print("Input numbers:");
      lcd.setCursor(0, 2);
      lcd.print("Press BACK to delete");
      inputIndex = 0;
      currentState = USER_INPUT;
      break;
    case USER_INPUT:
      readUserInput();
      if (inputIndex >= NUM_KEYS && digitalRead(NEXT) == LOW) {
        delay(200); // Wait for the NEXT button to be released
        currentState = CHECK;
      }
      break;

    case CHECK:
      lcd.clear();
      checkUserInput();
      currentState = WAIT;
      break;
    case WAIT:
      if (digitalRead(NEXT) == LOW) {
        currentState = GENERATE;
      } else if (digitalRead(BACK) == LOW) {
        currentState = USER_INPUT;
      }
      break;
  }
  if (digitalRead(END) == LOW) {
    currentState = IDLE;
    lcd.clear();
    lcd.print("Press START to begin");
  }
}

void generateNumbers() {
  for (int i = 0; i < NUM_KEYS; i++) {
    generatedNumbers[i] = random(1, NUM_BUTTONS + 1);
  }
}

void displayNumbers() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Memorize this:");
  lcd.setCursor(0, 1);
  for (int i = 0; i < NUM_KEYS; i++) {
    lcd.print(generatedNumbers[i]);
  }
}


void readUserInput() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (digitalRead(buttons[i]) == LOW && inputIndex < NUM_KEYS) {
      userInput[inputIndex] = i + 1;
      lcd.setCursor(inputIndex, 1);
      lcd.print(userInput[inputIndex]);
      inputIndex++;
      if (inputIndex >= NUM_KEYS) {
        lcd.setCursor(0, 2);
        lcd.print("Press NEXT          ");
      }
      delay(200); // Ignore any additional button presses for 200 milliseconds
      break;
    }
  }
  if (digitalRead(BACK) == LOW && inputIndex > 0) {
    inputIndex--;
    lcd.setCursor(inputIndex, 1);
    lcd.print(" "); // Clear the previous number
    delay(200); // Ignore any additional button presses for 200 milliseconds
  }
}

void checkUserInput() {
  bool isCorrect = true;
  for (int i = 0; i < NUM_KEYS; i++) {
    if (userInput[i] != generatedNumbers[i]) {
      isCorrect = false;
      break;
    }
  }
  if (isCorrect) {
    lcd.print("Good job!");
    lcd.setCursor(0, 2);
    lcd.print("NEXT to try again");
  } else {
    lcd.print("Please try again!");
    lcd.setCursor(0, 2);
    lcd.print("NEXT to try new");
  }
}