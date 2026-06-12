#pragma once

#include <SFML/System/String.hpp>

#include <string>

// SFML ожидает UTF-32 внутри sf::String. Наши строки в коде — UTF-8.
// Эта функция корректно переводит UTF-8 -> sf::String.
inline sf::String utf8(const std::string& s) {
    return sf::String::fromUtf8(s.begin(), s.end());
}

