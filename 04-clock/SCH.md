# 04-clock ESP32 桌面摆件 — 硬件原理图

> 以下 SVG 是完整的系统原理图，可直接用浏览器打开或导入立创EDA。

---

## 一、系统框图

<svg viewBox="0 0 1100 750" xmlns="http://www.w3.org/2000/svg" style="background:#1a1a2e;font-family:'Courier New',monospace;">
  <!-- 标题 -->
  <text x="550" y="38" text-anchor="middle" fill="#00ff88" font-size="20" font-weight="bold">ESP32 桌面时钟摆件 — 系统原理框图</text>
  <line x1="100" y1="48" x2="1000" y2="48" stroke="#00ff88" stroke-width="1" opacity="0.5"/>

  <!-- 主控 ESP32 -->
  <rect x="380" y="180" width="280" height="160" rx="12" fill="#16213e" stroke="#0f3460" stroke-width="3"/>
  <rect x="380" y="180" width="280" height="32" rx="12" fill="#0f3460"/>
  <rect x="380" y="200" width="280" height="12" fill="#0f3460"/>
  <text x="520" y="201" text-anchor="middle" fill="#00ff88" font-size="14" font-weight="bold">ESP32 Dev Module</text>
  <text x="520" y="228" text-anchor="middle" fill="#8899aa" font-size="11">ESP32-WROOM-32  /  Xtensa LX6 240MHz</text>
  <text x="520" y="248" text-anchor="middle" fill="#8899aa" font-size="11">Flash: 4MB  /  SRAM: 520KB</text>
  <text x="520" y="268" text-anchor="middle" fill="#8899aa" font-size="11">WiFi 802.11 b/g/n  +  Bluetooth 4.2</text>
  <text x="520" y="288" text-anchor="middle" fill="#8899aa" font-size="11">3.3V  /  500mA max</text>
  <text x="520" y="315" text-anchor="middle" fill="#ffcc00" font-size="10">┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐</text>
  <text x="520" y="330" text-anchor="middle" fill="#ffcc00" font-size="10">│ SPI  │ │  UART │ │  I/O  │ │ ADC  │</text>
  <text x="520" y="345" text-anchor="middle" fill="#ffcc00" font-size="10">└──────┘ └──────┘ └──────┘ └──────┘</text>

  <!-- TFT 屏幕 - 左侧 -->
  <rect x="60" y="380" width="220" height="140" rx="8" fill="#1a1a3e" stroke="#e94560" stroke-width="2"/>
  <rect x="60" y="380" width="220" height="30" rx="8" fill="#e94560"/>
  <rect x="60" y="400" width="220" height="10" fill="#e94560"/>
  <text x="170" y="400" text-anchor="middle" fill="#fff" font-size="12" font-weight="bold">TFT 彩屏 (ST7789)</text>
  <text x="170" y="425" text-anchor="middle" fill="#ccddee" font-size="11">240 × 320  /  16-bit RGB565</text>
  <text x="170" y="443" text-anchor="middle" fill="#ccddee" font-size="11">SPI 模式 3  /  40MHz</text>
  <text x="170" y="461" text-anchor="middle" fill="#ccddee" font-size="11">CS / DC / RST / MOSI / SCK</text>
  <text x="170" y="479" text-anchor="middle" fill="#ccddee" font-size="11">背光: PWM 控制 (LEDC CH0)</text>
  <text x="170" y="497" text-anchor="middle" fill="#ff8888" font-size="10">3.3V  /  80mA</text>

  <!-- 箭头 TFT → ESP32 -->
  <line x1="170" y1="520" x2="170" y2="560" stroke="#e94560" stroke-width="1.5" marker-end="url(#arrow-red)"/>
  <line x1="170" y1="560" x2="450" y2="560" stroke="#e94560" stroke-width="1.5"/>
  <line x1="450" y1="560" x2="450" y2="370" stroke="#e94560" stroke-width="1.5" marker-end="url(#arrow-red)"/>
  <text x="310" y="553" text-anchor="middle" fill="#e94560" font-size="10">SPI (CS=5, DC=17, RST=16, MOSI=23, SCK=18)</text>
  <text x="310" y="573" text-anchor="middle" fill="#e94560" font-size="10">BL (背光) = GPIO4</text>

  <!-- DHT22 - 左下方 -->
  <rect x="60" y="600" width="200" height="110" rx="8" fill="#1a2e1a" stroke="#44cc66" stroke-width="2"/>
  <rect x="60" y="600" width="200" height="28" rx="8" fill="#44cc66"/>
  <rect x="60" y="618" width="200" height="10" fill="#44cc66"/>
  <text x="160" y="620" text-anchor="middle" fill="#fff" font-size="12" font-weight="bold">DHT22 温湿度传感器</text>
  <text x="160" y="648" text-anchor="middle" fill="#aaddcc" font-size="11">温度: -40 ~ 80°C  (±0.5°C)</text>
  <text x="160" y="666" text-anchor="middle" fill="#aaddcc" font-size="11">湿度: 0 ~ 100%  (±2% RH)</text>
  <text x="160" y="684" text-anchor="middle" fill="#aaddcc" font-size="11">单总线协议  /  采样 ≥ 2s</text>
  <text x="160" y="702" text-anchor="middle" fill="#88ff88" font-size="10">3.3V  /  1.5mA</text>

  <!-- 箭头 DHT22 → ESP32 -->
  <line x1="160" y1="710" x2="160" y2="740" stroke="#44cc66" stroke-width="1.5" marker-end="url(#arrow-green)"/>
  <line x1="160" y1="740" x2="520" y2="740" stroke="#44cc66" stroke-width="1.5"/>
  <line x1="520" y1="740" x2="520" y2="370" stroke="#44cc66" stroke-width="1.5" marker-end="url(#arrow-green)"/>
  <text x="340" y="733" text-anchor="middle" fill="#44cc66" font-size="10">DATA = GPIO15  (避开 ADC2 WiFi 冲突)</text>

  <!-- 按键模块 - 右侧 -->
  <rect x="790" y="380" width="230" height="140" rx="8" fill="#2e1a1a" stroke="#ff6666" stroke-width="2"/>
  <rect x="790" y="380" width="230" height="30" rx="8" fill="#ff6666"/>
  <rect x="790" y="400" width="230" height="10" fill="#ff6666"/>
  <text x="905" y="400" text-anchor="middle" fill="#fff" font-size="12" font-weight="bold">按键模块 (3键)</text>
  <text x="905" y="430" text-anchor="middle" fill="#ffcccc" font-size="11">MENU   →  GPIO13  (INPUT_PULLUP)</text>
  <text x="905" y="450" text-anchor="middle" fill="#ffcccc" font-size="11">PLUS   →  GPIO14  (INPUT_PULLUP)</text>
  <text x="905" y="470" text-anchor="middle" fill="#ffcccc" font-size="11">MINUS  →  GPIO27  (INPUT_PULLUP)</text>
  <text x="905" y="490" text-anchor="middle" fill="#ffcccc" font-size="11">软件去抖 50ms</text>
  <text x="905" y="510" text-anchor="middle" fill="#ffcccc" font-size="10">短按 / 长按 / 双击 识别</text>

  <!-- 箭头 ESP32 → 按键 -->
  <line x1="650" y1="450" x2="790" y2="450" stroke="#ff6666" stroke-width="1.5" marker-end="url(#arrow-orange)"/>
  <text x="720" y="443" text-anchor="middle" fill="#ff6666" font-size="10">GPIO13,14,27</text>

  <!-- 蜂鸣器 - 右下 -->
  <rect x="790" y="560" width="230" height="110" rx="8" fill="#2e1a2e" stroke="#cc66ff" stroke-width="2"/>
  <rect x="790" y="560" width="230" height="28" rx="8" fill="#cc66ff"/>
  <rect x="790" y="578" width="230" height="10" fill="#cc66ff"/>
  <text x="905" y="580" text-anchor="middle" fill="#fff" font-size="12" font-weight="bold">蜂鸣器模块</text>
  <text x="905" y="610" text-anchor="middle" fill="#ddbbff" font-size="11">GPIO25  →  NPN 三极管驱动</text>
  <text x="905" y="630" text-anchor="middle" fill="#ddbbff" font-size="11">LEDC PWM: 2kHz / 8bit / 50%</text>
  <text x="905" y="650" text-anchor="middle" fill="#ddbbff" font-size="11">闹钟: 间歇 200ms / 200ms</text>
  <text x="905" y="668" text-anchor="middle" fill="#ddbbff" font-size="10">有源蜂鸣器  /  5V  /  30mA</text>

  <!-- 箭头 ESP32 → 蜂鸣器 -->
  <line x1="650" y1="615" x2="790" y2="615" stroke="#cc66ff" stroke-width="1.5" marker-end="url(#arrow-purple)"/>
  <text x="720" y="608" text-anchor="middle" fill="#cc66ff" font-size="10">GPIO25 (PWM)</text>

  <!-- 电源 - 底部居中 -->
  <rect x="380" y="600" width="280" height="90" rx="8" fill="#1a1a1a" stroke="#ffcc00" stroke-width="2"/>
  <rect x="380" y="600" width="280" height="28" rx="8" fill="#ffcc00"/>
  <rect x="380" y="618" width="280" height="10" fill="#ffcc00"/>
  <text x="520" y="620" text-anchor="middle" fill="#000" font-size="12" font-weight="bold">电源系统</text>
  <text x="520" y="650" text-anchor="middle" fill="#ffee88" font-size="11">USB 5V  →  AMS1117-3.3  →  3.3V</text>
  <text x="520" y="668" text-anchor="middle" fill="#ffee88" font-size="11">总功耗: ~350mA  /  1.2W</text>

  <!-- 箭头 电源 → ESP32 -->
  <line x1="520" y1="600" x2="520" y2="370" stroke="#ffcc00" stroke-width="1.5" marker-end="url(#arrow-yellow)"/>
  <text x="540" y="490" text-anchor="start" fill="#ffcc00" font-size="10">3.3V供电</text>

  <!-- 排版说明 -->
  <text x="550" y="720" text-anchor="middle" fill="#666" font-size="10">04-clock · ESP32 Desktop Ornament · 2026-07-02</text>
  <text x="550" y="735" text-anchor="middle" fill="#666" font-size="10">注: 实际 PCB 设计请参考立创EDA封装库</text>

  <!-- 箭头定义 -->
  <defs>
    <marker id="arrow-red" viewBox="0 0 10 10" refX="10" refY="5" markerWidth="6" markerHeight="6" orient="auto">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#e94560"/>
    </marker>
    <marker id="arrow-green" viewBox="0 0 10 10" refX="10" refY="5" markerWidth="6" markerHeight="6" orient="auto">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#44cc66"/>
    </marker>
    <marker id="arrow-orange" viewBox="0 0 10 10" refX="10" refY="5" markerWidth="6" markerHeight="6" orient="auto">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#ff6666"/>
    </marker>
    <marker id="arrow-purple" viewBox="0 0 10 10" refX="10" refY="5" markerWidth="6" markerHeight="6" orient="auto">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#cc66ff"/>
    </marker>
    <marker id="arrow-yellow" viewBox="0 0 10 10" refX="10" refY="5" markerWidth="6" markerHeight="6" orient="auto">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#ffcc00"/>
    </marker>
  </defs>
