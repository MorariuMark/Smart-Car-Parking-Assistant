# Smart Car Parking Assistant

## Overview
This is a smart car parking assistant I built using an ESP32. It uses an HC-SR04 ultrasonic sensor to measure the distance of objects in the rear, and an FC-51 IR sensor to detect obstacles in the front. 

The goal was to create a system that gives escalating audio and visual feedback as the vehicle gets closer to a hazard. I also added a Wi-Fi web server to the ESP32, which allows you to monitor the live sensor data and distances directly from a phone or tablet without needing to keep the board plugged into a computer.

## Key Features
* **Live Web Dashboard:** The ESP32 hosts a webpage over Wi-Fi to view real-time sensor states.
* **Escalating Alarms:** The hardware alerts change based on proximity thresholds:
  * **> 15cm (Clear):** Green LED is ON.
  * **11cm - 15cm (Notice):** Slow, synchronized buzzer beep and Red LED flash.
  * **6cm - 10cm (Warning):** Fast, synchronized buzzer beep and Red LED flash.
  * **<= 5cm (Critical):** Constant Red LED ON and continuous buzzer tone.
* **Logic Level Protection:** Includes a 1kΩ/2kΩ voltage divider. The HC-SR04 operates at 5V, so this steps the Echo signal down to a safe ~3.33V for the ESP32 input pin.

## Hardware Used
* ESP32 Microcontroller 
* HC-SR04 Ultrasonic Sensor (Rear)
* IR Obstacle Sensor (Front)
* Active/Passive Buzzer
* LEDs (1 Red, 1 Green)
* Resistors: 1x 1kΩ, 1x 2kΩ (for the voltage divider), 2x 220Ω (for LEDs)
* Breadboard and jumper wires

## Wiring & Pinout

| Component | ESP32 Pin | Notes |
| :--- | :--- | :--- |
| IR Sensor (OUT) | GPIO 4 | Powered by 3.3V |
| HC-SR04 (TRIG) | GPIO 5 | Powered by 5V (Vin) |
| HC-SR04 (ECHO) | GPIO 18 | **Requires Voltage Divider (1kΩ to Pin 18, 2kΩ to GND)** |
| Buzzer (+) | GPIO 13 | Connect other leg to GND |
| Red LED (+) | GPIO 19 | Use 220Ω resistor |
| Green LED (+) | GPIO 21 | Use 220Ω resistor |

**Important:** Do not connect the HC-SR04 ECHO pin directly to the ESP32 without the voltage divider, or you risk frying the GPIO pin with 5V.

## Installation 
1. Clone this repository: `git clone https://github.com/yourusername/Smart-Car-Parking-Assistant.git`
2. Open the project in the Arduino IDE or PlatformIO.
3. Update the Wi-Fi credentials (`ssid` and `password`) in the code to match your network.
4. Upload the code to your ESP32.
5. Open the Serial Monitor at 115200 baud to find the assigned IP address.
6. Type that IP address into your browser to view the dashboard.

## License
Released under the [MIT License](LICENSE).
