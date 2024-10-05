#include "m_thingspeak.h"
#include <HTTPClient.h>
#include <WiFi.h>

#include "env.h"


void postData(float bat_voltage, float t1, float h1, float pressure, float t2, float h2, float co2){
 
  if(WiFi.status()== WL_CONNECTED) {
    HTTPClient http;
 
    String serverPath = "http://api.thingspeak.com/update?api_key="+APIKEY+"&field1=" + bat_voltage 
    + "&field2=" + WiFi.RSSI()
    + "&field3="+t1
    + "&field4="+h1
    + "&field5="+pressure
    + "&field6="+t2
    + "&field7="+h2
    + "&field8="+co2
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
  } else
      Serial.println("Wi-Fi disconnected");
}