// ------------------------------
// Softair/Paintball bomb timer
// 
// Latest Rev.: 04/10/2018
//
// Used board: Arduino Nano - Fw 3.3
// Supply voltage: 9-12Vdc
// ------------------------------

#include "LCD03.h"

#define DEACT_WIRE_1 0x02     // Digital pin D2
#define DEACT_WIRE_2 0x03     // Digital pin D3
#define DEACT_WIRE_3 0x04     // Digital pin D4
#define DEACT_WIRE_4 0x05     // Digital pin D5
#define DEACTIVATION_KEY 0x06 // Digital pin D6
#define BUZZER 0x07           // Digital pin D7
#define GREEN_LED 0x08        // Digital pin D8
#define YELLOW_LED 0x09       // Digital pin D9
#define RED_LED 0x0A          // Digital pin D10
#define SIREN 0x0B            // Digital pin D11

// Create a 'LCD03' object instance
LCD03 lcd;

// Global variables
uint8_t hours = 100;
uint8_t minutes = 0;
uint8_t seconds = 0;
uint8_t numbers[8];
uint8_t wire = 5;
uint8_t halfWire1 = 0;
uint8_t halfWire2 = 0;
uint8_t exploWire = 0;
uint8_t codeArray[4];
uint8_t insertedCodeArray[4];
unsigned long halfHours = 0;
unsigned long thirdHours = 0;

// ----------------------------------------------------------------------------------------------------------
// Function to print seconds on the display and set the cursor on the specified positions passed as argument
// ----------------------------------------------------------------------------------------------------------
void printSeconds(uint8_t cursorPosition) {
  lcd.setCursor(10, 0);
  if(seconds < 10) {
    lcd.write(48); // #0
  }
  lcd.print(seconds);
  lcd.setCursor(cursorPosition, 1);
}

// ----------------------------------------------------------------------------------------------------------------------
// Function to print minutes and seconds on the display and set the cursor on the specified positions passed as argument
// ----------------------------------------------------------------------------------------------------------------------
void printMinutes(uint8_t cursorPosition) {
  lcd.setCursor(7, 0);
  if(minutes < 10) {
    lcd.write(48); // #0
  }
  lcd.print(minutes);
  lcd.print(":");
  printSeconds(cursorPosition);
}

// -----------------------------------------------------------------------------------------------------------------------------
// Function to print hours, minutes and seconds on the display and set the cursor on the specified positions passed as argument
// -----------------------------------------------------------------------------------------------------------------------------
void printHours(uint8_t cursorPosition) {
  lcd.setCursor(4, 0);
  if(hours < 10) {
    lcd.write(48); // #0
  }
  lcd.print(hours);
  lcd.print(":");
  printMinutes(cursorPosition);
}

//---------------------------------------------------------
// Function to wait for the keyboard button to be released
//---------------------------------------------------------
void waitButtonRelease() {
  uint16_t keystate = lcd.readKeypad();

  while(keystate != 0) {
    keystate = lcd.readKeypad();
  }
}

// ----------------------------
// Function to beep the buzzer
// ----------------------------
void buzzerBeep(uint8_t times) {
  for(uint8_t k = 0; k < times; k++) {
    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
    delay(50);
  }
}

// -----------------------------------------------------------------------------------------------------------
// Function to create an array of numbers without the specified element 'avoidableElement' passed as argument
// -----------------------------------------------------------------------------------------------------------
void createArray(uint8_t resultArray[], uint8_t maxIndex, uint8_t avoidableElement) {
  uint8_t index = 0;
  for(uint8_t k = 0; k < maxIndex; k++) {
    if(resultArray[k] != avoidableElement) {
      resultArray[index] = resultArray[k];
      index++;
    }
  }
}

// --------------------------------
// Function to choose random wires
// --------------------------------
void chooseRandomWires() {
  uint8_t resultArray[] = {1, 2, 3, 4};

  createArray(resultArray, 4, wire);
  halfWire1 = resultArray[random(0, 3)];

  createArray(resultArray, 3, halfWire1);
  halfWire2 = resultArray[random(0, 2)];

  createArray(resultArray, 2, halfWire2);
  exploWire = resultArray[0];
}

// ----------------------------------------------------------------
// Function to check the wire passed as argument and half the time
// ----------------------------------------------------------------
void halfTheTime(uint8_t &wire, uint8_t cursorPosition) {
  if(digitalRead(wire + 1) == HIGH && wire != 0) { // Half the time
    hours = hours / 2;
    minutes = minutes / 2;
    seconds = seconds / 2;
    wire = 0;
    buzzerBeep(3);
    calculateThresholds();
    printHours(cursorPosition);
  }  
}

