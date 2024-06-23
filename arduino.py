python
# Import necessary libraries
import time
import machine
import network
from sinric import SinricPro, SinricProSwitch

# WiFi credentials
WIFI_SSID = "Enter YOUR-WIFI-NAME"
WIFI_PASS = "Enter YOUR-WIFI-PASSWORD"

# Sinric Pro credentials
APP_KEY = "Enter YOUR-APP-KEY"
APP_SECRET = "Enter YOUR-APP-SECRET"

# Device IDs for Sinric Pro
device_ID_1 = "SWITCH_ID_NO_1_HERE"
device_ID_2 = "SWITCH_ID_NO_2_HERE"
device_ID_3 = "SWITCH_ID_NO_3_HERE"
device_ID_4 = "SWITCH_ID_NO_4_HERE"

# GPIO pins for relays
RelayPin1 = 23
RelayPin2 = 22
RelayPin3 = 21
RelayPin4 = 19

# GPIO pins for switches
SwitchPin1 = 13
SwitchPin2 = 12
SwitchPin3 = 14
SwitchPin4 = 27

# GPIO pin for WiFi LED
wifiLed = 2

# UART baud rate
BAUD_RATE = 9600

# Debounce time for switches (in milliseconds)
DEBOUNCE_TIME = 250

# Dictionary to store device configurations
devices = {
    device_ID_1: {'relayPIN': RelayPin1, 'flipSwitchPIN': SwitchPin1},
    device_ID_2: {'relayPIN': RelayPin2, 'flipSwitchPIN': SwitchPin2},
    device_ID_3: {'relayPIN': RelayPin3, 'flipSwitchPIN': SwitchPin3},
    device_ID_4: {'relayPIN': RelayPin4, 'flipSwitchPIN': SwitchPin4}
}

# Dictionary to store switch configurations
flipSwitches = {}

# Function to set up relays
def setupRelays():
    for device in devices.values():
        relayPIN = device['relayPIN']
        machine.Pin(relayPIN, machine.Pin.OUT).value(1)

# Function to set up switches
def setupFlipSwitches():
    for deviceId, device in devices.items():
        flipSwitchConfig = {
            'deviceId': deviceId,
            'lastFlipSwitchChange': 0,
            'lastFlipSwitchState': True
        }
        flipSwitchPIN = device['flipSwitchPIN']
        flipSwitches[flipSwitchPIN] = flipSwitchConfig
        machine.Pin(flipSwitchPIN, machine.Pin.IN, machine.Pin.PULL_UP)

# Function to handle power state changes
def onPowerState(deviceId, state):
    print(f"{deviceId}: {'on' if state else 'off'}")
    relayPIN = devices[deviceId]['relayPIN']
    machine.Pin(relayPIN, machine.Pin.OUT).value(not state)
    return True

# Function to handle switch state changes
def handleFlipSwitches():
    actualMillis = time.ticks_ms()
    for flipSwitchPIN, flipSwitch in flipSwitches.items():
        lastFlipSwitchChange = flipSwitch['lastFlipSwitchChange']
        if time.ticks_diff(actualMillis, lastFlipSwitchChange) > DEBOUNCE_TIME:
            lastFlipSwitchState = flipSwitch['lastFlipSwitchState']
            flipSwitchState = machine.Pin(flipSwitchPIN, machine.Pin.IN).value()
            if flipSwitchState != lastFlipSwitchState:
                flipSwitch['lastFlipSwitchChange'] = actualMillis
                deviceId = flipSwitch['deviceId']
                relayPIN = devices[deviceId]['relayPIN']
                newRelayState = not machine.Pin(relayPIN, machine.Pin.OUT).value()
                machine.Pin(relayPIN, machine.Pin.OUT).value(newRelayState)
                mySwitch = SinricPro[deviceId]
                mySwitch.sendPowerStateEvent(not newRelayState)
                flipSwitch['lastFlipSwitchState'] = flipSwitchState

# Function to set up WiFi connection
def setupWiFi():
    print("\r\n[WiFi]: Connecting")
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(WIFI_SSID, WIFI_PASS)
    while not wlan.isconnected():
        print(".", end="")
        time.sleep(0.25)
    machine.Pin(wifiLed, machine.Pin.OUT).value(1)
    print(f"connected!\r\n[WiFi]: IP-Address is {wlan.ifconfig()[0]}")

# Function to set up Sinric Pro
def setupSinricPro():
    for deviceId in devices.keys():
        mySwitch = SinricProSwitch(deviceId)
        mySwitch.onPowerState(onPowerState)
    SinricPro.begin(APP_KEY, APP_SECRET)
    SinricPro.restoreDeviceStates(True)

# Setup function
def setup():
    machine.UART(0, BAUD_RATE)
    machine.Pin(wifiLed, machine.Pin.OUT).value(0)
    setupRelays()
    setupFlipSwitches()
    setupWiFi()
    setupSinricPro()

# Main loop
def loop():
    SinricPro.handle()
    handleFlipSwitches()

# Run the setup and loop functions
setup()
while True:
    loop()

