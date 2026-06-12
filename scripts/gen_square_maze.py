#!/usr/bin/env python3
"""Генерация связных лабиринтов (DFS, шаг 2)."""
import random
import sys
from collections import deque


def generate_rect(width: int, height: int, seed: int) -> list[str]:
    if width % 2 == 0:
        width += 1
    if height % 2 == 0:
        height += 1
    rng = random.Random(seed)
    g = [["#" for _ in range(width)] for _ in range(height)]
    vis = [[False] * width for _ in range(height)]

    def carve(x: int, y: int) -> None:
        vis[y][x] = True
        g[y][x] = "."
        dirs = [(2, 0), (-2, 0), (0, 2), (0, -2)]
        rng.shuffle(dirs)
        for dx, dy in dirs:
            nx, ny = x + dx, y + dy
            if 1 <= nx < width - 1 and 1 <= ny < height - 1 and not vis[ny][nx]:
                g[y + dy // 2][x + dx // 2] = "."
                carve(nx, ny)

    carve(1, 1)
    g[1][1] = "S"
    g[height - 2][width - 2] = "E"
    return ["".join(row) for row in g]


def generate(size: int, seed: int) -> list[str]:
    return generate_rect(size, size, seed)


def verify(layout: list[str]) -> bool:
    w = len(layout[0])
    h = len(layout)
    sx = sy = ex = ey = None
    for y, r in enumerate(layout):
        if len(r) != w:
            return False
        for x, c in enumerate(r):
            if c == "S":
                sx, sy = x, y
            if c == "E":
                ex, ey = x, y

    def floor(x, y):
        return layout[y][x] != "#"

    q = deque([(sx, sy)])
    vis = {(sx, sy)}
    while q:
        x, y = q.popleft()
        if (x, y) == (ex, ey):
            return True
        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nx, ny = x + dx, y + dy
            if 0 <= nx < w and 0 <= ny < h and floor(nx, ny) and (nx, ny) not in vis:
                vis.add((nx, ny))
                q.append((nx, ny))
    return False


def shorten_layout(layout: list[str], remove_count: int) -> list[str] | None:
    """Удаляет remove_count подряд идущих внутренних рядов, сохраняя путь S->E."""
    h = len(layout)
    best = None
    for start in range(2, h - 2 - remove_count):
        cropped = layout[:start] + layout[start + remove_count :]
        if verify(cropped):
            best = cropped
    return best


def pick_spawns(layout: list[str], n: int = 3) -> list[tuple[int, int]]:
    pts = []
    for y, r in enumerate(layout):
        for x, c in enumerate(r):
            if c not in "#":
                pts.append((x, y))
    sx, sy = next((x, y) for x, y in pts if layout[y][x] == "S")
    ex, ey = next((x, y) for x, y in pts if layout[y][x] == "E")
    floor_pts = [
        (x, y)
        for x, y in pts
        if layout[y][x] == "." and (x, y) not in ((sx, sy), (ex, ey))
    ]
    step = max(1, len(floor_pts) // (n + 1))
    return [floor_pts[step * (i + 1)] for i in range(n)]


def print_cpp(layout: list[str]) -> None:
    for row in layout:
        print(f'                "{row}",')


LEVEL3_ORIGINAL = [
    "#########################",
    "#S#.........#...#.#.....#",
    "#.#.#.#####.#.#.#.#.#.###",
    "#.#.#...#.#...#.#.#.#...#",
    "#.#.###.#.#####.#.#.###.#",
    "#.#.#...#...#.#.#.#...#.#",
    "#.###.###.#.#.#.#.###.#.#",
    "#.....#...#...#.#.....#.#",
    "#######.#######.#.#####.#",
    "#.#.....#.......#...#.#.#",
    "#.#.#.#.#.#########.#.#.#",
    "#...#.#.#.#.......#...#.#",
    "#.###.###.###.###.###.#.#",
    "#.#.#...#...#.#.#...#.#.#",
    "#.#.###.###.#.#.###.###.#",
    "#...#.#...#...#...#...#.#",
    "###.#.###.#####.#.###.#.#",
    "#.#...#.......#.#...#...#",
    "#.###.#.#######.#.#####.#",
    "#.#...#...#...#.#.....#.#",
    "#.#.#####.#.#.#.#####.#.#",
    "#.#.....#.#.#.#...#...#.#",
    "#.#####.#.#.#.###.#.###.#",
    "#.......#...#.....#....E#",
    "#########################",
]


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "crop3":
        for n in (4, 3, 2):
            l3 = shorten_layout(LEVEL3_ORIGINAL, n)
            if l3:
                print(f"// L3 cropped {len(l3)}x{len(l3[0])} removed {n} rows")
                print_cpp(l3)
                print("spawns", pick_spawns(l3))
                break
        raise SystemExit(0)

    w3 = int(sys.argv[1]) if len(sys.argv) > 1 else 25
    h3 = int(sys.argv[2]) if len(sys.argv) > 2 else 21
    w5 = int(sys.argv[3]) if len(sys.argv) > 3 else 31
    h5 = int(sys.argv[4]) if len(sys.argv) > 4 else 23
    seed5 = int(sys.argv[5]) if len(sys.argv) > 5 else 907

    l3 = shorten_layout(LEVEL3_ORIGINAL, 4) or generate_rect(25, 21, 303)
    l5 = generate_rect(w5, h5, seed5)

    assert verify(l3) and verify(l5)
    print(f"// L3 {len(l3)}x{len(l3[0])}")
    print_cpp(l3)
    print("spawns", pick_spawns(l3))
    print(f"// L5 {len(l5)}x{len(l5[0])}")
    print_cpp(l5)
    print("spawns", pick_spawns(l5))
