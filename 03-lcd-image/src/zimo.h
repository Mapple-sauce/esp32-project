// zimo.h（已有）
#pragma once
#include <Arduino.h>
extern const uint8_t text[][60];   // 20x20，一字60字节（逐列式、低位在前）
extern const char* const ZIMO_CHARS[];  // 字模对应的汉字
extern const int ZIMO_COUNT;           // 汉字数量
