//Basic Arduino IDE sketch required to connect to MQTT server
//using ESP32, ESP32 CAM or ESP8266 microcontroller
#include <WiFi.h>
#include <WiFiMulti.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

const int PINO_BUZZER = 23; // Pino D23 conectado ao buzzer

//wifi and mqttt setup
WiFiMulti WiFiMulti;
const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";
const char* mqtt_server = "IP_SERVIDOR_BROKER";

//esp setup
#define MSG_BUFFER_SIZE	(50)
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];


//max30100 setup
#define REPORTING_PERIOD_MS     1000
PulseOximeter pox;
uint32_t tsLastReport = 0;

unsigned long lastBuzzTime = 0;
bool isBuzzing = false;
unsigned long buzzDuration = 1000; // 1 segundo
bool isBuzzerOn = false;

void onBeatDetected()
{
    Serial.println("Beat!");
    isBuzzing = true;
    lastBuzzTime = millis();
    isBuzzerOn = true;
    digitalWrite(PINO_BUZZER, HIGH); // Ligar o buzzer
}

void handleBuzzer()
{
    if (isBuzzing)
    {
        unsigned long currentMillis = millis();
        if (isBuzzerOn && currentMillis - lastBuzzTime >= buzzDuration)
        {
            // Desligar o buzzer
            digitalWrite(PINO_BUZZER, LOW);
            isBuzzerOn = false;
            lastBuzzTime = currentMillis;
        }
        else if (!isBuzzerOn && currentMillis - lastBuzzTime >= buzzDuration)
        {
            // Finalizar o ciclo de buzzer
            isBuzzing = false;
        }
    }
}

void setup_wifi() {
 
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  randomSeed(micros());
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_max30100()
{
  Serial.print("Initializing pulse oximeter..");
  if (!pox.begin()) {
      Serial.println("FAILED");
      for(;;);
  } else {
      Serial.println("SUCCESS");
  }

  
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      Serial.print("MQTT client state: ");
      Serial.println(client.state());

      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
    Serial.begin(115200);
    setup_wifi();
    setup_max30100();

    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    pox.setOnBeatDetectedCallback(onBeatDetected);

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    pinMode(PINO_BUZZER, OUTPUT); // Define o PINO do buzzer como saída
}

void loop() {
  pox.update();
  handleBuzzer();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    pox.update();
    lastMsg = now;
    Serial.print("Heart rate:");
    Serial.print(pox.getHeartRate());
    Serial.print("bpm / SpO2:");
    Serial.print(pox.getSpO2());
    Serial.println("%");

    float heartRate = pox.getHeartRate();
    float spO2 = pox.getSpO2();

    // Verificar se as leituras são válidas antes de publicá-las
    if (heartRate > 0 && spO2 > 0) {
        snprintf(msg, MSG_BUFFER_SIZE, "HeartRate:%.1f,SpO2:%.1f", heartRate, spO2);

        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("update", msg);
    } else {
        Serial.println("Invalid readings, skipping publish.");
    }
  }
}

