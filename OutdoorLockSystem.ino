#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Password.h>

#define BUZZER_PIN 11

Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Password password = Password("1234"); //Enter your password

String newPasswordString; //hold the new password
char newPassword[6]; //charater string of newPasswordString
byte a = 5;
bool value = true;

byte maxPasswordLength = 4; // Corrected maximum password length
byte currentPasswordLength = 0;

const byte ROWS = 3; // Three rows
const byte COLS = 3; // Three columns

char keys[ROWS][COLS] = {
  {'C', 'B', 'A'},
  {'9', '6', '3'},
  {'8', '5', '2'},
};

byte rowPins[ROWS] = {26, 25, 33};
byte colPins[COLS] = {32, 35, 34};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  servo.attach(10); // Attach servo to pin 10
  servo.write(50);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print("DOOR LOCK SYSTEM");
  delay(3000);
  lcd.clear();
}

void loop() {
  lcd.setCursor(1, 0);
  lcd.print("ENTER PASSWORD");
  char key = keypad.getKey();
  if (key != NO_KEY) {
    delay(60);
    if (key == 'C') {
      resetPassword();
    } else if (key == 'D') {
      if (value == false) {
        doorlocked();
        value = true;
      } else if (value == true) {
        dooropen();
        value = true;
      }
    } else {
      processNumberKey(key);
    }
  }
}

void processNumberKey(char key) {
  lcd.setCursor(a, 1);
  lcd.print("key");
  a++;
  if (a == 11) {
    a = 5;
  }
  currentPasswordLength++;
  password.append(key);
  if (currentPasswordLength == maxPasswordLength) {
    doorlocked();
    dooropen();
  }
}

void dooropen() {
  if (password.evaluate()) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(300);
    digitalWrite(BUZZER_PIN, LOW);
    servo.write(110); // Adjust angle for door open position
    delay(100);
    lcd.setCursor(0, 0);
    lcd.print("CORRECT PASSWORD");
    lcd.setCursor(0, 1);
    lcd.print("DOOR OPENED");
    delay(2000);
    lcd.clear();
    a = 5;
  } else {
    incorrectPassword();
  }
  resetPassword();
}

void doorlocked() {
  if (password.evaluate()) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(300);
    digitalWrite(BUZZER_PIN, LOW);
    servo.write(50); // Adjust angle for door locked position
    delay(100);
    lcd.setCursor(0, 0);
    lcd.print("CORRECT PASSWORD");
    lcd.setCursor(2, 1);
    lcd.print("DOOR LOCKED");
    delay(2000);
    lcd.clear();
    a = 5;
  } else {
    incorrectPassword();
  }
  resetPassword();
}

void incorrectPassword() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
  delay(200);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
  delay(200);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
  delay(200);
  lcd.setCursor(0, 0);
  lcd.print("WRONG PASSWORD!");
  lcd.setCursor(0, 1);
  lcd.print("PLEASE TRY AGAIN");
  delay(2000);
  lcd.clear();
  a = 5;
}

void resetPassword() {
  password.reset();
  currentPasswordLength = 0;
  lcd.clear();
  a = 5;
}
