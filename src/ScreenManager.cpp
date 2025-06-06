#include "ScreenManager.h"
#include "LevelLoader.h"
#include <random>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "LevelCreator.h"
#include <functional>

extern bool g_EditorBackToMenu;

ScreenManager::ScreenManager()
    : grid(gridSize, gridSize, marginLeft, marginTop, cellSize),
      generator(4, 4, cellSize, marginLeft, marginTop),  // Исправлен конструктор Generator
      player(0, 0),
      g(std::random_device{}()),
      shouldExit(false),
      levelCreator(),
      // Initialize scrollbar directly (x, y, width, height, totalItems, visibleItems)
      levelScrollBar(680.0f, 350.0f, 20.0f, 400.0f, 0, 10)
{
    levelCreatorUI = std::make_unique<LevelCreatorUI>(levelCreator);
    btnStart = std::make_unique<Button>(362, 350, 300, 70, "Start");
    btnCreator = std::make_unique<Button>(362, 440, 300, 70, "Creator");
    btnExit = std::make_unique<Button>(362, 530, 300, 70, "Exit");
    btnBack = std::make_unique<Button>(362, 800, 300, 60, "Back");
    
    // Set scrollbar callback
    levelScrollBar.SetOnScroll([this](int position) {
        levelLoader.SetScroll(position);
    });

    btnStart->SetOnClick([this]() { levelLoader.Reload(); screen = Screen::LevelSelect; });
    btnCreator->SetOnClick([this]() { screen = Screen::Creator; });
    btnExit->SetOnClick([this]() { shouldExit = true; });
    btnBack->SetOnClick([this]() { screen = Screen::MainMenu; });
    std::uniform_real_distribution<float> xDist(0, 1024), yDist(0, 1024), speedYDist(60, 120), speedXDist(-20, 20), radiusDist(1.5f, 3.5f);
    snowflakes.resize(120);
    for (auto& s : snowflakes) s = {{xDist(g), yDist(g)}, speedYDist(g), speedXDist(g), radiusDist(g)};
}

void ScreenManager::UpdateSnowflakes(float dt) {
    std::uniform_real_distribution<float> xDist(0, 1024);
    for (auto& s : snowflakes) {
        s.pos.y += s.speedY * dt;
        s.pos.x += s.speedX * dt;
        if (s.pos.y > 1024) { s.pos.y = -s.radius; s.pos.x = xDist(g); }
        if (s.pos.x < 0) s.pos.x = 1024;
        if (s.pos.x > 1024) s.pos.x = 0;
    }
}

void ScreenManager::DrawEndScreen(bool win) {
    ClearBackground(BLACK);
    int fontSize = 80;
    const char* text = win ? "You won" : "You Lose";
    Color color = win ? GREEN : RED;
    int textW = MeasureText(text, fontSize);
    DrawText(text, 512 - textW/2, 250, fontSize, color);
    Button btnRestart(362, 450, 300, 70, "Restart");
    Button btnMenu(362, 540, 300, 70, "Menu");
    btnRestart.SetOnClick([this]() {
        if (levelLoader.GetLastLoadedLevel() != -1) {
            levelLoader.SetSelectedLevel(levelLoader.GetLastLoadedLevel());
            screen = Screen::LevelSelect;
        }
    });
    btnMenu.SetOnClick([this]() { screen = Screen::MainMenu; });
    btnRestart.Update();
    btnMenu.Update();
    btnRestart.Draw();
    btnMenu.Draw();
}

// Helper method to check if player is touching any wolf
bool ScreenManager::IsPlayerTouchingAnyWolf() const {
    Vector2 playerPos = player.GetPosition();
    int px = (int)playerPos.x;
    int py = (int)playerPos.y;
    
    for (const auto& wolf : wolves) {
        Vector2 wolfPos = wolf.GetPosition();
        if (px == (int)wolfPos.x && py == (int)wolfPos.y) {
            return true;
        }
    }
    
    return false;
}

