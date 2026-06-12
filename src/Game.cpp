#include "Game.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

#include "Constants.hpp"
#include "Art.hpp"
#include "Utf.hpp"

namespace {
bool circlesOverlap(sf::Vector2f a, float ra, sf::Vector2f b, float rb) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    const float r = ra + rb;
    return dx * dx + dy * dy < r * r;
}
} // namespace

bool Game::loadLevelMusic() {
    const char* paths[] = {
        "assets/maze_music.mp3",
        "../assets/maze_music.mp3",
    };
    for (const char* path : paths) {
        if (levelMusic_.openFromFile(path)) {
            musicLoaded_ = true;
            levelMusic_.setVolume(70.f);
            return true;
        }
    }
    musicLoaded_ = false;
    return false;
}

bool Game::loadMenuMusic() {
    const char* paths[] = {
        "assets/menu_music.mp3",
        "../assets/menu_music.mp3",
    };
    for (const char* path : paths) {
        if (menuMusic_.openFromFile(path)) {
            menuMusicLoaded_ = true;
            menuMusic_.setVolume(70.f);
            return true;
        }
    }
    menuMusicLoaded_ = false;
    return false;
}

void Game::startMenuMusic() {
    if (!menuMusicLoaded_)
        return;
    menuMusic_.stop();
    menuMusic_.setLoop(true);
    menuMusic_.play();
}

void Game::stopMenuMusic() {
    if (menuMusicLoaded_)
        menuMusic_.stop();
}

void Game::startLevelMusic() {
    stopMenuMusic();
    stopDefeatSound();
    stopVictorySound();
    if (!musicLoaded_)
        return;
    levelMusic_.stop();
    levelMusic_.setLoop(true);
    levelMusic_.play();
}

void Game::stopLevelMusic() {
    if (musicLoaded_)
        levelMusic_.stop();
}

bool Game::loadDefeatSound() {
    const char* paths[] = {
        "assets/defeat_sound.mp3",
        "../assets/defeat_sound.mp3",
    };
    for (const char* path : paths) {
        if (defeatBuffer_.loadFromFile(path)) {
            defeatSound_.setBuffer(defeatBuffer_);
            defeatSound_.setVolume(85.f);
            defeatLoaded_ = true;
            return true;
        }
    }
    defeatLoaded_ = false;
    return false;
}

void Game::playDefeatSound() {
    stopLevelMusic();
    if (!defeatLoaded_)
        return;
    defeatSound_.stop();
    defeatSound_.play();
}

void Game::stopDefeatSound() {
    if (defeatLoaded_)
        defeatSound_.stop();
}

bool Game::loadVictorySound() {
    const char* paths[] = {
        "assets/victory_sound.mp3",
        "../assets/victory_sound.mp3",
    };
    for (const char* path : paths) {
        if (victoryBuffer_.loadFromFile(path)) {
            victorySound_.setBuffer(victoryBuffer_);
            victorySound_.setVolume(85.f);
            victoryLoaded_ = true;
            return true;
        }
    }
    victoryLoaded_ = false;
    return false;
}

void Game::playVictorySound() {
    stopLevelMusic();
    stopDefeatSound();
    if (!victoryLoaded_)
        return;
    victorySound_.stop();
    victorySound_.play();
}

void Game::stopVictorySound() {
    if (victoryLoaded_)
        victorySound_.stop();
}

bool Game::loadFont() {
    // Стандартный шрифт Windows — удобно для курсовой без отдельных ассетов
    const char* paths[] = {
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/times.ttf",
    };
    for (const char* p : paths) {
        if (font_.loadFromFile(p)) {
            fontLoaded_ = true;
            return true;
        }
    }
    fontLoaded_ = false;
    return false;
}

