
#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "raygui.h"
#include "camera.h"

#include "entity.h"

#define MAX_ENTITIES 10

typedef struct Game {
    int screenWidth;
    int screenHeight;
    char *screenTitle;
    int fps;
    float value;
    GameCamera gameCamera;
    RenderTexture2D sceneView;
    int sceneViewWidth;
    int sceneViewHeight;
    Vector3 playerPosition;
    bool explorationModeActive;
    Entity entities[MAX_ENTITIES];
    float gravity;
} Game;

Game CreateGame(char *title);
void InitGame(Game *game);
void UpdateGame(Game *game);
void RenderGame(Game *game);
void ShutDown(Game *game);
bool IsRunning(Game *game);


#endif
