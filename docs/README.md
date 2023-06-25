---
layout: home
permalink: index.html

# Please update this with your repository name and title
repository-name: e18-CO326-Smart-Energy-Management-System
title: Group 15 - Smart Energy Management System
---

[comment]: # "This is the standard layout for the project, but you can clean this and use your own template"

# Group 15 - Smart Energy Management System

---

<!-- 
This is a sample image, to show how to add images to your page. To learn more options, please refer [this](https://projects.ce.pdn.ac.lk/docs/faq/how-to-add-an-image/)

![Sample Image](./images/sample.png)
 -->

## Team
-  E/18/180 - M. K. N. M. Kodituwakku [email](mailto:e18180@eng.pdn.ac.lk)
-  E/18/227 - D.H. Mudalige [email](mailto:e18227@eng.pdn.ac.lk)
-  E/18/266 - K. N. I. Premathilaka [email](mailto:e18266@eng.pdn.ac.lk)

## Table of Contents
1. [About our project](#about-our-project)
2. [Implementation](#implementation)
3. [References](#references)
4. [Links](#links)

---

## About our project

Our smart power management system is an intelligent system that allows users to monitor and control their energy consumption in real-time. The system uses ESP8266/ESP32-based controllers to collect data from energy meters and other
appliances, and then uses that data to optimize power usage patterns.

## Implementation

As reference for our system we have referred to an energy meter system which is created using


ESP32. But in that system there is no comparison between the power consumption of multiple devices and device controlling. So in our system we hope to do that improvement using OPC UA.


Here are some further details about that system:
- The Electricity Energy Meter project aims to create an energy monitoring system that measures voltage, current, power, and total energy consumption using the ESP32 microcontroller and
Blynk Application.

- To measure the current, we will use the SCT-013 non-invasive AC current sensor split core type clamp meter sensor, which can measure AC current up to 100 amperes. For measuring voltage, we will use the AC Voltage Sensor Module ZMPT101B, which can accurately measure AC voltage with a voltage transformer.

- The ESP32 microcontroller will be programmed to read the voltage and current sensor data and calculate the power consumption. The total energy consumption will be calculated by integrating the power over time.


Overall, the Electricity Energy Meter project provides a cost-effective and user-friendly way of monitoring energy usage in homes, offices, and other settings. It can be easily implemented using the ESP32 microcontroller, SCT-013 and ZMPT101B sensors, and the Blynk Application.

## References
IoT Based Smart Electricity Energy Meter using ESP32 - https://www.youtube.com/watch?v=FVGvR9qlEc8

ESP32 AC Power Meter - https://youtu.be/PSzkaSy5lHY


## Links

- [Project Repository](https://github.com/cepdnaclk/{{ page.repository-name }}){:target="_blank"}
- [Project Page](https://cepdnaclk.github.io/{{ page.repository-name}}){:target="_blank"}
- [Department of Computer Engineering](http://www.ce.pdn.ac.lk/)
- [University of Peradeniya](https://eng.pdn.ac.lk/)


[//]: # (Please refer this to learn more about Markdown syntax)
[//]: # (https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet)