</svg>

---

## 二、详细接线原理图

<svg viewBox="0 0 1100 800" xmlns="http://www.w3.org/2000/svg" style="background:#f8f9fa;font-family:'Courier New',monospace;">
  <!-- 标题 -->
  <text x="550" y="30" text-anchor="middle" fill="#333" font-size="18" font-weight="bold">引脚级接线原理图</text>
  <line x1="50" y1="40" x2="1050" y2="40" stroke="#ccc" stroke-width="1"/>

  <!-- ESP32 主芯片（简化） -->
  <rect x="380" y="60" width="300" height="200" rx="8" fill="#e8f0fe" stroke="#1a73e8" stroke-width="2"/>
  <text x="530" y="90" text-anchor="middle" fill="#1a73e8" font-size="14" font-weight="bold">ESP32 Dev Module</text>
  <text x="530" y="110" text-anchor="middle" fill="#5f6368" font-size="10">(Top View / 俯视)</text>

  <!-- ESP32 引脚标注（左列引脚） -->
  <text x="350" y="145" text-anchor="end" fill="#e94560" font-size="11">GPIO5  (TFT_CS)    ←</text>
  <line x1="355" y1="142" x2="380" y2="142" stroke="#e94560" stroke-width="1.5"/>

  <text x="350" y="170" text-anchor="end" fill="#e94560" font-size="11">GPIO17 (TFT_DC)    ←</text>
  <line x1="355" y1="167" x2="380" y2="167" stroke="#e94560" stroke-width="1.5"/>

  <text x="350" y="195" text-anchor="end" fill="#e94560" font-size="11">GPIO16 (TFT_RST)   ←</text>
  <line x1="355" y1="192" x2="380" y2="192" stroke="#e94560" stroke-width="1.5"/>

  <text x="350" y="220" text-anchor="end" fill="#e94560" font-size="11">GPIO23 (TFT_MOSI)  ←</text>
  <line x1="355" y1="217" x2="380" y2="217" stroke="#e94560" stroke-width="1.5"/>

  <text x="350" y="245" text-anchor="end" fill="#e94560" font-size="11">GPIO18 (TFT_SCLK)  ←</text>
  <line x1="355" y1="242" x2="380" y2="242" stroke="#e94560" stroke-width="1.5"/>

  <!-- ESP32 右侧引脚 -->
  <text x="685" y="145" text-anchor="start" fill="#44cc66" font-size="11">→ GPIO15 (DHT22_DAT)</text>
  <line x1="680" y1="142" x2="670" y2="142" stroke="#44cc66" stroke-width="1.5"/>

  <text x="685" y="170" text-anchor="start" fill="#ff6666" font-size="11">→ GPIO13 (BTN_MENU)</text>
  <line x1="680" y1="167" x2="670" y2="167" stroke="#ff6666" stroke-width="1.5"/>

  <text x="685" y="195" text-anchor="start" fill="#ff6666" font-size="11">→ GPIO14 (BTN_PLUS)</text>
  <line x1="680" y1="192" x2="670" y2="192" stroke="#ff6666" stroke-width="1.5"/>

  <text x="685" y="220" text-anchor="start" fill="#ff6666" font-size="11">→ GPIO27 (BTN_MINUS)</text>
  <line x1="680" y1="217" x2="670" y2="217" stroke="#ff6666" stroke-width="1.5"/>

  <text x="685" y="245" text-anchor="start" fill="#cc66ff" font-size="11">→ GPIO25 (BUZZER)</text>
  <line x1="680" y1="242" x2="670" y2="242" stroke="#cc66ff" stroke-width="1.5"/>

  <!-- 电源引脚 -->
  <text x="685" y="270" text-anchor="start" fill="#ffcc00" font-size="11">→ 3.3V (主板供电)</text>
  <line x1="680" y1="267" x2="670" y2="267" stroke="#ffcc00" stroke-width="1.5"/>
  <text x="350" y="270" text-anchor="end" fill="#ffcc00" font-size="11">← GND</text>
  <line x1="355" y1="267" x2="380" y2="267" stroke="#ffcc00" stroke-width="1.5"/>

  <!-- ────────────────────── 分隔线 ────────────────────── -->
  <line x1="50" y1="290" x2="1050" y2="290" stroke="#ccc" stroke-width="1" stroke-dasharray="5,5"/>

  <!-- ====================================================== -->
  <!-- TFT 屏幕接线明细 -->
  <!-- ====================================================== -->
  <text x="80" y="320" fill="#e94560" font-size="14" font-weight="bold">① TFT 屏幕 (ST7789) — SPI 接口</text>

  <rect x="80" y="335" width="420" height="180" rx="6" fill="#fff5f5" stroke="#e94560" stroke-width="1.5"/>
  <text x="100" y="358" fill="#e94560" font-size="11" font-weight="bold">TFT 排针 (从左到右 10针)</text>
  <text x="100" y="380" fill="#333" font-size="11">  1. GND    — 电源地         →  ESP32 GND</text>
  <text x="100" y="400" fill="#333" font-size="11">  2. RST    — 复位           →  ESP32 GPIO16</text>
  <text x="100" y="420" fill="#333" font-size="11">  3. SCL    — SPI 时钟        →  ESP32 GPIO18 (2)</text>
  <text x="100" y="440" fill="#333" font-size="11">  4. D/C    — 数据/命令选择   →  ESP32 GPIO17</text>
  <text x="100" y="460" fill="#333" font-size="11">  5. CS     — SPI 片选         →  ESP32 GPIO5</text>
  <text x="100" y="480" fill="#333" font-size="11">  6. SDA    — SPI 数据 (MOSI)  →  ESP32 GPIO23 (18)</text>
  <text x="100" y="500" fill="#333" font-size="11">  7. SDO    — 数据回传 (MISO)  →  (可不接)</text>

  <!-- 右侧列 -->
  <rect x="530" y="335" width="250" height="180" rx="6" fill="#fff5f5" stroke="#e94560" stroke-width="1.5"/>
  <text x="550" y="358" fill="#e94560" font-size="11" font-weight="bold">背光控制</text>
  <text x="550" y="380" fill="#333" font-size="11">  8. VCC    —  3.3V</text>
  <text x="550" y="400" fill="#333" font-size="11">  9. LEDA   —  背光正极</text>
  <text x="550" y="420" fill="#333" font-size="11"> 10. LEDK   —  背光负极</text>
  <text x="550" y="450" fill="#e94560" font-size="11" font-weight="bold">背光接法 (ESP32 无 LEDK):</text>
  <text x="550" y="470" fill="#333" font-size="10">  LEDA → 3.3V</text>
  <text x="550" y="490" fill="#333" font-size="10">  LEDK → N-MOSFET Drain</text>
  <text x="550" y="505" fill="#333" font-size="10">  GPIO4 → MOSFET Gate</text>

  <!-- 或简化的背光接法 -->
  <rect x="800" y="335" width="220" height="100" rx="6" fill="#fff0e0" stroke="#ff8800" stroke-width="1.5"/>
  <text x="820" y="358" fill="#ff8800" font-size="11" font-weight="bold">⭐ 简化背光接法</text>
  <text x="820" y="380" fill="#333" font-size="10">没有 LEDK 的屏幕：</text>
  <text x="820" y="400" fill="#333" font-size="10">  BL 引脚直接连 GPIO4</text>
  <text x="820" y="420" fill="#333" font-size="10">  analogWrite(GPIO4, 200)</text>

  <!-- 分隔线 -->
  <line x1="50" y1="540" x2="1050" y2="540" stroke="#ccc" stroke-width="1" stroke-dasharray="5,5"/>

  <!-- ====================================================== -->
  <!-- DHT22 接线 -->
  <!-- ====================================================== -->
  <text x="80" y="565" fill="#44cc66" font-size="14" font-weight="bold">② DHT22 温湿度传感器</text>

  <rect x="80" y="580" width="420" height="110" rx="6" fill="#f0fff4" stroke="#44cc66" stroke-width="1.5"/>
  <text x="100" y="602" fill="#333" font-size="11">  1. VCC   →  3.3V</text>
  <text x="100" y="622" fill="#333" font-size="11">  2. DATA  →  GPIO15  (+ 4.7kΩ 上拉到 3.3V)</text>
  <text x="100" y="642" fill="#333" font-size="11">  3. NC    —  不接</text>
  <text x="100" y="662" fill="#333" font-size="11">  4. GND   →  GND</text>

  <!-- 原理图 DHT22 -->
  <rect x="560" y="580" width="250" height="110" rx="4" fill="#000" stroke="#44cc66" stroke-width="1"/>
  <text x="685" y="600" text-anchor="middle" fill="#44cc66" font-size="10">DHT22 原理</text>
  <line x1="580" y1="625" x2="600" y2="625" stroke="#44cc66" stroke-width="1.5"/>
  <text x="575" y="622" text-anchor="end" fill="#aaa" font-size="9">VCC</text>
  <line x1="580" y1="645" x2="600" y2="645" stroke="#44cc66" stroke-width="1.5"/>
  <text x="575" y="642" text-anchor="end" fill="#aaa" font-size="9">DATA</text>
  <text x="615" y="648" fill="#ffcc00" font-size="9">─▷ 4.7kΩ ── 3.3V</text>
  <line x1="580" y1="665" x2="600" y2="665" stroke="#44cc66" stroke-width="1.5"/>
  <text x="575" y="662" text-anchor="end" fill="#aaa" font-size="9">GND</text>
  <line x1="600" y1="625" x2="600" y2="665" stroke="#44cc66" stroke-width="0.5"/>

  <!-- 分隔线 -->
  <line x1="50" y1="710" x2="1050" y2="710" stroke="#ccc" stroke-width="1" stroke-dasharray="5,5"/>

  <!-- ====================================================== -->
  <!-- 按键接线 -->
  <!-- ====================================================== -->
  <text x="80" y="735" fill="#ff6666" font-size="14" font-weight="bold">③ 按键模块 (3键)</text>

  <rect x="80" y="750" width="420" height="100" rx="6" fill="#fff5f5" stroke="#ff6666" stroke-width="1.5"/>
  <text x="100" y="772" fill="#333" font-size="11">  MENU  →  GPIO13  ── 10kΩ ── 3.3V  (上拉)</text>
  <text x="100" y="792" fill="#333" font-size="11">  PLUS  →  GPIO14  ── 10kΩ ── 3.3V  (上拉)</text>
  <text x="100" y="812" fill="#333" font-size="11">  MINUS →  GPIO27  ── 10kΩ ── 3.3V  (上拉)</text>
  <text x="100" y="832" fill="#333" font-size="11">  GND   →  GND</text>
  <text x="100" y="848" fill="#888" font-size="10">  注: 用 ESP32 内部上拉 (INPUT_PULLUP) 可省外部电阻</text>

  <rect x="560" y="750" width="200" height="100" rx="4" fill="#000" stroke="#ff6666" stroke-width="1"/>
  <text x="660" y="772" text-anchor="middle" fill="#ff6666" font-size="10">单键电路</text>
  <text x="660" y="800" text-anchor="middle" fill="#aaa" font-size="10">3.3V</text>
  <text x="660" y="810" text-anchor="middle" fill="#aaa" font-size="10">│</text>
  <text x="660" y="820" text-anchor="middle" fill="#aaa" font-size="10">10kΩ</text>
  <text x="660" y="830" text-anchor="middle" fill="#aaa" font-size="10">│</text>
  <circle cx="660" cy="838" r="3" fill="#ff6666"/>
  <text x="677" y="835" fill="#ff6666" font-size="9">→ GPIO</text>
  <text x="660" y="848" text-anchor="middle" fill="#aaa" font-size="10">│</text>
  <text x="660" y="856" text-anchor="middle" fill="#aaa" font-size="9"> / 按键</text>
  <text x="660" y="864" text-anchor="middle" fill="#aaa" font-size="10">│</text>
  <text x="660" y="872" text-anchor="middle" fill="#aaa" font-size="10">GND</text>

  <!-- 页脚 -->
  <text x="550" y="790" text-anchor="middle" fill="#999" font-size="9">04-clock · 04-clock/hardware/SCH.md · 2026-07-02</text>
