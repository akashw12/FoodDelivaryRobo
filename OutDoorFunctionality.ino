#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h> // Include the Servo library
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define ROW_NUM     3 // three rows
#define COLUMN_NUM  4 // four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'*', '7', '4', '1'},
  {'0', '8', '5', '2'},
  {'#', '9', '6', '3'}
};

byte pin_rows[ROW_NUM]      = {19, 18, 5}; // GPIO19, GPIO18, GPIO5 connect to the row pins
byte pin_column[COLUMN_NUM] = {13, 4, 14, 15}; // GPIO13, GPIO4, GPIO14, GPIO15 connect to the column pins

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns and 2 rows
Servo servo; // Create a servo object

int cursorColumn = 5;
String enteredPassword = "";

const String correctPassword = "1234"; // Set the correct password
const int servoPin = 12; // Define the pin for the servo motor

TinyGPSPlus gps;
SoftwareSerial gpsSerial(1, 3);

#define MOTOR1_PIN1 34
#define MOTOR1_PIN2 35
#define MOTOR2_PIN1 32
#define MOTOR2_PIN2 33
#define TRIGGER_PIN 23
#define ECHO_PIN 22

const double distance_threshold = 10; // Threshold distance in meters
const int max_speed = 255; // Maximum motor speed
const int min_speed = 100; // Minimum motor speed
const int obstacle_threshold = 20; // Obstacle detection threshold in centimeters

const double pointA_lat = 18.5853281;
const double pointA_lon = 73.7378231;
const double pointB_lat = 18.5844443; // Update with Point B latitude
const double pointB_lon = 73.7367515; // Update with Point B longitude

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  lcd.init();
  lcd.backlight();
  servo.attach(servoPin);

  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  lcd.setCursor(0, 0);
  lcd.print("Enter your");
  lcd.setCursor(0, 1);
  lcd.print("password:");
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    lcd.clear(); // Clear the LCD when any key is pressed

    if (key == '#') { // Check if the user presses the '#' key
      if (enteredPassword == correctPassword) {
        lcd.setCursor(0, 0);
        lcd.print("Door unlocked");
        delay(2000); // Display message for 2 seconds
        
        // Rotate the servo motor to 90 degrees
        servo.write(90);
        delay(10000); // Delay to allow the servo to reach its position
        
        // Display door will lock in 5 sec
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Door will lock");
        lcd.setCursor(0, 1);
        lcd.print("in 5 sec");
        delay(5000);

        // Reset the servo to the initial position
        servo.write(0);
      } else {
        lcd.setCursor(0, 0);
        lcd.print("Wrong OTP");
        delay(2000); // Display message for 2 seconds
      }
      enteredPassword = ""; // Reset entered password
    } else if (key == '*') { // Check if the user presses the '*' key
      // Reset the entered password and display
      enteredPassword = "";
      lcd.setCursor(0, 1);
      lcd.print("               "); // Clear entered password display
      cursorColumn = 9; // Reset cursor position
    } else {
      enteredPassword += key; // Append the pressed key to the entered password

      // Display the entered password on the LCD
      lcd.setCursor(cursorColumn, 1);
      lcd.print(key);

      cursorColumn++; // Move cursor to the next position

      if (cursorColumn == 16 || key == '*') { // Reset cursor if reaching the limit or '*' is pressed
        cursorColumn = 0;
      }
    }

    // Re-display the prompt message after clearing
    lcd.setCursor(0, 0);
    lcd.print("Enter your");
    lcd.setCursor(0, 1);
    lcd.print("OTP:");
  }

  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        double current_lat = gps.location.lat();
        double current_lon = gps.location.lng();

        double distance_to_pointB = distanceBetweenPoints(current_lat, current_lon, pointB_lat, pointB_lon);
        
        if (distance_to_pointB <= distance_threshold) {
          stopMotors();
          Serial.println("Arrived at Point B!");
        } else {
          double heading = calculateHeading(current_lat, current_lon, pointB_lat, pointB_lon);
          
          if (checkObstacle()) {
            avoidObstacle();
          } else {
            adjustMotorSpeeds(heading);
          }
        }
      }
    }
  }
}

double distanceBetweenPoints(double lat1, double lon1, double lat2, double lon2) {
  double R = 6371000; // Earth radius in meters
  double dLat = radians(lat2 - lat1);
  double dLon = radians(lon2 - lon1);
  double a = sin(dLat / 2) * sin(dLat / 2) + cos(radians(lat1)) * cos(radians(lat2)) * sin(dLon / 2) * sin(dLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  double d = R * c;
  return d;
}

double calculateHeading(double lat1, double lon1, double lat2, double lon2) {
  double dLon = radians(lon2 - lon1);
  double y = sin(dLon) * cos(radians(lat2));
  double x = cos(radians(lat1)) * sin(radians(lat2)) - sin(radians(lat1)) * cos(radians(lat2)) * cos(dLon);
  double heading = atan2(y, x);
  return degrees(heading);
}

bool checkObstacle() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  double distance = duration * 0.034 / 2;
  if (distance <= obstacle_threshold) {
    return true; // Obstacle detected
  } else {
    return false; // No obstacle detected
  }
}

void avoidObstacle() {
  stopMotors();
  delay(500);

  digitalWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, HIGH);
  digitalWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, HIGH);
  delay(1000);

  digitalWrite(MOTOR1_PIN1, HIGH);
  digitalWrite(MOTOR1_PIN2, LOW);
  digitalWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, HIGH);
  delay(1000);
}

void adjustMotorSpeeds(double heading) {
  analogWrite(MOTOR1_PIN1, max_speed);
  analogWrite(MOTOR1_PIN2, LOW);
  analogWrite(MOTOR2_PIN1, max_speed);
  analogWrite(MOTOR2_PIN2, LOW);
}

void stopMotors() {
  digitalWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, LOW);
  digitalWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, LOW);
}
