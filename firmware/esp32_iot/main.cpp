#include <WiFi.h>
#include <HTTPClient.h>

// ---------------- WiFi ----------------
const char* ssid = "wifi username";
const char* password = "wifi password";

// ---------------- ThingSpeak ----------------
const char* thingspeakAPIKey = "L6LMHF1B0KSCODI";
const char* thingspeakURL = "http://api.thingspeak.com/update";

// ---------------- UART ----------------
// STM32 PA9 TX -> ESP32 GPIO16 RX
#define STM32_RX 16

char rxBuffer[200];
uint8_t rxIndex = 0;

// ThingSpeak requires at least 15 seconds between updates
unsigned long lastUploadTime = 0;
const unsigned long uploadInterval = 20000;

// ---------------- Setup ----------------
void setup()
{
    Serial.begin(115200);

    Serial2.begin(
        115200,
        SERIAL_8N1,
        STM32_RX,
        -1
    );

    Serial.println();
    Serial.println("BMS ESP32 UART + ThingSpeak");

    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.println("Waiting for STM32 data...");
}

// ---------------- Loop ----------------
void loop()
{
    // Reconnect if WiFi disconnects
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi disconnected. Reconnecting...");
        WiFi.reconnect();
        delay(1000);
        return;
    }

    while (Serial2.available())
    {
        char received = Serial2.read();

        if (received == '\n')
        {
            rxBuffer[rxIndex] = '\0';

            if (rxIndex > 0)
            {
                Serial.print("STM32 Data: ");
                Serial.println(rxBuffer);

                // Upload only if WiFi is connected
                if (WiFi.status() == WL_CONNECTED)
                {
                    if (millis() - lastUploadTime >= uploadInterval)
                    {
                        uploadToThingSpeak(rxBuffer);
                        lastUploadTime = millis();
                    }
                    else
                    {
                        Serial.println("Waiting for ThingSpeak upload interval...");
                    }
                }
            }

            rxIndex = 0;
        }
        else if (received != '\r')
        {
            if (rxIndex < sizeof(rxBuffer) - 1)
            {
                rxBuffer[rxIndex++] = received;
            }
            else
            {
                // Reset buffer if message is too long
                rxIndex = 0;
            }
        }
    }
}

// ---------------- ThingSpeak Upload ----------------
void uploadToThingSpeak(const char* data)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected. Upload skipped.");
        return;
    }

    HTTPClient http;

    String url = String(thingspeakURL);
    url += "?api_key=";
    url += thingspeakAPIKey;

    // Example: STM32 data should be parsed into individual fields.
    // For now, upload the complete UART message to Field 1.
    url += "&field1=";
    url += String(data);

    http.begin(url);

    int httpCode = http.GET();

    if (httpCode > 0)
    {
        String response = http.getString();

        Serial.print("ThingSpeak HTTP Code: ");
        Serial.println(httpCode);

        Serial.print("ThingSpeak Entry ID: ");
        Serial.println(response);

        if (response.toInt() > 0)
        {
            Serial.println("Upload Successful!");
        }
        else
        {
            Serial.println("Upload Failed!");
        }
    }
    else
    {
        Serial.print("HTTP Error: ");
        Serial.println(http.errorToString(httpCode));
    }

    http.end();
}