</svg>

---

## 三、电源原理图

<svg viewBox="0 0 900 400" xmlns="http://www.w3.org/2000/svg" style="background:#1a1a2e;font-family:'Courier New',monospace;">
  <text x="450" y="30" text-anchor="middle" fill="#ffcc00" font-size="16" font-weight="bold">电源系统原理图</text>
  <line x1="50" y1="40" x2="850" y2="40" stroke="#ffcc00" stroke-width="1" opacity="0.5"/>

  <!-- USB 输入 -->
  <rect x="50" y="80" width="160" height="80" rx="6" fill="#2e2e1a" stroke="#ffcc00" stroke-width="2"/>
  <text x="130" y="108" text-anchor="middle" fill="#ffcc00" font-size="12" font-weight="bold">USB 5V 输入</text>
  <text x="130" y="128" text-anchor="middle" fill="#ffee88" font-size="11">USB-C 或 MicroUSB</text>
  <text x="130" y="148" text-anchor="middle" fill="#ffee88" font-size="11">5V / 500mA</text>

  <!-- 箭头 -->
  <polyline points="210,120 250,120 250,120 280,120" fill="none" stroke="#ffcc00" stroke-width="2"/>
  <polygon points="278,115 290,120 278,125" fill="#ffcc00"/>

  <!-- 滤波电容 -->
  <line x1="250" y1="100" x2="250" y2="80" stroke="#ffcc00" stroke-width="1.5"/>
  <text x="240" y="78" text-anchor="end" fill="#ffcc00" font-size="10">C1 100μF</text>
  <line x1="250" y1="140" x2="250" y2="160" stroke="#ffcc00" stroke-width="1.5"/>
  <text x="240" y="172" text-anchor="end" fill="#ffcc00" font-size="10">C2 0.1μF</text>
  <text x="265" y="172" fill="#666" font-size="9">GND</text>

  <!-- 稳压器 AMS1117 -->
  <rect x="300" y="70" width="200" height="100" rx="6" fill="#1a1a3e" stroke="#0f3460" stroke-width="2"/>
  <text x="400" y="98" text-anchor="middle" fill="#00ccff" font-size="12" font-weight="bold">AMS1117-3.3</text>
  <text x="400" y="118" text-anchor="middle" fill="#8899aa" font-size="11">LDO 稳压器</text>
  <text x="400" y="138" text-anchor="middle" fill="#8899aa" font-size="11">Vin=5V  →  Vout=3.3V</text>
  <text x="400" y="158" text-anchor="middle" fill="#8899aa" font-size="11">Iout max: 800mA</text>

  <text x="290" y="95" text-anchor="end" fill="#ffcc00" font-size="10">Vin ○</text>
  <rect x="290" y="85" width="10" height="10" rx="2" fill="none" stroke="#ffcc00" stroke-width="1"/>
  <text x="290" y="130" text-anchor="end" fill="#666" font-size="10">GND ○</text>
  <rect x="290" y="120" width="10" height="10" rx="2" fill="none" stroke="#666" stroke-width="1"/>
  <text x="290" y="165" text-anchor="end" fill="#00ccff" font-size="10">Vout ○</text>
  <rect x="290" y="155" width="10" height="10" rx="2" fill="#00ccff"/>

  <!-- 箭头到 3.3V -->
  <polyline points="500,120 540,120 540,120 570,120" fill="none" stroke="#00ccff" stroke-width="2"/>
  <polygon points="568,115 580,120 568,125" fill="#00ccff"/>

  <!-- 输出电容 -->
  <line x1="540" y1="100" x2="540" y2="80" stroke="#00ccff" stroke-width="1.5"/>
  <text x="530" y="78" text-anchor="end" fill="#00ccff" font-size="10">C3 100μF</text>
  <line x1="540" y1="140" x2="540" y2="160" stroke="#00ccff" stroke-width="1.5"/>
  <text x="530" y="172" text-anchor="end" fill="#00ccff" font-size="10">C4 0.1μF</text>

  <!-- 3.3V 总线 -->
  <rect x="590" y="70" width="200" height="100" rx="6" fill="#0a1a0a" stroke="#00ff88" stroke-width="2"/>
  <text x="690" y="98" text-anchor="middle" fill="#00ff88" font-size="12" font-weight="bold">3.3V 总线</text>
  <text x="690" y="118" text-anchor="middle" fill="#aaddcc" font-size="11">→ ESP32 3.3V</text>
  <text x="690" y="138" text-anchor="middle" fill="#aaddcc" font-size="11">→ TFT VCC</text>
  <text x="690" y="158" text-anchor="middle" fill="#aaddcc" font-size="11">→ DHT22 VCC</text>

  <!-- 功耗估算 -->
  <rect x="50" y="200" width="800" height="120" rx="6" fill="#1a1a1a" stroke="#666" stroke-width="1"/>
  <text x="450" y="225" text-anchor="middle" fill="#ffcc00" font-size="13" font-weight="bold">功耗估算</text>

  <text x="80" y="255" fill="#ccc" font-size="11">ESP32:   .pio/build/esp32dev/firmware.elf</text>
  <text x="80" y="275" fill="#ccc" font-size="11">ESP32:   ~200mA (WiFi 发射时峰值 260mA)</text>
  <text x="80" y="295" fill="#ccc" font-size="11">TFT:     ~50mA  (背光 40mA + 逻辑 10mA)</text>
  <text x="500" y="255" fill="#ccc" font-size="11">DHT22:   ~1mA  (采样时 1.5mA, 空闲 0.2mA)</text>
  <text x="500" y="275" fill="#ccc" font-size="11">按键:    ~0.1mA (上拉电阻漏电流)</text>
  <text x="500" y="295" fill="#ccc" font-size="11">蜂鸣器:  ~30mA (仅在闹钟触发时)</text>

  <text x="450" y="340" text-anchor="middle" fill="#ffcc00" font-size="12">总功耗: ~280mA (典型)  /  1.2W   →  USB 5V 足够</text>

  <text x="450" y="380" text-anchor="middle" fill="#666" font-size="10">04-clock · 04-clock/hardware/SCH.md · 2026-07-02</text>