bool Game::init() {
    levels_ = buildLevels();
    loadFont();
    loadLevelMusic();
    loadMenuMusic();
    loadDefeatSound();
    loadVictorySound();
    art_ = makeArt();
    if (art_.playerSheet.loaded && art_.monsterPinkSheet.loaded)
        menuChase_.setSheets(&art_.playerSheet, &art_.monsterPinkSheet);

    window_.create(sf::VideoMode::getDesktopMode(), "The Maze Runner", sf::Style::Fullscreen);
    window_.setVerticalSyncEnabled(true);
    window_.setFramerateLimit(120);

    titleText_.setFont(font_);
    titleText_.setCharacterSize(42);
    titleText_.setFillColor(sf::Color::White);
    titleText_.setString(utf8("THE MAZE RUNNER"));

    hudText_.setFont(font_);
    hudText_.setCharacterSize(18);
    hudText_.setFillColor(sf::Color(220, 220, 230));

    centerText_.setFont(font_);
    centerText_.setCharacterSize(28);
    centerText_.setFillColor(sf::Color::White);

    hintText_.setFont(font_);
    hintText_.setCharacterSize(18);
    hintText_.setFillColor(sf::Color(200, 200, 210));

    menuItemText_.setFont(font_);
    menuItemText_.setCharacterSize(22);
    menuItemText_.setFillColor(sf::Color(220, 220, 230));

    startMenuMusic();
    return true;
}

unsigned Game::fitTileSizeForLevel(const LevelConfig& lvl) const {
    const unsigned base = lvl.tileSize > 0 ? lvl.tileSize : TILE_SIZE;
    if (lvl.layout.empty())
        return base;

    const unsigned mw = static_cast<unsigned>(lvl.layout[0].size());
    const unsigned mh = static_cast<unsigned>(lvl.layout.size());
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const unsigned maxW = static_cast<unsigned>(desktop.width * SCREEN_WIDTH_RATIO);
    const unsigned maxH = static_cast<unsigned>(desktop.height * SCREEN_HEIGHT_RATIO);

    if (mw * base <= maxW && mh * base + WINDOW_MARGIN <= maxH)
        return base;

    const unsigned byW = maxW / mw;
    const unsigned byH = (maxH > WINDOW_MARGIN) ? (maxH - WINDOW_MARGIN) / mh : MIN_TILE_SIZE;
    const unsigned fitted = std::min(byW, byH);
    return std::max(MIN_TILE_SIZE, fitted);
}

void Game::applyGameView() {
    const float ts = static_cast<float>(maze_.tileSize());
    const float mazeW = static_cast<float>(maze_.width()) * ts;
    const float mazeH = static_cast<float>(maze_.height()) * ts + static_cast<float>(WINDOW_MARGIN);

    const float winW = static_cast<float>(window_.getSize().x);
    const float winH = static_cast<float>(window_.getSize().y);

    sf::View view(sf::FloatRect(0.f, 0.f, mazeW, mazeH));
    const float mazeAspect = mazeW / mazeH;
    const float winAspect = winW / winH;
    if (winAspect > mazeAspect)
        view.setSize(mazeH * winAspect, mazeH);
    else
        view.setSize(mazeW, mazeW / winAspect);
    view.setCenter(mazeW * 0.5f, mazeH * 0.5f);
    window_.setView(view);
}

void Game::startLevel(int index) {
    currentLevel_ = index;
    const LevelConfig& lvl = levels_[currentLevel_];

    maze_.setTileSize(fitTileSizeForLevel(lvl));
    if (!maze_.loadFromLayout(lvl.layout) || !maze_.hasPathStartToExit()) {
        // Резерв только если layout битый (разная длина строк) — не упрощённый «змейка»-лабиринт
        const std::vector<std::string> fallback = {
            "################################",
            "#S....#..........#............E#",
            "#.###.#.#######.#.#############.#",
            "#.....#.#.....#.#.#............#",
            "###.###.#.###.#.#.#.###########.#",
            "#......#.#...#.#.#.#...........#",
            "#.####.#.#.#.#.#.#.###########.#",
            "#.#....#.#...#...#.#...........#",
            "#.#.####.#######.#.#.##########.#",
            "#...#...........#.#............#",
            "######.##########.#.############.#",
            "#................#..............#",
            "################################",
        };
        maze_.loadFromLayout(fallback);
    }
    maze_.setFloorTexture(art_.floorTileLoaded ? &art_.floorTile : nullptr);
    if (currentLevel_ >= 0 && currentLevel_ < ArtPack::WALL_TEXTURE_COUNT &&
        art_.wallTileLoaded[currentLevel_])
        maze_.setWallTexture(&art_.wallTiles[currentLevel_]);
    else
        maze_.setWallTexture(nullptr);
    exitPortal_.setPosition(maze_.exitPosition());
    exitPortal_.setSheet(art_.portalSheet.loaded ? &art_.portalSheet : nullptr);
    exitPortal_.setFallbackTexture(&art_.exitMarker);

    player_.reset(maze_.startPosition());
    if (art_.playerSheet.loaded) {
        player_.setSheet(&art_.playerSheet);
    } else {
        player_.setSheet(nullptr);
        player_.setTexture(&art_.player);
    }
    monsters_.clear();
    monsters_.reserve(lvl.monsters.size());

    for (const auto& spawn : lvl.monsters) {
        Monster m;
        m.spawn(spawn, lvl.baseMonsterSpeed, maze_);
        if (art_.monsterPinkSheet.loaded)
            m.setPinkSheet(&art_.monsterPinkSheet);
        monsters_.push_back(m);
    }

    catchingMonster_ = -1;

    timeLeft_ = lvl.timeLimitSeconds;
    state_ = GameState::Playing;
    startLevelMusic();
}

