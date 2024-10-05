#include <FS.h>

#include <SPI.h>
#include <TFT_eSPI.h> // Grafická knihovna speciálně pro ESP32 a TTGO T-Display

TFT_eSPI tft = TFT_eSPI();  // Vytvoření instance displeje

void setup() {

  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
  }
  Serial.println("\r\nSPIFFS available!");

  tft.init();                   // Inicializace displeje
  tft.setRotation(1);           // Nastavení orientace displeje
  tft.setTextColor(TFT_WHITE);  // Nastavení barvy textu na bílou
  tft.setTextSize(9);  
  tft.loadFont("ProcessingSans-Bold36");
}

void loop() {
  tft.fillScreen(TFT_BLACK);    // Vyčištění displeje na černou barvu

  // Kreslení obdélníků
  tft.drawRect(10, 10, 50, 30, TFT_RED); // Kreslí červený obdélník
  tft.fillRect(70, 10, 50, 30, TFT_BLUE); // Kreslí vyplněný modrý obdélník

  // Kreslení kruhů
  tft.drawCircle(35, 90, 20, TFT_YELLOW); // Kreslí žlutý kruh
  tft.fillCircle(105, 90, 20, TFT_GREEN); // Kreslí vyplněný zelený kruh

    tft.setCursor(10, 80);        // Nastavení kurzoru na pozici x=10, y=10
  tft.println("Ahoj světe");

  delay(2000);                  // Počká 2 sekundy
}
