#!/usr/bin/env python3
"""Нарезка portal_sheet.png из XDZT.gif (портал выхода).

Копирует исходный GIF в assets/portal.gif и собирает 8x8 лист кадров.
"""
from __future__ import annotations

import shutil
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
SRC_GIF = Path(r"C:\Users\Yura\Downloads\XDZT.gif")
ASSETS = ROOT / "assets"
OUT_GIF = ASSETS / "portal.gif"
OUT_SHEET = ASSETS / "portal_sheet.png"

COLS = 8
CELL_SIZE = 128  # масштаб с 256 для компактного листа


def load_gif_frames(path: Path) -> list[Image.Image]:
    img = Image.open(path)
    n = getattr(img, "n_frames", 1)
    frames: list[Image.Image] = []
    canvas = Image.new("RGBA", img.size, (0, 0, 0, 0))
    for i in range(n):
        img.seek(i)
        frame = img.convert("RGBA")
        disposal = img.info.get("disposal", 0)
        if disposal == 2:
            canvas = Image.new("RGBA", img.size, (0, 0, 0, 0))
        canvas = Image.alpha_composite(canvas, frame)
        frames.append(canvas.copy())
    return frames


def main() -> None:
    if not SRC_GIF.exists():
        raise SystemExit(f"GIF not found: {SRC_GIF}")

    ASSETS.mkdir(parents=True, exist_ok=True)
    shutil.copy2(SRC_GIF, OUT_GIF)

    frames = load_gif_frames(SRC_GIF)
    rows = (len(frames) + COLS - 1) // COLS
    sheet = Image.new("RGBA", (COLS * CELL_SIZE, rows * CELL_SIZE), (0, 0, 0, 0))

    for i, frame in enumerate(frames):
        cell = frame.resize((CELL_SIZE, CELL_SIZE), Image.Resampling.LANCZOS)
        col = i % COLS
        row = i // COLS
        sheet.paste(cell, (col * CELL_SIZE, row * CELL_SIZE), cell)

    sheet.save(OUT_SHEET)
    print(f"frames: {len(frames)}")
    print(f"saved {OUT_GIF}")
    print(f"saved {OUT_SHEET} ({sheet.size[0]}x{sheet.size[1]})")


if __name__ == "__main__":
    main()
