# 02-lcd — ST7789 2.4寸 TFT 屏幕显示 Demo

点亮 2.4寸 240x320 SPI 屏幕，驱动芯片 **ST7789**，使用 **TFT_eSPI** 库。

## 功能

- 🎨 **主屏** — 白底黑字，标题、中文星期、彩色文字
- 🎯 **调色板** — 16 色条 + 颜色名称
- 🔵 **图形** — 圆、圆角矩形、三角形、线、点阵
- 🔤 **文字展示** — 各字号英文 + 中文 bitmap 字模
- 🔄 自动轮播，每页停留 3 秒

## 硬件接线

### 屏幕引脚定义

2.4寸 TFT 屏幕（10-Pin SPI 接口）：

```
┌────────────────────────────────┐
│  1  2  3  4  5  6  7  8  9 10 │
│ GND VCC SCL MOSI RST  DC CS BL SDO │
└────────────────────────────────┘
        排针（从右往左）
```

### 接线表

| 屏幕引脚# | 屏幕引脚 | 功能 | → ESP32 | 备注 |
|:---------:|:--------:|:----:|:-------:|:----:|
| **1** | GND | 电源地 | **GND** | |
| **2** | RST | 复位 | **D15** | |
| **3** | SCL | SPI 时钟 | **D2** | |
| **4** | D/C | 数据/命令选择 | **D4** | |
| **5** | CS | SPI 片选 | **D5** | |
| **6** | SDA | SPI 数据 (MOSI) | **D18** | |
| **7** | SDO | SPI 数据回传 (MISO) | **D19** | 可不接 |
| **8** | VCC | 电源 3.3V | **3.3V** | |
| **9** | LEDA | 背光正极 | **D21** | |
| **10** | LEDK | 背光负极 | **D22** | 拉 Low 点亮 |

### 关键接线说明

| 注意点 | 说明 |
|-------|------|
| **背光** | LEDA(D21) + LEDK(D22) 双控，D22 需 `digitalWrite(LOW)` 点亮 |
| **MISO** | SDO 可不接，TFT 是单向通信 |
| **VCC** | 必须接 3.3V，**不可接 5V**，会烧屏 |

## PlatformIO 配置

```ini
[env:featheresp32]
platform = espressif32
board = featheresp32
framework = arduino
monitor_speed = 115200

build_flags =
  -D USER_SETUP_LOADED=1
  -D ST7789_DRIVER
  -D TFT_WIDTH=240
  -D TFT_HEIGHT=320
  -D TFT_MOSI=18
  -D TFT_SCLK=2
  -D TFT_CS=5
  -D TFT_DC=4
  -D TFT_RST=15
  -D TFT_BL=21
  -D TFT_MISO=19
  -D SPI_FREQUENCY=27000000
  -D TFT_RGB_ORDER=TFT_BGR          ; ← 关键：ST7789 用 BGR
  -D TOUCH_CS=-1

lib_deps = bodmer/TFT_eSPI @ ^2.5.0
```

## 颜色配置说明

这块屏幕花了些时间才搞定颜色，以下是关键点：

### TFT_RGB_ORDER 选择

ST7789 驱动芯片的 RGB 顺序因批次而异。大部分 ST7789 是 **BGR** 模式：

```
TFT_RGB_ORDER=TFT_RGB    → 红蓝互换（RED=蓝, BLUE=红）❌
TFT_RGB_ORDER=TFT_BGR    → 颜色正确 ✅
```

判断方法：烧录纯色测试代码，如果 `TFT_RED` 显示蓝色、`TFT_BLUE` 显示红色，就是 BGR。

### 初始化时序

```cpp
tft.init();
tft.setRotation(0);

// RGB565 色彩模式（16-bit）
tft.writecommand(0x3A);
tft.writedata(0x55);
tft.invertDisplay(false);
```

## 中文显示

项目使用 bitmap 字模（20x20 像素）显示中文，字模数据在 `zimo.h` / `zimo.cpp` 中：

| 文件 | 说明 |
|------|------|
| `src/zimo.h` | 字模头文件，47 个汉字（天气+星期+杭州） |
| `src/zimo.cpp` | 字模点阵数据，`text[][]` 数组 |

如需扩展中文字库，使用 PCtoLCD2002 取模软件：**20x20 像素、逐列式、低位在前、阴码**。

当前支持字模：晴/多云/阴/雨/雪/星/期/一~日/杭/州 等 47 字。

## 屏幕效果

```text
┌─────────────────────────┐
│ ESP32 LCD Demo          │  ← Font 4
│                         │
│ 杭州 Hangzhou           │  ← 中文字模(20x20) + Font 2
│ 2026-07-01              │  ← Font 2
│ 星期四 Thursday         │  ← 中文字模 + Font 2
│ ─────────────────────── │
│ RED GREEN BLUE MAGENTA  │  ← 彩色文字
│                         │
│ ST7789 240x320 BGR mode │  ← Font 1
│ 02-lcd Demo  v1.0       │  ← Font 1
└─────────────────────────┘
```

## 依赖库

- `TFT_eSPI` v2.5.43 — TFT 驱动库（PlatformIO 自动安装）
- `zimo.h` / `zimo.cpp` — 内置中文字模（本地源码）

## 内存占用

- RAM: 6.6% (21.8 KB)
- Flash: 22.0% (288 KB)
