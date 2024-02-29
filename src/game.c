#include <stddef.h>
#include "game.h"

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
 Entity player = {
     .isInitialized = true,
     .type = ENTITY_PLAYER,
     .player = {.base = {.position = {0.0f, 0.3f, 2.0f},
                         .model = LoadModel("resources/models/player.obj"),
                         .direction = {0.0f, 0.0f, 0.0f},
                         .velocity = {0.0f, 0.0f, 0.0f},
                         .color = BLUE,
                         .mass = 70.0f,
                         .speed = 4.0f},
                .health = 100,
                .jumps = 0}};

 game->entities[0] = player;

}

void UpdateGameEntities(Game *game, bool explorationMode) {
  if (!explorationMode) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
      if (game->entities[i].isInitialized) {
        UpdateEntity(game, &game->entities[i]);
      }
    }
  }
}

void UpdateGame(Game *game) {

  Vector2 globalMousePosition = GetMousePosition();

  bool isMouseOnSceneView =
      globalMousePosition.x >= 0 &&
      globalMousePosition.x <= game->sceneViewWidth - 40 &&
      globalMousePosition.y >= 0 &&
      globalMousePosition.y <= game->sceneViewHeight - 40;

  if (isMouseOnSceneView) {

    if (IsKeyPressed(KEY_C)) {
      game->explorationModeActive = !game->explorationModeActive;
    }

    UpdateGameEntities(game, game->explorationModeActive);
    UpdateGameCamera(&game->gameCamera, game->explorationModeActive, game->playerPosition);

  }
}

void RenderGameEntities(Game *game) {
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (game->entities[i].isInitialized) {
      RenderEntity(game, &game->entities[i]);
    }
  }
}

const char* getPlayerStateString(PlayerState state) {
  switch (state) {
    case NORMAL:
      return "NORMAL";
    case JUMPING:
      return "JUMPING";
    case DOUBLE_JUMPING:
      return "DOUBLE_JUMPING";
    case FALLING:
      return "FALLING";
    case ON_TOP:
      return "ON_TOP";
    case ROLLING:
      return "ROLLING";
    case SPRINTING:
      return "SPRINTING";
    case SHOOTING:
      return "SHOOTING";
  }
}

void RenderGame(Game *game) {
    BeginTextureMode(game->sceneView);

        ClearBackground(RAYWHITE);

        BeginMode3D(game->gameCamera.camera);
            RenderGameEntities(game);
            DrawGrid(100, 1.0f);

        EndMode3D();

    EndTextureMode();


    BeginDrawing();

        ClearBackground(RAYWHITE);

        GuiGroupBox((Rectangle){ 20, 20, (float)game->sceneViewWidth-40, (float)game->sceneViewHeight-40 }, "Scene View");

        Rectangle sourceRec = { 0, 0, (float)game->sceneView.texture.width, -(float)game->sceneView.texture.height };
        Rectangle destRec = { 40, 40, (float)game->sceneView.texture.width, (float)game->sceneView.texture.height };
        Vector2 origin = { 0, 0 };
        DrawTexturePro(game->sceneView.texture, sourceRec, destRec, origin, 0.0f, WHITE);

        GuiGroupBox((Rectangle){ (float)game->sceneViewWidth, 20, 700, (float)game->sceneViewHeight-40 }, "Control View");

        game->value = GuiSlider((Rectangle){ (float)game->sceneViewWidth + 40, 40, 216, 16 }, TextFormat("%0.2f", game->value), NULL, game->value, 0.0f, 1.0f);

        game->explorationModeActive = GuiCheckBox((Rectangle){ (float)game->sceneViewWidth + 20, 120, 16, 16 }, "Exploration Mode", game->explorationModeActive);

        // TODO: fix later, just a way to see the player state
        GuiLabel((Rectangle){ (float)game->sceneViewWidth + 20, 160, 200, 20 }, TextFormat("Player State: %s", getPlayerStateString(game->entities[0].player.state)));

        DrawFPS(40,40);


    EndDrawing();

}

void ShutDown(Game *game) {
    UnloadRenderTexture(game->sceneView);
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

    RenderTexture2D sceneViewTexture = LoadRenderTexture(sceneViewWidth - 80, sceneViewHeight - 80);

    Game game = {
            .screenWidth = width,
            .screenHeight = height,
            .screenTitle = title,
            .fps = 60,
            .value = 0.5f,
            .gameCamera = gameCamera,
            .sceneView = sceneViewTexture,
            .sceneViewWidth = sceneViewWidth,
            .sceneViewHeight = sceneViewHeight,
            .playerPosition = { 0.0f, 1.0f, 0.0f },
            .gravity = -9.8f
    };

    return game;
}