void Game::nextLevel() {
    if (currentLevel_ + 1 >= static_cast<int>(levels_.size())) {
        state_ = GameState::GameComplete;
        hintText_.setString(
            utf8("Поздравляем! Все уровни пройдены!  |  Enter / Esc / M — главное меню"));
        return;
    }
    startLevel(currentLevel_ + 1);
}

void Game::returnToMainMenu() {
    stopLevelMusic();
    stopDefeatSound();
    stopVictorySound();
    startMenuMusic();
    state_ = GameState::MainMenu;
    window_.setView(window_.getDefaultView());
    if (menuSelectedLevel_ >= static_cast<int>(levels_.size()))
        menuSelectedLevel_ = 0;
}

void Game::processEvents() {
    sf::Event event{};
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window_.close();

        if (event.type == sf::Event::KeyPressed) {
            const bool menuKey =
                event.key.code == sf::Keyboard::Enter ||
                event.key.code == sf::Keyboard::Escape ||
                event.key.code == sf::Keyboard::M;

            if (event.key.code == sf::Keyboard::Escape) {
                if (state_ == GameState::MainMenu)
                    window_.close();
                else if (state_ == GameState::LevelComplete || state_ == GameState::Caught ||
                         state_ == GameState::Dying || state_ == GameState::GameOver ||
                         state_ == GameState::GameComplete)
                    returnToMainMenu();
                else if (state_ == GameState::Playing)
                    returnToMainMenu();
                continue;
            }

            if (state_ == GameState::MainMenu) {
                const int n = static_cast<int>(levels_.size());
                if (n > 0) {
                    if (event.key.code == sf::Keyboard::Up)
                        menuSelectedLevel_ = (menuSelectedLevel_ - 1 + n) % n;
                    if (event.key.code == sf::Keyboard::Down)
                        menuSelectedLevel_ = (menuSelectedLevel_ + 1) % n;
                    // Цифры 1–9 (основная клавиатура и NumPad)
                    int digit = -1;
                    if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num9)
                        digit = static_cast<int>(event.key.code) - static_cast<int>(sf::Keyboard::Num1);
                    if (event.key.code >= sf::Keyboard::Numpad1 && event.key.code <= sf::Keyboard::Numpad9)
                        digit = static_cast<int>(event.key.code) - static_cast<int>(sf::Keyboard::Numpad1);
                    if (digit >= 0 && digit < n) menuSelectedLevel_ = digit;
                    if (event.key.code == sf::Keyboard::Enter)
                        startLevel(menuSelectedLevel_);
                }
            }

            if (state_ == GameState::LevelComplete) {
                if (event.key.code == sf::Keyboard::Enter)
                    nextLevel();
                else if (event.key.code == sf::Keyboard::M)
                    returnToMainMenu();
            }

            if ((state_ == GameState::GameComplete || state_ == GameState::GameOver) && menuKey)
                returnToMainMenu();
        }
    }
}

