#include <Arduino.h>
#include <stdio.h>
#include <WiFi.h>
#include <Wire.h>
#include "never.h"
#include <ArduinoOTA.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include <Preferences.h>

// Hardware settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

IPAddress local_IP(10, 204, 26, 250);
IPAddress gateway(10, 204, 26, 1);
IPAddress subnet(255, 255, 255, 0);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Preferences prefs;

bool oledReady = false;
int counter = 0;

void bootGuard() {
     
      prefs.begin("NVS", false);
      int counter = prefs.getInt("counter", 0);

      counter++;
      prefs.putInt("counter", counter);

      Serial.printf("Current counter: %d\n", counter);
      display.printf("Current counter: %d\n", counter);

      if (counter > 3) {

          Serial.println("Boot guard: crash loop detected, rolling back");
          prefs.putInt("counter", 0);

          const esp_partition_t *rollbackTarget = esp_ota_get_next_update_partition(NULL);
          esp_ota_set_boot_partition(rollbackTarget);
          ESP.restart();
      }
}

void setup() {
    Serial.begin(115200);
    bootGuard();
    Wire.begin();

    //WiFi.config(local_IP, gateway, subnet);

    // 1. Eerst WiFi verbinden
    WiFi.begin(AP_SSID, AP_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP());

    ArduinoOTA.setHostname("esp32-device"); 
    ArduinoOTA.begin();

    Serial.println("Hallo via OTA gelukt!");

    if(display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
        oledReady = true;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        Serial.print("v7 is GELUKT!");
        display.print("v7 is GELUKT!");
        
        display.display();
    } else {
        Serial.print("Oled is false!");
    }
    // werkt alleen als bootloader-rollback actief is; onschadelijk anders — eigen vangnet: bootGuard()
    esp_ota_mark_app_valid_cancel_rollback();

    prefs.putInt("counter", 0);
} 

void loop() {

    ArduinoOTA.handle();

}

