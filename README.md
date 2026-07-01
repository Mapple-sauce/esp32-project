# ESP32 学习项目

从零开始学习 ESP32 开发，基于 VSCode + PlatformIO + WSL 环境。

## 项目列表

| 编号 | 项目 | 功能 | 状态 |
|------|------|------|------|
| [00-blink](00-blink/) | LED 闪烁 | GPIO2 板载 LED 闪烁 | ✅ 完成 |
| [01-wifi](01-wifi/) | WiFi + NTP + B站API | 连接WiFi→获取北京时间→获取B站粉丝数 | ✅ 完成 |
| [02-lcd](02-lcd/) | ST7789 TFT 显示 | 白底中英文、图形、颜色条轮播 | ✅ 完成 |

## 开发环境

- **MCU:** ESP32-WROOM (ESP32-D0WD-V3 rev v3.1)
- **调试器:** CP210x USB to UART (COM3 @ 115200)
- **IDE:** VSCode + PlatformIO
- **系统:** WSL (Ubuntu) 编译 + Windows 烧录
- **烧录工具:** esptool 5.3.1

## 快速开始

```bash
# 1. 编译
cd ~/quant-project/01-wifi
export PATH="$HOME/.local/bin:$PATH"
pio run

# 2. 复制固件到 Windows 盘
cp .pio/build/featheresp32/*.bin /mnt/d/

# 3. 烧录（三个文件必须指定正确地址）
powershell.exe -Command "cd C:\; & 'C:\Users\Mapple\AppData\Roaming\Python\Python312\Scripts\esptool.exe' --port COM3 --baud 115200 --before default-reset --after hard-reset write-flash 0x1000 D:\bootloader.bin 0x8000 D:\partitions.bin 0x10000 D:\firmware.bin"
```

> **注意:** ESP32 Flash 布局 — bootloader `0x1000` / 分区表 `0x8000` / 应用程序 `0x10000`

## 硬件接线

### CP210x → ESP32-WROOM

| CP210x | ESP32 |
|--------|-------|
| TXD | RXD0 (GPIO3) |
| RXD | TXD0 (GPIO1) |
| GND | GND |
| 3.3V | 3.3V |

更多参考：[00-reference/README.md](00-reference/README.md)
