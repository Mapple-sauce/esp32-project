/* LED 500ms 间隔闪烁 + 串口输出 */

#include <Arduino.h>

const int LED_PIN = 2;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Booting...");
  Serial.println("LED blink @500ms");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.print(millis()); Serial.println(" ms: LED ON");
  delay(500);

  digitalWrite(LED_PIN, LOW);
  Serial.print(millis()); Serial.println(" ms: LED OFF");
  delay(500);
}