void Game::update(float dt) {
    if (state_ == GameState::MainMenu) {
        menuChase_.update(dt, static_cast<float>(window_.getSize().x));
        return;
    }

    if (state_ == GameState::Playing || state_ == GameState::Caught ||
        state_ == GameState::Dying)
        exitPortal_.update(dt);

    if (state_ == GameState::Caught) {
        if (catchingMonster_ >= 0 && catchingMonster_ < static_cast<int>(monsters_.size())) {
            monsters_[catchingMonster_].updateKill(dt);
            if (monsters_[catchingMonster_].killFinished()) {
                pendingGameOverHint_ = utf8("Монстр поймал вас!  |  Enter / Esc / M — главное меню");
                player_.startDeath();
                state_ = GameState::Dying;
            }
        }
        return;
    }

    if (state_ == GameState::Dying) {
        player_.updateDeath(dt);
        if (player_.deathFinished()) {
            state_ = GameState::GameOver;
            hintText_.setString(pendingGameOverHint_);
        }
        return;
    }

    if (state_ != GameState::Playing) return;

    player_.handleInput();
    player_.update(dt, maze_);

    for (auto& monster : monsters_)
        monster.update(dt, maze_, player_.position(), player_.speed());

    timeLeft_ -= dt;
    if (timeLeft_ <= 0.f) {
        playDefeatSound();
        pendingGameOverHint_ = utf8("Время вышло!  |  Enter / Esc / M — главное меню");
        player_.startDeath();
        state_ = GameState::Dying;
        return;
    }

    for (int i = 0; i < static_cast<int>(monsters_.size()); ++i) {
        const auto& monster = monsters_[i];
        if (circlesOverlap(player_.position(), player_.radius(), monster.position(), monster.radius())) {
            playDefeatSound();
            if (art_.monsterPinkSheet.loaded) {
                catchingMonster_ = i;
                monsters_[i].startKill();
                state_ = GameState::Caught;
            } else {
                pendingGameOverHint_ = utf8("Монстр поймал вас!  |  Enter / Esc / M — главное меню");
                player_.startDeath();
                state_ = GameState::Dying;
            }
            return;
        }
    }

    const sf::Vector2f exitPos = maze_.exitPosition();
    const float dx = player_.position().x - exitPos.x;
    const float dy = player_.position().y - exitPos.y;
    if (std::sqrt(dx * dx + dy * dy) < maze_.tileSize() * 0.45f) {
        playVictorySound();
        if (currentLevel_ + 1 >= static_cast<int>(levels_.size())) {
            state_ = GameState::GameComplete;
            hintText_.setString(
                utf8("Поздравляем! Все уровни пройдены!  |  Enter / Esc / M — главное меню"));
        } else {
            state_ = GameState::LevelComplete;
            hintText_.setString(
                utf8("Уровень пройден!  |  Enter — следующий уровень  |  Esc / M — главное меню"));
        }
    }
}

void Game::drawHud() {
    if (!fontLoaded_) return;

    const LevelConfig& lvl = levels_[currentLevel_];
    std::ostringstream oss;
    oss << lvl.name << "  |  Время: " << static_cast<int>(std::ceil(timeLeft_)) << " с"
        << "  |  Уровень " << (currentLevel_ + 1) << "/" << levels_.size();
    hudText_.setString(utf8(oss.str()));
    hudText_.setPosition(8.f, static_cast<float>(maze_.height() * maze_.tileSize() + 8));
    window_.draw(hudText_);
}

void Game::drawOverlay(const sf::String& message) {
    sf::RectangleShape dim(sf::Vector2f(static_cast<float>(window_.getSize().x),
                                        static_cast<float>(window_.getSize().y)));
    dim.setFillColor(sf::Color(0, 0, 0, 160));
    window_.draw(dim);

    if (!fontLoaded_) return;

    centerText_.setCharacterSize(20);
    centerText_.setString(message);
    const auto bounds = centerText_.getLocalBounds();
    centerText_.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    centerText_.setPosition(window_.getSize().x / 2.f, window_.getSize().y / 2.f);
    window_.draw(centerText_);
}

