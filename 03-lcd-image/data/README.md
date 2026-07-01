# 03-lcd-image 背景图片

## 准备图片

1. 用工具生成 240×320 像素图片
2. 转成 RGB565 格式的 raw 文件：

```bash
python3 << 'EOF'
from PIL import Image
import numpy as np

img = Image.open("background.png").convert("RGB")
img = img.resize((240, 320))

raw = bytearray()
for y in range(320):
    for x in range(240):
        r, g, b = img.getpixel((x, y))
        # RGB565: RRRRR GGGGGG BBBBB
        rgb565 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
        raw.append(rgb565 & 0xFF)       # low byte
        raw.append((rgb565 >> 8) & 0xFF)  # high byte

with open("data/background.raw", "wb") as f:
    f.write(raw)
print(f"Written {len(raw)} bytes (expected 153600)")
EOF
```

## 上传到 ESP32

```bash
# PlatformIO 方式
pio run --target uploadfs

# 或 esptool 方式
# 先编译一次，SPIFFS 镜像在 .pio/build/featheresp32/spiffs.bin
# 烧录到 0x290000（默认 SPIFFS 分区地址）
```
