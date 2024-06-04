# ESP32 Smart Switch Control with SinricPro

This project provides a way to control switches using an ESP32 and the SinricPro platform. You can manage multiple switches, get real-time status updates, and control your devices remotely using the SinricPro service.

## Features

- Control up to 4 relays using an ESP32.
- Monitor switch states and toggle relays via physical switches.
- Real-time remote control using SinricPro.
- Debouncing to ensure accurate switch state changes.
- Debugging support for easier development and troubleshooting.

## Hardware Requirements

- ESP32 Development Board
- Relays (up to 4)
- Switches (up to 4)
- WiFi Network

## Software Requirements

- Arduino IDE with ESP32 board support
- SinricPro library
- WiFi library
- Map library

## Project Directory Structure

.
├── src/
│   ├── main.cpp
│   └── platformio.ini
└── README.md





## Setup Instructions

### Hardware Setup

1. Connect your relays to the specified GPIO pins on the ESP32.
2. Connect your physical switches to the specified GPIO pins on the ESP32.

### Software Setup

1. Install the Arduino IDE.
2. Install the necessary libraries:
    - SinricPro
    - WiFi
    - Map

3. Clone this repository:

4. Open the arduino.cpp file in Arduino IDE and configure the following:


#define WIFI_SSID         "Enter YOUR-WIFI-NAME"
#define WIFI_PASS         "Enter YOUR-WIFI-PASSWORD"
#define APP_KEY           "Enter YOUR-APP-KEY"
#define APP_SECRET        "Enter YOUR-APP-SECRET"

#define device_ID_1       "SWITCH_ID_NO_1_HERE"
#define device_ID_2       "SWITCH_ID_NO_2_HERE"
#define device_ID_3       "SWITCH_ID_NO_3_HERE"
#define device_ID_4       "SWITCH_ID_NO_4_HERE"

#define RelayPin1         23  // D23
#define RelayPin2         22  // D22
#define RelayPin3         21  // D21              
#define RelayPin4         19  // D19

#define SwitchPin1        13  // D13
#define SwitchPin2        12  // D12
#define SwitchPin3        14  // D14
#define SwitchPin4        27  // D27


Replace the placeholders with your WiFi credentials, SinricPro keys, and device IDs.

5. Upload the code to your ESP32.
  
###Usage
1. Power on the ESP32.
2. The ESP32 will connect to the specified WiFi network.
3. The device will register with the SinricPro service.
4. Control the relays through the SinricPro app or physical switches.

   
###Debugging
To enable serial debugging, uncomment the following line in the code:
//#define ENABLE_DEBUG
This will allow you to see debug messages on the serial monitor.

Contributions
Contributions are welcome! Please open an issue or submit a pull request for any improvements or bug fixes.