</svg>

---

## 四、完整物料清单 (BOM)

| # | 元件 | 型号/规格 | 数量 | 单价(约) | 备注 |
|---|------|-----------|:----:|:--------:|------|
| 1 | **主控板** | ESP32 Dev Module (CP2102) | 1 | ¥25 | 建议买双排针版本 |
| 2 | **TFT 屏幕** | ST7789 240x320 2.4" | 1 | ¥20 | 10-Pin SPI 接口 |
| 3 | **温湿度传感器** | DHT22 (AM2302) | 1 | ¥12 | 蓝色外壳版 |
| 4 | **按键** | 6x6x7mm 轻触开关 | 3 | ¥0.5 | 带帽 |
| 5 | **蜂鸣器** | 有源蜂鸣器 5V | 1 | ¥2 | 无源也可(PWM驱动) |
| 6 | **电阻** | 4.7kΩ 1/4W | 1 | ¥0.1 | DHT22 上拉 |
| 7 | **电阻** | 100Ω 1/4W | 1 | ¥0.1 | 蜂鸣器限流 |
| 8 | **电容** | 100μF 16V 铝电解 | 2 | ¥0.5 | 电源滤波 |
| 9 | **电容** | 0.1μF 瓷片 | 2 | ¥0.3 | 去耦 |
| 10 | **排针** | 2.54mm 单排 (20pin) | 1 | ¥1 | 接屏幕用 |
| 11 | **杜邦线** | 公母 / 母母 各10cm | 20 | ¥2 | 面包板阶段 |
| 12 | **面包板** | 830孔 | 1 | ¥5 | 原型测试用 |
| 13 | **USB 线** | MicroUSB 或 Type-C | 1 | ¥3 | 供电+烧录 |

