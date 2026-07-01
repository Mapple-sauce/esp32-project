# 03-lcd-image 背景图片

## 如何制作 background.raw

```bash
pip install Pillow
python3 << 'PYEOF'
from PIL import Image
import numpy as np

img = Image.open("your_240x320_image.png").convert("RGB")
img = img.resize((240, 320), Image.LANCZOS)

raw = bytearray()
for y in range(320):
    for x in range(240):
        r, g, b = img.getpixel((x, y))
        rgb565 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
        raw.append(rgb565 & 0xFF)        # low byte
        raw.append((rgb565 >> 8) & 0xFF) # high byte

with open("data/background.raw", "wb") as f:
    f.write(raw)
print(f"Written {len(raw)} bytes (expected 153600)")
PYEOF
```

## 上传 SPIFFS

```bash
pio run --target uploadfs
```

或手动烧录 SPIFFS 镜像（地址 0x290000）。
