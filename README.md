# Wireless Communication System Using ESP32 and ESP-NOW Protocol

## Overview
This project implements a wireless communication system using **ESP32 microcontrollers** and the **ESP-NOW protocol**. The system consists of **one Master ESP32** and **three Slave ESP32 devices**, enabling seamless communication between the nodes for controlling LEDs, sending button states, and managing a call queue.

The system is designed for low-latency communication and does not require Wi-Fi or internet connectivity, making it ideal for local applications.

![image alt](https://github.com/atorab025/Wireless-Communication-ES_NOW/blob/9922be9e9907eef7e0eca7f7d23da9811e65320a/images/1.PNG)

---

## Features
- **ESP-NOW Protocol**: Low-power, low-latency communication without Wi-Fi dependency.
- **Multi-Device Communication**: Supports communication between one Master and three Slaves.
- **LED Control**: Slaves control LEDs based on commands sent by the Master or internal button states.
- **Queue Management**: The Master maintains and displays a queue of active calls from Slaves on an I2C LCD screen.
- **Buzzer Alerts**: Audible feedback on Master for new incoming requests.
- **Queue Status LED**: Visual indication of active queue status using an onboard LED.
- **Button Control**: Each Slave has dedicated buttons for sending commands to the Master.

---

## Components Used
1. **ESP32 Microcontrollers**: 4 units (1 Master and 3 Slaves)
2. **LEDs**: For visual feedback on Slave and Master devices.
3. **Buttons**: To trigger data transmissions.
4. **Buzzer**: To alert the Master of incoming requests.
5. **I2C LCD (16x2)**: Displays the current queue status on the Master.
6. **Miscellaneous**: Resistors, wires, breadboards.

---

## Hardware Setup
### Master
- **LEDs**:
  - LED1 (Pin 15)
  - LED2 (Pin 2)
  - LED3 (Pin 4)
- **Buttons**:
  - BTN1 (Pin 5)
  - BTN2 (Pin 18)
  - BTN3 (Pin 23)
- **Buzzer**: Pin 33
- **Queue LED**: Pin 5
- **I2C LCD**: Connected to the appropriate SDA and SCL pins.

### Slaves
- **LED**: Pin 4
- **Buttons**:
  - Primary Button (Pin 2)
  - Slave Button 1 (Pin 15)
  
### Connections
1. Connect the LEDs and buttons as per the GPIO pins defined in the code.
2. Ensure the I2C LCD is properly wired to the Master with its SDA and SCL pins connected.
3. Provide 5V or 3.3V power to the ESP32 devices.

---

## Software Details
### Libraries Used
1. **ESP-NOW**: For wireless communication.
2. **WiFi**: Required for ESP-NOW initialization.
3. **LiquidCrystal_I2C**: For LCD display control.

### Code Structure
#### Master
- Initializes ESP-NOW and sets up peers for communication with Slaves.
- Manages a queue of incoming requests from Slaves.
- Updates the I2C LCD to display the current queue status.
- Controls LEDs and buzzer based on incoming messages.

#### Slaves
- Initializes ESP-NOW and registers the Master as a peer.
- Sends button states and LED control commands to the Master.
- Controls onboard LED based on received commands from the Master.

### Protocol
1. **Data Transmission**: 
   - Uses a structured message (`struct_message`) containing:
     - `led`: LED state (ON/OFF)
     - `slaveId`: Identifier for the Slave device
2. **Queue Management**:
   - The Master maintains a queue of active Slaves sending requests.
   - Updates the queue on the LCD and activates the Queue LED.

---

## Installation
1. Install the **Arduino IDE** and set up the ESP32 core for Arduino:
   - [ESP32 Board Setup](https://github.com/espressif/arduino-esp32)
2. Install the required libraries:
   - `ESP32 WiFi`
   - `ESP-NOW`
   - `LiquidCrystal_I2C`
3. Upload the respective code to the Master and Slave devices:
   - Use the provided `Master` and `Slave` code files.

---

## Usage
1. Power up the ESP32 devices.
2. Press a button on any Slave to send a request to the Master.
3. The Master:
   - Updates the queue on the LCD.
   - Activates the respective Slave's LED.
   - Sounds a buzzer for new requests.
4. Queue management and LED control can be observed in real-time.

---

## Future Improvements
- Add encryption to the ESP-NOW communication for enhanced security.
- Increase the number of Slaves by extending the peer list on the Master.
- Integrate a web interface for remote queue monitoring.
- Implement power-saving features for Slaves using deep sleep.

---
