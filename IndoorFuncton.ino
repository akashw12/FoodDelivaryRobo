#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi credentials
const char* ssid = "Redmi Note 9 Pro";
const char* password = "12345679";

WebServer server(80);

// Pin Definitions
#define IN1  27  // Motor A
#define IN2  26
#define IN3  25  // Motor B
#define IN4  33
#define ENA  14  // PWM for Motor A
#define ENB  12  // PWM for Motor B
#define BUZZER_PIN 13 // Pin for the buzzer

void setup() {
  Serial.begin(115200);
  
  // Initialize motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT); // Set buzzer pin as output
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());  // Get IP Address

  // Web server routes
  server.on("/", handleRoot);
  server.on("/forward", []() { forward(255); server.send(200, "text/plain", "Moving Forward"); });
  server.on("/backward", []() { backward(255); server.send(200, "text/plain", "Moving Backward"); });
  server.on("/left", []() { turnLeft(255); server.send(200, "text/plain", "Turning Left"); });
  server.on("/right", []() { turnRight(255); server.send(200, "text/plain", "Turning Right"); });
  server.on("/stop", []() { stopCar(); server.send(200, "text/plain", "Stopped"); });
  
  // Horn route
  server.on("/horn", []() { soundHorn(); server.send(200, "text/plain", "Horn Activated"); });

  server.on("/setSpeed", []() {
    if (server.hasArg("speed")) {
      int speed = server.arg("speed").toInt();
      analogWrite(ENA, speed);
      analogWrite(ENB, speed);
      server.send(200, "text/plain", "Speed Set");
    } else {
      server.send(400, "text/plain", "Speed not provided");
    }
  });

  server.begin();
}

void loop() {
  server.handleClient();  // Handle client requests
}

// Functions to control the motors
void forward(int speed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward(int speed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft(int speed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(IN1, LOW);  // Motor A moves backward
  digitalWrite(IN2, HIGH); // Motor A backward
  digitalWrite(IN3, HIGH); // Motor B moves forward
  digitalWrite(IN4, LOW);  // Motor B forward
}

void turnRight(int speed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(IN1, HIGH); // Motor A moves forward
  digitalWrite(IN2, LOW);  // Motor A forward
  digitalWrite(IN3, LOW);  // Motor B moves backward
  digitalWrite(IN4, HIGH); // Motor B backward
}

void stopCar() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void soundHorn() {
  digitalWrite(BUZZER_PIN, HIGH); // Turn on the buzzer
  delay(1000); // Sound for 1 second
  digitalWrite(BUZZER_PIN, LOW); // Turn off the buzzer
}

void handleRoot() {
  String html = "<html><head>";
  html += "<style>";
  html += "body { display: flex; justify-content: center; align-items: center; height: 100vh; font-family: Arial, sans-serif; background-image: url('https://pngimg.com/uploads/robot/robot_PNG53.png'); background-size: cover; background-position: center; animation: fadeIn 3s ease-in-out; }";
  html += ".container { text-align: center; background-color: rgba(255, 255, 255, 0.8); border-radius: 10px; box-shadow: 0 0 20px rgba(0, 0, 0, 0.1); padding: 20px; }";
  html += "button { background-color: #007BFF; color: white; border: none; border-radius: 5px; padding: 15px 20px; margin: 10px; font-size: 16px; cursor: pointer; transition: background-color 0.3s; }";
  html += "button:hover { background-color: #0056b3; }";
  html += "input { padding: 10px; border: 1px solid #ccc; border-radius: 5px; width: 60px; text-align: center; }";
  
  // Keyframes for fade effect
  html += "@keyframes fadeIn { from { opacity: 0; } to { opacity: 1; } }";
  
  html += "</style>";
  html += "</head><body>";
  html += "<div class='container'>";
  html += "<h1>Baby Remote Control</h1>"; 
  html += "<button onclick=\"fetch('/forward')\">Forward</button><br>";
  html += "<button onclick=\"fetch('/backward')\">Backward</button><br>";
  html += "<button onclick=\"fetch('/left')\">Left</button><br>";
  html += "<button onclick=\"fetch('/right')\">Right</button><br>";
  html += "<button onclick=\"fetch('/stop')\">Stop</button><br>";
  html += "Set Speed: <input type='number' id='speed' min='0' max='255' value='255'> ";
  html += "<button onclick=\"setSpeed()\">Set Speed</button><br>";
  html += "<button onclick=\"fetch('/horn')\">Horn</button><br>"; // Add Horn button
  html += "<script>function setSpeed() { var speed = document.getElementById('speed').value; fetch('/setSpeed?speed=' + speed); }</script>";
  html += "</div>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