// --------------------------------------------------------------
// Function to check wires connections before starting the timer
// --------------------------------------------------------------
uint8_t checkWires(uint8_t cursorPosition) {
  if(wire == 0) { // All wires can let the bomb explode
    if(digitalRead(2) == HIGH || digitalRead(3) == HIGH || digitalRead(4) == HIGH || digitalRead(5) == HIGH) {
      return 1; // Explosion
    }
  } else {
    if(digitalRead(exploWire + 1) == HIGH) return 1; // Explosion
    
    halfTheTime(halfWire1, cursorPosition); // Half the time with halfWire1
    
    halfTheTime(halfWire2, cursorPosition); // Half the time with halfWire2

    if(digitalRead(wire + 1) == HIGH) { // Deactivated
      buzzerBeep(2);
      return 2;
    }
  }

  return 0;
}

// ---------------------------------------------------------------------
// Function to check wires and deactivation key connections before game
// ---------------------------------------------------------------------
void checkUp() {
  uint8_t message = 0;

  lcd.noCursor();

  while(1) {
    if(digitalRead(2) == HIGH) {
      if(message != 1) {
        lcd.clear();
        lcd.print("Controlla Filo 1");
        message = 1;
      }
      continue;
    }

    if(digitalRead(3) == HIGH) {
      if(message != 2) {
        lcd.clear();
        lcd.print("Controlla Filo 2");
        message = 2;
      }
      continue;
    }

    if(digitalRead(4) == HIGH) {
      if(message != 3) {
        lcd.clear();
        lcd.print("Controlla Filo 3");
        message = 3;
      }
      continue;
    }

    if(digitalRead(5) == HIGH) {
      if(message != 4) {
        lcd.clear();
        lcd.print("Controlla Filo 4");
        message = 4;
      }
      continue;
    }

    if(digitalRead(DEACTIVATION_KEY) == LOW) {
      if(message != 5) {
        lcd.clear();
        lcd.print("Controlla Chiave");
        lcd.print("   Disinnesco");
      }
      message = 5;
      continue;
    }

    // All checks passed
    break;
  }
}

// ------------------------------------------------------
// Function to return the ASCII value of the key pressed
// ------------------------------------------------------
uint8_t checkKey(uint16_t key){
    if(key == KEYPAD_1) {
      return 49;
    }

    if(key == KEYPAD_2) {
      return 50;
    }

    if(key == KEYPAD_3) {
      return 51;
    }

    if(key == KEYPAD_4) {
      return 52;
    }

    if(key == KEYPAD_5) {
      return 53;
    }

    if(key == KEYPAD_6) {
      return 54;
    }

    if(key == KEYPAD_7) {
      return 55;
    }

    if(key == KEYPAD_8) {
      return 56;
    }

    if(key == KEYPAD_9) {
      return 57;
    }

    if(key == KEYPAD_0) {
     return 48;
    }

    if(key == KEYPAD_STAR) {
      return 100;
    }

    if(key == KEYPAD_HASH) {
      return 200;
    }
}

