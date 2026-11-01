#!/usr/bin/env python3
"""Render Rebble App Store screenshots at exact platform resolutions.

Mirrors Mood Ring Face (no watch bezel — store requirement).
Showcases selectable character faces across moods (max 5 per platform).
"""
from __future__ import annotations

import os
import shutil
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "store" / "screenshots"
FACES = ROOT / "resources" / "images" / "faces"

# Exact display sizes (Rebble store). color flag matches PBL_COLOR.
PLATFORMS = {
    "basalt": (144, 168, True),
    "diorite": (144, 168, False),
    "flint": (144, 168, False),
    "emery": (200, 228, True),
}

# Approximate Pebble 64-color GColor values used by mood.c
DUKE_BLUE = (0, 51, 102)
VIVID_CERULEAN = (0, 170, 255)
CHROME_YELLOW = (255, 170, 0)
CANDY_RED = (170, 0, 0)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)

# Up to 5 store shots — mix of people + animals and mood colors.
SCENES = [
    {
        "file": "01-mia-relaxed.png",
        "character": "mia",
        "face": "relaxed.png",
        "label": "RELAXED",
        "bpm": 58,
        "bg": DUKE_BLUE,
        "fg": WHITE,
    },
    {
        "file": "02-kai-calm.png",
        "character": "kai",
        "face": "calm.png",
        "label": "CALM",
        "bpm": 74,
        "bg": VIVID_CERULEAN,
        "fg": WHITE,
        "show_date": True,
    },
    {
        "file": "03-nova-alert.png",
        "character": "nova",
        "face": "alert.png",
        "label": "ALERT",
        "bpm": 96,
        "bg": CHROME_YELLOW,
        "fg": BLACK,
    },
    {
        "file": "04-cat-anxious.png",
        "character": "cat",
        "face": "anxious.png",
        "label": "ANXIOUS",
        "bpm": 118,
        "bg": CANDY_RED,
        "fg": WHITE,
    },
    {
        "file": "05-fox-calm.png",
        "character": "fox",
        "face": "calm.png",
        "label": "CALM",
        "bpm": 72,
        "bg": VIVID_CERULEAN,
        "fg": WHITE,
    },
]


def font(size: int, bold: bool = False) -> ImageFont.ImageFont:
    if bold:
        candidates = [
            "C:/Windows/Fonts/consolab.ttf",
            "C:/Windows/Fonts/lucon.ttf",
            "C:/Windows/Fonts/segoeuib.ttf",
            "C:/Windows/Fonts/arialbd.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        ]
    else:
        candidates = [
            "C:/Windows/Fonts/segoeui.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/calibri.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        ]
    for path in candidates:
        if os.path.isfile(path):
            return ImageFont.truetype(path, size)
    return ImageFont.load_default()


def leco_font(size: int) -> ImageFont.ImageFont:
    candidates = [
        "C:/Windows/Fonts/consolab.ttf",
        "C:/Windows/Fonts/lucon.ttf",
        "C:/Windows/Fonts/courbd.ttf",
        "C:/Windows/Fonts/arialbd.ttf",
    ]
    for path in candidates:
        if os.path.isfile(path):
            return ImageFont.truetype(path, size)
    return font(size, bold=True)


def draw_text_center(draw: ImageDraw.ImageDraw, text: str, y: int, w: int, fnt, fill) -> None:
    bbox = draw.textbbox((0, 0), text, font=fnt)
    tw = bbox[2] - bbox[0]
    draw.text(((w - tw) // 2, y), text, font=fnt, fill=fill)


def to_bw_sprite(src: Image.Image) -> Image.Image:
    rgba = src.convert("RGBA")
    out = Image.new("RGBA", rgba.size, (0, 0, 0, 0))
    px = rgba.load()
    opx = out.load()
    w, h = rgba.size
    for y in range(h):
        for x in range(w):
            r, g, b, a = px[x, y]
            if a < 32:
                continue
            lum = 0.299 * r + 0.587 * g + 0.114 * b
            if lum < 70:
                opx[x, y] = (0, 0, 0, 255)
            else:
                opx[x, y] = (255, 255, 255, 255)
    return out


def load_face(character: str, name: str, color: bool, target: int) -> Image.Image:
    img = Image.open(FACES / character / name).convert("RGBA")
    if not color:
        img = to_bw_sprite(img)
    if img.size[0] != target:
        img = img.resize((target, target), Image.NEAREST)
    return img


def render(w: int, h: int, color: bool, scene: dict) -> Image.Image:
    bg = scene["bg"] if color else BLACK
    fg = scene["fg"] if color else WHITE
    img = Image.new("RGB", (w, h), bg)
    draw = ImageDraw.Draw(img)

    time_size = 36 if w >= 200 else 28
    draw_text_center(draw, "10:42", 2 if h > 180 else 0, w, leco_font(time_size), fg)

    y_after_time = 40 if w >= 200 else 32
    if scene.get("show_date"):
        draw_text_center(draw, "Sun 1 Nov", y_after_time, w, font(max(11, w // 13)), fg)

    face_size = 72 if w >= 200 else 56
    face = load_face(scene["character"], scene["face"], color, face_size)
    face_x = (w - face_size) // 2
    # Match watchface.c: center sprite between time band and caption band
    time_bottom = 64 if scene.get("show_date") else 48
    caption_h = 42
    band_top = time_bottom
    band_bot = h - caption_h
    face_y = band_top + (band_bot - band_top - face_size) // 2
    if face_y < band_top:
        face_y = band_top
    if face_y + face_size > band_bot:
        face_y = band_bot - face_size
    img.paste(face, (face_x, face_y), face)

    if color:
        cx = w // 2
        cy = face_y + face_size // 2 + 2
        r = face_size // 2 + 6
        draw.ellipse((cx - r, cy - r, cx + r, cy + r), outline=fg, width=2)
        draw.ellipse((cx - r - 3, cy - r - 3, cx + r + 3, cy + r + 3), outline=fg, width=1)

    label_y = h - (48 if w >= 200 else 40)
    draw_text_center(draw, scene["label"], label_y, w, font(max(14, w // 11), bold=True), fg)

    if scene["bpm"] is None:
        bpm_text = "No HR"
    else:
        bpm_text = f"{scene['bpm']} BPM"
    draw_text_center(draw, bpm_text, h - (22 if w >= 200 else 18), w, font(max(11, w // 13)), fg)

    return img


def main() -> None:
    if OUT.exists():
        shutil.rmtree(OUT)
    for plat, (w, h, color) in PLATFORMS.items():
        dest = OUT / plat
        dest.mkdir(parents=True, exist_ok=True)
        for scene in SCENES:
            shot = render(w, h, color, scene)
            path = dest / scene["file"]
            shot.save(path, "PNG", optimize=True)
            print(f"wrote {path.relative_to(ROOT)} ({w}x{h})")
    print("Done:", OUT)


if __name__ == "__main__":
    main()
