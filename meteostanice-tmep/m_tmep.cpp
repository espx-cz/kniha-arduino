#include "m_tmep.h"
#include <HTTPClient.h>
#include <WiFi.h>

#include "env.h"


void postData(float bat_voltage, float t1, float h1, float pressure, float t2, float h2, float co2){
 
  if(WiFi.status()== WL_CONNECTED) {
    HTTPClient http;
 
    String serverPath = "http://"+domenaBME+".tmep.cz/index.php?"
    + "t="+t1
    + "&h="+h1
    + "&p="+pressure
    + "&rssi="+ WiFi.RSSI()
    + "&voltage="+bat_voltage
    ;
 
        http.begin(serverPath.c_str());
 
    int httpResponseCode = http.GET();
 
    if (httpResponseCode>0) {
      Serial.print("HTTP response: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();

  //SHT
serverPath = "http://"+domenaSHT+".tmep.cz/index.php?"
    + "t="+t2
    + "&h="+h2
    + "&co="+co2
    + "&rssi="+ WiFi.RSSI()
    + "&voltage="+bat_voltage
    /*
    + "&field6="+t2
    + "&field7="+h2
    + "&field8="+co2
    */
    ;
 
        http.begin(serverPath.c_str());
 
    httpResponseCode = http.GET();
 
    if (httpResponseCode>0) {
      Serial.print("HTTP response: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();

  } else
      Serial.println("Wi-Fi disconnected");
}