#include <stddef.h>
#import "game.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


// private functions
void initWindow(int width, int height, char *title) {
    InitWindow(width, height, title);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowState(FLAG_FULLSCREEN_MODE);

    if(GetMonitorCount() > 1) {
        SetWindowMonitor(1);
    }

    SetTargetFPS(60);

}

// public functions

void InitGame(Game *game) {

}

void UpdateGame(Game *game) {

    Vector2 globalMousePosition = GetMousePosition();

    bool isMouseOnSceneView = globalMousePosition.x >= 0 &&
                              globalMousePosition.x <= game->sceneViewWidth - 40 &&
                              globalMousePosition.y >= 0 &&
                              globalMousePosition.y <= game->sceneViewHeight - 40;

    if (isMouseOnSceneView) {
        UpdateGameCamera(&game->gameCamera, game->playerPosition);
    }

}

void RenderGame(Game *game) {
    BeginTextureMode(game->target);

        ClearBackground(RAYWHITE);

        BeginMode3D(game->gameCamera.camera);

            DrawCube(game->playerPosition, 2.0f, 2.0f, 2.0f, RED);
            DrawCubeWires(game->playerPosition, 2.0f, 2.0f, 2.0f, MAROON);

            DrawGrid(100, 1.0f);

        EndMode3D();

    EndTextureMode();


    BeginDrawing();

        ClearBackground(RAYWHITE);

        GuiGroupBox((Rectangle){ 20, 20, game->sceneViewWidth-40, game->sceneViewHeight-40 }, "Scene View");

        Rectangle sourceRec = { 0, 0, (float)game->target.texture.width, -(float)game->target.texture.height };
        Rectangle destRec = { 40, 40, (float)game->target.texture.width, (float)game->target.texture.height };
        Vector2 origin = { 0, 0 };
        DrawTexturePro(game->target.texture, sourceRec, destRec, origin, 0.0f, WHITE);

        GuiGroupBox((Rectangle){ game->sceneViewWidth, 20, 700, game->sceneViewHeight-40 }, "Control View");

        game->value = GuiSlider((Rectangle){ game->sceneViewWidth + 40, 40, 216, 16 }, TextFormat("%0.2f", game->value), NULL, game->value, 0.0f, 1.0f);

        DrawFPS(40,40);


    EndDrawing();

}

void ShutDown(Game *game) {
    UnloadRenderTexture(game->target);
}

bool IsRunning(Game *game) {
    return !WindowShouldClose();
}

Game CreateGame(char *title) {

    int width = 1900;
    int height = 1080;

    int sceneViewWidth = 1200;
    int sceneViewHeight = 1080;

    initWindow(width, height, title);

    GameCamera gameCamera = CreateGameCamera();

    RenderTexture2D target = LoadRenderTexture(sceneViewWidth - 80, sceneViewHeight - 80);

    Game game = {
            .screenWidth = width,
            .screenHeight = height,
            .screenTitle = title,
            .fps = 60,
            .value = 0.5f,
            .gameCamera = gameCamera,
            .target = target,
            .sceneViewWidth = sceneViewWidth,
            .sceneViewHeight = sceneViewHeight,
            .playerPosition = { 0.0f, 1.0f, 0.0f }
    };

    return game;
}



