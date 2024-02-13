#import "camera.h"
#include <stdio.h>
#import "raymath.h"

GameCamera CreateGameCamera() {
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, 10.0f };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type

    GameCamera gameCamera = {
        .camera = camera,
        .speed = 0.09f,
        .zoomSpeed = 0.9f,
        .dragSpeed = 0.003f,

        .pitch = -45.0f,
        .yaw = 90.0f,
        .sensitivity = 0.2f,
        .distance = 6.0f,

        .maxPitch = 89.0f,
        .currentRotationAngle = 0.0f,
        .directionToTarget = {0,0,0}
    };

    return gameCamera;
}

void InitGameCamera(GameCamera *camera) {

}

void UpdateGameCamera(GameCamera *camera, bool explorationMode, Vector3 playerPosition) {
    Vector2 mousePositionDelta = GetMouseDelta();

    Vector3 direction = Vector3Subtract(
            camera->camera.target,
            camera->camera.position);

    // Ignore the Y component (pitch angle) - this will prevent the look target angle from restricting the player movement
    direction.y = 0;

    camera->directionToTarget = Vector3Normalize(direction);

    // Calculate the rotation angles to align the player model with the camera
    camera->currentRotationAngle = atan2(camera->directionToTarget.x, camera->directionToTarget.z) * RAD2DEG;

   if (explorationMode) {
       UpdateExplorationModeCamera(camera, mousePositionDelta);
   } else {
       UpdatePlayerModeCamera(camera, playerPosition, mousePositionDelta);
   }

}

void UpdatePlayerModeCamera(GameCamera *gameCamera, Vector3 playerPosition, Vector2 mousePositionDelta) {
    gameCamera->camera.target = playerPosition;

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        gameCamera->yaw += mousePositionDelta.x * gameCamera->sensitivity;
        gameCamera->pitch -= mousePositionDelta.y * gameCamera->sensitivity;

        if (gameCamera->pitch > gameCamera->maxPitch) gameCamera->pitch = gameCamera->maxPitch;
        if (gameCamera->pitch < -gameCamera->maxPitch) gameCamera->pitch = -gameCamera->maxPitch;
    }

    gameCamera->distance -= GetMouseWheelMove() * gameCamera->zoomSpeed;
    if (gameCamera->distance < 1.0f) gameCamera->distance = 1.0f;
    if (gameCamera->distance > 15.0f) gameCamera->distance = 15.0f;


    Vector3 front;
    front.x = cosf(gameCamera->yaw * DEG2RAD) * cosf(gameCamera->pitch * DEG2RAD);
    front.y = sinf(gameCamera->pitch * DEG2RAD);
    front.z = sinf(gameCamera->yaw * DEG2RAD) * cosf(gameCamera->pitch * DEG2RAD);

    gameCamera->camera.position.x = playerPosition.x - front.x * gameCamera->distance;
    gameCamera->camera.position.y = playerPosition.y - front.y * gameCamera->distance;
    gameCamera->camera.position.z = playerPosition.z - front.z * gameCamera->distance;

}

void UpdateExplorationModeCamera(GameCamera *gameCamera, Vector2 mousePositionDelta) {

    printf("Exploration Mode\n");

    // ASWD free camera move
    if (IsKeyDown(KEY_W)) CameraMoveForward(&gameCamera->camera, gameCamera->speed, true);
    if (IsKeyDown(KEY_A)) CameraMoveRight(&gameCamera->camera, -gameCamera->speed, true);
    if (IsKeyDown(KEY_S)) CameraMoveForward(&gameCamera->camera, -gameCamera->speed, true);
    if (IsKeyDown(KEY_D)) CameraMoveRight(&gameCamera->camera, gameCamera->speed, true);


    // Camera free rotation with Q and E
    if (IsKeyDown(KEY_Q)) CameraYaw(&gameCamera->camera, gameCamera->speed, true);
    if (IsKeyDown(KEY_E)) CameraYaw(&gameCamera->camera, -gameCamera->speed, true);

    // Camera rotation with mouse drag
    if (!IsKeyDown(KEY_LEFT_SHIFT)) {
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            CameraYaw(&gameCamera->camera, -mousePositionDelta.x * gameCamera->dragSpeed, true);
            CameraPitch(&gameCamera->camera, -mousePositionDelta.y * gameCamera->dragSpeed, true, true, false);
        }
    }

    // Camera Zoom
    if (GetMouseWheelMove() != 0) {
        CameraMoveToTarget(&gameCamera->camera, (-GetMouseWheelMove() * gameCamera->zoomSpeed));
    }
}
