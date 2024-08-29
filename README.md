# ESP8266-Based Extension Outlet Scheduler with Time Adjustments and Real-Time Monitoring via Local Web Server

## Project Overview

This project is a Power Management System that utilizes the ESP8266 microcontroller to control and monitor extension outlets in a residential setting. The system allows users to schedule on/off times for connected devices, view real-time power consumption data, and receive alerts when energy usage thresholds are exceeded. The user interface is accessed via a local web server, making it easy to manage connected appliances.

## Features

- **Automated Scheduling**: Schedule appliances to turn on/off at specific times to reduce manual intervention.
- **Real-Time Monitoring**: Monitor power consumption and energy usage of connected devices in real-time.
- **Alert System**: Receive alerts when devices exceed specified energy usage thresholds.
- **User-Friendly Web Interface**: Control and monitor the system via a local web server accessible on any device within the network.

## Materials Used

- **NodeMCU ESP8266**: For controlling and monitoring the power outlets.
- **SSR-40DA**: Solid State Relay for controlling the on/off state of appliances.
- **Universal Outlet**: Standard outlet socket used in the extension.
- **PZEM-004T V3 Module**: Module for measuring power consumption.
- **Switch**: SPST switch for manual control.
- **0.96 OLED**: Display for real-time feedback on system status.
- **Case 3D**: Custom 3D-printed case for housing the components.
- **IEC 320 Inlet AC Power Plug Socket**: Used for connecting the power supply to the system.
- **220V to 5V DC Converter**: For providing the necessary power to the ESP8266 and other components.

## Circuit Diagram

![Circuit Diagram](.Diagram/Circuit Diagram.jpg)

## Installation Guide for ESP8266 Core in Arduino IDE

To use the ESP8266 with Arduino IDE, you'll need to install the necessary core files. Follow these steps to set up your development environment.

### Step 1: Open Arduino IDE & Preferences

1. Make sure you have the **Arduino IDE** installed on your system. If not, download it from the [official Arduino website](https://www.arduino.cc/en/software).
2. Start the Arduino IDE.
3. Go to **File > Preferences** to open the Preferences window.
4. In the "Additional Board Manager URLs" field, paste the following URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json

### Step 2: Open Board Manager & Install Package

1. Go to **Tools > Board: > Boards Manager...**
2. In the Boards Manager window, scroll down to find the **ESP8266** platform.
3. Click on the drop-down, select the version you want, and then click the **Install** button.

### Step 3: Finalizing

1. After the installation is complete, restart the Arduino IDE.
2. Go to **Tools > Board:** and select **NodeMCU 1.0 (ESP-12E Module)** or another ESP8266 board that matches your hardware.
3. Your ESP8266 is now ready to be programmed using Arduino IDE!

For more detailed instructions or troubleshooting, you can refer to the full guide on [Instructables](https://www.instructables.com/Installing-ESP8266-and-ESP32-Core-in-Arduino-Windo/).

### Cloning the Repository

```bash
git clone https://github.com/John-Bernardo-Laboratory/ESP8266-Automated-Outlet-Scheduler.git
