#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "demo.thingsboard.io";
const char* token = ""; // ใส่ Token ที่คัดลอกมา
const char* client_id = "ESP32_RGB";

WiFiClient espClient;
PubSubClient client(espClient);

const int redPin = 5;
const int greenPin = 18;
const int bluePin = 19;

void sendRGBStatus(int r, int g, int b) {
    char payload[50];
    sprintf(payload, "{\"r\":%d,\"g\":%d,\"b\":%d}", r, g, b);
    client.publish("v1/devices/me/telemetry", payload);
    Serial.println("Sent: " + String(payload));
}

void callback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println("Received payload: " + message);

    int r, g, b;
    sscanf(message.c_str(), "{\"r\":%d,\"g\":%d,\"b\":%d}", &r, &g, &b);

    analogWrite(redPin, r);
    analogWrite(greenPin, g);
    analogWrite(bluePin, b);

    sendRGBStatus(r, g, b); // อัปเดตค่า RGB ไปที่ ThingsBoard
}

void reconnect() {
    while (!client.connected()) {
        if (client.connect(client_id, token, NULL)) {
            Serial.println("Connected to ThingsBoard");
            client.subscribe("v1/devices/me/rpc/request/+");
        } else {
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    reconnect();
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    // ทดสอบส่งค่าไปยัง ThingsBoard ทุกๆ 10 วินาที
    static unsigned long lastTime = 0;
    if (millis() - lastTime > 10000) {
        lastTime = millis();
        sendRGBStatus(255, 0, 0); // อัปเดตสีเป็นแดง
    }
}
