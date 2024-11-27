#include <ESP8266WiFi.h>

// Wifi name
#define WLAN_SSID "."
// Wifi password
#define WLAN_PASS "12345679"

int led_counter = 0;
int led_status = HIGH ;

void setup() {
    // Set pin 2 and 5 as OUTPUT
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
    
    // Finish setup, set busy pin LOW
    digitalWrite(5, LOW);
}

void loop() {
    if (Serial.available()) {
        int msg = Serial.read();
        if (msg == 'o') Serial.print('O');
    }
    
    led_counter++;
    if (led_counter == 100) {
        led_counter = 0;
        // Toggle LED
        if (led_status == HIGH)
            led_status = LOW;
        else
            led_status = HIGH;
            
        digitalWrite(2, led_status);
    }
    delay(10);
}
