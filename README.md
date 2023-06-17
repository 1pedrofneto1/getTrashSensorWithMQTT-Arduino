# getTrashSensorWithMQTT-Arduino 🚀🔥

Program for an ESP32 device that connects to a Wi-Fi network and an AWS IoT Core MQTT broker. 

It also includes libraries for working with the HX711 and HC-SR04 sensors and defines MQTT topics for sending data to and receiving commands from these sensors.

# Start 🚀🔥

You need to run init.bat file to use environment variables.

# Enviroment variables 🚀🔥

- WIFI_SSID (YOUR_SSID_HERE)
- WIFI_PASSWORD (YOUR_PASSWORD_HERE)
- MQTT_BROKER (YOUR_MQTT_BROKER_HERE)
- MQTT_PORT (YOUR_MQTT_PORT)
- MQTT_CLIENT_ID (YOUR_MQTT_CLIENT_ID_HERE)
- AWS_KEY (AWS_KEY_FILE_PATH)
- AWS_CERTIFICATE (AWS_CERTIFICATE_FILE_PATH)
- AWS_CA (AWS_CA_FILE_PATH)

# Dependencies 💻

The WiFi.h, PubSubClient.h and WiFiClientSecure.h libraries are already included in ESP32 by default. The HX711.h and Ultrasonic.h libraries can be installed through the Arduino IDE's Library Manager. 

To do this, open the Arduino IDE, go to Sketch > Include Library > Manage Libraries.... In the window that opens, search for the “HX711” and “Ultrasonic” libraries and click on “Install” for each one of them.