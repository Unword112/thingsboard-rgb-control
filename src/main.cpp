#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "demo.thingsboard.io";
const char* ID = "";
const char* token = "";
const int port = 1883;

#define R 5
#define G 4
#define B 2

WiFiClient espClient;
PubSubClient client(espClient);

int red = 0;
int green = 0;
int blue = 0;

void setup_wifi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected!");
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(ID, token, "")) {
      client.subscribe("v1/devices/me/attributes"); // Subscribe
    } else {
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
      message += (char)payload[i];
  }
  Serial.println(message);

  int rIndex = message.indexOf("\"r\":");
  if (rIndex != -1) {
      red = message.substring(rIndex + 4, message.indexOf(",", rIndex) == -1 ? message.indexOf("}", rIndex) : message.indexOf(",", rIndex)).toInt();
      Serial.printf("red: %d\n", red);
  }

  int gIndex = message.indexOf("\"g\":");
  if (gIndex != -1) {
      green = message.substring(gIndex + 4, message.indexOf(",", gIndex) != -1 ? message.indexOf(",", gIndex) : message.indexOf("}", gIndex)).toInt();
      Serial.printf("green: %d\n", green);
  }

  int bIndex = message.indexOf("\"b\":");
  if (bIndex != -1) {
      blue = message.substring(bIndex + 4, message.indexOf("}", bIndex)).toInt();
      Serial.printf("blue: %d\n", blue);
  }

  Serial.printf("Updated RGB -> red: %d, green: %d, blue: %d\n", red, green, blue);
}

void setColor(int red, int green, int blue) {
  analogWrite(R, red);
  analogWrite(G, green);
  analogWrite(B, blue);
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);

  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  setColor(red, green, blue);
  delay(1000);
}
