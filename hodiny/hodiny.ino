#define DIN 23
#define CS 22
#define SCK 21

#include "display.h"
#include "numbers.h"

#include <WiFiManager.h>          // WiFi Manager knihovna
#include "time.h"
#include "sntp.h"

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";

uint8_t * numbers[10] = {
  num0,
  num1,
  num2,
  num3,
  num4,

  num5,
  num6,
  num7,
  num8,
  num9
};
void showTime(uint8_t h, uint8_t m, uint8_t marker) {
  copyOnPos(3,numbers[h/10]);
  copyOnPos(9,numbers[h%10]);
  copyOnPos(18,numbers[m/10]);
  copyOnPos(24,numbers[m%10]);
  
  drawMarker(marker);
  
  showBuffer();
}

void setup() {
  // put your setup code here, to run once:

Serial.begin(115200);
delay(100);
displayInit(DIN, CS, SCK);
WiFiManager wifiManager;

showTime(0,0,1);
//copyOnPos(26,num0);

//sntp_set_time_sync_notification_cb( timeavailable );
sntp_servermode_dhcp(1);
configTzTime(time_zone, ntpServer1, ntpServer2);


  Serial.println("INIT");
  if (!wifiManager.autoConnect("ESP32_NTP_Clock")) {
    Serial.println("Nepodařilo se připojit a nedošlo k resetu.");
    ESP.restart();
  }

Serial.println("C");
}

uint8_t marker=1;

void loop() {
  // put your main code here, to run repeatedly:
struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("No time available (yet)");
    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  //Serial.println(timeinfo.tm_hour);
  showTime(timeinfo.tm_hour,timeinfo.tm_min, marker);
  marker = marker?0:1;
  delay(1000);
}
