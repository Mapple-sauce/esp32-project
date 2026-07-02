#!/usr/bin/env python3
"""
04-clock ESP32 Desktop Ornament
立创EDA (EasyEDA) 原理图生成器

用法:
    python3 generate_schematic.py

输出:
    output/04-clock-schematic.json  — 立创EDA原理图文件
    output/04-clock-bom.csv         — BOM物料清单
    output/04-clock-netlist.csv     — 网络连接表

在立创EDA中: 导入 → 立创EDA专业版(.json) → 选择生成的json文件
"""

import json
import math
import os
import uuid
from dataclasses import dataclass, field
from typing import List, Tuple, Optional

# ============================================================
# 常量
# ============================================================

GRID = 100          # 立创EDA网格单位 (100 = 1inch = 25.4mm)
CANVAS_W = 2800     # 画布宽度
CANVAS_H = 2200     # 画布高度

# 颜色定义 (立创EDA颜色值)
COLOR_RED = 0xFF0000
COLOR_GREEN = 0x00FF00
COLOR_BLUE = 0x0000FF
COLOR_YELLOW = 0xFFFF00
COLOR_BLACK = 0x000000
COLOR_GRAY = 0x888888
COLOR_ORANGE = 0xFF8800
COLOR_PURPLE = 0x8800FF

# ============================================================
# 数据结构
# ============================================================

@dataclass
class Point:
    x: float
    y: float

@dataclass
class NetSegment:
    """网络连接记录"""
    net_name: str
    comp1: str          # 元件1 (如 "U1")
    pin1: str           # 引脚1 (如 "GPIO15")
    comp2: str          # 元件2
    pin2: str           # 引脚2
    note: str = ""      # 备注

