# Temperature & Pressure Monitoring System Using Arduino UNO Q
A compact embedded system designed to read, process, and display real-time environmental data. This project measures ambient temperature and atmospheric pressure, processing the signals to provide accurate, stable readings on a local display or serial monitor.

Features

*Real-Time Tracking: Continuous monitoring of ambient temperature and atmospheric pressure.



*Modular Code: Clear separation between sensor abstraction, data processing, and display logic.

Hardware:


1.Arduino UNO Q


2.BMP388 sensor


[Microcontroller]                    [Sensor Module]
     
      3.3V       ------------>       VCC  
       GND       ------------>       GND
       SDA       ------------>       SDA
       SCL       ------------>       SCL


Prerequisites
1.Arduino App Lab
2.WebUI - HTML(Available in Bricks)
3.Adafruit BMP3XX Library(Available in Sketch Libraries)

Installation & Flashing

1.Clone the Repository:

Step 1.Clone this repository to your local machine using git:

Bash

git clone https://github.com/devojeet/tempandpressuremonitoring.git

2.Install Dependencies:

Step 2.Search for and install the required sensor and display libraries listed in the Prerequisites

4.Compile and Upload:

Step 4.Connect your microcontroller via USB, then click Upload.