// --------------------------------------
// Function to start the countdown timer
// --------------------------------------
void startTimer() {
  unsigned long hwTime = millis();
  unsigned long delayTime = 0;
  unsigned long startTime;
  unsigned long passedTime;
  uint16_t keystate;
  uint8_t temp;
  uint8_t cursorPosition = 6;
  uint8_t explode = 0;
  uint8_t deactivated = 0;
  uint8_t code = 0;
  uint8_t wiresStatus = 0;
  uint8_t keysReleased = 1;

  lcd.clear();
  lcd.cursor();

  printHours(cursorPosition);

  lcd.print("____");
  lcd.setCursor(6, 1);

  // Turn on the green led
  digitalWrite(GREEN_LED, HIGH);

  startTime = hwTime;

  while(explode == 0 && deactivated == 0) {
    delayTime += millis() - hwTime;
    hwTime = millis();
    passedTime = hwTime - startTime;
    
    // Blinking leds
    if(delayTime >= 500) {
      if(passedTime <= halfHours) {
        digitalWrite(GREEN_LED, HIGH);
      } else if(passedTime <= thirdHours) {
        digitalWrite(YELLOW_LED, HIGH);
      } else {
        digitalWrite(RED_LED, HIGH);
      }
    } else {
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(RED_LED, LOW);
    }

    if(delayTime >= 1000) {
      if(seconds > 0) {
        seconds -= 1;
        printSeconds(cursorPosition);
      } else if(minutes > 0){
        minutes -= 1;
        seconds = 59;
        printMinutes(cursorPosition);
      } else if(hours > 0) {
        hours -= 1;
        minutes = 59;
        seconds = 59;
        printHours(cursorPosition);
      } else {
        // ESPLOSIONE!
        explode = 1;
      }

      delayTime = delayTime - 1000;
    }

    // Check the wires status
    wiresStatus = checkWires(cursorPosition);
    if(wiresStatus == 1) {
      explode = 1;
    } else if(wiresStatus == 2) {
      deactivated = 1;
    }

    // Check the deactivation key
    if(digitalRead(DEACTIVATION_KEY) == LOW) {
      buzzerBeep(2);
      deactivated = 1;
    }

    // Check if the bomb has exploded
    if(explode == 0 && deactivated == 0) {
      // Start reading input
      keystate = lcd.readKeypad();
  
      if(keystate != 0 && keysReleased == 1) {
        buzzerBeep(1);
        temp = checkKey(keystate);

        keysReleased = 0; // Indicate that a key has been pressed

        if(temp < 100 && cursorPosition < 10) {
          insertedCodeArray[cursorPosition - 6] = temp - 48;
          //Serial.print(cursorPosition - 6);
          //Serial.print("\n");
          lcd.write(temp);
          cursorPosition++;
          if(cursorPosition == 10) {
            code = 1; // Signal that the code has been completely inserted
            lcd.setCursor(--cursorPosition, 1);
          }
        }
  
        // Check for the 'OK' key
        if(temp == 100) {
          if(code == 0) continue; // Check if a code has been inserted (4 digit)
          
          // Compare code
          for(uint8_t k = 0; k < 4; k++) {
            if(codeArray[k] != insertedCodeArray[k]) {
              explode = 1;
              break;
            }
          }
          // Check if the code was correct
          if(explode != 1) deactivated = 1;
        }

        // Check for the 'Delete' key
        if(temp == 200) {
          if(cursorPosition == 6) continue; // Check if the cursor is on the first character
          lcd.setCursor(--cursorPosition, 1);
        }
      } else if(keystate == 0 && keysReleased == 0) {
        keysReleased = 1;
      }  
    }
  }

  lcd.noCursor();
  lcd.setCursor(0, 1);
  
  if(explode == 1) {
    lcd.print("   BOOOOOOOM!");
    digitalWrite(SIREN, HIGH);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
  } else {
    lcd.print(" Disinnescata!!");
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
  }

  // Wait 8 seconds
  delay(8000);

  digitalWrite(SIREN, LOW);

  uint8_t message = 0;
  hwTime = millis();

  // Wait for the key press to restart
  while(1) {
    delayTime += millis() - hwTime;
    hwTime = millis();

    if(delayTime >= 3000) {
      delayTime = delayTime - 3000;
      lcd.setCursor(0, 1);
      if(message == 0) {
        message = 1;
        if(explode == 1) {
          lcd.print("   BOOOOOOOM!");
        } else {
          lcd.print(" Disinnescata!!");
        }    
      } else {
        message = 0;
        lcd.setCursor(0, 1);
        lcd.print("    Premi *");
        lcd.write(0);
        lcd.print("    ");
      }
    }

    keystate = lcd.readKeypad();

    if(keystate != 0) {
      temp = checkKey(keystate);

      if(temp == 100) {
        break;
      }
    }
  }
}

// ---------------------------
// Initial setup of the board
// ---------------------------
void setup() {
  // Made an output the built-in test led
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, INPUT_PULLUP); // Wire 1
  pinMode(3, INPUT_PULLUP); // Wire 2
  pinMode(4, INPUT_PULLUP); // Wire 3
  pinMode(5, INPUT_PULLUP); // Wire 4
  pinMode(DEACTIVATION_KEY, INPUT_PULLUP); // Deactivation key input
  pinMode(BUZZER, OUTPUT);      // Beep buzzer
  pinMode(GREEN_LED, OUTPUT);   // Green Led
  pinMode(YELLOW_LED, OUTPUT);  // Yellow Led
  pinMode(RED_LED, OUTPUT);     // Red Led
  pinMode(SIREN, OUTPUT);       // Siren and/or other alarm

  // Initialize the serial port
  //Serial.begin(9600);

  digitalWrite(GREEN_LED, HIGH);
  delay(300);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  delay(300);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  delay(300);
  digitalWrite(RED_LED, LOW);

  buzzerBeep(2);

  // Random seed setting
  randomSeed(analogRead(0));

  // Call the function to initialize the LCD
  initializeLCD();
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  // Call the function to setup the countdown timer
  setupTimer();

  // Call the function to start the countdown timer
  startTimer();
}

