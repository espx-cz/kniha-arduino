// Moje verze je T5_V2.3.1_2.13 - interní označení je V213
//#define LILYGO_T5_V213 
//#include <boards.h>
#include <GxEPD2_BW.h>

/*
The current version of LilyGo V2.13, the ink screen version is DEPG0213BN, please select DEPG0213BN in the sketch
*/

GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(/*CS=*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // DEPG0213BN 122x250, SSD1680, TTGO T5 V2.4.1, V2.3.1


//#include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w  form DKE GROUP

// FreeFonts from Adafruit_GFX
//#include <Fonts/FreeMonoBold9pt7b.h>
//#include <Fonts/FreeMonoBold12pt7b.h>
//#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
//#include <GxIO/GxIO_SPI/GxIO_SPI.h>
//#include <GxIO/GxIO.h>

//GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
//GxEPD_Class display(io, EPD_RSET, EPD_BUSY);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Serial.printf("CS %d, DP %d, RSET %d\n", EPD_CS, EPD_DC,  EPD_RSET);
  Serial.printf("%d(w) x %d(h)\n", display.width(),display.height());
  //SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);

  display.init(); // enable diagnostic output on Serial

  display.setRotation(3);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold24pt7b);
  display.setCursor(0, 30);
  display.println("Ahoj světe");
  display.display(false);
}

void loop() {
  // put your main code here, to run repeatedly:
  

}
