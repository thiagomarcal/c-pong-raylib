#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "rcamera.h"

typedef struct GameCamera {
    Camera3D camera;
    float speed;
    float zoomSpeed;
    float dragSpeed;
    float pitch;
    float yaw;
    float sensitivity;
    float distance;
    float maxPitch;
    float currentRotationAngle;
    Vector3 directionToTarget;
} GameCamera;

GameCamera CreateGameCamera();
void InitGameCamera(GameCamera *camera);
void UpdateGameCamera(GameCamera *camera, bool explorationMode, Vector3 playerPosition);
void RenderGameCamera(GameCamera *camera);
void UpdatePlayerModeCamera(GameCamera *gameCamera, Vector3 playerPosition, Vector2 mousePositionDelta);
void UpdateExplorationModeCamera(GameCamera *gameCamera, Vector2 mousePositionDelta);

#endif
