/**
 * ESP32 Blink
 * 经典的 LED 闪烁示例，验证开发板正常工作
 *
 * 默认 GPIO2 是大多数 ESP32 开发板上的板载 LED
 */

#include <Arduino.h>

// LED 引脚 - ESP32 Dev Module 通常板载 LED 在 GPIO2
// 如果你的板子不同，请修改这个值
#define LED_BUILTIN 2

void setup()
{
  // 初始化串口，方便调试
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nESP32 Blink 启动!");

  // 设置 LED 引脚为输出模式
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH); // 点亮 LED
  Serial.println("LED ON");
  delay(1000);                     // 等待 1 秒

  digitalWrite(LED_BUILTIN, LOW);  // 熄灭 LED
  Serial.println("LED OFF");
  delay(1000);                     // 等待 1 秒
}
