/**
 * LCD 彩色测试 (TFT_eSPI)
 * ST7789 240x320 软件 SPI
 *
 * 接线:
 *   RST -> GPIO15   SCK -> GPIO2
 *   DC  -> GPIO4     CS  -> GPIO16
 *   MOSI-> GPIO17    BL  -> GPIO18
 *   VCC -> 3.3V      GND -> GND
 */

#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft;

#define LED_PIN 2

void setup() {
  Serial.begin(115200);
  delay(200);

  // LED 闪烁确认运行
  pinMode(LED_PIN, OUTPUT);
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH); delay(200);
    digitalWrite(LED_PIN, LOW);  delay(200);
  }

  Serial.println("\nLCD TFT_eSPI test...");

  // 背光
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft.init();
  tft.setRotation(0);

  // 彩色填充
  tft.fillScreen(TFT_BLACK);  delay(500);
  Serial.println("BLACK");
  tft.fillScreen(TFT_RED);    delay(500);
  Serial.println("RED");
  tft.fillScreen(TFT_GREEN);  delay(500);
  Serial.println("GREEN");
  tft.fillScreen(TFT_BLUE);   delay(500);
  Serial.println("BLUE");
  tft.fillScreen(TFT_BLACK);  delay(500);

  // 文字
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("TFT_eSPI OK!");
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.println("240x320 ST7789");
  tft.println("MOSI=17 SCK=2");
  tft.println("CS=16 DC=4 RST=15");

  Serial.println("Done!");
}

void loop() {
  // LED 心跳，确认运行
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  delay(1000);
}
