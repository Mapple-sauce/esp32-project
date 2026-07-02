# 04-clock — ESP32 桌面时钟摆件

基于 ESP32 + TFT 彩屏 + 按键模块的多功能桌面时钟，带闹钟、倒计时、天气、B站粉丝数、温湿度显示。

## 📂 项目结构

```
04-clock/
├── platformio.ini              # 构建配置 + 库依赖
├── include/
│   └── User_Setup.h            # TFT 引脚配置 (ST7789/ST7735)
├── src/
│   ├── main.cpp                # 入口：初始化 + 事件调度
│   ├── config.h                # 集中配置（WiFi/API Key/引脚）
│   ├── display/
│   │   ├── DisplayManager.h    # TFT 驱动封装
│   │   └── DisplayManager.cpp  # 背光、动效、绘图基础
│   ├── network/
│   │   ├── WifiManager         # WiFi 连接 + 自动重连
│   │   ├── BilibiliClient      # B站 粉丝数/播放量
│   │   └── WeatherClient       # 和风天气 API
│   ├── sensors/
│   │   └── DHT22Sensor         # DHT22 温湿度
│   ├── time/
│   │   ├── TimeManager         # NTP 校时
│   │   └── AlarmManager        # [NEW] 闹钟数据管理 + NVS 存储
│   ├── audio/
│   │   └── Buzzer              # [NEW] 蜂鸣器 PWM 驱动
│   └── ui/
│       ├── ScreenManager       # 页面调度 + 轮播
│       ├── ButtonManager       # [NEW] 按键驱动/去抖/长按识别
│       ├── ClockPage           # 时钟页
│       ├── WeatherPage         # 天气页
│       ├── BilibiliPage        # B站数据页
│       ├── IndoorPage          # 室内温湿度页
│       ├── AlarmPage           # [NEW] 闹钟设置页
│       └── TimerPage           # [NEW] 倒计时页
└── data/
    └── fonts/                  # 中文字体 .vlw 文件
```

## 🎯 功能清单

| # | 功能 | 状态 | 说明 |
|---|------|------|------|
| 1 | ⏰ NTP 时钟 | ✅ 基础 | 大号数字显示时间 + 日期 + 星期 |
| 2 | 🌤 天气 | ✅ 基础 | 和风天气 API，室外温度/湿度/风力 |
| 3 | 📺 B站数据 | ✅ 基础 | 粉丝数/视频/播放量 |
| 4 | 🏠 室内温湿度 | ✅ 基础 | DHT22 传感器 |
| 5 | 🔄 自动轮播 | ✅ 基础 | 12秒切页 |
| 6 | 🎨 动效 | ⏳ 待优化 | 页面切换动画、背光呼吸 |
| 7 | 🔘 按键模块 | 🆕 待实现 | 3键或编码器，菜单/+/-
| 8 | ⏰ 闹钟 | 🆕 待实现 | 可设多组闹钟 + 重复日 + NVS存储 |
| 9 | ⏱ 倒计时 | 🆕 待实现 | 可设时长、开始/暂停/重置 |
| 10 | 🔊 蜂鸣器 | 🆕 待实现 | PWM 方波驱动小喇叭 |
| 11 | 📱 WiFiManager 配网 | 🆕 待实现 | 手机连 ESP32 配网 |
| 12 | 🌙 自动亮度 | 🆕 待实现 | 根据时段自动调背光 |

## 🧠 架构设计原则

```
loop() 调度模型（非阻塞）:
┌─────────────────────────────────────────────┐
│  WiFi.maintain()  ← 每 loop 都跑              │
│  DHT22.read()     ← 每 5s                    │
│  NTP.update()     ← 每 1h                    │
│  Weather.fetch()  ← 每 15min                 │
│  Bilibili.fetch() ← 每 10min                 │
│                                              │
│  ButtonManager.scan()  ← 每 50ms             │
│  AlarmManager.check()  ← 每 1s (触发检查)    │
│                                              │
│  ScreenManager.update()   ← 每帧渲染          │
└─────────────────────────────────────────────┘
```

**闹钟触发模型：**
```
AlarmManager.check()
  ↓
  当前时间匹配某闹钟 && 闹钟启用 && !已触发？
  ├─ ✅ → 触发闹钟
  │      ├─ PWM 蜂鸣器响
  │      ├─ 屏幕闪烁（红色警告）
  │      ├─ 弹出通知 "⏰ 闹钟！"
  │      └─ 等待按键 → 停止 / 贪睡 5min
  └─ ❌ → 继续轮询
```

## 📟 页面导航

```
  [时钟页] ←→ [天气页] ←→ [B站页] ←→ [室内页]
     ↑                                             自动轮播 12s
     ↓                                             菜单键手动切换
  [闹钟页] ←→ [倒计时页]
              菜单键进入设置，OK 确认，长按返回
```

## 🔌 硬件接线（含新增部分）

| 组件 | 引脚 | ESP32 GPIO |
|------|------|-----------|
| TFT CS | CS | 5 |
| TFT DC | DC | 17 |
| TFT RST | RST | 16 |
| TFT MOSI | MOSI | 23 |
| TFT SCK | SCK | 18 |
| TFT BL | 背光 | 4 |
| DHT22 | DATA | 15 |
| 按键-菜单 | KEY_MENU | 13 |
| 按键-加 | KEY_PLUS | 14 |
| 按键-减 | KEY_MINUS | 27 |
| 蜂鸣器 | BUZZER | 25 |
| BOOT 键 | 备用(停止闹钟) | 0 |

## 🏗 构建与烧录

```bash
# 编译
cd ~/quant-project/04-clock && pio run

# 烧录
pio run --target upload

# 监视串口
pio device monitor

# 清理
pio run --target clean
```

## 📦 依赖库

```ini
lib_deps =
    bodmer/TFT_eSPI            # TFT 驱动（无官方替代）
    adafruit/DHT sensor library # DHT22（无官方替代）
    adafruit/Adafruit Unified Sensor
    bblanchon/ArduinoJson     # JSON 解析（无官方替代）
```

## 📐 编码规范

- **注释**：中文，代码标识符用英文
- **命名**：类 `UpperCamelCase`，函数 `lowerCamelCase()`，宏 `UPPER_SNAKE_CASE`
- **模块化**：每个模块一对 `.h/.cpp`，功能内聚
- **非阻塞**：`loop()` 中不允许超过 50ms 的 `delay()`，全部用 `millis()` 时间差轮询
- **错误容错**：DHT22 和网络 API 偶发失败正常，必须有重试/超时机制
- **全局配置**：所有可调参数集中在 `config.h`
- **闹钟存储**：使用 `Preferences` (NVS) 存储闹钟配置，断电不丢失
- **按键去抖**：软件去抖 ≥ 50ms，支持短按、长按、双击三种事件
