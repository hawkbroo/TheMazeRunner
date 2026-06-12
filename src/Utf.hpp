#pragma once

#include <SFML/System/String.hpp>

#include <string>

// Перевод UTF-8 строки в sf::String (для русского текста в UI).
inline sf::String utf8(const std::string& s) {
    return sf::String::fromUtf8(s.begin(), s.end());
}
