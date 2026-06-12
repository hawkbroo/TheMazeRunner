from PIL import Image

path = r"C:\Users\Yura\Projects\TheMazeRunner\docs\лабиринт4.jpg"
out = r"C:\Users\Yura\Projects\TheMazeRunner\docs\level4_extracted.txt"

img = Image.open(path).convert("L")
w, h = img.size

best = None
for cells in range(15, 35):
    cw, ch = max(1, w // cells), max(1, h // cells)
    grid = []
    for gy in range(cells):
        row = []
        for gx in range(cells):
            x0 = gx * cw + cw // 4
            y0 = gy * ch + ch // 4
            x1 = (gx + 1) * cw - cw // 4
            y1 = (gy + 1) * ch - ch // 4
            dark = total = 0
            for y in range(y0, max(y0 + 1, y1), max(1, (y1 - y0) // 6)):
                for x in range(x0, max(x0 + 1, x1), max(1, (x1 - x0) // 6)):
                    if img.getpixel((min(x, w - 1), min(y, h - 1))) < 128:
                        dark += 1
                    total += 1
            row.append("#" if dark > total * 0.12 else ".")
        grid.append("".join(row))

    # score: prefer connected border, not all walls/floor
    score = sum(r.count("#") for r in grid)
    if best is None or (200 < score < 800):
        best = (cells, grid, score)

lines = [f"image={w}x{h}"]
if best:
    cells, grid, score = best
    lines.append(f"cells={cells} score={score}")
    lines.extend(grid)
else:
    lines.append("failed")

with open(out, "w", encoding="utf-8") as f:
    f.write("\n".join(lines))
