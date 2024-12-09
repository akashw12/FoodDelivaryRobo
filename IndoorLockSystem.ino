#include <Keypad.h>
#include <Servo.h>

// Define the number of rows and columns
const byte ROWS = 4;  // 4 Rows
const byte COLS = 4;  // 4 Columns

// Define the keys on your keypad
char keys[ROWS][COLS] = {
  {'A', '3', '2', '1'},
  {'B', '6', '5', '4'},
  {'C', '9', '8', '7'},
  {'D', '#', '0', '*'}
};

// Connect keypad rows and columns to Arduino pins
byte rowPins[ROWS] = {2, 3, 4, 5}; // Rows connected to pins 2, 3, 4, 5
byte colPins[COLS] = {6, 7, 8, 9}; // Columns connected to pins 6, 7, 8, 9

// Create the Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Define password
String correctPassword = "123";
String enteredPassword = "";

// Create a Servo object
Servo myServo;
int lockPosition = 0;    // Locked position
int unlockPosition = 90;  // Unlocked position

void setup() {
  Serial.begin(9600);
  myServo.attach(10);     // Attach servo to pin 10
  myServo.write(lockPosition);  // Start in locked position
}

void loop() {
  char key = keypad.getKey();  // Check for key press

  if (key) {  // If a key is pressed, process it
    Serial.println(key);
    
    if (key == 'A') {
      // Clear all input
      enteredPassword = "";
      Serial.println("Input cleared.");
    } 
    else if (key == 'B') {
      // Bypass password and unlock immediately
      unlockLockBypass();
    } 
    else if (key == 'C') {
      // Ignore C input (optional)
      Serial.println("Ignored input: C");
    } 
    else if (key == 'D') {
      // Confirm password entry
      if (enteredPassword.length() == 3) {
        if (enteredPassword == correctPassword) {
          unlockLock();  // Unlock if the password is correct
        } else {
          Serial.println("Wrong Password!");  // Notify if wrong password
        }
      } else {
        Serial.println("Enter 3 digits first.");
      }
      enteredPassword = "";  // Reset entered password after confirmation
    } 
    else if (enteredPassword.length() < 3) {
      // Add pressed key to entered password if it's less than 3 digits
      enteredPassword += key;  
      Serial.print("Entered Password: ");
      Serial.println(enteredPassword);
    }
  }
}

void unlockLock() {
  Serial.println("Correct Password! Unlocking...");
  myServo.write(unlockPosition); // Unlock the servo
  delay(5000);                   // Wait for 5 seconds (lock stays open)
  Serial.println("Locking again...");
  myServo.write(lockPosition);   // Lock the servo again
}

void unlockLockBypass() {
  Serial.println("Bypassing password! Unlocking immediately...");
  myServo.write(unlockPosition); // Unlock the servo
  delay(5000);                   // Wait for 5 seconds (lock stays open)
  Serial.println("Locking again...");
  myServo.write(lockPosition);   // Lock the servo again
}
