# Autonomous Delivery Robot . 
An IoT-based robot designed for autonomous delivery in both outdoor and indoor environments. The robot leverages GPS for outdoor navigation and a virtual remote over Wi-Fi for indoor control. It features an OTP-secured lock system powered by a servo motor.

## Demo Video
[Watch the Demo](https://drive.google.com/drive/folders/1_cMjT9L0ji3Vs7SeWIW5NX41lQoKs-yc)

# Features
Outdoor Navigation: Uses the NEO-6M GPS module for autonomous navigation.
Indoor Control: Operates via a virtual remote on a Wi-Fi network.
Secure Delivery: OTP-based locking mechanism using a servo motor.
Drive System: Powered by DC motors and an L298N motor driver.

# Components Used
ESP32: Microcontroller for processing and communication.
NEO-6M GPS Module: For real-time location tracking.
Servo Motor: For OTP-based lock operation.
DC Motors with L298N Driver: For movement control.
Power Supply: Battery pack for powering components.

# How It Works
Outdoor Functionality
The robot receives GPS coordinates via the NEO-6M module.
Navigation is autonomously handled based on pre-set waypoints.
Indoor Functionality
Switches to Wi-Fi-based control.
Controlled using a web-based or mobile virtual remote interface.
OTP Lock System
OTP is generated and sent to the recipient.
The servo motor unlocks the compartment when the correct OTP is entered.

# Future Enhancements
Add obstacle detection using ultrasonic sensors.
Enable integration with delivery apps.
Improve GPS accuracy for challenging environments.
