# 04-clock 硬件设计

## 文件说明

| 文件 | 说明 |
|------|------|
| `SCH.md` | 硬件原理图文档（含 3 张 SVG 原理图、BOM、接线表） |
| `generate_schematic.py` | 立创EDA 原理图 JSON 生成脚本 |
| `output/04-clock-schematic.json` | **可直接导入立创EDA的原理图** |
| `output/04-clock-bom.csv` | BOM 物料清单 |
| `output/04-clock-netlist.csv` | 网络连接表 |

## 用法

### 方法一：直接导入 JSON（最快）

1. 打开 **立创EDA专业版**
2. `文件` → `导入` → `立创EDA专业版(.json)`
3. 选择 `output/04-clock-schematic.json`
4. 原理图自动生成，包含所有元件和连接

### 方法二：重新生成

```bash
cd hardware
python3 generate_schematic.py
```

### 方法三：参考原理图手工绘制

打开 `SCH.md`，里面有完整的 SVG 系统框图、接线图和电源图，按图在立创EDA中绘制即可。

## 引脚定义速查

```
ESP32 GPIO    →   外设
──────────────────────
GPIO4   →  TFT BL (背光PWM)
GPIO5   →  TFT CS (SPI片选)
GPIO16  →  TFT RST (复位)
GPIO17  →  TFT DC (数据/命令)
GPIO18  →  TFT SCK (SPI时钟)
GPIO23  →  TFT MOSI (SPI数据)
GPIO15  →  DHT22 DATA (4.7kΩ上拉)
GPIO13  →  KEY_MENU
GPIO14  →  KEY_PLUS
GPIO27  →  KEY_MINUS
GPIO25  →  蜂鸣器 (PWM)
GPIO0   →  BOOT键 (停止闹钟)
GPIO2   →  板载LED (心跳)
```

供电:  USB 5V → AMS1117-3.3 → 3.3V 总线