void ScreenManager::Update(float dt) {
    UpdateSnowflakes(dt);

    if (shouldExit) {
        CloseWindow();
        return;
    }

    Vector2 mouse = GetMousePosition();
    switch (screen) {
    case Screen::MainMenu:
        btnStart->Update();
        btnCreator->Update();
        btnExit->Update();
        break;
    case Screen::LevelSelect: {
        Button btnBack(312, 200, 180, 48, "Back");
        Button btnPlay(512, 200, 180, 48, "Play");
        btnBack.SetOnClick([this]() { screen = Screen::MainMenu; });
        btnPlay.SetOnClick([this]() {
            if (levelLoader.GetSelectedLevel() != -1) {
                levelLoader.SetLastLoadedLevel(levelLoader.GetSelectedLevel());
                std::string levelName = levelLoader.GetUserLevels()[levelLoader.GetSelectedLevel()];
                std::ifstream in("maps/" + levelName + ".lvl");
                int rows = 0, cols = 0; std::string mapName;
                if (in) {
                    in >> rows >> cols; in.ignore(); std::getline(in, mapName);
                    std::vector<int> objTypes(rows * cols); for (int& t : objTypes) in >> t;
                    std::vector<MapCell> mapCells(rows * cols);
                    for (int i = 0; i < rows * cols; ++i) {
                        int type, mass; in >> type >> mass;
                        Texture* tex = nullptr;
                        switch ((ResourceType)type) {
                            case ResourceType::Tree: tex = &tree_; break;
                            case ResourceType::Coal: tex = &coal_; break;
                            case ResourceType::Stone: tex = &iron_; break;
                            default: tex = nullptr; break;
                        }
                        if (objTypes[i] == (int)EditorObjectType::Generator) tex = &generator_;
                        if (objTypes[i] == (int)EditorObjectType::Player) tex = &player_idle_texture;
                        if (objTypes[i] == (int)EditorObjectType::Wolf) tex = &wolf_texture;
                        mapCells[i].Set((ResourceType)type, mass, tex);
                    }
                    
                    Vector2 playerPos{-1,-1};
                    std::vector<Vector2> wolfPositions; // Store all wolf positions
                    generatorX = -1; generatorY = -1;
                    
                    // Find player, wolves, and generator positions
                    for (int y = 0; y < rows; ++y) {
                        for (int x = 0; x < cols; ++x) {
                            int idx = y * cols + x;
                            // Detect generator (2x2)
                            if (x < cols-1 && y < rows-1 && 
                                objTypes[idx] == (int)EditorObjectType::Generator &&
                                objTypes[idx+1] == (int)EditorObjectType::Generator &&
                                objTypes[idx+cols] == (int)EditorObjectType::Generator &&
                                objTypes[idx+cols+1] == (int)EditorObjectType::Generator) {
                                generatorX = x; 
                                generatorY = y;
                            }
                            
                            // Detect player
                            if (objTypes[idx] == (int)EditorObjectType::Player) {
                                playerPos = {(float)x, (float)y};
                            }
                            
                            // Detect wolves
                            if (objTypes[idx] == (int)EditorObjectType::Wolf) {
                                wolfPositions.push_back({(float)x, (float)y});
                            }
                        }
                    }
                    
                    float marginLeft = 50.0f;
                    float marginTop = 150.0f;
                    float cellSize = std::min((1024.0f - marginLeft - 50.0f) / cols, (1024.0f - marginTop - 50.0f) / rows);
                    grid = Grid(cols, rows, marginLeft, marginTop, cellSize);
                    grid.SetMapCells(mapCells);
                    // Создаем генератор с правильными параметрами
                    generator = Generator(generatorX, generatorY, cellSize, marginLeft, marginTop);
                    
                    grid.SetGenerator(&generator);
                    
                    // Initialize player
                    player = Player((int)playerPos.x, (int)playerPos.y);
                    player.SetGridSize(cols, rows);
                    
                    // Initialize wolves
                    wolves.clear();
                    for (const auto& wolfPos : wolfPositions) {
                        wolves.emplace_back((int)wolfPos.x, (int)wolfPos.y, &wolfStrategy);
                        wolves.back().SetGridSize(cols, rows);
                    }
                    
                    // If no wolves were found, add a default one to avoid crashes
                    if (wolves.empty()) {
                        wolves.emplace_back(0, 0, &wolfStrategy);
                        wolves[0].SetGridSize(cols, rows);
                    }
                    
                    // Initialize game states
                    playerState = std::make_unique<PlayerTurnState>(player, wolves, grid, currentState);
                    wolfState = std::make_unique<WolfTurnState>(player, wolves, grid, currentState);
                    playerState->SetWolfState(wolfState.get());
                    wolfState->SetPlayerState(playerState.get());
                    currentState = playerState.get();
                    
                    player.ResetTurn();
                    // Don't start wolf's turn immediately - let the player move first
                    levelLoader.SetLastLoadedLevel(levelLoader.GetSelectedLevel());
                    screen = Screen::Game;
                }
            }
        });
        btnBack.Update();
        btnPlay.Update();

        // Update the scrollbar with the current number of levels
        levelScrollBar.SetTotalItems(levelLoader.GetUserLevels().size());
        levelScrollBar.Update();
        
        int visibleLevels = 10;
        for (int i = 0; i < visibleLevels; ++i) {
            int idx = levelLoader.GetScroll() + i;
            if (idx >= (int)levelLoader.GetUserLevels().size()) break;
            Button levelBtn(362, 350 + i * 40, 300, 38, levelLoader.GetUserLevels()[idx], 28);
            levelBtn.SetOnClick([this, idx]() { levelLoader.SetSelectedLevel(idx); });
            levelBtn.Update();
        }
        break;
    }
    case Screen::Creator:
        levelCreator.Update();
        if (levelCreatorUI) levelCreatorUI->Update();
        if (g_EditorBackToMenu) { g_EditorBackToMenu = false; screen = Screen::MainMenu; }
        break;
    case Screen::Game: {
        if (currentState) currentState->Update(dt);
        
        // Check for loss if player is touching any wolf
        if (IsPlayerTouchingAnyWolf()) {
            screen = Screen::Lose;
        }
        
        // Update all wolves
        for (auto& wolf : wolves) {
            wolf.Update(dt);
        }
        
        // Check win condition
        if (generatorX != -1 && generatorY != -1) {
            Vector2 p = player.GetPosition();
            int px = (int)p.x, py = (int)p.y;
            for (int dx = -1; dx <= 2; ++dx) {
                for (int dy = -1; dy <= 2; ++dy) {
                    if (dx >= 0 && dx < 2 && dy >= 0 && dy < 2) continue;
                    int cx = generatorX + dx, cy = generatorY + dy;
                    if (px == cx && py == cy) {
                        screen = Screen::Win;
                    }
                }
            }
        }
        break;
    }
    case Screen::Lose:
        break;
    case Screen::Win:
        break;
    }
}

