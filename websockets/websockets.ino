#include <WiFi.h>
#include <WebSocketsServer.h>

const char* ssid = "ssid";
const char* password = "moje_tajne_heslo";

WebSocketsServer webSocket = WebSocketsServer(81);

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

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server spuštěn na portu 81");
}

void loop() {
  webSocket.loop();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Odpojený\n", num);
      break;
    case WStype_CONNECTED:
      Serial.printf("[%u] Připojený\n", num);
      webSocket.sendTXT(num, "Ahoj z ESP32");
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Text: %s\n", num, payload);
      webSocket.sendTXT(num, "Zpráva přijata");
      break;
  }
}
