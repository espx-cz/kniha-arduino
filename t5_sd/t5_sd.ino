#include <GxEPD2_BW.h>

GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(/*CS=*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // DEPG0213BN 122x250, SSD1680, TTGO T5 V2.4.1, V2.3.1


// FreeFonts from Adafruit_GFX
//#include <Fonts/FreeMonoBold9pt7b.h>
//#include <Fonts/FreeMonoBold12pt7b.h>
//#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
//#include <GxIO/GxIO_SPI/GxIO_SPI.h>
//#include <GxIO/GxIO.h>

#include "image.h"

//SD CARD 

//#include "FS.h"
#include "SD.h"
#include "SPI.h"
SPIClass hspi(HSPI);

#define DW 122
#define DW8 16
#define DH 250

uint8_t bitmap[(DW8)*DH]; //

void Unmount_SD(SPIClass spi) {
  // Tested with Kingston 32GB
  unsigned long time1;
  time1 = micros();
  Serial.println("Ejecting the ESP32-CAM SD");
  SD.end();
  spi.end();
  // Set MOSI High
  pinMode(15, INPUT_PULLUP);
  // Disable the SD by an HIGH CS/SS
  pinMode(13, OUTPUT); digitalWrite(13, HIGH);
  // Provide dummy clocks
  pinMode(14, OUTPUT);
  for (int i = 1; i <= 8; i++) {
    digitalWrite(14, LOW);
    digitalWrite(14, HIGH);
  }
  // Set CLK High
  pinMode(14, INPUT_PULLUP);
  // Check if MISO became "LOW"
  pinMode(2, INPUT_PULLDOWN);
  delay(1);
  if (digitalRead(2)) {
    Serial.println(F("Failed to unmount the SD"));
  } else
  { Serial.printf("Unmounting of the SD-micro card takes : %ld [uSec]\n", (micros() - time1) );
    Serial.println(F("SD has been unmounted safely"));
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Serial.printf("CS %d, DP %d, RSET %d\n", EPD_CS, EPD_DC,  EPD_RSET);
  delay(1000);
  Serial.printf("%d(w) x %d(h)\n", display.width(),display.height());
  int dw = display.width();
  int dh = display.height();
  //SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);

  
  hspi.begin(/*EPD_SCLK=*/14, /*EPD_MISO=*/2, /*EPD_MOSI=*/15);


  if(!SD.begin(13, hspi)){
        Serial.println("Card Mount Failed");
//        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
//        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    File file = SD.open("/image.bin", FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for writing");
    }
    Serial.println("File opened");
    int bytes = file.write(image, DW8*DH);
    if(bytes){
        Serial.printf("File written, %d bytes written\n",bytes);
    } else {
        Serial.println("Write failed");
    }
    file.close();


    file = SD.open("/image2.bin", FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
    }
    Serial.println("File 2 opened");
    bytes = file.print(R"(((
      HEREDOC
      test
      )-==");
    if(bytes){
        Serial.printf("File written, %d bytes written\n",bytes);
    } else {
        Serial.println("Write failed");
    }
    file.close();

    Serial.println("File closed");
    SD.end();
    Serial.println("SD closed");

  display.init(); // enable diagnostic output on Serial
  Serial.printf("DISP INIT\n");
  display.setRotation(3);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold24pt7b);
  display.setCursor(0, 30);
  display.println("Ahoj Bimbulo");
  

  
  //display.display(false);
  Serial.printf("DISP DISP\n");
  //delay(5000);
  /*
  for (int i=0;i<DW8;i++) {
    Serial.println(image[i+DW8]);
  }
  */
  //display.writeImage(bitmap,0,0,dw,dh, true);
  display.writeImage(image,0,0,dw,dh, true, false, false);
  display.refresh(false);
  Serial.printf("DISP DONE\n");

  Unmount_SD(hspi);
}

void loop() {
  // put your main code here, to run repeatedly:
  

}
