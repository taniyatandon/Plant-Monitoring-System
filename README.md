# Plant Monitoring System (IoT)

## Overview
The Plant Monitoring System is an IoT-based solution designed to automate plant care by monitoring soil moisture, temperature, and humidity in real time. The system uses a NodeMCU (ESP8266) to collect sensor data and transmit it to the cloud using WiFi.

It automatically controls a water pump based on soil moisture levels and allows remote monitoring through the Blynk IoT platform.

---

## Features
- Real-time monitoring of temperature, humidity, and soil moisture  
- Automatic irrigation based on soil moisture threshold  
- Manual pump control using Blynk app  
- LCD display for local monitoring  
- Cloud-based data visualization  
- Alert system for extreme conditions  
- Offline mode support when WiFi is unavailable  

---

## Hardware Components
- NodeMCU ESP8266  
- DHT11 Sensor  
- Soil Moisture Sensor  
- Relay Module  
- 6V Water Pump  
- LCD (I2C)  
- Batteries and Power Module  

---

## Working Principle
1. Sensors collect environmental data  
2. NodeMCU processes the data  
3. Data is sent to the cloud using WiFi  
4. Blynk app displays real-time values  
5. If soil moisture is below threshold → pump turns ON  
6. If moisture is sufficient → pump turns OFF  

---

## Source Code
The complete Arduino code is available in the `code/` directory.

### Libraries Used
- ESP8266WiFi  
- Blynk  
- DHT Sensor Library  
- LiquidCrystal I2C  

---

## Project Demo
[Watch Demo Video](https://drive.google.com/file/d/1JlxVYKAlIpQMIJShHAkQYmtrWfak7TDN/view?usp=drive_link)

---

## Project Presentation
[View PPT](docs/plant_monitoring_system.ppt)

---

## Applications
- Smart agriculture  
- Home gardening  
- Greenhouse monitoring  
- Automation systems  

---

## Conclusion
The system successfully automates plant monitoring and irrigation using IoT. It reduces manual effort and ensures optimal plant health through real-time monitoring and control.

---

## Note
This project was developed as part of a team. The code and implementation were executed collaboratively.

---

## Author
Taniya Tandon
