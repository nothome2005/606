#include <memory>
#include "ScreenManager.h"
#include "raylib.h"
#include "resource_dir.h"
#include "resource_manager.h"

extern bool g_EditorBackToMenu;

int main ()
{
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    ShowCursor();
    InitWindow(1024, 1024, "MiniPunk");
    HideCursor();
    SetTargetFPS(60);
    SearchAndSetResourceDir("resources");
    LoadResources();

    std::unique_ptr<ScreenManager> screenManager = std::make_unique<ScreenManager>();

    while (!WindowShouldClose())
    {
        float delta = GetFrameTime();
        screenManager->Update(delta);
        BeginDrawing();
        screenManager->Draw();
        EndDrawing();
    }
    UnloadResources();
    CloseWindow();
    return 0;
}