> **总计约 ¥72** (不含 3D 打印外壳)

---

## 五、PCB 设计建议

### 5.1 布局建议

```
┌──────────────────────────────────┐
│  [USB]     [AMS1117]   [电容组]   │  ← 电源区（左上）
│                                  │
│                    [按键×3]       │  ← 人机交互（右上）
│  [ESP32]                         │
│                    [蜂鸣器]       │  ← 声音输出
│                                  │
│  [TFT 屏幕 —— 排针直插]          │  ← 显示输出（下方）
│                                  │
│  [DHT22]                         │  ← 传感器（右下，远离热源）
└──────────────────────────────────┘
```

### 5.2 走线规则

| 规则 | 说明 |
|------|------|
| **电源线宽** | VCC/GND 主干 ≥ 1.0mm，分支 ≥ 0.5mm |
| **信号线宽** | SPI 信号 ≥ 0.3mm |
| **SPI 等长** | MOSI + SCK 走线尽量等长，< 10mm 差 |
| **去耦电容** | ESP32 每个电源引脚附近放 0.1μF |
| **DHT22 上拉** | 4.7kΩ 尽量靠近 DHT22 的 DATA 引脚 |
| **覆铜** | 顶层底层均 GND 覆铜，间距 0.3mm |
| **天线区** | ESP32 PCB 天线下方不铺铜，不走过线 |

