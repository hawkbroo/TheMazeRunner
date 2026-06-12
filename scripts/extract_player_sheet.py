#!/usr/bin/env python3
"""Нарезка спрайт-листа игрока: 8 направлений x 7 анимационных рядов.

Ряды (сверху вниз на исходнике):
  0 IDLE
  1 RUN A
  2 RUN B
  3 FALL (начало смерти)
  4 DEAD on back
  5 DEAD face down A
  6 DEAD face down B

Колонки (слева направо):
  BACK, BACK_RIGHT, RIGHT, FRONT_RIGHT, FRONT, FRONT_LEFT, LEFT, BACK_LEFT
"""
from __future__ import annotations

from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "assets" / "player_spritesheet.png"
OUT = ROOT / "assets" / "player_sheet.png"

COLS = 8
ROWS = 7
BG = (71, 112, 76)


def is_sprite(px: tuple[int, int, int, int]) -> bool:
    r, g, b, a = px
    if a < 10:
        return False
    return abs(r - BG[0]) + abs(g - BG[1]) + abs(b - BG[2]) > 40 or a > 200


def detect_grid(img: Image.Image) -> tuple[list[tuple[int, int]], list[tuple[int, int]]]:
    w, h = img.size
    pixels = img.load()

    row_energy = [sum(1 for x in range(w) if is_sprite(pixels[x, y])) for y in range(h)]

    def bands(energy: list[int], min_gap: int, min_band: int) -> list[tuple[int, int]]:
        seps: list[tuple[int, int]] = []
        start = None
        for i, value in enumerate(energy):
            if value < 30:
                if start is None:
                    start = i
            elif start is not None:
                seps.append((start, i - 1))
                start = None
        if start is not None:
            seps.append((start, len(energy) - 1))

        bands_out: list[tuple[int, int]] = []
        prev_end = 0
        for sep_start, sep_end in seps:
            if sep_start - prev_end >= min_gap:
                bands_out.append((prev_end, sep_start - 1))
            prev_end = sep_end + 1
        if len(energy) - prev_end >= min_gap:
            bands_out.append((prev_end, len(energy) - 1))

        return [band for band in bands_out if band[1] - band[0] + 1 >= min_band]

    rows = bands(row_energy, min_gap=60, min_band=60)

    # Колонки ищем по первому ряду (IDLE), иначе вертикальные промежутки теряются.
    y0, y1 = rows[0]
    col_energy = [sum(1 for y in range(y0, y1 + 1) if is_sprite(pixels[x, y])) for x in range(w)]
    cols = bands(col_energy, min_gap=40, min_band=40)

    if len(rows) != ROWS or len(cols) != COLS:
        raise RuntimeError(f"expected {ROWS}x{COLS}, got {len(rows)}x{len(cols)}: rows={rows}, cols={cols}")

    return rows, cols


def trim_alpha(cell: Image.Image) -> Image.Image:
    alpha = cell.split()[-1]
    bbox = alpha.getbbox()
    if bbox is None:
        return cell
    return cell.crop(bbox)


def main() -> None:
    src = Image.open(SRC).convert("RGBA")
    rows, cols = detect_grid(src)

    max_w = max(c[1] - c[0] + 1 for c in cols)
    max_h = max(r[1] - r[0] + 1 for r in rows)

    out = Image.new("RGBA", (COLS * max_w, ROWS * max_h), (0, 0, 0, 0))

    for row_idx, (y0, y1) in enumerate(rows):
        for col_idx, (x0, x1) in enumerate(cols):
            cell = trim_alpha(src.crop((x0, y0, x1 + 1, y1 + 1)))
            cx = col_idx * max_w + (max_w - cell.width) // 2
            cy = row_idx * max_h + (max_h - cell.height)
            out.paste(cell, (cx, cy), cell)

    OUT.parent.mkdir(parents=True, exist_ok=True)
    out.save(OUT)
    print(f"grid cells: {max_w}x{max_h}")
    print(f"saved {OUT} ({out.size[0]}x{out.size[1]})")


if __name__ == "__main__":
    main()
