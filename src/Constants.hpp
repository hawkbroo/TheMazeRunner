#pragma once

constexpr unsigned TILE_SIZE = 32;           // размер клетки по умолчанию
constexpr unsigned WINDOW_MARGIN = 80;         // место под HUD
constexpr float SCREEN_WIDTH_RATIO = 0.92f;  // доля ширины экрана под лабиринт
constexpr float SCREEN_HEIGHT_RATIO = 0.85f;
constexpr unsigned MIN_TILE_SIZE = 18;

constexpr float PLAYER_SPEED = 130.f;
constexpr float MONSTER_CHASE_FACTOR = 0.82f;  // монстр чуть медленнее игрока

constexpr float CHASE_START_DISTANCE = 220.f;  // начало погони
constexpr float CHASE_STOP_DISTANCE = 140.f;   // гистерезис: конец погони

constexpr float COLLISION_RADIUS_SCALE = 0.88f;  // проще проходить углы

constexpr float FLOOR_TEXTURE_ZOOM = 16.f;
constexpr float WALL_TEXTURE_ZOOM = 8.f;