void Game::drawMenuBackground() {
    if (!art_.menuBackgroundLoaded)
        return;

    const float winW = static_cast<float>(window_.getSize().x);
    const float winH = static_cast<float>(window_.getSize().y);
    const float tw = static_cast<float>(art_.menuBackground.getSize().x);
    const float th = static_cast<float>(art_.menuBackground.getSize().y);

    sf::Sprite bg(art_.menuBackground);
    const float scale = std::max(winW / tw, winH / th);
    bg.setScale(scale, scale);
    bg.setPosition((winW - tw * scale) * 0.5f, (winH - th * scale) * 0.5f);
    window_.draw(bg);
}

void Game::drawNeonMenuText(sf::Text& text, bool selected) {
    text.setOutlineColor(sf::Color(191, 64, 255, selected ? 255 : 220));
    text.setOutlineThickness(selected ? 4.f : 2.5f);
    text.setFillColor(selected ? sf::Color(248, 230, 255) : sf::Color(208, 80, 255));

    if (selected) {
        sf::Text glow = text;
        glow.setFillColor(sf::Color(208, 64, 255, 50));
        glow.setOutlineColor(sf::Color(208, 64, 255, 90));
        glow.setOutlineThickness(10.f);
        glow.setPosition(text.getPosition().x, text.getPosition().y);
        window_.draw(glow);
    }
    window_.draw(text);
}

void Game::drawMainMenu() {
    drawMenuBackground();

    const float winW = static_cast<float>(window_.getSize().x);
    const float winH = static_cast<float>(window_.getSize().y);
    menuChase_.draw(window_, winH * 0.38f, winW);

    if (!fontLoaded_) return;
    const unsigned itemSize = static_cast<unsigned>(std::max(32.f, winH * 0.042f));
    const float lineStep = std::max(48.f, winH * 0.072f);
    menuItemText_.setCharacterSize(itemSize);
    float y = winH * 0.54f;
    for (int i = 0; i < static_cast<int>(levels_.size()); ++i) {
        std::ostringstream oss;
        oss << (i == menuSelectedLevel_ ? "> " : "  ") << (i + 1) << ". " << levels_[i].name;
        menuItemText_.setString(utf8(oss.str()));
        const auto lb = menuItemText_.getLocalBounds();
        menuItemText_.setOrigin(lb.width / 2.f, lb.height / 2.f);
        menuItemText_.setPosition(window_.getSize().x / 2.f, y);
        drawNeonMenuText(menuItemText_, i == menuSelectedLevel_);
        y += lineStep;
    }

    const unsigned hintSize = static_cast<unsigned>(std::max(14.f, winH * 0.018f));
    menuItemText_.setCharacterSize(hintSize);
    menuItemText_.setString(utf8(
        "Стрелки вверх/вниз — выбор  |  1–5 — быстрый выбор  |  Enter — начать  |  Esc — выход"));
    const auto hb = menuItemText_.getLocalBounds();
    menuItemText_.setOrigin(hb.width / 2.f, hb.height / 2.f);
    menuItemText_.setPosition(window_.getSize().x / 2.f, winH - winH * 0.05f);
    drawNeonMenuText(menuItemText_, true);
}

void Game::render() {
    window_.clear(sf::Color(25, 28, 38));

    if (state_ == GameState::MainMenu) {
        drawMainMenu();
        window_.display();
        return;
    }

    if (state_ == GameState::Playing || state_ == GameState::Caught || state_ == GameState::Dying) {
        applyGameView();
        maze_.draw(window_);
        exitPortal_.draw(window_, static_cast<float>(maze_.tileSize()));
        for (int i = 0; i < static_cast<int>(monsters_.size()); ++i) {
            if (state_ == GameState::Caught && i != catchingMonster_)
                continue;
            monsters_[i].draw(window_);
        }
        if (state_ != GameState::Caught)
            player_.draw(window_);
        if (state_ == GameState::Playing)
            drawHud();
    }

    if (state_ == GameState::LevelComplete || state_ == GameState::GameComplete ||
        state_ == GameState::GameOver) {
        window_.setView(window_.getDefaultView());
        drawOverlay(hintText_.getString());
    }

    window_.display();
}

void Game::run() {
    while (window_.isOpen()) {
        const float dt = clock_.restart().asSeconds();
        processEvents();
        update(dt);
        render();
    }
}
