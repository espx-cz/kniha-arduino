#include <Arduino.h>

const uint8_t NO_OP_CMD         = 0x00;
const uint8_t DIGIT_0_CMD       = 0x01;
const uint8_t DIGIT_1_CMD       = 0x02;
const uint8_t DIGIT_2_CMD       = 0x03;
const uint8_t DIGIT_3_CMD       = 0x04;
const uint8_t DIGIT_4_CMD       = 0x05;
const uint8_t DIGIT_5_CMD       = 0x06;
const uint8_t DIGIT_6_CMD       = 0x07;
const uint8_t DIGIT_7_CMD       = 0x08;
const uint8_t DECODE_MODE_CMD   = 0x09;
const uint8_t INTENSITY_CMD     = 0x0A;
const uint8_t SCAN_LIMIT_CMD    = 0x0B;
const uint8_t SHUTDOWN_CMD      = 0x0C;
const uint8_t DISPLAY_TEST_CMD  = 0x0F;

const uint8_t SHUTDOWN_OFF      = 0x01;
const uint8_t NO_DECODE         = 0x00;

uint8_t DIN;
uint8_t CLOCK;
uint8_t CS;

uint8_t displayBuffer[4*8]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void send8(uint8_t value) {
  for (uint8_t i=0;i<8;i++) {
    uint8_t bit = (value&0x80)?1:0;
    //Serial.print(bit);
    digitalWrite(DIN,bit);
    digitalWrite(CLOCK,1);
    digitalWrite(CLOCK,0);
    //delay(1);
    value = value<<1;
  }
  //Serial.print(".");
}
void sendCmd (uint8_t address, uint8_t value) {
  digitalWrite(CLOCK,0);
  digitalWrite(CS,0);
  send8(address);
  send8(value);
  digitalWrite(CS,1);
}
void sendCmd4 (uint8_t address, uint8_t value) {
  //Serial.println("");
  digitalWrite(CLOCK,0);
  digitalWrite(CS,0);
  send8(address);
  send8(value);
  //digitalWrite(CS,1);

  //digitalWrite(CS,0);
  send8(address);
  send8(value);
  //digitalWrite(CS,1);

  //digitalWrite(CS,0);
  send8(address);
  send8(value);
  //digitalWrite(CS,1);

  //digitalWrite(CS,0);
  send8(address);
  send8(value);
  
  digitalWrite(CS,1);
  digitalWrite(CLOCK,0);
}

void sendCmd4M (uint8_t address, uint8_t value1,uint8_t value2,uint8_t value3,uint8_t value4) {
  //Serial.println("");
  digitalWrite(CLOCK,0);
  digitalWrite(CS,0);
  send8(address);
  send8(value1);
  //digitalWrite(CS,1);

  //digitalWrite(CS,0);
  send8(address);
  send8(value2);
  //digitalWrite(CS,1);

  //digitalWrite(CS,0);
  send8(address);
  send8(value3);
  //digitalWrite(CS,1);

  //digitalWrite(CS,0);
  send8(address);
  send8(value4);
  
  digitalWrite(CS,1);
  digitalWrite(CLOCK,0);
}

void displayInit(uint8_t tDIN,uint8_t tCS,uint8_t tCLOCK) {
  DIN = tDIN;
  CS = tCS;
  CLOCK = tCLOCK;

  pinMode(CS,OUTPUT);
  pinMode(DIN,OUTPUT);
  pinMode(CLOCK,OUTPUT);
  digitalWrite(CS,1);
  digitalWrite(CLOCK,0);
  sendCmd4(SHUTDOWN_CMD, SHUTDOWN_OFF);
  sendCmd4(DECODE_MODE_CMD, NO_DECODE);
  sendCmd4(SCAN_LIMIT_CMD, 0xff);
  sendCmd4(INTENSITY_CMD, 0x02);
  sendCmd4(DISPLAY_TEST_CMD, 0);
}

void copyOnPos(uint8_t pos, uint8_t *num) {
  uint16_t mask;
  uint16_t bitmap;

  uint8_t segment;
  uint8_t shift;

  shift = pos & 7;
  segment = pos >> 3;

  for (uint8_t i=0;i<8;i++) {
    mask = 0xFC00; //pos0
    mask = mask >> shift;
    mask = mask ^ 0xffff;
    bitmap = num[i]<<(10-shift); //8 + 2
    //bitmap = bitmap >> pos2shift[pos];
    displayBuffer[i*4+segment] = displayBuffer[i*4+segment] & (mask>>8) | (bitmap>>8);
    displayBuffer[i*4+segment+1] = displayBuffer[i*4+segment+1] & (mask&0xff) | (bitmap&0xff);
  }
}

void showBuffer() {
  for (uint8_t digit = DIGIT_0_CMD; digit <= DIGIT_7_CMD; digit++)
    {
      uint8_t pos = digit-1;
      uint8_t line = pos*4;
       sendCmd4M(digit, displayBuffer[line], displayBuffer[line+1],displayBuffer[line+2],displayBuffer[line+3]);
  
    };
}

#define mkln(line)   displayBuffer[line*4+1] = displayBuffer[line*4+1] & 0xfe;\
  displayBuffer[line*4+2] = displayBuffer[line*4+2] & 0x7F;\
  if (marker) {\
    displayBuffer[line*4+1] = displayBuffer[line*4+1] | 0x01;\
    displayBuffer[line*4+2] = displayBuffer[line*4+2] | 0x80;\
  }


void drawMarker(uint8_t marker) {
  //markerlines = 1,2,4,5
  //markerdots = 1,2
  mkln(1);
  mkln(2);
  mkln(4);
  mkln(5);

}