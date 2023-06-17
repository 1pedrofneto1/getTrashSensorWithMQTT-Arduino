#include <WiFi.h>
#include <PubSubClient.h>
#include <HX711.h>
#include <WiFiClientSecure.h>
#include <Ultrasonic.h>

const char* ssid = getenv("WIFI_SSID");
const char* password = getenv("WIFI_PASSWORD");

const char* mqtt_broker = getenv("MQTT_BROKER");  
const int mqtt_port = atoi(getenv("MQTT_PORT"));                         
const char* mqtt_client_id = getenv("MQTT_CLIENT_ID");   

const char* topic_hx711_data = "HX711/data";    
const char* topic_hcsr04_data = "HC-SR04/data";  

const char* topic_hx711_comm = "HX711/commands";     
const char* topic_hcsr04_comm = "HC-SR04/commands"; 

static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA508OiVQHEi9Hz8rUJ20IVlBZjKWTZx+7/P1lXrFQcHtmcsQM
qKjTRsHuY2WcbE1vRU+QQj+h+oe/9fSjuHFZbgUHdaOeM5bePjhGenRso2vZppO/
GrqTsq/njWVCUloCuJ5JLNCv1l5xxPUZ+BUjQ2N9rEICK14Uqgl1ybEzFJB8uSEq
EyQM4CFhFufu9JgHEQAud9L8nEQuNUa9TP8OaBA8ArZo883QSqXm+aYLNJZ2t/Kr
b9nn29FUhl5gyyxrdqnoPldO3wEmQvpOl9dlJ0v1Uv2bMFQCDoyWPj6bKw2T/5Bb
I+FHhkK8g7QgL5P81VbFeWc0cFX4HbkTMqOBvwIDAQABAoIBADRVoshXhrIKmIjf
L1ksRjKhQ2GbJNq0R2haPNFL9RM+GFPnH6YZbJQZ1MIQf2/RpVddRRp4I0H37OrN
Sy20F7PY13M6bdTmUapqOSgDp4DiKgCSdS6rADfxrMZFHthbHTqANbY6EU7DV6Jk
glerEKtym0CaPrtpMeu28DuOKCyChkFwjICQegGeLZv2paDZOeD1hFGrbOMr6LL1
HGiTmmGUnkvamPForENJT5Z9qnMUI8iZ1YFRRI+E4g+U41qR65mO85f3bnqjoKRY
pBVuQgqoXtPsHJTC4VWMZTLQFVUSIqIisVHmdOYy0LhA5dr4sC62CEUSAI8LEx/g
pyDOdkECgYEA/Us5jNkVykEfxfL/bs7qXuZa4y4YaB3eV5+gtAUlXMoG42o4NtCv
j0kqPgulo1z+rpkx2WwRnYBpJe1g8Fm7MppU9QTWSHm9CfUDYDqIn9B4MSOgWGjT
UvXAc7DIAyj8wphqEdzSs9JFvpgP9okOBTuf6OAv6LC8BgJTlYwl+8cCgYEA6cez
lF45uQU1sILtpwktTI6nIvjL8Ev4qK4iE4qPiv5AmFQcDMbCTuMjoTLU0D7tx3xe
YCbio23SFy3CnqcUKa1CSM6MlagCYRZZrWnnrPIZJQACKfjjiiSlRtJuPDWXscT6
KtAA/j/EhUqRnlyWNebQgAGpj0UHXbfjDnDfmkkCgYEAvVtZYMSBl7xr5m4tK0eg
yuVnGONC98Msj1EUsMAQGHgoAidMcx47wOCvpcbLQ3+oHhsEu0ghOx/ee64khEpw
ufnKfYnqMDBidGko5u2bae5NjF7X5oAnS6PRBiwousQPgeKzmTr+gdQ2s6+2b1Yy
lxLmDxBagJhvWAaxOeGWG80CgYEA4IqKZrBWGurxFZe78drzonvF9uYm46fkrweJ
/WDx3Fu26VioFHF3wlOSLktncpluw1MmRo4y0ywxRUgTjVUHHbynVMzm5PuZRW01
N0XoWwmxswMeEPhYfKAUC0Ek1jnXeMysSCC/Q3U2KZxk30kokZ8flkOCe/SeTZrI
wF8TtdkCgYEAibKg9tAiQqHPDz7Z3Wan78+hJ5LlkQw8EJHjPTxu7UAAmLbz8nWy
UdeYvrHRiaZMVVfGJ4cBJnnz6bzgtet3Rw2qxCRI4XIfzWEI6ue0gPfx86RK0s8v
EOpvsZ11ZTkHjMDjMyQudti0JkOVUpbzu/D7ptJdLZrYGjID2UDP6a0=
-----END RSA PRIVATE KEY-----
)KEY";

static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAINZcH0OswfBkMirXk13WtgNRlQMMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMzA2MTIxNDM5
MThaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDnTw6JVAcSL0fPytQn
bQhWUFmMpZNnH7v8/WVesVBwe2ZyxAyoqNNGwe5jZZxsTW9FT5BCP6H6h7/19KO4
cVluBQd1o54zlt4+OEZ6dGyja9mmk78aupOyr+eNZUJSWgK4nkks0K/WXnHE9Rn4
FSNDY32sQgIrXhSqCXXJsTMUkHy5ISoTJAzgIWEW5+70mAcRAC530vycRC41Rr1M
/w5oEDwCtmjzzdBKpeb5pgs0lna38qtv2efb0VSGXmDLLGt2qeg+V07fASZC+k6X
12UnS/VS/ZswVAIOjJY+PpsrDZP/kFsj4UeGQryDtCAvk/zVVsV5ZzRwVfgduRMy
o4G/AgMBAAGjYDBeMB8GA1UdIwQYMBaAFOYaj4YIRN5qH48tXtEsYymXRbyjMB0G
A1UdDgQWBBSyw/8IF1mkDEGtPohzU2tG03+ntTAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEACSaDmMqbe6mepX7Opw1gC3pU
2SV92kgtf5+4Isumgpv8uJHssvZwuDmaW8ymxywczUK8brGpkeWi3l9iCQvyvZw6
x/z0qQr/GQikgLT4u6s6RU1EsIzV3nc2X9RJ9WcDJmggBWJhwvicVW7+WCQbQIKF
KCX/56SmEGEi78fJK5t0IKtqnqCkRN0uqfjBp0ptcpPXd7hW76Mpdht8jdp79iKy
MF4VqjQ1SHYqB7xF2tc1ZDgQRiGnSWPqJC9qS08xEtAt8xwXAqdzdOu9HZ1Sh67T
3X8frNhxTN0LhVgvoK1CZ/aNdXK+pT+3JhvkFJrSR4K6S1vwY32nlNGI15sPzw==
-----END CERTIFICATE-----
)KEY";

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

  espClient.setCACert(AWS_CERT_CA);
  espClient.setCertificate(AWS_CERT_CRT);
  espClient.setPrivateKey(AWS_CERT_PRIVATE);

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