void ScreenManager::Draw() {
    if (shouldExit) return;

    ClearBackground(BLACK);
    DrawTexture(g_background, 0, 0, WHITE);
    for (const auto& s : snowflakes) DrawCircleV(s.pos, s.radius, WHITE);
    Vector2 mouse = GetMousePosition();
    switch (screen) {
    case Screen::MainMenu:
        btnStart->Draw(); btnCreator->Draw(); btnExit->Draw(); break;
    case Screen::LevelSelect: {
        Button btnBack(312, 200, 180, 48, "Back");
        Button btnPlay(512, 200, 180, 48, "Play");
        btnBack.Draw(); btnPlay.Draw();
        DrawText("Select level:", 362, 300, 38, WHITE);
        
        // Draw the level list and scrollbar
        int visibleLevels = 10;
        for (int i = 0; i < visibleLevels; ++i) {
            int idx = levelLoader.GetScroll() + i;
            if (idx >= (int)levelLoader.GetUserLevels().size()) break;
            Button levelBtn(362, 350 + i * 40, 300, 38, levelLoader.GetUserLevels()[idx], 28);
            levelBtn.Draw();
        }
        
        // Draw the scrollbar if there are more levels than visible slots
        if ((int)levelLoader.GetUserLevels().size() > visibleLevels) {
            levelScrollBar.Draw();
        }
        
        if (levelLoader.GetSelectedLevel() != -1)
            DrawText(("Selected: " + levelLoader.GetUserLevels()[levelLoader.GetSelectedLevel()]).c_str(), 362, 750, 28, WHITE);
        break;
    }
    case Screen::Creator:
        if (levelCreatorUI) levelCreatorUI->Draw(); 
        break;
    case Screen::Game: {
        // Draw in the correct order
        grid.Draw();
        
        // Use the grid's margins and cell size for consistent rendering
        float cellSize = grid.GetCellSize();
        float marginLeft = grid.GetMarginLeft();
        float marginTop = grid.GetMarginTop();
        
        // Draw entities
        generator.Draw(cellSize, marginLeft, marginTop); 
        player.Draw(cellSize, marginLeft, marginTop); 
        
        // Draw all wolves
        for (const auto& wolf : wolves) {
            wolf.Draw(cellSize, marginLeft, marginTop);
        }
        break;
    }
    case Screen::Lose:
        DrawEndScreen(false);
        break;
    case Screen::Win:
        DrawEndScreen(true);
        break;
    }
    DrawTexture(cursor_, (int)mouse.x-25, (int)mouse.y-20, WHITE);
}