### 5.3 立创EDA 开板建议

1. **板材**: FR-4 双面板
2. **板厚**: 1.6mm
3. **铜厚**: 1oz
4. **尺寸**: 预估 100mm × 70mm
5. **表面处理**: HASL (有铅喷锡) 即可
6. **数量**: 5片 (够做 2~3 个迭代)

---

## 六、总接线表

| ESP32 GPIO | 连接目标 | 方向 | 备注 |
|:----------:|----------|:----:|------|
| **GND** | 所有 GND | — | 公共地 |
| **3.3V** | 所有 VCC | 电源 | |
| **GPIO4** | TFT BL (背光) | OUT | PWM |
| **GPIO5** | TFT CS | OUT | SPI 片选 |
| **GPIO16** | TFT RST | OUT | 复位 |
| **GPIO17** | TFT DC | OUT | 数据/命令 |
| **GPIO18** | TFT SCLK | OUT | SPI 时钟 |
| **GPIO23** | TFT MOSI | OUT | SPI 数据 |
| **GPIO15** | DHT22 DATA | IN | 4.7kΩ 上拉 |
| **GPIO13** | KEY_MENU | IN | INPUT_PULLUP |
| **GPIO14** | KEY_PLUS | IN | INPUT_PULLUP |
| **GPIO27** | KEY_MINUS | IN | INPUT_PULLUP |
| **GPIO25** | Buzzer | OUT | PWM, 三极管驱动 |
| **GPIO0** | BOOT 键 | IN | 保留 (停止闹钟) |
| **GPIO2** | 板载 LED | OUT | 心跳指示 |
