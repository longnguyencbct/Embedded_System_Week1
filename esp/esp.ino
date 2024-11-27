#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Wifi name
#define WLAN_SSID "."
// Wifi password
#define WLAN_PASS "12345679"

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883

#define AIO_USERNAME "longnguyencbct"
#define AIO_KEY "aio_BCMY79YbcNSKmYJDzR8KxQI6iWhO"

// MQTT client setup
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// MQTT topic for publishing
Adafruit_MQTT_Publish light_pub(&mqtt, AIO_USERNAME "/feeds/led");

int led_counter = 0;
int led_status = HIGH ;

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

    // Connect to Adafruit MQTT
    while (mqtt.connect() != 0) {
        delay(500);
    }

    // Set busy pin LOW after setup
    digitalWrite(5, LOW);
}

void loop() {
    if (Serial.available()) {
        int msg = Serial.read();
        if (msg == 'o') {
            Serial.print('O');
        } else if (msg == 'a') {
            light_pub.publish(0);
        } else if (msg == 'A') {
            light_pub.publish(1);
        }
    }

    led_counter++;
    if (led_counter == 100) {
        // Every 1 second
        led_counter = 0;
        // Toggle LED
        if (led_status == HIGH) {
            led_status = LOW;
        } else {
            led_status = HIGH;
        }

        digitalWrite(2, led_status);
    }
    delay(10);
}
