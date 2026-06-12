#pragma once

// Размер одной клетки лабиринта в пикселях
constexpr unsigned TILE_SIZE = 32;

// Полоса под HUD под лабиринтом
constexpr unsigned WINDOW_MARGIN = 80;

// Доля экрана под окно (заголовок Windows + панель задач)
constexpr float SCREEN_WIDTH_RATIO = 0.92f;
constexpr float SCREEN_HEIGHT_RATIO = 0.85f;
constexpr unsigned MIN_TILE_SIZE = 18;

// Скорости (пикселей в секунду)
constexpr float PLAYER_SPEED = 130.f;
constexpr float MONSTER_CHASE_FACTOR = 0.82f; // чуть медленнее игрока

// Дистанции ИИ преследования
constexpr float CHASE_START_DISTANCE = 220.f;
constexpr float CHASE_STOP_DISTANCE = 140.f;

// Чуть меньший радиус коллизии — плавнее проходятся углы коридоров
constexpr float COLLISION_RADIUS_SCALE = 0.88f;

// Крупность текстур: больше — крупнее рисунок на клетке
constexpr float FLOOR_TEXTURE_ZOOM = 16.f;
constexpr float WALL_TEXTURE_ZOOM = 8.f; // в 2 раза мельче пола
