#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Definice pinů
// pinouts from https://github.com/Xinyuan-LilyGO/TTGO-T-Display
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4

// Inicializace displeje
//Adafruit_ST7789 tft = Adafruit_ST7789( TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

SPIClass spi = SPIClass(VSPI);
Adafruit_ST7789 tft = Adafruit_ST7789(&spi, TFT_CS, TFT_DC, TFT_RST);

void setup() {
  // Inicializace sériové komunikace
  Serial.begin(115200);
  Serial.print(F("Hello! ST77xx TFT Test"));
   pinMode(TFT_BL, OUTPUT);      // TTGO T-Display enable Backlight pin 4
  digitalWrite(TFT_BL, HIGH);   // T-Display turn on Backlight

  // Inicializace SPI
  spi.begin(TFT_SCLK, -1, TFT_MOSI, -1);

  // Inicializace displeje
  tft.init(135, 240);  // Nastavení rozlišení displeje 135x240
  tft.setRotation(2);  // Nastavení orientace displeje

  // Vyplnění displeje barvou
  tft.fillScreen(ST77XX_WHITE);

  // Nastavení textové barvy a velikosti
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);

  // Zobrazení textu
  tft.setCursor(10, 10);
  tft.println("Ahoj světe!");

  // Kreslení základních tvarů
  tft.drawRect(10, 50, 100, 50, ST77XX_RED);  // Obdélník
  tft.fillCircle(60, 150, 30, ST77XX_BLUE);   // Vyplněný kruh
}

void loop() {
  // Kód zde může být prázdný nebo obsahovat další logiku
}
