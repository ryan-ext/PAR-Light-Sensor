/**************************************************************************
  This sketch uses libraries for the Adafruit AS7341 light sensor and the
  TFT screen drivers for the Feather ESP32-S2 TRT Reverse microcontroller.
  
  Written by Ryan Kurasaki
  
  
  
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit ESP32-S2 TFT Feather
    ----> http://www.adafruit.com/products/5300

  Check out the links above for our tutorials and wiring diagrams.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_AS7341.h>

// Use dedicated hardware SPI pins
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_AS7341 as7341;

void setup() {
  Serial.begin(9600);
  Serial.print(F("Hello! Feather TFT Test"));

  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize TFT
  tft.init(135, 240);  // Init ST7789 240x135
  tft.setRotation(4);
  tft.fillScreen(ST77XX_BLACK);

  Serial.println(F("Initialized"));

  uint16_t time = millis();
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  if (!as7341.begin()) {
    Serial.println("Could not find AS7341");
    while (1) {
      delay(10);
    }
  }

  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_8X);

  Serial.println("OK!");



  // large block of text
  tft.fillScreen(ST77XX_BLACK);


  Serial.println("done");
  delay(1000);

  Serial.println("PPFD\t415 nm\t445 nm\t480 nm\t515 nm\t555 nm\t590 nm\t630 nm\t680 nm\tClear\tNearIR");
}

void loop() {
  uint16_t readings[12];  //Raw readings
  uint16_t readings2[12]; //Calibrated readings for display
  if (!as7341.readAllChannels(readings)) {
    Serial.println("Error reading all channels!");
    return;
  }

  tft.fillScreen(ST77XX_BLACK);
  uint16_t color[] = { 0x981F, 0x181F, 0x001F, 0x07E0, 0xFFE0, 0xFC40, 0xF800, 0xD000 };
  uint16_t m;

  readings2[0] = readings[0] / 0.17;
  readings2[1] = readings[1] / 0.30;
  readings2[2] = readings[2] / 0.38;
  readings2[3] = readings[3] / 0.49;
  readings2[6] = readings[6] / 0.59;
  readings2[7] = readings[7] / 0.65;
  readings2[8] = readings[8] / 0.75;
  readings2[9] = readings[9] / 1.00;

  //Find max reading for normalizing wavelength bars on the TFT display
  m = max(readings2[0], readings2[1]);
  m = max(m, readings2[2]);
  m = max(m, readings2[3]);
  m = max(m, readings2[6]);
  m = max(m, readings2[7]);
  m = max(m, readings2[8]);
  m = max(m, readings2[9]);

  int sum;
  sum = (readings[0] + readings[1] + readings[2] + readings[3] + readings[6] + readings[7] + readings[8] + readings[9]);
  //Calibrated ppfd reading. Should be performed with reference meter and is most accurate when the light source is
  //kept the same or is similar.
  float PPFD = sum * 0.0298 + 1.03;

  int n = 0;
  float h;
  int scale = 100;
  int w = 13;
  int b = 230;
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(3);
  tft.println("PPFD ");
  tft.println(PPFD, 0);
  h = (readings2[0] * scale / m / exp(-0.5));
  tft.fillRect(10 + n * 15, b - h, w, h, 0x981F);
  tft.drawRect(10 + n * 15, b - h, w, h, 0x981F);

  n = n + 1;
  h = (readings2[1] * scale / m / exp(-0.75));
  tft.fillRect(10 + n * 15, b - h, w, h, 0x003A);
  tft.drawRect(10 + n * 15, b - h, w, h, 0x003A);

  n = n + 1;
  h = (readings2[2] * scale / m / exp(-0.6));
  tft.fillRect(10 + n * 15, b - h, w, h, 0x06BE);
  tft.drawRect(10 + n * 15, b - h, w, h, 0x06BE);

  n = n + 1;
  h = (readings2[3] * scale / m / exp(-0.3));
  tft.fillRect(10 + n * 15, b - h, w, h, 0x07E0);
  tft.drawRect(10 + n * 15, b - h, w, h, 0x07E0);

  n = n + 1;

  h = (readings2[6] * scale / m / exp(-0.2));
  tft.fillRect(10 + n * 15, b - h, w, h, 0xFFE0);
  tft.drawRect(10 + n * 15, b - h, w, h, 0xFFE0);
  n = n + 1;
  h = (readings2[7] * scale / m / exp(-0.05));
  tft.fillRect(10 + n * 15, b - h, w, h, 0xFC40);
  tft.drawRect(10 + n * 15, b - h, w, h, 0xFC40);

  n = n + 1;
  h = (readings2[8] * scale / m / exp(-0.1));
  tft.fillRect(10 + n * 15, b - h, w, h, 0xFB2A);
  tft.drawRect(10 + n * 15, b - h, w, h, 0xFB2A);

  n = n + 1;
  h = (readings2[9] * scale / m / exp(-0.2));
  tft.fillRect(10 + n * 15, b - h, w, h, 0xF000);
  tft.drawRect(10 + n * 15, b - h, w, h, 0xF000);

  n = n + 1;


  if (Serial.available())  // if there is data comming
  {
    String command = Serial.readStringUntil('\n');  // read string until meet newline character
    if (command == "title") {
      Serial.println("415 nm\t445 nm\t480 nm\t515 nm\t555 nm\t590 nm\t630 nm\t680 nm\tClear\tNearIR");
    }
    //Adjust gain
    if (command == "g1") {
      as7341.setGain(AS7341_GAIN_1X);
    }
    if (command == "g2") {
      as7341.setGain(AS7341_GAIN_2X);
    }
    if (command == "g4") {
      as7341.setGain(AS7341_GAIN_4X);
    }
    if (command == "g8") {
      as7341.setGain(AS7341_GAIN_8X);
    }
    if (command == "g16") {
      as7341.setGain(AS7341_GAIN_16X);
    }
    if (command == "g32") {
      as7341.setGain(AS7341_GAIN_32X);
    }
    if (command == "g64") {
      as7341.setGain(AS7341_GAIN_64X);
    }
    if (command == "g128") {
      as7341.setGain(AS7341_GAIN_128X);
    }
    if (command == "g256") {
      as7341.setGain(AS7341_GAIN_256X);
    }
    if (command == "g512") {
      as7341.setGain(AS7341_GAIN_512X);
    }
    if (command == "t6") {
      as7341.setATIME(1);
      as7341.setASTEP(1079);
    }
    if (command == "t12") {
      as7341.setATIME(3);
      as7341.setASTEP(1079);
    }
    if (command == "t17") {
      as7341.setATIME(4);
      as7341.setASTEP(1223);
    }
    if (command == "t25") {
      as7341.setATIME(8);
      as7341.setASTEP(998);
    }
    if (command == "t34") {
      as7341.setATIME(11);
      as7341.setASTEP(1018);
    }
    if (command == "t69") {
      as7341.setATIME(23);
      as7341.setASTEP(1048);
    }
    if (command == "t100") {
      as7341.setATIME(35);
      as7341.setASTEP(998);
    }
    if (command == "t133") {
      as7341.setATIME(47);
      as7341.setASTEP(998);
    }
    if (command == "t148") {
      as7341.setATIME(52);
      as7341.setASTEP(1017);
    }
    if (command == "t216") {
      as7341.setATIME(77);
      as7341.setASTEP(995);
    }
    if (command == "t222") {
      as7341.setATIME(79);
      as7341.setASTEP(997);
    }

    //Display readings on Serial Monitor upon entering '1'
    if (command == "1") {
      Serial.print(readings[0]);
      Serial.print('\t');
      Serial.print(readings[1]);
      Serial.print('\t');
      Serial.print(readings[2]);
      Serial.print('\t');
      Serial.print(readings[3]);
      Serial.print('\t');
      Serial.print(readings[6]);
      Serial.print('\t');
      Serial.print(readings[7]);
      Serial.print('\t');
      Serial.print(readings[8]);
      Serial.print('\t');
      Serial.print(readings[9]);
      Serial.print('\t');
      Serial.print(readings[10]);
      Serial.print('\t');
      Serial.print(readings[11]);
      Serial.print('\t');
      Serial.print(sum);
      Serial.print('\t');
      Serial.println(PPFD, 0);
    }
  }
  delay(1000);
}
