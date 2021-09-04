/*
ESPboy_VoltAmperMeter using INA219 www.ESPboy.com
project by RomanS espboy.edu@gmail.com
*/


#include <Adafruit_INA219.h> //https://github.com/adafruit/Adafruit_INA219
#include "lib/ESPboyInit.h"
#include "lib/ESPboyInit.cpp"

ESPboyInit myESPboy;
Adafruit_INA219 ina219;


void drawDegLine(uint8_t x, uint8_t y, uint16_t length, float deg, uint32_t color){
  myESPboy.tft.drawLine(x, y, x+length*cos(radians(deg)), y+length*sin(radians(deg)), color);
};

void drawDegLineShort(uint8_t x, uint8_t y, uint16_t length1, uint16_t length2, float deg, uint32_t color){
  myESPboy.tft.drawLine(x+length1*cos(radians(deg)), y+length1*sin(radians(deg)), x+length2*cos(radians(deg)), y+length2*sin(radians(deg)), color);
};

void drawGaugeVoltage(){
  for (uint8_t i=0; i<37; i++){
    if(!(i%2)) drawDegLineShort(64, 64, 64-5, 64, -180+i*(180/35), TFT_WHITE);
    else drawDegLineShort(64, 64, 64-2, 64,-180+i*(180/35), TFT_WHITE);
  }
}


void drawAux(){
  myESPboy.tft.drawString("BusVoltage",0,80);
  myESPboy.tft.drawString("V",116,80);
  myESPboy.tft.drawString("ShuntVoltage",0,90);
  myESPboy.tft.drawString("mV",116,90);
  myESPboy.tft.drawString("LoadVoltage",0,100);
  myESPboy.tft.drawString("V",116,100);
  myESPboy.tft.drawString("Current",0,110);
  myESPboy.tft.drawString("mA",116,110);
  myESPboy.tft.drawString("Power",0,120);
  myESPboy.tft.drawString("mW",116,120);
  myESPboy.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  myESPboy.tft.drawLine(0,65,128,65,TFT_BLUE);
  drawDegLine(64, 64, 64-7, -180, TFT_YELLOW);
}


void drawArrowVoltage(float busvoltage){
 static float arrowPreviousPosition = 0;
 static float arrowCurrentPosition = 0;
 static String toPrint;
 
  arrowCurrentPosition = (arrowPreviousPosition + (busvoltage-arrowPreviousPosition)/2);

  if (arrowCurrentPosition != arrowPreviousPosition){
    drawDegLine(64, 64, 64-7, -180+arrowPreviousPosition*(180/35), TFT_BLACK);
    
    myESPboy.tft.setTextColor(TFT_BLUE,  TFT_BLACK);
    myESPboy.tft.drawString("Voltage", (21-7)*3 , 30);
    myESPboy.tft.drawString("V", (21-1)*3 , 50);
    
    myESPboy.tft.drawString("      ", (21-5)*3 , 40);
    toPrint = (String)busvoltage;
    myESPboy.tft.drawString(toPrint, (21-toPrint.length())*3, 40);
    myESPboy.tft.setTextColor(TFT_GREEN, TFT_BLACK);
    
    drawDegLine(64, 64, 64-7, -180+arrowCurrentPosition*(180/35), TFT_YELLOW);
    arrowPreviousPosition = arrowCurrentPosition;
   }  
}


void setup(){
  Serial.begin(115200);
  
  //Init ESPboy
  myESPboy.begin("VoltAmper meter");
  myESPboy.tft.setTextColor(TFT_RED, TFT_BLACK);
    
  if (!ina219.begin()) {
    myESPboy.tft.drawString("Failed init INA219",10,100);
    while (1) { delay(100); }
  }
  ina219.setCalibration_32V_1A();
  //ina219.setCalibration_16V_400mA();

  myESPboy.tft.fillScreen(TFT_BLACK);
  myESPboy.tft.setTextColor(TFT_YELLOW, TFT_BLACK);

  drawGaugeVoltage();
  drawAux();
}



void loop() {
  static float shuntvoltage = 0;
  static float busvoltage = 0;
  static float current_mA = 0;
  static float loadvoltage = 0;
  static float power_mW = 0;
  static String toPrint;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  if (shuntvoltage<0) shuntvoltage=0;
  if (busvoltage<0) busvoltage=0;
  if (current_mA<0) current_mA=0;
  if (power_mW<0) power_mW=0;
  if (loadvoltage<0) loadvoltage=0;
  
  toPrint = (String)busvoltage + " ";
  myESPboy.tft.drawString(toPrint,80,80); //V
  toPrint = (String)shuntvoltage + " ";
  myESPboy.tft.drawString(toPrint,80,90); //mV
  toPrint = (String)loadvoltage + " ";
  myESPboy.tft.drawString(toPrint,80,100);//V
  toPrint = (String)current_mA + " ";
  myESPboy.tft.drawString(toPrint,80,110);//mA
  toPrint = (String)power_mW + " ";
  myESPboy.tft.drawString(toPrint,80,120);//mW

  drawArrowVoltage(busvoltage);

  delay(50);
}
