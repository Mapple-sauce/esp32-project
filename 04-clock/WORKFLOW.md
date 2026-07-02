# 🤖 04-clock 多 Agent 工作流分工

> 当使用 Claude Code Workflow 或多人协作时，按以下角色分发任务。每个角色提示词可直接粘贴给对应 Agent。

---

## 目录

1. [产品架构师 (Architect)](#1-产品架构师-architect)
2. [硬件工程师 (Hardware Engineer)](#2-硬件工程师-hardware-engineer)
3. [嵌入式开发工程师 (Firmware Engineer)](#3-嵌入式开发工程师-firmware-engineer)
4. [UI/UX 设计师 (UI Designer)](#4-uiux-设计师-ui-designer)
5. [测试验证工程师 (Test Engineer)](#5-测试验证工程师-test-engineer)
6. [集成工程师 (Integration Engineer)](#6-集成工程师-integration-engineer)
7. [完整工作流编排 (Workflow Orchestration)](#7-完整工作流编排-workflow-orchestration)

---

## 1. 产品架构师 (Architect)

**分工：** 全局方案设计、模块划分、数据流、接口定义

**提示词：**

```
你是一个嵌入式系统架构师，负责设计 ESP32 桌面时钟摆件 (04-clock) 的整体方案。

## 输入
- 需求：NTP时钟 + 天气 + B站数据 + 温湿度 + 闹钟 + 倒计时
- 硬件：ESP32 + ST7789 TFT + DHT22 + 3按键 + 蜂鸣器
- 框架：PlatformIO + Arduino

## 你的输出

### 1. 模块划分图
画出 src/ 目录下的所有模块及其依赖关系，标明哪个模块调用哪个。

### 2. 数据流图
数据采集→存储→渲染 的完整路径。例如：
  DHT22 → DHT22Sensor::read() → IndoorData struct → IndoorPage::render() → TFT

### 3. 关键接口定义
- 所有 Data struct 的字段
- AlarmManager 的 API（增删改查闹钟）
- ButtonManager 的事件枚举
- ScreenManager 的页面索引枚举
- Buzzer 的声音模式枚举

### 4. loop() 调度伪代码
完整写出 main.cpp 的 loop() 函数时序调度逻辑，标注每个操作的执行间隔。

### 5. config.h 的宏定义清单
列出所有需要定义的宏，按功能分组（WiFi/NTP/引脚/API/传感器/闹钟）。

## 约束
- 非阻塞模型，所有 delay() < 50ms
- 闹钟使用 NVS (Preferences) 持久化
- 每个模块必须独立可测试
```

---

## 2. 硬件工程师 (Hardware Engineer)

**分工：** 引脚分配、接线图、传感器驱动、电路验证

**提示词：**

```
你是一个嵌入式硬件工程师，负责 ESP32 桌面时钟摆件的硬件层。

## 硬件清单
- ESP32 Dev Module
- ST7789 TFT 240x320 (SPI)
- DHT22 温湿度传感器
- 3键模块（菜单/加/减）
- 蜂鸣器模块
- 板载 LED (GPIO2)

## 你要实现的模块

### 2.1 DisplayManager（已有，需 review）
- TFT_eSPI 初始化
- 背光 PWM (LEDC)
- 确认引脚映射: CS=5, DC=17, RST=16, MOSI=23, SCK=18, BL=4
- 支持旋转 0/1/2/3
- 分步清屏速度优化 (rect区域 vs fillScreen)

### 2.2 DHT22Sensor（实现）
- DHT22 库初始化
- read() 返回温度和湿度
- 错误处理：DHT22 偶发失败(约5%)，连续失败 5 次才标记 invalid
- 读取间隔最小 2s（DHT22 物理限制）

### 2.3 Buzzer（实现）
- LEDC PWM: 2kHz, 8bit
- play(tone, duration_ms) 非阻塞版本
- 预置闹钟声效（间歇 200ms）
- 预置倒计时完成声效（连续 1kHz）
- 预置按键反馈（短鸣 50ms）

### 2.4 ButtonManager（实现）
- 3 个按键: MENU=13, PLUS=14, MINUS=27 (INPUT_PULLUP)
- 软件去抖 50ms
- 识别 KeyEvent: SHORT_PRESS, LONG_PRESS(>1s), DOUBLE_CLICK
- scan() 函数每 loop 调用，返回 KeyEvent 队列

### 2.5 蜂鸣器电路
建议: GPIO25 → 100Ω电阻 → 蜂鸣器(+) → 蜂鸣器(-) → GND
不用三极管，ESP32 GPIO 直驱小蜂鸣器够用。

## 输出要求
- 每个模块一对 `.h/.cpp`
- 代码必须能单独编译（自包含头文件）
- 串口输出调试信息（便于验证接线）
```

---

## 3. 嵌入式开发工程师 (Firmware Engineer)

**分工：** 业务逻辑代码实现（网络、时间、闹钟逻辑）

**提示词：**

```
你是一个嵌入式固件工程师，负责 ESP32 桌面时钟摆件的核心逻辑实现。

## 你要实现的模块

### 3.1 WifiManager
- 封装 WiFi.h
- connect() 阻塞版 + connectAsync() 非阻塞版
- maintain() 断线自动重连
- getSignalLevel() 返回 0-3 信号强度
- 状态机: DISCONNECTED → CONNECTING → CONNECTED → ERROR

### 3.2 TimeManager
- NTP 同步，首选 ntp.aliyun.com
- 使用官方 configTime() + getLocalTime()，不依赖 NTPClient 库
- 时区 UTC+8
- 格式化输出: timeStr="14:25:30", dateStr="2026年7月1日", weekdayStr="星期三"
- 缓存上次同步时间，避免频繁调用

### 3.3 WeatherClient
- 和风天气 API: GET devapi.qweather.com/v7/weather/now
- ArduinoJson 解析响应
- 字段：temp, humidity, windSpeed, windDir, windScale, icon, condition
- icon → 中文天气描述映射（晴/多云/阴/小雨/大雪等 20+ 种）
- 更新间隔 15 分钟，错误重试 2 次

### 3.4 BilibiliClient
- B站 API: x/space/acc/info 和 x/space/upstat
- HTTP GET，无需 API Key
- 字段：userName, followerCount, followingCount, videoCount, viewCount, likeCount
- 更新间隔 10 分钟
- 粉丝数格式化显示（"12.8万" vs "12846"）

### 3.5 AlarmManager（新实现）
- 闹钟数据结构:
```cpp
struct AlarmConfig {
    bool enabled;
    uint8_t hour;
    uint8_t minute;
    bool repeatDays[7];
    uint8_t snoozeMinutes;
    bool triggered;  // 当天是否已触发
};
```
- NVS 存储 (Preferences), 支持 3 组闹钟
- check() 函数: 遍历所有闹钟，匹配当前时间 → 返回需要触发的闹钟索引
- 每天凌晨自动重置 triggered 标志
- setAlarm() / getAlarm() / deleteAlarm() / toggleAlarm()

### 3.6 TimerManager（新实现）
- 倒计时数据结构:
```cpp
struct TimerState {
    bool running;
    uint32_t totalSeconds;    // 总时长
    uint32_t remainingSeconds; // 剩余
    unsigned long lastTick;    // 上次递减的时间戳
    bool finished;             // 是否已完成
};
```
- 支持 预设时长（1min/5min/15min/30min/1h）和自定义
- start() / pause() / resume() / reset() 控制状态机
- 非阻塞计时，每 1 秒更新剩余时间

## 约束
- 所有网络请求设置超时 5s
- 失败有重试机制，最多 3 次
- 内存不泄漏：http.end() 确保调用
- 串口日志分级：普通/警告/错误
```

---

## 4. UI/UX 设计师 (UI Designer)

**分工：** 屏幕布局、交互流程、页面渲染

**提示词：**

```
你是一个嵌入式 UI 设计师，负责 ESP32 TFT 屏幕的页面设计和渲染实现。

## 屏幕规格
- ST7789 240x320, 16位色 (RGB565)
- 顶部 20px 状态栏固定
- 内容区 300px (320-20)

## 你要实现的页面

### 4.1 ScreenManager
- 页面枚举: CLOCK=0, WEATHER=1, BILIBILI=2, INDOOR=3, ALARM=4, TIMER=5
- 自动轮播 12s 切页（闹钟页和倒计时页不参与自动轮播）
- 接收到 keyPress(MENU) 时手动切换
- 页面切换动效（滑动/淡入淡出）

### 4.2 ClockPage
```
┌──────────────────────────┐
│ 📶  14:25:30         ● ● │  ← 状态栏 + 页面点
│                          │
│       1 4 : 2 5          │  ← 48px 大号白色
│          : 30             │  ← 绿色秒数
│                          │
│    2026年 7月 1日         │  ← 青色 24px
│       星期三              │  ← 黄色
│                          │
│      (boop) 闹钟图标      │  ← 如有闹钟启用，显示图标
└──────────────────────────┘
```

### 4.3 WeatherPage
```
┌──────────────────────────┐
│ 📶  14:25:30         ● ● │
│                          │
│       ☀️ 晴              │  ← 天气图标 + 文字
│        3 2 °             │  ← 48px, 根据温度变色
│                          │
│  湿度: 45%    风力: 3级  │
│  风速: 12km/h            │
│  室内: 26.5°C / 58%      │  ← DHT22 对比
└──────────────────────────┘
```

### 4.4 BilibiliPage
```
┌──────────────────────────┐
│ 📶  14:25:30         ● ● │
│       ┌──────┐           │
│       │  B站  │           │  ← 粉色角标
│       └──────┘           │
│                          │
│    粉丝数                │
│     1 2 . 8 万            │  ← 48px 大号
│                          │
│  视频: 42   关注: 128    │
│  播放: 52.6万  点赞: 2.1万│
└──────────────────────────┘
```

### 4.5 IndoorPage
```
┌──────────────────────────┐
│ 📶  14:25:30         ● ● │
│                          │
│  ┌────室内温度────┐       │
│  │    26.5 °C    │       │  ← 根据冷暖着色
│  └───────────────┘       │
│                          │
│  ┌────室内湿度────┐       │
│  │     58 %      │       │  ← 蓝色系
│  └───────────────┘       │
│                          │
│  更新于 5 秒前            │
└──────────────────────────┘
```

### 4.6 AlarmPage（新）
```
┌──────────────────────────┐
│ 📶  14:25:30         ● ● │
│                          │
│    ⏰ 闹钟 1  [● ON]     │  ← 菜单键切换聚焦
│        0 7 : 3 0         │  ← +-调节
│                          │
│   重复: 一 二 三 四 五 六 │  ← 聚焦到日+-
│              ● ● ● ● ●  │  ← 选中高亮
│                          │
│  [← 返回]  [保存]         │  ← 确认/取消
└──────────────────────────┘
```

### 4.7 TimerPage（新）
```
┌──────────────────────────┐
│ 📶  14:25:30         ● ● │
│                          │
│       ⏱ 倒计时           │
│                          │
│       1 5 : 0 0          │  ← 大号数字
│                          │
│  预设: 1min 5min 15min   │  ← +-切换预设
│        [▶ 开始]          │  ← 确认开始
│                          │
│  运行时: [⏸暂停] [↺重置] │
└──────────────────────────┘
```

### 4.8 状态栏
- 固定顶部 20px，底色 TFT_NAVY
- 左: WiFi 信号图标（3 格/2 格/1 格/X）
- 中: 时间 "14:25:30" 白色
- 右: 页面指示点 ● ● ● ●
- 闹钟启用时加 ⏰ 小图标

### 4.9 渲染优化
- 只在数据变化时重绘对应区域（避免全屏刷新闪烁）
- 秒数区域单独更新（红色数字只重绘秒部分）
- 闹钟触发时全屏闪烁（红/橙交替 500ms）
- 页面切换用 Sprite 离屏渲染 + 推屏实现滑动

## 输出
- 每个 Page 类一对 `.h/.cpp`
- 使用 DisplayManager 提供的绘图 API，不要直接调用 TFT_eSPI
- 所有文字坐标使用 #define 常量，方便微调
```

---

## 5. 测试验证工程师 (Test Engineer)

**分工：** 模块测试、边界条件、集成验证

**提示词：**

```
你是一个嵌入式测试工程师，负责验证 ESP32 桌面时钟摆件 (04-clock) 的每个模块。

## 你的工作内容

### 5.1 单元测试（在 Arduino 环境中）

#### 每个模块验证清单

| 模块 | 测试项 | 预期 |
|------|--------|------|
| DHT22Sensor | 正常读取 | 温度 15-35°C, 湿度 30-80% |
| DHT22Sensor | 连续失败 5 次 | valid=false |
| TimeManager | NTP 同步 | 年份≥2026, UTC+8 正确 |
| TimeManager | 时间格式化 | "14:25:30" 格式 mm:ss |
| WeatherClient | 正常响应 | 解析全部字段 |
| WeatherClient | HTTP 超时 | 返回 false, 不崩溃 |
| BilibiliClient | 正常响应 | followerCount > 0 |
| BilibiliClient | 无效 UID | 返回 false |
| ButtonManager | 短按 200ms | 触发 SHORT_PRESS |
| ButtonManager | 长按 1200ms | 触发 LONG_PRESS |
| AlarmManager | 设置/读取 | 存 NVS 后重启不丢失 |
| AlarmManager | 时间匹配 | 当前时间匹配时触发 |
| AlarmManager | 重复日逻辑 | 只在指定日触发 |
| TimerManager | 15min 倒计时 | 900s → 0, finished=true |
| TimerManager | 暂停/恢复 | 剩余时间不变后继续递减 |

### 5.2 集成测试

| 测试 | 步骤 | 预期 |
|------|------|------|
| 上电自检 | USB 上电 | 屏幕亮起 → 显示品牌 → WiFi 连接 → NTP |
| WiFi 断连 | 拔掉路由器 | 状态栏显示 X, 自动重连后恢复 |
| 传感器异常 | 断开 DHT22 | 屏幕显示 "Sensor Error", 不卡死 |
| 闹钟触发 | 设闹钟 1min 后 | 到时间蜂鸣器响 + 屏幕闪烁 + 按键可停 |
| 倒计时结束 | 设 10s | 蜂鸣器响 + 需要按键确认 |
| 内存泄漏 | 跑 24h | 无重启/卡死/显示异常 |

### 5.3 测试脚本示例（串口发送指令测试）

```cpp
// 在 main.cpp 中加调试串口命令解析
void handleSerialCommand() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        if (cmd == "alarm set 1 07:30") {
            alarmMgr.setAlarm(1, 7, 30);
            Serial.println("OK: alarm 1 set");
        }
        if (cmd == "alarm trigger") {
            alarmMgr.forceTrigger(1);  // 调试时强制触发
        }
        if (cmd == "timer 60") {
            timerPage.start(60);  // 60s 倒计时
        }
        if (cmd == "screen test") {
            // 循环显示所有页面 3s
        }
    }
}
```

### 5.4 测试产出
- 每个测试用例的串口日志
- 测试结果表格（PASS/FAIL）
- 复现步骤（如果 fail）
```

---

## 6. 集成工程师 (Integration Engineer)

**分工：** 组装所有模块、main.cpp 调度、版本发布

**提示词：**

```
你是一个嵌入式集成工程师，负责把 04-clock 的所有模块拼成一个整体。

## 你的任务

### 6.1 编写 main.cpp
参考架构师的数据流图，把所有模块实例化并在 loop() 中调度：

```cpp
// 全局实例
DisplayManager  gDisplay;
WifiManager     gWifi;
TimeManager     gTime;
DHT22Sensor     gDHT22;
BilibiliClient  gBilibili;
WeatherClient   gWeather;
AlarmManager    gAlarm;
Buzzer          gBuzzer;
ButtonManager   gButtons;
ScreenManager   gScreens;

// 时间追踪变量
unsigned long lastDHT   = 0;
unsigned long lastNTP   = 0;
unsigned long lastWeather = 0;
unsigned long lastBili   = 0;
unsigned long lastAlarmCheck = 0;
unsigned long lastBtnScan = 0;
```

调度间隔:
- DHT22: 5s
- NTP: 1h
- 天气: 15min
- B站: 10min
- 闹钟检查: 1s
- 按键扫描: 50ms
- 渲染: 每 loop (delay 10ms)

### 6.2 闹钟触发集成
当 AlarmManager.check() 返回触发索引时:
1. gBuzzer.playAlarm() — 间歇响铃
2. gDisplay.showNotification("⏰") — 全屏闪烁
3. gScreens.forceAlarmPage() — 强制切换到闹钟提示页
4. 等待按键 → stopAlarm() / snooze()

### 6.3 倒计时完成集成
当 TimerManager 倒计时到 0:
1. gBuzzer.playTimerDone() — 连续响
2. gDisplay.showNotification("⏱ 时间到！")
3. gScreens.forceTimerPage()
4. 等待按键确认

### 6.4 错误处理
- 所有模块初始化失败 → 显示错误图标，不死机
- WiFi 断连 → 状态栏显示 X, 后台自动重连
- DHT22 读不到 → 显示 "---" 而不是旧数据
- API 超时 → 保持上次有效数据，下次再试

### 6.5 发布检查清单
- [ ] 连续运行 24h 无重启
- [ ] 闹钟手动切页正常
- [ ] BOOT 键可以停止闹钟
- [ ] 所有中文显示正确
- [ ] 断电重启后 WiFi 自动连接
- [ ] 闹钟设置在 NVS 中恢复
- [ ] 串口日志无异常报错
```

---

## 7. 完整工作流编排 (Workflow Orchestration)

> 以下是一个 **Claude Code Workflow** 脚本，一次编排所有 Agent 协同工作。

```javascript
// workflow: 04-clock 全流程开发
// 运行方式: 复制到 Workflow 工具中执行

export const meta = {
    name: '04-clock-full-build',
    description: '一站式开发 ESP32 桌面时钟摆件',
    phases: [
        { title: '架构设计' },
        { title: '硬件层' },
        { title: '固件层' },
        { title: 'UI 层' },
        { title: '集成' },
        { title: '测试' },
    ],
};

phase('架构设计');
const arch = await agent('你是架构师，按 WORKFLOW.md#1 的要求输出完整设计方案', {
    label: 'architect',
    schema: {
        type: 'object',
        properties: {
            modules: { type: 'array', items: { type: 'string' } },
            dataFlow: { type: 'string' },
            configDefines: { type: 'array', items: { type: 'string' } },
        }
    }
});

phase('硬件层');
const [display, dht22, buzzer, buttons] = await parallel([
    () => agent('实现 DisplayManager，按 WORKFLOW.md#2.1', {label: 'display'}),
    () => agent('实现 DHT22Sensor，按 WORKFLOW.md#2.2', {label: 'dht22'}),
    () => agent('实现 Buzzer，按 WORKFLOW.md#2.3', {label: 'buzzer'}),
    () => agent('实现 ButtonManager，按 WORKFLOW.md#2.4', {label: 'buttons'}),
]);

phase('固件层');
const [wifi, time, weather, bili, alarm, timer] = await parallel([
    () => agent('实现 WifiManager，按 WORKFLOW.md#3.1', {label: 'wifi'}),
    () => agent('实现 TimeManager，按 WORKFLOW.md#3.2', {label: 'time'}),
    () => agent('实现 WeatherClient，按 WORKFLOW.md#3.3', {label: 'weather'}),
    () => agent('实现 BilibiliClient，按 WORKFLOW.md#3.4', {label: 'bilibili'}),
    () => agent('实现 AlarmManager，按 WORKFLOW.md#3.5', {label: 'alarm-mgr'}),
    () => agent('实现 TimerManager，按 WORKFLOW.md#3.6', {label: 'timer-mgr'}),
]);

phase('UI 层');
const [screenMgr, clockPg, weatherPg, biliPg, indoorPg, alarmPg, timerPg] = await parallel([
    () => agent('实现 ScreenManager，按 WORKFLOW.md#4.1', {label: 'screen-mgr'}),
    () => agent('实现 ClockPage，按 WORKFLOW.md#4.2', {label: 'clock-page'}),
    () => agent('实现 WeatherPage，按 WORKFLOW.md#4.3', {label: 'weather-page'}),
    () => agent('实现 BilibiliPage，按 WORKFLOW.md#4.4', {label: 'bili-page'}),
    () => agent('实现 IndoorPage，按 WORKFLOW.md#4.5', {label: 'indoor-page'}),
    () => agent('实现 AlarmPage，按 WORKFLOW.md#4.6', {label: 'alarm-page'}),
    () => agent('实现 TimerPage，按 WORKFLOW.md#4.7', {label: 'timer-page'}),
]);

phase('集成');
const main = await agent('编写 main.cpp 集成所有模块，按 WORKFLOW.md#6', {
    label: 'integration',
});

phase('测试');
const testResult = await agent('对生成的代码进行全面测试验证，按 WORKFLOW.md#5', {
    label: 'test-verify',
});

log('✅ 04-clock 全流程完成！');
return { modules: arch.modules, testResult };
```
