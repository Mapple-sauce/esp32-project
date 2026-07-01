# 00-blink — LED 闪烁

第一个 ESP32 项目：点亮板载 LED。

## 功能

- GPIO2 板载 LED 交替闪烁（500ms 间隔）
- 串口输出 `ms: LED ON / LED OFF`

## 硬件

| 项目 | 说明 |
|------|------|
| 板载 LED | GPIO2 |
| 波特率 | 115200 |

## 接线

只需将 CP210x 连接到 ESP32-WROOM：

| CP210x | ESP32 |
|--------|-------|
| TXD | RXD0 (GPIO3) |
| RXD | TXD0 (GPIO1) |
| GND | GND |
| 3.3V | 3.3V |

## 配置

```ini
[env:featheresp32]
platform = espressif32
board = featheresp32
framework = arduino
monitor_speed = 115200
```
