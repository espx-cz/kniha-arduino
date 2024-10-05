#define DEBUG 1
#include "dbg.h"

// definice pro Meteokit
#include "meteokit.h"

// Délka spánku meteostanice
#define SLEEP_SEC 5*60

// Ovládání sběrnice I2C
#include <Wire.h>

//BME280
#define ABOVESEA 354.0F
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;

//SCD41
#include <SparkFun_SCD4x_Arduino_Library.h>
SCD4x scd;

//Baterie
#include <ESP32AnalogRead.h>
ESP32AnalogRead adc;


//Wi-Fi
#include "m_wifi.h"
#include "m_tmep.h"

//OTA
#include "m_ota.h"

#if DEBUG
void printBME() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Sea Level Pressure = ");
    Serial.print(bme.seaLevelForAltitude(ABOVESEA, bme.readPressure()) / 100.0F);
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}
#endif

#if DEBUG
void printSCD() {
    Serial.print("CO2 = ");
    Serial.print(scd.getCO2());
    Serial.println(" ppm");

    Serial.print("Teplota = ");
    Serial.print(scd.getTemperature(), 1);
    Serial.println(" °C");

    Serial.print("Vlhkost = ");
    Serial.print(scd.getHumidity(), 1);
    Serial.println(" %");
    Serial.println();
}
#endif



void setup() {
  #if DEBUG
  Serial.begin(115200);
  #endif

  esp_sleep_wakeup_cause_t wakeupReason = esp_sleep_get_wakeup_cause();

  Dprintln("INIT");
  pinMode(PIN_ON, OUTPUT);      // Set EN pin for uSUP stabilisator as output
  digitalWrite(PIN_ON, HIGH);   // Turn on the uSUP power

  WiFiOff();

  Wire.begin(SDA, SCL);

  unsigned BMEstatus;
    
  BMEstatus = bme.begin();  
  if (!BMEstatus) {
      Dprintln("Nelze najít senzor BME280!");
      Dprint("SensorID je: 0x"); Dprintln2(bme.sensorID(),16);
      Dprint("        ID 0xFF znamená špatnou adresu, BMP 180 nebo BMP 085\n");
      Dprint("        ID 0x56-0x58 znamená BMP 280,\n");
      Dprint("        ID 0x60 znamená BME 280.\n");
      Dprint("        ID 0x61 znamená BME 680.\n");
      while (1) delay(10); //nekonečná smyčka
  }
  Dprint("-- Test --");
  Dprintln(BMEstatus);

  #if DEBUG
  printBME();
  #endif

  //SCD41
  //             begin, autoCalibrate
  //               |      |
  if (scd.begin(false, true) == false)
  {
    Dprintln("SCD41 nebyl správně inicializován");
    while (1) delay(10);
  }

  if (scd.startLowPowerPeriodicMeasurement() == true)  {
    Dprintln("LP mód spuštěn");
  }

  //ADC
  adc.attach(PIN_ADC);

  float bat_voltage = adc.readVoltage() * VoltageDividerRatio;
  Dprint("Baterie = " );
  Dprint(bat_voltage);
  Dprintln("V");

  while (!scd.readMeasurement()) {
    delay(100);
  }

  #if DEBUG
  printSCD();
  #endif

  if(!WiFiConnection()) {
    Dprintln("Spojení selhalo");
    // ESP.restart();
  }
  else {
    Dprintln("Wi-Fi připojeno");
  }

  doOTA(wakeupReason);

  postData(
    bat_voltage,
    bme.readTemperature(),
    bme.readHumidity(),
    bme.seaLevelForAltitude(354.0F, bme.readPressure()) / 100.0F,
    scd.getTemperature(),
    scd.getHumidity(),
    scd.getCO2()

  );

  //Jdeme spát
  digitalWrite(PIN_ON, LOW); //vypnout senzory
  Dprintln("ESP hibernuje");
      // Vypnout RTC (Low Power) periferie
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,   ESP_PD_OPTION_OFF);
    // Vypnout RTC Slow Memory
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    // Vypnout RTC Fast Memory
  esp_sleep_pd_config(ESP_PD_DOMAIN_RC_FAST_MEM, ESP_PD_OPTION_OFF);
    // Vypnout krystalový oscilátor 
  esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL,         ESP_PD_OPTION_OFF);

  esp_sleep_enable_timer_wakeup(SLEEP_SEC * 1000000);
  esp_deep_sleep_start();

}

void loop() {
  ;
}
