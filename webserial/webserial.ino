#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

const char* ssid = "ssid";
const char* password = "moje_tajne_heslo";

AsyncWebServer server(80);

void receivedWebSerialData(uint8_t *data, size_t len) {
  Serial.print("Přijato: ");
  for (size_t i = 0; i < len; i++) {
    Serial.print((char)data[i]);
  }
  Serial.println();
  WebSerial.println("Zpráva přijata");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Připojování k WiFi...");
  }

  Serial.println("WiFi připojeno");
  Serial.print("IP adresa: ");
  Serial.println(WiFi.localIP());

  WebSerial.begin(&server);
  WebSerial.msgCallback(receivedWebSerialData);

  server.begin();
  Serial.println("WebSerial server spuštěn na portu 80");
}

void loop() {
  // Není třeba nic dělat, WebSerial pracuje asynchronně
}


