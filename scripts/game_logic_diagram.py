#!/usr/bin/env python3
"""Рендер схемы логики игры The Maze Runner (PNG)."""
from __future__ import annotations

from pathlib import Path

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyArrowPatch, FancyBboxPatch


def _setup_font() -> None:
    for name in ("Segoe UI", "Arial", "DejaVu Sans"):
        try:
            plt.rcParams["font.family"] = name
            break
        except Exception:
            pass
    plt.rcParams["axes.unicode_minus"] = False


def _box(ax, x, y, w, h, text, fc="#E8F4FC", ec="#2C5282", fontsize=9):
    patch = FancyBboxPatch(
        (x - w / 2, y - h / 2),
        w,
        h,
        boxstyle="round,pad=0.03,rounding_size=0.08",
        linewidth=1.4,
        edgecolor=ec,
        facecolor=fc,
        zorder=2,
    )
    ax.add_patch(patch)
    ax.text(x, y, text, ha="center", va="center", fontsize=fontsize, zorder=3, wrap=True)


def _arrow(ax, x1, y1, x2, y2, label="", color="#444444", rad=0.0, fontsize=7):
    arr = FancyArrowPatch(
        (x1, y1),
        (x2, y2),
        arrowstyle="-|>",
        mutation_scale=12,
        linewidth=1.2,
        color=color,
        connectionstyle=f"arc3,rad={rad}",
        zorder=1,
    )
    ax.add_patch(arr)
    if label:
        mx, my = (x1 + x2) / 2, (y1 + y2) / 2
        ax.text(mx, my + 0.12, label, ha="center", va="bottom", fontsize=fontsize, color=color)


def render_game_logic_diagram(out_path: Path) -> Path:
    _setup_font()
    fig = plt.figure(figsize=(14, 10), facecolor="white")
    gs = fig.add_gridspec(2, 1, height_ratios=[1.05, 2.6], hspace=0.28)

    # --- Главный цикл ---
    ax0 = fig.add_subplot(gs[0])
    ax0.set_xlim(0, 12)
    ax0.set_ylim(0, 2.2)
    ax0.axis("off")
    ax0.set_title("Рисунок 1 — Главный игровой цикл (Game::run)", fontsize=13, fontweight="bold", pad=10)

    loop_boxes = [
        (1.2, "while\n(окно открыто)"),
        (3.4, "dt = clock\n.restart()"),
        (5.6, "process\nEvents()"),
        (7.8, "update(dt)"),
        (10.0, "render()"),
    ]
    for x, txt in loop_boxes:
        _box(ax0, x, 1.1, 1.55, 0.95, txt, fc="#F0FFF4", ec="#276749", fontsize=8.5)
    for i in range(len(loop_boxes) - 1):
        x1 = loop_boxes[i][0] + 0.78
        x2 = loop_boxes[i + 1][0] - 0.78
        _arrow(ax0, x1, 1.1, x2, 1.1, fontsize=7)
    _arrow(ax0, 10.78, 1.1, 1.2, 0.35, label="след. кадр", rad=0.35, color="#276749")
    _box(ax0, 1.2, 0.35, 1.55, 0.55, "цикл", fc="#F0FFF4", ec="#276749", fontsize=8)

    # --- Конечный автомат ---
    ax1 = fig.add_subplot(gs[1])
    ax1.set_xlim(0, 14)
    ax1.set_ylim(0, 10)
    ax1.axis("off")
    ax1.set_title("Рисунок 2 — Диаграмма состояний игры (GameState)", fontsize=13, fontweight="bold", pad=10)

    states = {
        "MainMenu": (2.0, 5.0, "#DBEAFE", "#1D4ED8"),
        "Playing": (7.0, 5.0, "#D1FAE5", "#047857"),
        "Caught": (10.0, 7.2, "#FEF3C7", "#B45309"),
        "Dying": (11.2, 5.0, "#FFEDD5", "#C2410C"),
        "GameOver": (12.5, 2.5, "#FEE2E2", "#B91C1C"),
        "LevelComplete": (7.0, 8.2, "#E0E7FF", "#4338CA"),
        "GameComplete": (7.0, 1.8, "#FCE7F3", "#BE185D"),
    }
    for name, (x, y, fc, ec) in states.items():
        label = name.replace("MainMenu", "MainMenu\n(меню)").replace("Playing", "Playing\n(уровень)")
        _box(ax1, x, y, 2.0, 0.95, label, fc=fc, ec=ec, fontsize=8.5)

    _arrow(ax1, 3.0, 5.0, 6.0, 5.0, "Enter\nстарт уровня", color="#1D4ED8")
    _arrow(ax1, 8.0, 5.55, 8.0, 7.75, "выход\n(не последний)", color="#4338CA")
    _arrow(ax1, 7.0, 7.75, 7.0, 5.55, "Enter\nслед. уровень", color="#4338CA", rad=0.2)
    _arrow(ax1, 7.0, 4.45, 7.0, 2.35, "выход\n(все уровни)", color="#BE185D")
    _arrow(ax1, 8.0, 5.0, 9.0, 7.2, "монстр\nпоймал", color="#B45309")
    _arrow(ax1, 10.0, 6.75, 11.2, 5.45, "kill\nготов", color="#C2410C")
    _arrow(ax1, 8.2, 5.0, 10.2, 5.0, "время\nвышло", color="#C2410C", rad=-0.15)
    _arrow(ax1, 11.2, 4.55, 12.0, 3.0, "смерть\nготова", color="#B91C1C")
    _arrow(ax1, 2.0, 4.55, 2.0, 1.5, "Esc", color="#1D4ED8")
    ax1.text(1.0, 1.5, "закрыть\nокно", fontsize=7, color="#1D4ED8", ha="center")
    _arrow(ax1, 6.0, 4.55, 3.0, 4.55, "Esc", color="#1D4ED8", rad=0.12)
    _arrow(ax1, 7.0, 7.75, 3.0, 5.35, "Esc / M", color="#1D4ED8", rad=0.25)
    _arrow(ax1, 7.0, 1.35, 3.0, 4.65, "Esc / Enter / M", color="#BE185D", rad=-0.25)
    _arrow(ax1, 12.5, 3.0, 3.0, 4.65, "Esc / Enter / M", color="#B91C1C", rad=0.3)

    legend_y = 0.6
    ax1.text(
        0.5,
        legend_y,
        "update(dt): движение игрока и монстров, таймер, проверка коллизий и выхода  |  "
        "processEvents(): клавиатура, выбор уровня, переходы по Enter/Esc/M",
        fontsize=8,
        color="#374151",
        va="center",
    )

    out_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(out_path, dpi=160, bbox_inches="tight", facecolor="white")
    plt.close(fig)
    return out_path


if __name__ == "__main__":
    root = Path(__file__).resolve().parents[1]
    p = render_game_logic_diagram(root / "docs" / "diagrams" / "game_logic.png")
    print(f"Saved: {p}")
