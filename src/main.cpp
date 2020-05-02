#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_SHT31.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <secrets.h>

// Secrets are defined inside secrets.h
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

Adafruit_PCD8544 display = Adafruit_PCD8544(14, 13, 27, 15, 26);
Adafruit_SHT31 sht1 = Adafruit_SHT31();
Adafruit_SHT31 sht2 = Adafruit_SHT31();

void setup() {
    Serial.begin(115200);

    Serial.println("Booting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    ArduinoOTA.setHostname("prusa-buddy");
    ArduinoOTA
        .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else  // U_SPIFFS
                type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Serial.println("Start updating " + type);
        })
        .onEnd([]() { Serial.println("\nEnd"); })
        .onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
            Serial.println("");
        })
        .onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
                Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR)
                Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR)
                Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR)
                Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR)
                Serial.println("End Failed");
        });

    ArduinoOTA.begin();

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    display.begin();
    display.setContrast(50);
    display.setRotation(2);
    display.setTextSize(1);
    display.setTextColor(BLACK);

    display.display();
    delay(2000);

    Serial.println("init SHT31");
    if (!sht1.begin(0x44)) {
        Serial.println("Couldn't find first SHT3x");
    }

    if (!sht2.begin(0x45)) {
        Serial.println("Couldn't find second SHT3x");
    }
}

void loop() {
    ArduinoOTA.handle();

    float t1 = sht1.readTemperature();
    float h1 = sht1.readHumidity();
    float t2 = sht2.readTemperature();
    float h2 = sht2.readHumidity();

    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("Inside:");
    display.printf("T:%.1f H:%.1f", t1, h1);

    display.println("");
    display.println("");

    display.println("Outside:");
    display.printf("T:%.1f H:%.1f", t2, h2);
    display.display();
    delay(2000);
}