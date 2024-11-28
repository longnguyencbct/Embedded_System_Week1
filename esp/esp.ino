#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// WiFi credentials
#define WLAN_SSID "."
#define WLAN_PASS "12345679"

// Adafruit MQTT server credentials
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "longnguyencbct"
#define AIO_KEY "aio_BCMY79YbcNSKmYJDzR8KxQI6iWhO"

// MQTT client setup
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// MQTT topic for publishing
Adafruit_MQTT_Publish light_pub(&mqtt, AIO_USERNAME "/feeds/led");

// MQTT topic for subscribing
Adafruit_MQTT_Subscribe light_sub(&mqtt, AIO_USERNAME "/feeds/led", MQTT_QOS_1);

int led_counter = 0;
int led_status = HIGH;

// Callback function for received MQTT messages
void lightCallback(char *data, uint16_t len) {
    if (data[0] == '0') {
        Serial.print('a');
    } else if (data[0] == '1') {
        Serial.print('A');
    }
}

void setup() {
    // Configure pins
    pinMode(2, OUTPUT);
    pinMode(5, OUTPUT);

    // Set busy pin HIGH
    digitalWrite(5, HIGH);

    Serial.begin(115200);

    // Connect to WiFi
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    // Subscribe to MQTT topic
    light_sub.setCallback(lightCallback);
    mqtt.subscribe(&light_sub);

    // Connect to Adafruit MQTT
    while (mqtt.connect() != 0) {
        mqtt.disconnect();
        delay(500);
    }

    // Set busy pin LOW after setup
    digitalWrite(5, LOW);
}

void loop() {
    // Process incoming MQTT messages
    mqtt.processPackets(10);

    // Read Serial input
    if (Serial.available()) {
        char msg = Serial.read();
        if (msg == 'o') {
            Serial.print('O');
        } else if (msg == 'a') {
            light_pub.publish(0);
        } else if (msg == 'A') {
            light_pub.publish(1);
        }
    }

    // LED toggle logic
    led_counter++;
    if (led_counter == 100) {
        led_counter = 0;
        // Toggle LED
        led_status = (led_status == HIGH) ? LOW : HIGH;
        digitalWrite(2, led_status);
    }
    delay(10);
}
