# ESP32 开发参考手册

> 快速查阅，不用每次重新查。

---

## 🔌 硬件接线

### CP210x → ESP32-WROOM

| CP210x | ESP32-WROOM | 备注 |
|---|---|---|
| TXD | RXD0 (GPIO3) | 交叉连接 |
| RXD | TXD0 (GPIO1) | 交叉连接 |
| GND | GND | 共地 |
| 3.3V | 3.3V/VIN | 供电 |

### LCD 屏 (ST7789 240x320)

| LCD 引脚 | ESP32 GPIO | 功能 |
|---|---|---|
| NO1 GND | GND | 电源地 |
| NO2 RESET | GPIO15 | 复位 |
| NO3 SCL/SCK | GPIO2 | SPI 时钟 |
| NO4 RS/DC | GPIO4 | 数据/命令选择 |
| NO5 CS | GPIO16 | 片选 |
| NO6 SDA/MOSI | GPIO17 | SPI 数据 |
| NO7 SDO/MISO | (未接) | 可接 GPIO5 |
| NO8 VCC | 3.3V | 电源 |
| NO9 A/BL | GPIO18 | 背光正极 |
| NO10 K | GND | 背光负极 |

---

## 🔥 烧录

### COM 口

```
CP210x 默认端口: COM3
波特率: 115200
```

如果 COM3 找不到，检查设备管理器或：
```powershell
wmic path Win32_SerialPort get DeviceID,Name
```

### ESP32 Flash 布局

| 地址 | 内容 | 来源文件 |
|---|---|---|
| `0x1000` | bootloader | `.pio/build/featheresp32/bootloader.bin` |
| `0x8000` | 分区表 | `.pio/build/featheresp32/partitions.bin` |
| `0x10000` | 应用程序 | `.pio/build/featheresp32/firmware.bin` |

### 烧录命令（WSL 编译 → Windows 烧录）

```bash
# 1. 在 WSL 中编译
cd ~/quant-project/lcd-test-4-2
pio run

# 2. 复制文件到 Windows
cp .pio/build/featheresp32/*.bin /mnt/d/

# 3. 在 PowerShell 中烧录（WSL 里执行）
powershell.exe -Command "cd C:\; & 'C:\Users\Mapple\AppData\Roaming\Python\Python312\Scripts\esptool.exe' --port COM3 --baud 115200 --before default-reset --after hard-reset write-flash 0x1000 D:\bootloader.bin 0x8000 D:\partitions.bin 0x10000 D:\firmware.bin"
```

### 只用 Windows esptool 快速烧录（不用 PlatformIO）

```powershell
# Windows PowerShell (管理员)
& 'C:\Users\Mapple\AppData\Roaming\Python\Python312\Scripts\esptool.exe' --port COM3 --baud 115200 write-flash 0x1000 D:\bootloader.bin 0x8000 D:\partitions.bin 0x10000 D:\firmware.bin
```

### 擦除 Flash

```powershell
& 'C:\Users\Mapple\AppData\Roaming\Python\Python312\Scripts\esptool.exe' --port COM3 erase-flash
```

---

## 🛠 PlatformIO 配置

### 可用的 board 配置

```ini
[env:featheresp32]
platform = espressif32
board = featheresp32           # 已验证可用的配置
framework = arduino
monitor_speed = 115200
```

### TFT_eSPI + ST7789 配置

```ini
[env:featheresp32]
platform = espressif32
board = featheresp32
framework = arduino
monitor_speed = 115200

lib_deps = bodmer/TFT_eSPI @ ^2.5.43

build_flags =
  -D USER_SETUP_LOADED=1
  -D ST7789_DRIVER=1
  -D TFT_WIDTH=240
  -D TFT_HEIGHT=320
  -D TFT_MOSI=17
  -D TFT_SCLK=2
  -D TFT_CS=16
  -D TFT_DC=4
  -D TFT_RST=15
  -D TFT_BL=18
  -D SPI_FREQUENCY=27000000
  -D TOUCH_CS=-1
```

---

## 💻 环境路径

### 项目路径

| 位置 | 路径 |
|---|---|
| WSL 项目根目录 | `/home/mapple/quant-project/` |
| Windows 映射路径 | `\\wsl.localhost\Ubuntu\home\mapple\quant-project` |
| LCD 测试项目 | `/home/mapple/quant-project/lcd-test-4-2/` |

### 工具路径

| 工具 | 路径 |
|---|---|
| PlatformIO | `~/.local/bin/pio` |
| Windows esptool | `C:\Users\Mapple\AppData\Roaming\Python\Python312\Scripts\esptool.exe` |

---

## ⚡ 快速启动（下次来直接用）

```bash
# 1. 打开 WSL
wsl

# 2. 进入项目
cd ~/quant-project/lcd-test-4-2

# 3. 编译
export PATH="$HOME/.local/bin:$PATH"
pio run

# 4. 复制固件
cp .pio/build/featheresp32/bootloader.bin /mnt/d/
cp .pio/build/featheresp32/partitions.bin /mnt/d/
cp .pio/build/featheresp32/firmware.bin /mnt/d/

# 5. 烧录
powershell.exe -Command "cd C:\; & 'C:\Users\Mapple\AppData\Roaming\Python\Python312\Scripts\esptool.exe' --port COM3 --baud 115200 --before default-reset --after hard-reset write-flash 0x1000 D:\bootloader.bin 0x8000 D:\partitions.bin 0x10000 D:\firmware.bin"

# 6. 看串口输出
powershell.exe -Command "
\$port = New-Object System.IO.Ports.SerialPort 'COM3',115200,None,8,One;
\$port.ReadTimeout = 5000;
\$port.Open();
Start-Sleep -Seconds 3;
\$result = \$port.ReadExisting();
Write-Host \$result;
\$port.Close();
"
```
