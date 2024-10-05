#include <time.h>
#include <WiFi.h>


const char* ssid       = "ssid";
const char* password   = "moje_tajne_heslo";

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";

const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";  
// TimeZone rule pro Europe/Prague


void setup()
{
  Serial.begin(115200);


  configTzTime(time_zone, ntpServer1, ntpServer2);

  //connect to WiFi
  Serial.printf("Připojování k %s ", ssid);
  /*
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" Připojeno");
*/

}

void loop() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    //return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  // Časový interval mezi zobrazením času
  delay(10000);
}