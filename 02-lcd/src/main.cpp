/**
 * 02-lcd — ST7789 240x320 屏幕显示 Demo
 *
 * 功能：白底信息屏 + 轮播演示
 *   - 页面 1：主屏 — 标题、日期、中文、彩色文字
 *   - 页面 2：调色板 — 16 色条 + 颜色名称
 *   - 页面 3：图形 — 圆、矩形、三角形
 *   - 页面 4：文字展示 — 各字号英文 + 中文
 *
 * 硬件: ESP32-WROOM (featheresp32)
 * 屏幕: 2.4" TFT ST7789, SPI 240x320
 * 驱动: TFT_eSPI, RGB→BGR 颜色模式
 *
 * 接线:
 *   引脚# | 屏幕引脚 | 功能     | → ESP32
 *   ------|----------|----------|--------
 *   1     | GND      | 地       | GND
 *   2     | RST      | 复位     | D15
 *   3     | SCL      | SPI 时钟 | D2
 *   4     | D/C      | 数据/命令| D4
 *   5     | CS       | 片选     | D5
 *   6     | SDA      | MOSI     | D18
 *   7     | SDO      | MISO     | D19(不接)
 *   8     | VCC      | 3.3V     | 3.3V
 *   9     | LEDA     | 背光+    | D21
 *   10    | LEDK     | 背光-    | D22
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "zimo.h"

TFT_eSPI tft;

const int BACKLIGHT_NEG = 22;

// ==================== 中文字模工具 ====================
static const int GW = 20, GH = 20, BPC = 3;
static inline uint8_t rd8(const uint8_t *p) { return *p; }

static void drawCN(TFT_eSPI &t, int x, int y, uint16_t c, int idx) {
  const uint8_t *bmp = text[idx];
  for (int col = 0; col < GW; ++col) {
    uint8_t b0 = rd8(bmp + col * BPC + 0);
    uint8_t b1 = rd8(bmp + col * BPC + 1);
    uint8_t b2 = rd8(bmp + col * BPC + 2);
    for (int r = 0; r < 8; ++r) { if (b0 & (1 << r)) t.drawPixel(x + col, y + r, c); }
    for (int r = 0; r < 8; ++r) { if (b1 & (1 << r)) t.drawPixel(x + col, y + 8 + r, c); }
    for (int r = 0; r < 4; ++r) { if (b2 & (1 << r)) t.drawPixel(x + col, y + 16 + r, c); }
  }
}

// ==================== 页面函数 ====================

void page_main() {
  tft.fillScreen(TFT_WHITE);

  // 大标题
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("ESP32 LCD Demo", 10, 5, 4);

  // 中文：杭州（bitmap 字模）
  drawCN(tft, 10, 45, TFT_BLACK, 45);  // 杭
  drawCN(tft, 34, 45, TFT_BLACK, 46);  // 州
  tft.setTextColor(TFT_DARKGREY, TFT_WHITE);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("Hangzhou", 60, 50, 2);

  // 模拟日期
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("2026-07-01", 10, 85, 2);

  // 中文：星期四
  drawCN(tft, 10, 115, TFT_BLACK, 36);  // 星
  drawCN(tft, 34, 115, TFT_BLACK, 37);  // 期
  drawCN(tft, 58, 115, TFT_BLACK, 41);  // 四
  tft.setTextColor(TFT_DARKGREY, TFT_WHITE);
  tft.drawString("Thursday", 84, 120, 2);

  // 分隔线
  tft.drawFastHLine(10, 150, 300, TFT_LIGHTGREY);

  // 彩色字体展示
  tft.setTextColor(TFT_RED, TFT_WHITE);
  tft.drawString("RED", 10, 165, 2);
  tft.setTextColor(TFT_GREEN, TFT_WHITE);
  tft.drawString("GREEN", 70, 165, 2);
  tft.setTextColor(TFT_BLUE, TFT_WHITE);
  tft.drawString("BLUE", 140, 165, 2);
  tft.setTextColor(TFT_MAGENTA, TFT_WHITE);
  tft.drawString("MAGENTA", 210, 165, 2);

  // 底部信息
  tft.setTextColor(TFT_DARKGREY, TFT_WHITE);
  tft.drawString("ST7789 240x320  BGR mode", 10, 200, 1);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("02-lcd Demo  v1.0", 10, 220, 1);
}

void page_color_bars() {
  tft.fillScreen(TFT_WHITE);

  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Color Palette", 160, 4, 2);

  const uint16_t colors[] = {
    TFT_BLACK, TFT_MAROON, TFT_GREEN, TFT_OLIVE,
    TFT_NAVY,  TFT_PURPLE, TFT_CYAN, TFT_SILVER,
    TFT_DARKGREY, TFT_RED, TFT_GREEN, TFT_YELLOW,
    TFT_BLUE, TFT_MAGENTA, TFT_CYAN, TFT_WHITE,
  };
  const char *names[] = {
    "BLK","MAR","GRN","OLV",
    "NAV","PUR","CYN","SIL",
    "GRY","RED","GRN","YEL",
    "BLU","MAG","CYN","WHT",
  };

  int barW = 320 / 16;
  for (int i = 0; i < 16; i++) {
    int x = i * barW;
    tft.fillRect(x, 20, barW, 180, colors[i]);
    uint16_t fg = (i < 8) ? TFT_WHITE : TFT_BLACK;
    tft.setTextColor(fg, colors[i]);
    tft.setTextDatum(BC_DATUM);
    tft.drawString(names[i], x + barW / 2, 200, 1);
  }

  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_DARKGREY, TFT_WHITE);
  tft.drawString("16 colors  ST7789 BGR", 10, 220, 1);
}

void page_shapes() {
  tft.fillScreen(TFT_WHITE);

  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Shapes", 160, 4, 2);

  tft.setTextDatum(TL_DATUM);

  // 圆
  tft.fillCircle(50, 80, 30, TFT_RED);
  tft.drawCircle(50, 80, 30, TFT_BLACK);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("Circle", 15, 120, 1);

  // 矩形
  tft.fillRoundRect(120, 50, 80, 60, 8, TFT_BLUE);
  tft.drawRoundRect(120, 50, 80, 60, 8, TFT_BLACK);
  tft.drawString("RoundRect", 125, 120, 1);

  // 三角形
  tft.fillTriangle(240, 50, 220, 110, 290, 110, TFT_GREEN);
  tft.drawTriangle(240, 50, 220, 110, 290, 110, TFT_BLACK);
  tft.drawString("Triangle", 225, 120, 1);

  // 线
  tft.drawLine(10, 160, 310, 160, TFT_BLACK);
  tft.drawFastHLine(10, 170, 300, TFT_RED);
  tft.drawFastVLine(10, 160, 30, TFT_BLUE);
  tft.drawFastVLine(310, 160, 30, TFT_GREEN);

  // 点阵
  for (int i = 0; i < 100; i++) {
    int px = 30 + (i % 10) * 28;
    int py = 190 + (i / 10) * 5;
    tft.drawPixel(px, py, TFT_BLACK);
  }
  tft.drawString("drawPixel dot grid", 160, 225, 1);
}

void page_fonts() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TL_DATUM);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("TFT_eSPI Built-in Fonts", 10, 4, 2);

  int y = 28;
  tft.drawString("Font 1: The quick brown fox", 10, y, 1);  y += 14;
  tft.drawString("Font 2: ABC abc 123", 10, y, 2);          y += 20;
  tft.drawString("Font 4: 240x320 Demo", 10, y, 4);         y += 30;
  tft.drawString("Font 6: Hello!", 10, y, 6);               y += 42;
  tft.drawString("Font 7: BIG TEXT", 10, y, 7);             y += 46;

  // 中文 bitmap 行
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Chinese bitmap fonts:", 10, y, 2);  y += 22;
  drawCN(tft, 10, y, TFT_CYAN, 15);   // 中
  drawCN(tft, 34, y, TFT_CYAN, 36);   // 星
  drawCN(tft, 58, y, TFT_CYAN, 37);   // 期
  drawCN(tft, 82, y, TFT_CYAN, 38);   // 一
  drawCN(tft, 106, y, TFT_CYAN, 39);  // 二
  drawCN(tft, 130, y, TFT_CYAN, 40);  // 三
  drawCN(tft, 154, y, TFT_CYAN, 41);  // 四
  drawCN(tft, 178, y, TFT_CYAN, 42);  // 五
  drawCN(tft, 202, y, TFT_CYAN, 43);  // 六
  drawCN(tft, 226, y, TFT_CYAN, 44);  // 日
}

// ==================== setup / loop ====================

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(BACKLIGHT_NEG, OUTPUT);
  digitalWrite(BACKLIGHT_NEG, LOW);

  tft.init();
  tft.setRotation(0);  // 竖屏 240x320

  // 关键初始化：RGB565 色彩模式 + BGR 顺序（ST7789 通常为 BGR）
  tft.writecommand(0x3A);
  tft.writedata(0x55);
  tft.invertDisplay(false);

  Serial.println("02-lcd Demo started");
}

void loop() {
  page_main();        delay(3000);
  page_color_bars();  delay(3000);
  page_shapes();      delay(3000);
  page_fonts();       delay(3000);
}
