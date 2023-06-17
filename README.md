# getTrashSensorWithMQTT-Arduino 🚀🔥

Program for an ESP32 device that connects to a Wi-Fi network and an AWS IoT Core MQTT broker. 

It also includes libraries for working with the HX711 and HC-SR04 sensors and defines MQTT topics for sending data to and receiving commands from these sensors.

# Start 🚀🔥

You need to run init.bat file to use environment variables.

# Dependencies 💻

The WiFi.h, PubSubClient.h and WiFiClientSecure.h libraries are already included in ESP32 by default. The HX711.h and Ultrasonic.h libraries can be installed through the Arduino IDE's Library Manager. 

To do this, open the Arduino IDE, go to Sketch > Include Library > Manage Libraries.... In the window that opens, search for the “HX711” and “Ultrasonic” libraries and click on “Install” for each one of them.