// Function to initialize the LCD display
void initializeLCD() {
  uint8_t rightArrow[] = {
    128,
    132,
    130,
    159,
    130,
    132,
    128,
    128
  };

  // Turn off the internal pull-up resistors on SDA (23-A4) and SCL (24-A5) pins
  pinMode(SDA, INPUT);
  pinMode(SCL, INPUT);

  // Initialise the LCD
  lcd.begin();

  // Turn on the backlight
  lcd.backlight();
  
  // Write to the LCD the initial message
  lcd.setCursor(3, 0);
  lcd.print("Timed Bomb");

  lcd.setCursor(6, 1);
  lcd.print("v1.0");

  // Create a custom char (right arrow)
  lcd.createChar(0, rightArrow);

  // Double the scan rate
  lcd.doubleRateScan();

  // Wait for x seconds
  delay(1000);
 
  // Clear the LCD
  lcd.clear();
}

// -------------------------------
// Function to setup the timer
// -------------------------------
void setupTimer() {
  uint8_t reset;
  uint16_t keystate;
  uint8_t cursorPosition;
  uint8_t temp;
  uint8_t timerSet;
  uint8_t codeSet;

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  do
  {
    lcd.clear();
    lcd.print(" Imposta  Tempo");
    lcd.setCursor(4, 1);
    lcd.print("HH:MM:SS");
    lcd.setCursor(4, 1);
    lcd.blink();

    timerSet = 0;
    cursorPosition = 4;

    //uint8_t i = 0;

    // -------------------------------------------------
    // Read six characters from the keyboard (HH-MM-SS)
    // -------------------------------------------------
    while(1) {
      keystate = lcd.readKeypad();

      if(keystate != 0) {
        buzzerBeep(1);
        temp = checkKey(keystate);

        // Check for a backspace
        if(temp == 200) {
          if(cursorPosition > 4) { // Check if the cursor is not on the first character
            if(cursorPosition - 1 == 6 || cursorPosition - 1 == 9) {
              lcd.setCursor(cursorPosition - 2, 1);
              cursorPosition -= 2;
            } else {
              lcd.setCursor(cursorPosition - 1, 1);
              cursorPosition -= 1;             
            }
            //i -= 2;
          } else {
            waitButtonRelease();
            continue; // Ignore the key
          }
        } else if(temp == 100){
          if(timerSet == 1) {
            waitButtonRelease();
            break; 
          }
          else {
            waitButtonRelease();
            continue; 
          }
        } else {
          // Check to avoid too high minutes and seconds (above 59)
          if(cursorPosition == 7 || cursorPosition == 10) {
            if(temp > 53) continue;
          }
          numbers[cursorPosition - 4] = temp - 48;
          
          lcd.write(temp);
          cursorPosition++;
        }
  
        // Check to avoid timer's colon (:)
        if(cursorPosition == 6 || cursorPosition == 9) {
          lcd.setCursor(++cursorPosition, 1);
        } else if(cursorPosition == 12){
          lcd.setCursor(14, 1);
          lcd.write(42); //*
          lcd.write(0);  // Custom char right arrow
          lcd.setCursor(11, 1);
          cursorPosition = 11;
          timerSet = 1;
        }

        waitButtonRelease();
      }
    }

    // ----------------------------------------
    // Set the number of the deactivation wire
    // ----------------------------------------
    lcd.clear();
    lcd.print("  Imposta Filo");
    lcd.setCursor(6, 1);
    lcd.print("N. _");
    lcd.setCursor(9, 1);
    lcd.noBlink();

    wire = 5;
    halfWire1 = 0;
    halfWire2 = 0;

    while(1) {
      keystate = lcd.readKeypad();

      if(keystate != 0) {
        buzzerBeep(1);
        temp = checkKey(keystate);
  
        // Accept only numbers from 0 to 4
        if(temp > 47 && temp < 53) {
          if (temp == 48) {
            if (wire != 0) {
              //lcd.clear();
              //lcd.print("  Imposta Filo  ");
              lcd.setCursor(1, 1);
              lcd.print("Nessun  Filo *");
              lcd.write(0); // Custom char right arrow
              wire = 0;
            }
          } else {
            if(wire == 0) {
              lcd.clear();
              lcd.print("  Imposta Filo");
              lcd.setCursor(6, 1);
              lcd.print("N. ");
            }
            wire = temp - 48;
            lcd.write(temp);
            lcd.setCursor(9, 1);
            lcd.setCursor(14, 1);
            lcd.write(42); //*
            lcd.write(0);  // Custom char right arrow
            lcd.setCursor(9, 1);
          }
        } else if(temp == 100 && wire != 5) { // Check if a wire has been set
          if(wire != 0) {
            chooseRandomWires();
          }
          waitButtonRelease();
          break;
        }

        waitButtonRelease();
      }
    }
  
    // --------------------------
    // Set the deactivation code
    // --------------------------
    lcd.clear();
    lcd.print(" Imposta Codice");
    lcd.setCursor(6, 1);
    lcd.print("____");
    lcd.setCursor(6, 1);
    lcd.cursor();

    codeSet = 0;
    cursorPosition = 6;
  
    while(1) {
      keystate = lcd.readKeypad();

      if(keystate != 0) {
        buzzerBeep(1);
        temp = checkKey(keystate);

        // Check for the 'OK' command only if the code has been completely inserted
        if(temp == 100) {
          if(codeSet == 1) {
            waitButtonRelease();
            break;
          } else {
            waitButtonRelease();
            continue;
          }
        }

        // Check for the 'Delete' command only if the cursor is over the first character
        if(temp == 200) {
          if(cursorPosition > 6) {
            lcd.setCursor(--cursorPosition, 1);    
          }
        } else {
          codeArray[cursorPosition - 6] = temp - 48;
          lcd.write(temp);
          cursorPosition++;
        }

        // Check if the code has been submitted completely
        if(cursorPosition == 10) {
          codeSet = 1;
          cursorPosition--;
          lcd.setCursor(14, 1);
          lcd.write(42); //*
          lcd.write(0);  // Custom char right arrow
          lcd.setCursor(cursorPosition, 1);
        }

        waitButtonRelease();
      }    
    }

    // Check all the wires and the activation key position
    checkUp();
  
    // ----------------------
    // Show a summary screen
    // ----------------------
    lcd.clear();
  
    // Show the time
    for(uint8_t k = 0; k < 8; k++) {
      if(k == 2 || k == 5) {
        lcd.write(58); // Colon
        continue;
      }
      lcd.write(numbers[k] + 48);
    }
  
    // Show the deactivation wire number
    lcd.write(32); // Space
    if(wire == 0) {
      lcd.print("F-");
    } else {
      lcd.write(70); // F uppercase
      lcd.write(wire + 48);
    }
  
    // Show the deactivation code
    lcd.write(32); // Space
    for(uint8_t i = 0; i < 4; i++) {
      lcd.write(codeArray[i] + 48);
    }
  
    // On the second line write the available commands
    lcd.print("*Start    #Reset");

    delay(1000);
    
    keystate = 0;
    while(1) {
      keystate = lcd.readKeypad();
      if(keystate != 0) {
        temp = checkKey(keystate);

        // Reset command
        if(temp == 200) {
          buzzerBeep(1);
          cursorPosition = 4;
          reset = 1;
          break;
        }

        // Start command
        if(temp == 100) {
          buzzerBeep(1);
          calculateTime();
          reset = 0;
          break;
        }
      }
    }
  } while (reset == 1);
}

// -----------------------------------------------------------------------
// Function to calculate the total amount of milliseconds of the set time
// -----------------------------------------------------------------------
void calculateTime() {
  hours = numbers[0] * 10 + numbers[1];
  minutes = numbers[3] * 10 + numbers[4];
  seconds = numbers[6] * 10 + numbers[7];

  calculateThresholds();
}

// -----------------------------------------------------------
// Function to calculate the thresholds for the blinking leds
// -----------------------------------------------------------
void calculateThresholds() {
  // Calculate the total amount of milliseconds to wait
  unsigned long totalMsTime = (seconds * 1000);
  totalMsTime += minutes * 60000;
  totalMsTime += hours * 3600000;
  
  // Calculate the 1/3 of the total time
  halfHours = totalMsTime / 3;

  // Calculate the 2/3 of the total time
  thirdHours = halfHours * 2;
}