class SchematicGen:
    """立创EDA原理图生成器"""

    def __init__(self):
        self._element_id = 0
        self._net_id = 0
        self.elements = []
        self.nets: List[NetSegment] = []
        self.net_segments: List[NetSegment] = []

    # ── 工具 ──────────────────────────────────────────────

    def _new_id(self) -> str:
        self._element_id += 1
        return f"gid{self._element_id}"

    def _new_uuid(self) -> str:
        return uuid.uuid4().hex[:16]

    # ── 元件 ──────────────────────────────────────────────

    def add_comp(self, name: str, value: str,
                 x: float, y: float,
                 lib_id: str = "",
                 package: str = "",
                 rotation: int = 0,
                 color: int = 0x000000,
                 w: float = 0, h: float = 0,
                 pins: list = None) -> str:
        """添加元件到原理图"""
        cid = self._new_id()
        uid = self._new_uuid()

        comp = {
            "type": "component",
            "head": {
                "uuid": uid,
                "c_para": {
                    "name": name,
                    "value": value
                },
                "c_name": value.split("\\n")[0] if "\\n" in value else value,
                "package": package,
                "libId": lib_id,
                "manufacturer": "",
                "datasheet": ""
            },
            "shape": {
                "x": x,
                "y": y,
                "w": w,
                "h": h,
                "pins": pins or [],
                "rectangles": [],
                "circles": [],
                "lines": [],
                "texts": [{
                    "type": "text",
                    "x": 0,
                    "y": -h//2 - 30 if h > 0 else -40,
                    "w": 0,
                    "h": 0,
                    "stroke_width": 0,
                    "color": color,
                    "text": name,
                    "font_size": 12,
                    "text_anchor": "middle",
                    "text_orient": 0
                }, {
                    "type": "text",
                    "x": 0,
                    "y": h//2 + 30 if h > 0 else 40,
                    "w": 0,
                    "h": 0,
                    "stroke_width": 0,
                    "color": color,
                    "text": value,
                    "font_size": 10,
                    "text_anchor": "middle",
                    "text_orient": 0
                }]
            },
            "position": f"{x},{y}",
            "rotation": rotation,
            "show": True,
            "locked": False
        }

        # 添加矩形主体
        if w > 0 and h > 0:
            comp["shape"]["rectangles"].append({
                "x": -w//2, "y": -h//2,
                "w": w, "h": h,
                "stroke_width": 2,
                "color": color,
                "fill": 0xFFF8E1,
                "r": 5
            })

        self.elements.append(comp)
        return cid

    def _box_pins(self, pin_defs: list, box_w: float, box_h: float):
        """生成矩形元件的引脚定义

        pin_defs: [(name, side, offset_ratio), ...]
           side: 'L'左边 'R'右边 'T'顶部 'B'底部
           offset_ratio: 从0-1的相对位置
        """
        pins = []
        for pin_name, side, ratio, pin_type in pin_defs:
            if side == 'L':
                px, py = -box_w//2, -box_h//2 + box_h * ratio
                orient = 'right'
            elif side == 'R':
                px, py = box_w//2, -box_h//2 + box_h * ratio
                orient = 'left'
            elif side == 'T':
                px, py = -box_w//2 + box_w * ratio, -box_h//2
                orient = 'down'
            else:  # 'B'
                px, py = -box_w//2 + box_w * ratio, box_h//2
                orient = 'up'

            pins.append({
                "x": px, "y": py,
                "length": 10,
                "direction": orient,
                "type": pin_type,
                "color": 0x000000,
                "name": pin_name
            })
        return pins

    # ── 导线 ──────────────────────────────────────────────

    def add_wire(self, points: List[Tuple[float, float]],
                 color: int = 0x000000, width: float = 0.3,
                 net_name: str = "") -> str:
        """添加导线"""
        wid = self._new_id()
        uid = self._new_uuid()

        if len(points) < 2:
            return wid

        wire = {
            "type": "wire",
            "head": {
                "uuid": uid,
                "c_para": {
                    "netName": net_name,
                    "wire_width": str(width),
                    "color": str(color)
                }
            },
            "shape": {
                "x": points[0][0],
                "y": points[0][1],
                "w": points[-1][0] - points[0][0],
                "h": points[-1][1] - points[0][1]
            },
            "position": ";".join(f"{p[0]},{p[1]}" for p in points),
            "show": True,
            "locked": False
        }
        self.elements.append(wire)
        return wid

    def add_wire_L(self, x1: float, y1: float, x2: float, y2: float,
                   color: int = 0x000000, net: str = ""):
        """添加直线导线"""
        return self.add_wire([(x1, y1), (x2, y2)], color, 0.3, net)

    def add_bus_wire(self, points: List[Tuple[float, float]],
                     net_name: str = ""):
        """总线风格导线 (加粗蓝色)"""
        return self.add_wire(points, COLOR_BLUE, 0.6, net_name)

    # ── 网络标签 ──────────────────────────────────────────

    def add_netlabel(self, name: str, x: float, y: float,
                     rotation: int = 0) -> str:
        """添加网络标签"""
        nid = self._new_id()
        uid = self._new_uuid()

        label = {
            "type": "netlabel",
            "head": {
                "uuid": uid,
                "c_para": {
                    "netName": name
                }
            },
            "shape": {
                "x": x, "y": y,
                "w": len(name) * 6,
                "h": 12
            },
            "position": f"{x},{y}",
            "rotation": rotation,
            "show": True,
            "locked": False
        }
        self.elements.append(label)
        return nid

    # ── VCC / GND ─────────────────────────────────────────

    def add_power(self, name: str, x: float, y: float,
                  is_gnd: bool = False) -> str:
        """添加电源/地符号"""
        vid = self._new_id()
        uid = self._new_uuid()

        comp = {
            "type": "component",
            "head": {
                "uuid": uid,
                "c_para": {"name": name, "value": ""},
                "c_name": name,
                "libId": "GND" if is_gnd else "VCC",
                "manufacturer": "",
                "datasheet": ""
            },
            "shape": {
                "x": x, "y": y,
                "w": 0, "h": 0,
                "pins": [{
                    "x": 0, "y": 0,
                    "length": 0,
                    "direction": "up" if is_gnd else "down",
                    "type": "end",
                    "color": 0,
                    "name": name
                }],
                "lines": [],
                "rectangles": [],
                "circles": [],
                "texts": [{
                    "type": "text",
                    "x": 0,
                    "y": -30 if is_gnd else 30,
                    "w": 0, "h": 0,
                    "stroke_width": 0,
                    "color": COLOR_RED if not is_gnd else COLOR_BLACK,
                    "text": name,
                    "font_size": 12,
                    "text_anchor": "middle",
                    "text_orient": 0
                }]
            },
            "position": f"{x},{y}",
            "rotation": 0,
            "show": True,
            "locked": False
        }

        # GND 符号: 线条
        if is_gnd:
            comp["shape"]["lines"] = [
                {"x1": -12, "y1": 0, "x2": 12, "y2": 0, "stroke_width": 2, "color": 0},
                {"x1": -8,  "y1": 6, "x2": 8,  "y2": 6, "stroke_width": 1.5, "color": 0},
                {"x1": -4,  "y1": 12, "x2": 4,  "y2": 12, "stroke_width": 1, "color": 0},
            ]
        # VCC 符号: 短线
        else:
            comp["shape"]["lines"] = [
                {"x1": -8, "y1": 0, "x2": 8, "y2": 0, "stroke_width": 2, "color": COLOR_RED},
            ]

        self.elements.append(comp)
        return vid

    # ── 文本注释 ──────────────────────────────────────────

    def add_text(self, text: str, x: float, y: float,
                 font_size: int = 14, color: int = 0x000000,
                 anchor: str = "start") -> str:
        tid = self._new_id()
        uid = self._new_uuid()

        txt = {
            "type": "text",
            "head": {
                "uuid": uid,
                "c_para": {
                    "text": text,
                    "font_size": str(font_size),
                    "color": str(color)
                }
            },
            "shape": {
                "x": x, "y": y,
                "w": len(text) * font_size * 0.6,
                "h": font_size * 1.2
            },
            "position": f"{x},{y}",
            "rotation": 0,
            "show": True,
            "locked": False
        }
        self.elements.append(txt)
        return tid

    # ── 网络表 ────────────────────────────────────────────

    def add_net(self, net_name: str,
                comp1: str, pin1: str,
                comp2: str, pin2: str,
                note: str = ""):
        self.nets.append(NetSegment(net_name, comp1, pin1, comp2, pin2, note))

    # ── 导出 ──────────────────────────────────────────────

    def export_schematic(self) -> dict:
        """导出为立创EDA原理图JSON"""
        sch = {
            "head": {
                "docType": "3",
                "editorVersion": "6.5.40",
                "cId": "",
                "xGui": {
                    "showGrid": True,
                    "snapToGrid": True,
                    "showPageBorder": False
                },
                "plugin": ""
            },
            "canvas": {
                "BICKERCOLOR": 16777215,
                "GRIDCOLOR": 14737632,
                "HIDE": 0,
                "SNAP": 1,
                "zoom": 8,
                "width": CANVAS_W,
                "height": CANVAS_H
            },
            "elements": self.elements,
            "nets": [{
                "name": seg.net_name,
                "pins": [
                    {"cid": seg.comp1, "p": seg.pin1},
                    {"cid": seg.comp2, "p": seg.pin2}
                ]
            } for seg in self.nets]
        }
        return sch

    def export_bom(self) -> str:
        """导出BOM CSV"""
        rows = []
        # 收集所有元件
        for el in self.elements:
            if el.get("type") == "component":
                head = el.get("head", {})
                c_para = head.get("c_para", {})
                name = c_para.get("name", "")
                value = c_para.get("value", "").replace("\\n", " ")
                pkg = head.get("package", "")
                rows.append({
                    "Designator": name,
                    "Value": value,
                    "Package": pkg,
                    "Qty": 1,
                    "Description": f"{name} {value}"
                })

        # 去重合并同型号
        merged = {}
        for r in rows:
            key = f"{r['Value']}|{r['Package']}"
            if key in merged:
                merged[key]["Qty"] += 1
                merged[key]["Designator"] += f", {r['Designator']}"
            else:
                merged[key] = r

        lines = ["Designator,Value,Package,Qty,Description"]
        for r in merged.values():
            lines.append(f"{r['Designator']},{r['Value']},{r['Package']},{r['Qty']},{r['Description']}")
        return "\n".join(lines)

    def export_netlist(self) -> str:
        """导出连接表CSV"""
        lines = ["NetName,Comp1,Pin1,Comp2,Pin2,Note"]
        for seg in self.nets:
            lines.append(f"{seg.net_name},{seg.comp1},{seg.pin1},{seg.comp2},{seg.pin2},{seg.note}")
        return "\n".join(lines)

    def export_all(self, output_dir: str = "output"):
        """导出所有文件"""
        os.makedirs(output_dir, exist_ok=True)

        with open(f"{output_dir}/04-clock-schematic.json", "w",
                  encoding="utf-8") as f:
            json.dump(self.export_schematic(), f, ensure_ascii=False, indent=2)
        with open(f"{output_dir}/04-clock-bom.csv", "w",
                  encoding="utf-8") as f:
            f.write(self.export_bom())
        with open(f"{output_dir}/04-clock-netlist.csv", "w",
                  encoding="utf-8") as f:
            f.write(self.export_netlist())

        print(f"✅ 原理图: {output_dir}/04-clock-schematic.json")
        print(f"✅ BOM:    {output_dir}/04-clock-bom.csv")
        print(f"✅ 网络表: {output_dir}/04-clock-netlist.csv")
        print(f"\n📖 在立创EDA中: 导入 → 立创EDA专业版(.json) → 选择 json 文件")


