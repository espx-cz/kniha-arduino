//OTA
#include <ESPmDNS.h>
#include <WebServer.h>
#include <ElegantOTA.h>
const char* host = "meteo";

void doOTA(esp_sleep_wakeup_cause_t wakeupReason) {
  
  // pokud se zařízení probralo z jiného důvodu, než je probuzení ze spánku, tak pustíme webserver a OTA
  if (wakeupReason!=ESP_SLEEP_WAKEUP_TIMER) {
    MDNS.begin(host);
    WebServer server(80);
    ElegantOTA.begin(&server);    // Start ElegantOTA
    server.begin();
    //čekám 2 minuty na přenos
    unsigned long int timer = millis();
    while((millis()-timer) < 2 * 60 * 1000) {
        server.handleClient();
        ElegantOTA.loop();
    }
  }
}