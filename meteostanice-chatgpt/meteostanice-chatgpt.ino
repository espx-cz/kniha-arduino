#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SparkFun_SCD4x_Arduino_Library.h>

#include <WiFi.h>
#include <time.h>

#include <SPIFFS.h>
#include <FS.h>

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "env.h"

//LaskaKit Meteo Mini C3
#define SDA 19
#define SCL 18
#define PIN_ON 3

Adafruit_BME280 bme;
SCD4x scd4x; 

void setup() {
  Serial.begin(115200);
  pinMode(PIN_ON, OUTPUT);
  digitalWrite(PIN_ON, HIGH);

  Wire.begin(SDA, SCL);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS nenalezen");
    while (1);
  }
  if (!bme.begin(0x76)) {
    Serial.println("BME280 nenalezen!");
    while (1);
  }
  if (!scd4x.begin(false, true)) {
    Serial.println("SCD41 nenalezen!");
    while (1);
  }
  scd4x.startPeriodicMeasurement();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Připojování...");
  }
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
}

void loop() {
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float humidity = bme.readHumidity();
  uint16_t co2;
  float temperature_co2, humidity_co2;

  if (scd4x.readMeasurement()) { // Změněno na použití nové knihovny
    co2 = scd4x.getCO2();
    temperature_co2 = scd4x.getTemperature();
    humidity_co2 = scd4x.getHumidity();

    File file = SPIFFS.open("/data.txt", FILE_APPEND);
    if (!file) {
      Serial.println("Nelze otevřít soubor pro zápis");
      return;
    }

    time_t now;
    time(&now);
    file.printf("%d,%f,%f,%f,%d\n", now, temperature, pressure, humidity, co2);
    file.close();
    trimFile();
  }

  static unsigned long lastSendTime = 0;
  if (millis() - lastSendTime > 3600000) {
    sendDataToChatGPT();
    lastSendTime = millis();
  }

  delay(600000); // Měření každých 10 minut
}

void trimFile() {
  File file = SPIFFS.open("/data.txt");
  if (!file) {
    Serial.println("Nelze otevřít soubor pro čtení");
    return;
  }

  int lines = 0;
  while (file.available()) {
    file.readStringUntil('\n');
    lines++;
  }
  file.close();

  if (lines > 288) {
    File file = SPIFFS.open("/data.txt");
    String newData = "";
    int skipLines = lines - 288;

    for (int i = 0; i < skipLines; i++) {
      file.readStringUntil('\n');
    }

    while (file.available()) {
      newData += file.readStringUntil('\n') + "\n";
    }
    file.close();

    file = SPIFFS.open("/data.txt", FILE_WRITE);
    file.print(newData);
    file.close();
  }
}




void sendDataToChatGPT() {
  File file = SPIFFS.open("/data.txt");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  String data;
  while (file.available()) {
    data += file.readStringUntil('\n');
  }
  file.close();

  WiFiClientSecure client;
  HTTPClient http;
  http.begin(client, "https://api.openai.com/v1/chat/completions");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(openai_api_key));

  String json = R"(
  {
    "model": "gpt-4",
    "messages": [
      {
        "role": "system",
        "content": "You are an AI model providing weather predictions based on historical data."
      },
      {
        "role": "user",
        "content": "Jste AI model ChatGPT a vaší úlohou je předpovídat počasí na další hodinu na základě historických údajů o teplotě, tlaku a vlhkosti. Níže jsou uvedeny údaje za posledních 24 hodin.\n\nČasové razítko (hodina): Teplota (°C), Tlak (hPa), Vlhkost (%)\n" + data + "\n\nNa základě těchto dat předpověz počasí na další hodinu (24:00). Vrátíš pouze očekávanou teplotu jako číslo s desetinnou tečkou."
      }
    ],
    "max_tokens": 10
  }
  )";

  int httpResponseCode = http.POST(json);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.println("Error in HTTP request");
  }
  http.end();
}