# ============================================================
# 主程序 — 构建完整原理图
# ============================================================

def build_schematic() -> SchematicGen:
    g = SchematicGen()

    # ── 布局坐标 ──────────────────────────────────────────
    # 所有坐标单位 = 10mil (0.254mm)

    # 区域划分:
    COL1 = 200     # 左侧 (USB/电源输入)
    COL2 = 550     # 中左 (电源转换)
    COL3 = 1000    # 中 (ESP32)
    COL4 = 1600    # 中右 (按键/蜂鸣器)
    COL5 = 2100    # 右侧 (外部接口)

    ROW1 = 200     # 顶部 (电源)
    ROW2 = 500     # 中上 (ESP32)
    ROW3 = 850     # 中 (ESP32)
    ROW4 = 1200    # 中下 (传感器/显示)
    ROW5 = 1600    # 底部

    # ── 1. 电源系统 ──────────────────────────────────────

    # USB座
    usb_id = g.add_comp("J1", "USB Type-C", COL1, ROW1,
                        "", "USB-C-2.0", 0, COLOR_BLUE, 160, 80,
                        g._box_pins([
                            ("VBUS", "R", 0.3, "end"),
                            ("VBUS", "R", 0.7, "end"),
                            ("GND", "L", 0.3, "end"),
                            ("GND", "L", 0.7, "end"),
                        ], 160, 80))

    # 输入电容
    g.add_comp("C1", "100μF 16V", COL1 + 350, ROW1,
               "Capacitor", "CAP-TH", 0, COLOR_GREEN, 80, 40,
               g._box_pins([("+", "T", 0.5, "end"), ("-", "B", 0.5, "end")], 80, 40))

    g.add_comp("C2", "0.1μF", COL1 + 350, ROW1 + 120,
               "Capacitor", "0805", 0, COLOR_GREEN, 60, 30,
               g._box_pins([("+", "T", 0.5, "end"), ("-", "B", 0.5, "end")], 60, 30))

    # AMS1117-3.3 稳压器
    reg_id = g.add_comp("U2", "AMS1117-3.3\\nLDO 5V→3.3V",
                        COL2 + 50, ROW1 + 60,
                        "AMS1117-3.3", "SOT-223", 0, COLOR_ORANGE, 140, 100,
                        g._box_pins([
                            ("Vin", "L", 0.3, "end"),
                            ("GND", "L", 0.7, "end"),
                            ("Vout", "R", 0.5, "end"),
                        ], 140, 100))

    # 输出电容
    g.add_comp("C3", "100μF 16V", COL2 + 280, ROW1 + 20,
               "Capacitor", "CAP-TH", 0, COLOR_GREEN, 80, 40,
               g._box_pins([("+", "T", 0.5, "end"), ("-", "B", 0.5, "end")], 80, 40))
    g.add_comp("C4", "0.1μF", COL2 + 280, ROW1 + 120,
               "Capacitor", "0805", 0, COLOR_GREEN, 60, 30,
               g._box_pins([("+", "T", 0.5, "end"), ("-", "B", 0.5, "end")], 60, 30))

    # 电源符号
    g.add_power("5V", COL1 + 100, ROW1 - 100, False)
    g.add_power("3.3V", COL3 + 100, ROW2 - 200, False)
    gnd_power1 = g.add_power("GND", COL1 + 100, ROW1 + 200, True)
    gnd_power2 = g.add_power("GND", COL2 + 200, ROW1 + 280, True)

    # ── 电源走线 ──────────────────────────────────────────

    # 5V: USB → C1+ → AMS1117 Vin
    g.add_wire_L(COL1 + 160, ROW1, COL1 + 350, ROW1, COLOR_RED, "5V_NET")
    g.add_wire_L(COL1 + 350, ROW1 - 40, COL1 + 350, ROW1, COLOR_RED, "5V_NET")
    g.add_wire_L(COL1 + 350, ROW1, COL2 + 50, ROW1 + 30, COLOR_RED, "5V_NET")
    g.add_netlabel("5V", COL2 + 80, ROW1 + 5, 0)

    # GND: USB → C1- → C2- → AMS1117 GND
    g.add_wire_L(COL1 + 350, ROW1 + 40, COL1 + 350, ROW1 + 120)
    g.add_wire_L(COL1 + 350, ROW1 + 120, COL1 + 200, ROW1 + 120)
    g.add_wire_L(COL1 + 200, ROW1 + 120, COL1 + 200, ROW1 + 200)
    g.add_wire_L(COL2 + 50, ROW1 + 110, COL2 + 200, ROW1 + 110)
    g.add_wire_L(COL2 + 200, ROW1 + 110, COL2 + 200, ROW1 + 280)

    # 3.3V: AMS1117 Vout → C3+ → C4+ → 3.3V总线
    g.add_wire_L(COL2 + 190, ROW1 + 60, COL2 + 280, ROW1 + 20, COLOR_RED, "3V3_NET")
    g.add_wire_L(COL2 + 280, ROW1 + 20, COL2 + 280, ROW1, COLOR_RED, "3V3_NET")

    g.add_netlabel("3.3V", COL2 + 300, ROW1 - 20, 0)

    # ── 2. ESP32 主控 ────────────────────────────────────

    esp32_id = g.add_comp("U1", "ESP32 Dev Module\\nCP2102版", COL3, ROW2 + 100,
                          "ESP32 Dev Module", "", 0, COLOR_BLUE, 280, 300,
                          g._box_pins([
                              # 左侧引脚 (信号输入)
                              ("GPIO15\nDHT22", "L", 0.10, "end"),
                              ("GPIO13\nMENU", "L", 0.22, "end"),
                              ("GPIO14\nPLUS", "L", 0.34, "end"),
                              ("GPIO27\nMINUS", "L", 0.46, "end"),
                              ("GPIO25\nBUZZER", "L", 0.58, "end"),
                              ("GPIO0\nBOOT", "L", 0.70, "end"),
                              ("GPIO2\nLED", "L", 0.82, "end"),
                              ("EN\nRST", "L", 0.94, "end"),
                              # 右侧引脚 (SPI + 电源)
                              ("GPIO5\nTFT_CS", "R", 0.10, "end"),
                              ("GPIO17\nTFT_DC", "R", 0.22, "end"),
                              ("GPIO16\nTFT_RST", "R", 0.34, "end"),
                              ("GPIO23\nTFT_MOSI", "R", 0.46, "end"),
                              ("GPIO18\nTFT_SCK", "R", 0.58, "end"),
                              ("GPIO4\nTFT_BL", "R", 0.70, "end"),
                              ("3.3V", "R", 0.85, "power"),
                              ("GND", "R", 0.95, "power"),
                          ], 280, 300))

    # ESP32 电源
    g.add_wire_L(COL3 + 140, ROW2 + 100 + 255, COL3 + 280, ROW1 + 80, COLOR_RED, "3V3_NET")
    g.add_wire_L(COL3 + 200, ROW2 + 100 + 285, COL3 + 200, ROW2 + 100 + 340, COLOR_BLACK, "GND_NET")
    g.add_power("GND", COL3 + 200, ROW2 + 100 + 370, True)

    g.add_netlabel("3.3V", COL3 + 270, ROW1 + 80, 0)

    # ── 3. TFT 屏幕 ─────────────────────────────────────

    tft_id = g.add_comp("J2", "TFT ST7789\\n240×320 SPI",
                        COL3 - 600, ROW4 + 50, "", "Header-2.54-8P", 0,
                        COLOR_PURPLE, 200, 160,
                        g._box_pins([
                            ("VCC\n3.3V", "T", 0.2, "power"),
                            ("GND", "T", 0.5, "power"),
                            ("BL\n背光", "T", 0.8, "end"),
                            ("CS", "L", 0.7, "end"),
                            ("DC", "L", 0.85, "end"),
                            ("MOSI", "B", 0.2, "end"),
                            ("SCK", "B", 0.5, "end"),
                            ("RST", "B", 0.8, "end"),
                        ], 200, 160))

    # 背光接法: BL → GPIO4 (右侧走线)
    # TFT 电源
    g.add_wire_L(COL3 - 550, ROW4 + 20, COL3 - 550, ROW4 - 100)
    g.add_netlabel("3.3V", COL3 - 550, ROW4 - 120, 0)

    # SPI 走线 (从 ESP32 右侧到 TFT)
    # GPIO5(CS)  → TFT CS
    g.add_wire_L(COL3 + 140, ROW2 + 100 + 30, COL3 + 280, ROW4 + 80)
    g.add_wire_L(COL3 + 280, ROW4 + 80, COL3 - 400, ROW4 + 80)
    g.add_netlabel("TFT_CS", COL3 + 280, ROW4 + 60, 0)

    # GPIO17(DC) → TFT DC
    g.add_wire_L(COL3 + 140, ROW2 + 100 + 66, COL3 + 280, ROW4 + 100)
    g.add_wire_L(COL3 + 280, ROW4 + 100, COL3 - 400, ROW4 + 100)
    g.add_netlabel("TFT_DC", COL3 + 280, ROW4 + 80, 0)

    # GPIO16(RST) → TFT RST
    g.add_wire_L(COL3 + 140, ROW2 + 100 + 102, COL3 + 300, ROW4 + 130)
    g.add_netlabel("TFT_RST", COL3 + 280, ROW4 + 110, 0)

    # GPIO23(MOSI) → TFT MOSI
    g.add_wire_L(COL3 + 140, ROW2 + 100 + 138, COL3 + 280, ROW4 + 145)
    g.add_netlabel("TFT_MOSI", COL3 + 100, ROW4 + 125, 0)

    # GPIO18(SCK) → TFT SCK
    g.add_wire_L(COL3 + 140, ROW2 + 100 + 174, COL3 + 320, ROW4 + 160)
    g.add_netlabel("TFT_SCK", COL3 + 280, ROW4 + 140, 0)

    # GPIO4(BL) → TFT BL
    g.add_wire_L(COL3 + 140, ROW2 + 100 + 210, COL3 + 280, ROW4 + 175)

    # ── 4. DHT22 ─────────────────────────────────────────

    dht22_resistor = g.add_comp("R1", "4.7kΩ\n上拉电阻",
                                COL3 - 200, ROW5, "", "0805", 0,
                                COLOR_GREEN, 80, 40,
                                g._box_pins([
                                    ("1", "L", 0.5, "end"),
                                    ("2", "R", 0.5, "end"),
                                ], 80, 40))

    dht22_id = g.add_comp("U3", "DHT22\\n温湿度传感器",
                          COL3 - 550, ROW5, "", "DHT22", 0,
                          COLOR_GREEN, 160, 100,
                          g._box_pins([
                              ("VCC\n3.3V", "T", 0.3, "power"),
                              ("DATA", "L", 0.5, "end"),
                              ("NC", "B", 0.3, "end"),
                              ("GND", "B", 0.7, "power"),
                          ], 160, 100))

    # DHT22 电路
    g.add_wire_L(COL3 - 550, ROW5 - 50, COL3 - 550, ROW5 - 120)
    g.add_netlabel("3.3V", COL3 - 550, ROW5 - 140, 0)

    g.add_wire_L(COL3 - 550, ROW5 + 70, COL3 - 550, ROW5 + 130)
    g.add_power("GND", COL3 - 550, ROW5 + 160, True)

    # GPIO15 → DATA (经过4.7kΩ上拉到3.3V)
    g.add_wire_L(COL3 - 470, ROW5, COL3 - 240, ROW5)
    g.add_wire_L(COL3 - 240, ROW5, COL3 - 200, ROW5)   # 到电阻
    g.add_wire_L(COL3 - 200, ROW5, COL3 - 100, ROW5)   # 到ESP32 GPIO15
    g.add_netlabel("DHT22_DATA", COL3 - 300, ROW5 - 20, 0)

    # DATA → 3.3V (4.7kΩ上拉)
    g.add_wire_L(COL3 - 200, ROW5 - 20, COL3 - 200, ROW5 - 120)
    g.add_netlabel("3.3V", COL3 - 200, ROW5 - 140, 0)

    # ── 5. 按键模块 ──────────────────────────────────────

    btn_y = ROW2 + 220
    btn_spacing = 60

    for i, (label, gpio, y_offset) in enumerate([
        ("MENU", "GPIO13", 0),
        ("PLUS", "GPIO14", btn_spacing * 1),
        ("MINUS", "GPIO27", btn_spacing * 2),
    ]):
        bx = COL4
        by = btn_y + y_offset
        btn_id = g.add_comp(
            f"SW{i+1}", f"KEY_{label}\\n→{gpio}",
            bx, by, "", "SW-PB-6x6", 0, COLOR_ORANGE, 80, 50,
            g._box_pins([
                ("1", "T", 0.3, "end"),
                ("2", "T", 0.7, "end"),
                ("3", "B", 0.3, "end"),
                ("4", "B", 0.7, "end"),
            ], 80, 50))

        # GND（按键另一侧接地）
        g.add_wire_L(bx, by + 25, bx, by + 80)
        g.add_power("GND", bx, by + 110, True)

        # GPIO 引出线
        gpio_pin = gpio.replace("GPIO", "")
        g.add_wire_L(bx - 40, by, COL3 + 140, ROW2 + 100 + 30 * (i + 1))
        g.add_netlabel(f"KEY_{label}", bx - 50, by - 20, 0)

    # ── 6. 蜂鸣器 ────────────────────────────────────────

    buzzer_id = g.add_comp("LS1", "蜂鸣器\\n有源5V",
                           COL4, ROW4 + 100, "", "BUZZER-TH", 0,
                           COLOR_PURPLE, 120, 80,
                           g._box_pins([
                               ("+", "T", 0.3, "end"),
                               ("-", "T", 0.7, "end"),
                           ], 120, 80))

    # 限流电阻
    g.add_comp("R2", "100Ω\n限流", COL4, ROW4 + 50,
               "Resistor", "0805", 0, COLOR_GREEN, 60, 30,
               g._box_pins([("1", "L", 0.5, "end"), ("2", "R", 0.5, "end")], 60, 30))

    # GPIO25 → 电阻 → 蜂鸣器 → 3.3V
    g.add_wire_L(COL3 + 140, ROW2 + 100 + 174, COL4 - 50, ROW4 + 50)
    g.add_netlabel("BUZZER_PWM", COL4 - 50, ROW4 + 30, 0)

    g.add_wire_L(COL4 - 30, ROW4 + 50, COL4 - 24, ROW4 + 80)   # 电阻到蜂鸣器b+
    g.add_wire_L(COL4 + 24, ROW4 + 80, COL4 + 50, ROW4 + 50)
    g.add_wire_L(COL4 + 50, ROW4 + 50, COL4 + 200, ROW4 + 50)
    g.add_netlabel("3.3V", COL4 + 220, ROW4 + 50, 0)

    # ── 7. 网络连接表 ────────────────────────────────────

    g.add_net("3V3_NET", "J1", "VBUS", "U2", "Vin", "USB 5V → AMS1117")
    g.add_net("3V3_NET", "U2", "Vout", "U1", "3.3V", "AMS1117 → ESP32")
    g.add_net("3V3_NET", "U2", "Vout", "J2", "VCC", "AMS1117 → TFT")
    g.add_net("3V3_NET", "U2", "Vout", "U3", "VCC", "AMS1117 → DHT22")
    g.add_net("3V3_NET", "U2", "Vout", "R1", "2", "AMS1117 → 上拉")
    g.add_net("3V3_NET", "U2", "Vout", "LS1", "+", "AMS1117 → 蜂鸣器")
    g.add_net("GND_NET", "J1", "GND", "U1", "GND", "共地")
    g.add_net("GND_NET", "U2", "GND", "GND", "", "AMS1117接地")
    g.add_net("GND_NET", "U3", "GND", "GND", "", "DHT22接地")
    g.add_net("DHT22_DATA", "U3", "DATA", "U1", "GPIO15", "DHT22 → ESP32")
    g.add_net("DHT22_DATA", "U3", "DATA", "R1", "1", "DATA上拉")
    g.add_net("TFT_CS", "U1", "GPIO5", "J2", "CS", "SPI片选")
    g.add_net("TFT_DC", "U1", "GPIO17", "J2", "DC", "SPI数据/命令")
    g.add_net("TFT_RST", "U1", "GPIO16", "J2", "RST", "屏幕复位")
    g.add_net("TFT_MOSI", "U1", "GPIO23", "J2", "MOSI", "SPI数据")
    g.add_net("TFT_SCK", "U1", "GPIO18", "J2", "SCK", "SPI时钟")
    g.add_net("TFT_BL", "U1", "GPIO4", "J2", "BL", "背光PWM")
    g.add_net("KEY_MENU", "U1", "GPIO13", "SW1", "1", "按键-菜单")
    g.add_net("KEY_PLUS", "U1", "GPIO14", "SW2", "1", "按键-加")
    g.add_net("KEY_MINUS", "U1", "GPIO27", "SW3", "1", "按键-减")
    g.add_net("BUZZER_PWM", "U1", "GPIO25", "LS1", "+", "蜂鸣器PWM")

    # ── 8. 版面装饰 ──────────────────────────────────────

    # 标题块
    g.add_text("04-clock — ESP32 桌面时钟摆件", CANVAS_W//2, 50, 18, COLOR_BLUE, "middle")
    g.add_text("原理图 v1.0 | 2026-07-02 | 立创EDA 可导入", CANVAS_W//2, 80, 12, COLOR_GRAY, "middle")

    # 区域标签
    g.add_text("═══ 电源系统 ═══", 400, 140, 12, COLOR_RED, "middle")
    g.add_text("═══ ESP32 主控 ═══", COL3, ROW2 + 70, 12, COLOR_BLUE, "middle")
    g.add_text("═══ TFT 屏幕 ═══", COL3 - 500, ROW4 + 30, 12, COLOR_PURPLE, "middle")
    g.add_text("═══ DHT22 ═══", COL3 - 450, ROW5 - 40, 12, COLOR_GREEN, "middle")
    g.add_text("═══ 按键模块 ═══", COL4, ROW2 + 160, 12, COLOR_ORANGE, "middle")
    g.add_text("═══ 蜂鸣器 ═══", COL4, ROW4 + 50, 12, COLOR_PURPLE, "middle")

    # 引脚说明
    g.add_text("注: 按键使用 ESP32 内部 INPUT_PULLUP, 可不接外部上拉电阻", 200, ROW2 + 500, 10, COLOR_GRAY)
    g.add_text("注: DHT22 DATA 引脚需 4.7kΩ 上拉到 3.3V", 200, ROW2 + 520, 10, COLOR_GRAY)
    g.add_text("注: 蜂鸣器需串联 100Ω 限流电阻, 或用 NPN 三极管驱动", 200, ROW2 + 540, 10, COLOR_GRAY)

    return g


if __name__ == "__main__":
    g = build_schematic()
    g.export_all()
