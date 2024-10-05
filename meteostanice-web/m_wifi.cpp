#include "m_wifi.h"
#include <WiFi.h>
#include <WiFiManager.h>          // WiFi manager by tzapu https://github.com/tzapu/WiFiManager

void WiFiOff() {
  WiFi.mode(WIFI_OFF);
  delay(1);
}
// pripojeni k WiFi
bool WiFiConnection(){
  
  WiFi.mode(WIFI_STA); // Probudit WiFi
 
    // Instance WiFiManageru
    WiFiManager wm;
	wm.setDebugOutput(false);

    //pokud chceme vymazat uložené sítě, tak odkomentovat:
    //wm.resetSettings();
 
    // Manager bude nabízet konfiguraci 3 minuty
    wm.setConfigPortalTimeout(180);
    
    bool res;
    res = wm.autoConnect("Meteo Mini", "metemete"); // SSID a heslo konfigurační sítě
 
  return res;
}