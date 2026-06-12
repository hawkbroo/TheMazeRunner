#!/usr/bin/env python3
"""Нарезка розового монстра из monster_spritesheet.png (прозрачный фон).

Верхний ряд листа, авто-поиск границ кадров по прозрачным промежуткам:
  6  БЕГ
  5  ПРЕВРАЩЕНИЕ В ЗЛОГО
  5  УБИВАНИЕ ИГРОКА

Выход: 6 колонок x 3 ряда.
"""
from __future__ import annotations

from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "assets" / "monster_spritesheet.png"
OUT = ROOT / "assets" / "monster_pink_sheet.png"

COLS = 6
ROWS = 3
RUN_FRAMES = 6
TRANSFORM_FRAMES = 5
KILL_FRAMES = 5
PINK_FRAMES = RUN_FRAMES + TRANSFORM_FRAMES + KILL_FRAMES

GROUPS: list[tuple[int, int]] = [
    (0, RUN_FRAMES),
    (RUN_FRAMES, RUN_FRAMES + TRANSFORM_FRAMES),
    (RUN_FRAMES + TRANSFORM_FRAMES, PINK_FRAMES),
]


def is_sprite(px: tuple[int, int, int, int]) -> bool:
    return px[3] > 30


def detect_top_row(img: Image.Image) -> tuple[int, int]:
    w, h = img.size
    pixels = img.load()
    row_e = [sum(1 for x in range(w) if is_sprite(pixels[x, y])) for y in range(h)]

    seps: list[tuple[int, int]] = []
    start = None
    for y, value in enumerate(row_e):
        if value < 100:
            if start is None:
                start = y
        elif start is not None:
            seps.append((start, y - 1))
            start = None
    if start is not None:
        seps.append((start, h - 1))

    bands: list[tuple[int, int]] = []
    prev_end = 0
    for sep_start, sep_end in seps:
        if sep_start - prev_end >= 15:
            bands.append((prev_end, sep_start - 1))
        prev_end = sep_end + 1
    if h - prev_end >= 15:
        bands.append((prev_end, h - 1))

    candidates = [b for b in bands if b[1] - b[0] + 1 >= 50 and b[0] < 350]
    if not candidates:
        raise RuntimeError("top animation row not found")
    return candidates[0]


def detect_frame_bounds(img: Image.Image, y0: int, y1: int) -> list[tuple[int, int]]:
    w, _ = img.size
    pixels = img.load()
    col_e = [sum(1 for y in range(y0, y1 + 1) if is_sprite(pixels[x, y])) for x in range(w)]

    seps: list[int] = []
    for x in range(2, w - 2):
        if col_e[x] <= 5 and col_e[x] <= col_e[x - 1] and col_e[x] <= col_e[x + 1]:
            if not seps or x - seps[-1] >= 8:
                seps.append(x)

    frames: list[tuple[int, int]] = []
    prev = 0
    for sep in seps:
        if sep - prev >= 20:
            frames.append((prev, sep - 1))
        prev = sep + 1
    if w - prev >= 20:
        frames.append((prev, w - 1))

    frames = [f for f in frames if f[1] - f[0] + 1 >= 30]

    # Убираем узкие артефакты между группами (остатки подписей).
    cleaned: list[tuple[int, int]] = []
    for band in frames:
        if band[1] - band[0] + 1 < 40 and cleaned:
            continue
        cleaned.append(band)

    if len(cleaned) < PINK_FRAMES:
        raise RuntimeError(f"expected {PINK_FRAMES} frames, got {len(cleaned)}: {cleaned}")

    return cleaned[:PINK_FRAMES]


def trim_alpha(cell: Image.Image) -> Image.Image:
    alpha = cell.split()[-1]
    bbox = alpha.getbbox()
    if bbox is None:
        return cell
    return cell.crop(bbox)


def main() -> None:
    src = Image.open(SRC).convert("RGBA")
    y0, y1 = detect_top_row(src)
    bounds = detect_frame_bounds(src, y0, y1)

    cells: list[Image.Image] = []
    for x0, x1 in bounds:
        raw = src.crop((x0, y0, x1 + 1, y1 + 1))
        cells.append(trim_alpha(raw))

    max_w = max(c.width for c in cells)
    max_h = max(c.height for c in cells)
    out = Image.new("RGBA", (COLS * max_w, ROWS * max_h), (0, 0, 0, 0))

    idx = 0
    for row_idx, (start, end) in enumerate(GROUPS):
        for col_idx in range(end - start):
            cell = cells[idx]
            cx = col_idx * max_w + (max_w - cell.width) // 2
            cy = row_idx * max_h + (max_h - cell.height)
            out.paste(cell, (cx, cy), cell)
            idx += 1

    OUT.parent.mkdir(parents=True, exist_ok=True)
    out.save(OUT)
    print(f"row y: {y0}-{y1}")
    print("frame bounds:")
    for i, b in enumerate(bounds):
        print(f"  {i}: x={b[0]}-{b[1]} w={b[1]-b[0]+1}")
    print(f"cell max: {max_w}x{max_h}")
    print(f"saved {OUT} ({out.size[0]}x{out.size[1]})")


if __name__ == "__main__":
    main()
