#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Definice pinů pro TFT displej
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS   5
#define TFT_DC   16
#define TFT_RST  23
#define TFT_BL   4

#define BUTTON_LEFT 0
#define BUTTON_RIGHT 35


// Nastavení Wi-Fi
const char* ssid = "ssid";
const char* password = "moje_tajne_heslo";

// Nastavení MQTT pro HiveMQ
const char* mqtt_server = "de7fc916008b43698f22326831ee5eda.s1.eu.hivemq.cloud";  // Veřejný broker HiveMQ
const int mqtt_port = 8883;
const char* mqtt_user = "adent";
const char* mqtt_password = "as+47-kw";
const char* topic_room1 = "house/room1/temperature";
const char* topic_room2 = "house/room2/temperature";

// Inicializace TFT displeje
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// Inicializace klienta pro Wi-Fi a MQTT
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Proměnné pro uložení teplot
float tempRoom1 = 0.0;
float tempRoom2 = 0.0;

// Připojení k Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Připojuji se k WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi připojeno");
  Serial.println("IP adresa: ");
  Serial.println(WiFi.localIP());
}

// Callback funkce pro zpracování zpráv z MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Zpráva přišla na topic: ");
  Serial.print(topic);
  Serial.print(". Zpráva: ");
  Serial.println(message);

  if (String(topic) == topic_room1) {
    tempRoom1 = message.toFloat();
  } else if (String(topic) == topic_room2) {
    tempRoom2 = message.toFloat();
  }
  drawTemperatures();
}

// Připojení k MQTT brokeru
void reconnect() {
  while (!client.connected()) {
    Serial.print("Připojuji se k MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Připojeno");
      client.subscribe(topic_room1);
      client.subscribe(topic_room2);
    } else {
      Serial.print("Připojení selhalo, rc=");
      Serial.print(client.state());
      Serial.println(" Zkusím to znovu za 5 sekund");
      delay(5000);
    }
  }
}

// Kreslení teplot na displeji
void drawTemperatures() {
  tft.fillScreen(ST77XX_BLACK);
  
  // Zobrazení teploty pro místnost 1
  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print("Room 1: ");
  tft.print(tempRoom1);
  tft.println(" C");

  // Zobrazení sloupce pro teplotu místnosti 1
  tft.fillRect(10, 40, map(tempRoom1, 15, 40, 0, 230), 20, ST77XX_RED);

  // Zobrazení teploty pro místnost 2
  tft.setCursor(10, 70);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print("Room 2: ");
  tft.print(tempRoom2);
  tft.println(" C");

  // Zobrazení sloupce pro teplotu místnosti 2
  tft.fillRect(10, 100, map(tempRoom2, 15, 40, 0, 230), 20, ST77XX_BLUE);
}

void setup() {
  // Inicializace sériové komunikace
  Serial.begin(115200);

    // Nastavení pinů pro tlačítka
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);

  // Nastavení pinu pro podsvícení displeje
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);  // Zapnutí podsvícení

  // Inicializace displeje
  tft.init(135, 240);  // Nastavení rozlišení displeje 135x240
  tft.setRotation(1);  // Nastavení orientace displeje

  // Připojení k Wi-Fi
  setup_wifi();

  // Deaktivace ověřování certifikátu
  espClient.setInsecure();

  // Nastavení MQTT serveru a callback funkce
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void checkButton(int buttonPin, bool &buttonPressed, const char* topic) {
  if (digitalRead(buttonPin) == LOW) {
    if (!buttonPressed) {
      client.publish(topic, "1");
      buttonPressed = true;
    }
  } else {
    if (buttonPressed) {
      client.publish(topic, "0");
      buttonPressed = false;
    }
  }
}



  // Proměnné pro sledování stavu tlačítek
  bool leftButtonPressed = false;
  bool rightButtonPressed = false;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  // Kontrola stavu tlačítek
  checkButton(BUTTON_LEFT, leftButtonPressed, "house/display/button/left");
  checkButton(BUTTON_RIGHT, rightButtonPressed, "house/display/button/right");
}
