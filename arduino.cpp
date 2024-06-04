//#define ENABLE_DEBUG   // Uncomment this line to enable serial debugging.

#ifdef ENABLE_DEBUG
  #define DEBUG_ESP_PORT Serial
  #define NODEBUG_WEBSOCKETS
  #define NDEBUG
#endif 

#include <Arduino.h>
#include <WiFi.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"
#include <map>

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

#define wifiLed           2   // D2
#define BAUD_RATE         9600   
#define DEBOUNCE_TIME     250

typedef struct {      
  int relayPIN;
  int flipSwitchPIN;
} deviceConfig_t;

std::map<String, deviceConfig_t> devices = {
  // {deviceId, {relayPIN, flipSwitchPIN}}
  {device_ID_1, {RelayPin1, SwitchPin1}},
  {device_ID_2, {RelayPin2, SwitchPin2}},
  {device_ID_3, {RelayPin3, SwitchPin3}},
  {device_ID_4, {RelayPin4, SwitchPin4}}
};

typedef struct {      
  String deviceId;
  bool lastFlipSwitchState;
  unsigned long lastFlipSwitchChange;
} flipSwitchConfig_t;

std::map<int, flipSwitchConfig_t> flipSwitches;    

void setupRelays() { 
  for (auto &device : devices) {           
    int relayPIN = device.second.relayPIN; 
    pinMode(relayPIN, OUTPUT);             
    digitalWrite(relayPIN, HIGH);
  }
}

void setupFlipSwitches() {
  for (auto &device : devices) {                   
    flipSwitchConfig_t flipSwitchConfig;              
    flipSwitchConfig.deviceId = device.first;        
    flipSwitchConfig.lastFlipSwitchChange = 0;        
    flipSwitchConfig.lastFlipSwitchState = true;

    int flipSwitchPIN = device.second.flipSwitchPIN; 
    flipSwitches[flipSwitchPIN] = flipSwitchConfig; 
    pinMode(flipSwitchPIN, INPUT_PULLUP); 
  }
}

bool onPowerState(String deviceId, bool &state) {
  Serial.printf("%s: %s\r\n", deviceId.c_str(), state ? "on" : "off");
  int relayPIN = devices[deviceId].relayPIN;
  digitalWrite(relayPIN, !state);
  return true;
}

void handleFlipSwitches() {
  unsigned long actualMillis = millis();
  for (auto &flipSwitch : flipSwitches) {
    unsigned long lastFlipSwitchChange = flipSwitch.second.lastFlipSwitchChange;

    if (actualMillis - lastFlipSwitchChange > DEBOUNCE_TIME) {
      int flipSwitchPIN = flipSwitch.first;
      bool lastFlipSwitchState = flipSwitch.second.lastFlipSwitchState;
      bool flipSwitchState = digitalRead(flipSwitchPIN);
      if (flipSwitchState != lastFlipSwitchState) {
#ifdef TACTILE_BUTTON
        if (flipSwitchState) {
#endif      
          flipSwitch.second.lastFlipSwitchChange = actualMillis;
          String deviceId = flipSwitch.second.deviceId;
          int relayPIN = devices[deviceId].relayPIN;
          bool newRelayState = !digitalRead(relayPIN);
          digitalWrite(relayPIN, newRelayState);

          SinricProSwitch &mySwitch = SinricPro[deviceId];
          mySwitch.sendPowerStateEvent(!newRelayState);
#ifdef TACTILE_BUTTON
        }
#endif      
        flipSwitch.second.lastFlipSwitchState = flipSwitchState;
      }
    }
  }
}

void setupWiFi() {
  Serial.printf("\r\n[WiFi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  digitalWrite(wifiLed, HIGH);
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro() {
  for (auto &device : devices) {
    const char *deviceId = device.first.c_str();
    SinricProSwitch &mySwitch = SinricPro[deviceId];
    mySwitch.onPowerState(onPowerState);
  }

  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, LOW);

  setupRelays();
  setupFlipSwitches();
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
  handleFlipSwitches();
}
