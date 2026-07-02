# 🎯 04-clock ESP32 桌面时钟摆件 — 项目提示词

> 把这个文档发给 AI，它就知道整个项目要做什么、怎么做、有什么规矩。

---

## 一、项目一句话

基于 ESP32 + TFT 彩屏 + 按键模块做一个桌面摆件，显示时间/天气/B站数据/温湿度，带闹钟和倒计时功能。

## 二、硬件清单

| 硬件 | 数量 | 说明 |
|------|------|------|
| ESP32 Dev Module | 1 | 主控 |
| TFT 彩屏 ST7789 240x320 | 1 | SPI 接口显示 |
| DHT22 | 1 | 温湿度传感器 |
| 按键模块 3键 | 1 | 菜单/加/减 |
| 蜂鸣器模块 | 1 | 闹钟发声 |
| USB 线 | 1 | 供电+烧录 |

## 三、引脚定义

```
TFT_CS=5   TFT_DC=17  TFT_RST=16
TFT_MOSI=23 TFT_SCLK=18 TFT_BL=4
DHT22_DAT=15
BTN_MENU=13 BTN_PLUS=14 BTN_MINUS=27
BUZZER=25
BOOT=0 (保留)
```

## 四、软件架构

### 核心模块

| 模块 | 文件 | 职责 |
|------|------|------|
| main.cpp | 入口 | 初始化 + loop 调度 |
| config.h | 配置头文件 | 所有可调参数集中定义 |
| DisplayManager | display/ | TFT 封装、背光 PWM、转场动效 |
| WifiManager | network/ | WiFi 连接 + 自动重连 |
| BilibiliClient | network/ | B站 API 获取粉丝数/播放量 |
| WeatherClient | network/ | 和风天气 API 获取天气数据 |
| DHT22Sensor | sensors/ | 读取室内温湿度 |
| TimeManager | time/ | NTP 校时 + 时间格式化 |
| AlarmManager | time/ | 闹钟数据管理 + NVS 持久化 |
| Buzzer | audio/ | 蜂鸣器 PWM 方波驱动 |
| ScreenManager | ui/ | 页面调度 + 自动轮播 |
| ButtonManager | ui/ | 按键扫描 + 去抖 + 事件识别 |
| ClockPage | ui/ | 时钟显示页面 |
| WeatherPage | ui/ | 天气显示页面 |
| BilibiliPage | ui/ | B站数据页面 |
| IndoorPage | ui/ | 室内温湿度页面 |
| AlarmPage | ui/ | 闹钟设置页面 |
| TimerPage | ui/ | 倒计时页面 |

### 调度逻辑（main.cpp loop）

```cpp
void loop() {
    // 1. 网络维护（每 loop）
    wifi.maintain();

    // 2. 传感器读取（每 5s）
    // 3. NTP 同步（每 1h）
    // 4. 天气获取（每 15min）
    // 5. B站获取（每 10min）

    // 6. 按键扫描（每 50ms）
    // 7. 闹钟检查（每 1s）

    // 8. 屏幕渲染（每帧）
    screenManager.update();

    delay(10);
}
```

### 闹钟触发逻辑

```
loop中 AlarmManager.check():
  遍历所有闹钟配置
  if 闹钟启用 && 时:分匹配 && !已触发
    → buzzer.play()   // PWM 间歇发声
    → 屏幕闪烁警告
    → 显示通知 "⏰ 闹钟！"
    → 阻塞等待按键: [停止] [贪睡5min]

倒计时 TimerManager:
  if 倒计时运行 && 时间到
    → buzzer.play()   // 不同音调
    → 屏幕闪烁
    → 等待按键确认
```

## 五、按键交互协议

```cpp
enum KeyEvent {
    KEY_NONE,         // 无事件
    KEY_SHORT_PRESS,  // 短按 < 500ms → 确认/切换
    KEY_LONG_PRESS,   // 长按 ≥ 1000ms → 返回/特殊功能
    KEY_DOUBLE_CLICK  // 双击 → 快捷操作
};

// 页面中的按键含义:
// 普通页面: MENU=切换到闹钟页
// 闹钟页:   MENU=切换聚焦项  PLUS/MINUS=调节值  OK=保存
// 倒计时页: MENU=切换聚焦  PLUS/MINUS=设值  OK=开始/暂停
// 闹钟响时: MENU=停止  BOOT=贪睡
```

## 六、UI 页面布局

### 闹钟设置页
```
┌──────────────────┐
│ ⏰ 闹钟设置       │
│                   │
│ 🕐 07:30  [ON]   │  ← +-调时间, 菜单切换
│ 重复: 一二三四五六 │  ← 菜单切换到日, +-开关
│ [保存]  [取消]    │  ← OK确认/取消
└──────────────────┘
```

### 倒计时页
```
┌──────────────────┐
│ ⏱ 倒计时         │
│                   │
│     15:00        │  ← 大号数字
│                   │
│ [▶开始] [⏸暂停]  │  ← 按键操控
│ [↺重置]          │
└──────────────────┘
```

## 七、闹钟数据存储（NVS Preferences）

```cpp
struct AlarmConfig {
    bool enabled;           // 启用/禁用
    uint8_t hour;           // 时 0-23
    uint8_t minute;         // 分 0-59
    bool repeat[7];         // 每周重复日
    uint8_t snoozeMinutes;  // 贪睡分钟数
};

Preferences pref;
pref.begin("alarm", false);
pref.putBytes("alarm1", &cfg, sizeof(AlarmConfig));
// 支持最多 3 组闹钟
```

## 八、蜂鸣器控制

```cpp
// 闹钟响铃模式: 间歇 200ms on / 200ms off
// 倒计时结束: 连续 500ms 音
// 按键反馈: 短 50ms 滴

// 使用 LEDC PWM
ledcSetup(1, 2000, 8);   // 2kHz 方波
ledcAttachPin(BUZZER_PIN, 1);
ledcWrite(1, 128);        // 50% 占空比 = 发声
ledcWrite(1, 0);          // 停止
```

## 九、实现步骤（按顺序）

| Phase | 内容 | 交付物 |
|-------|------|--------|
| 1 | 项目骨架 + 显示屏点亮 | DisplayManager, WifiManager, platformio.ini |
| 2 | 时钟功能 | TimeManager, ClockPage, NTP |
| 3 | 温湿度传感器 | DHT22Sensor, IndoorPage |
| 4 | 天气 + B站 API | WeatherClient, BilibiliClient, 对应页面 |
| 5 | **按键模块** | ButtonManager, 3键+编码器, 去抖, 长按 |
| 6 | **闹钟功能** | AlarmManager, AlarmPage, NVS 存储 |
| 7 | **倒计时** | TimerPage, 倒计时逻辑 |
| 8 | **蜂鸣器** | Buzzer 驱动, 闹钟/倒计时声效联动 |
| 9 | 打磨完善 | 背光控制、动效、状态栏、WiFiManager |

## 十、硬性规范

1. **🚫 禁止** 使用超过 50ms 的 `delay()` — 全部用 `millis()` 非阻塞定时
2. **✅ 优先** 使用 ESP32 官方内置库（`WiFi.h`, `HTTPClient.h`, `time.h`, `Preferences.h`）
3. **📌 全局配置** 集中在 `config.h`，不允许在 cpp 里硬编码
4. **🛡 错误容错** DHT22 读取失败、API 超时等必须处理，不能死机
5. **💾 持久化** 闹钟配置用 `Preferences` (NVS)，不能每次重启丢失
6. **🧩 模块化** 每个模块一对 `.h/.cpp`，头文件自包含，不循环引用
