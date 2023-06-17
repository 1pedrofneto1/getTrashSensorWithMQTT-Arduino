#include <WiFi.h>
#include <PubSubClient.h>
#include <HX711.h>
#include <WiFiClientSecure.h>
#include <Ultrasonic.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

const char* ssid = getenv("WIFI_SSID");
const char* password = getenv("WIFI_PASSWORD");

const char* mqtt_broker = getenv("MQTT_BROKER");  
const int mqtt_port = atoi(getenv("MQTT_PORT"));                         
const char* mqtt_client_id = getenv("MQTT_CLIENT_ID");   

const char* topic_hx711_data = "HX711/data";    
const char* topic_hcsr04_data = "HC-SR04/data";  

const char* topic_hx711_comm = "HX711/commands";     
const char* topic_hcsr04_comm = "HC-SR04/commands"; 

std::string readFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (file) {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return content;
    } else {
        std::cerr << "Erro ao abrir o arquivo: " << filePath << std::endl;
        exit(1);
    }
}

const std::string aws_ca = readFromFile(std::getenv("AWS_CA"));
const std::string aws_key = readFromFile(std::getenv("AWS_KEY"));
const std::string aws_cert = readFromFile(std::getenv("AWS_CERTIFICATE"));

#define DT_PIN 4
#define SCK_PIN 5

#define TRIGGER_PIN 13
#define ECHO_PIN 12

HX711 hx711;
Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);

bool hx711_sensor_active = false;  
bool sr04_sensor_active = false;  

float calibration_factor = 101820; 

WiFiClientSecure espClient; 
PubSubClient mqttClient(espClient); 

//
unsigned long previousMillis_hx711 = 0;
unsigned long previousMillis_hcsr04 = 0;
const long interval_hx711 = 5000;
const long interval_hcsr04 = 5000;

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Conectando ao broker MQTT...");
    if (mqttClient.connect(mqtt_client_id)) {
      Serial.println("Conectado ao broker MQTT!");
      mqttClient.subscribe(topic_hx711_data);
      mqttClient.subscribe(topic_hcsr04_data);
      mqttClient.subscribe(topic_hx711_comm);
      mqttClient.subscribe(topic_hcsr04_comm);
    } else {
      Serial.print("Falha na conexão - Estado: ");
      Serial.print(mqttClient.state());
      Serial.println(" - Tentando novamente em 5 segundos...");
      delay(4000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (strcmp(topic, "HX711/commands") == 0) {
    Serial.printf("tópico : %s | mensagem : %s\n", topic, message);
    if (message == "on") {
      hx711_sensor_active = true;
    } else if (message == "off") {
      hx711_sensor_active = false;
    }
  }

  if (strcmp(topic, "HC-SR04/commands") == 0) {
    Serial.printf("tópico : %s | mensagem : %s\n", topic, message);
    if (message == "on") {
      sr04_sensor_active = true;
    } else if (message == "off") {
      sr04_sensor_active = false;
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Conectado ao Wi-Fi!");

  mqttClient.setServer(mqtt_broker, mqtt_port);
  mqttClient.setCallback(mqttCallback);

  espClient.setCACert(aws_ca);
  espClient.setCertificate(aws_cert);
  espClient.setPrivateKey(aws_key);

  hx711.begin(DT_PIN, SCK_PIN);
  hx711.tare();                         
  hx711.set_scale(calibration_factor);  
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  } else {
    mqttClient.loop();

    if (hx711_sensor_active) {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis_hx711 >= interval_hx711) {
        previousMillis_hx711 = currentMillis;

        float peso = hx711.get_units();  
        Serial.printf("| Peso: %.2f Kg\n", peso);

        String payload_hx711 = String(peso);
        mqttClient.publish(topic_hx711_data, payload_hx711.c_str());
      }
    }

    if (sr04_sensor_active) {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis_hcsr04 >= interval_hcsr04) {
        previousMillis_hcsr04 = currentMillis;

        unsigned int nivel = ultrasonic.read();  
        Serial.printf("| Nível: %d Cm\n", nivel);

        String payload_hcsr04 = String(nivel);
        mqttClient.publish(topic_hcsr04_data, payload_hcsr04.c_str());
      }
    }
  }
}