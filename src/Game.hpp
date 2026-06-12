#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <vector>

#include "LevelData.hpp"
#include "Maze.hpp"
#include "Monster.hpp"
#include "Player.hpp"
#include "ExitPortal.hpp"
#include "MenuChase.hpp"
#include "Art.hpp"

// Состояния конечного автомата игры.
enum class GameState {
    MainMenu,
    Playing,
    Caught,       // монстр догнал, проигрывается анимация атаки
    Dying,        // анимация смерти игрока
    LevelComplete,
    GameComplete,
    GameOver
};

// Главный класс: окно, цикл, меню, уровни, звук.
class Game {
public:
    bool init();  // окно, ресурсы, UI
    void run();   // главный цикл

private:
    void processEvents();
    void update(float dt);
    void render();

    void startLevel(int index);
    void nextLevel();
    void returnToMainMenu();
    void applyGameView();  // камера под размер лабиринта
    unsigned fitTileSizeForLevel(const LevelConfig& lvl) const;

    bool loadFont();
    bool loadLevelMusic();
    bool loadMenuMusic();
    bool loadDefeatSound();
    bool loadVictorySound();
    void startLevelMusic();
    void stopLevelMusic();
    void startMenuMusic();
    void stopMenuMusic();
    void playDefeatSound();
    void stopDefeatSound();
    void playVictorySound();
    void stopVictorySound();

    void drawHud();
    void drawOverlay(const sf::String& message);
    void drawMainMenu();
    void drawMenuBackground();
    void drawNeonMenuText(sf::Text& text, bool selected);

    sf::RenderWindow window_;
    sf::Font font_;
    bool fontLoaded_{false};

    GameState state_{GameState::MainMenu};
    std::vector<LevelConfig> levels_;
    int currentLevel_{0};
    int menuSelectedLevel_{0};

    Maze maze_;
    ExitPortal exitPortal_;
    MenuChase menuChase_;
    Player player_;
    std::vector<Monster> monsters_;

    float timeLeft_{0.f};
    sf::Clock clock_;
    sf::String pendingGameOverHint_;
    int catchingMonster_{-1};

    sf::Text titleText_;
    sf::Text hudText_;
    sf::Text centerText_;
    sf::Text hintText_;
    sf::Text menuItemText_;

    ArtPack art_;

    sf::Music levelMusic_;
    bool musicLoaded_{false};

    sf::Music menuMusic_;
    bool menuMusicLoaded_{false};

    sf::SoundBuffer defeatBuffer_;
    sf::Sound defeatSound_;
    bool defeatLoaded_{false};

    sf::SoundBuffer victoryBuffer_;
    sf::Sound victorySound_;
    bool victoryLoaded_{false};
};
