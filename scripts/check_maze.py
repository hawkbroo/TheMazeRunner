#!/usr/bin/env python3
"""Проверка: квадрат, ширина строк, путь S -> E."""
from collections import deque

from gen_square_maze import generate, verify

LEVEL3 = generate(25, 303)
LEVEL5 = generate(29, 505)


def check_square(name, layout):
    w = len(layout[0])
    h = len(layout)
    if w != h:
        print(f"{name}: not square {w}x{h}")
        return False
    if not verify(layout):
        print(f"{name}: invalid")
        return False
    print(f"{name}: OK {w}x{w} square")
    return True


if __name__ == "__main__":
    ok = check_square("L3", LEVEL3) and check_square("L5", LEVEL5)
    raise SystemExit(0 if ok else 1)
