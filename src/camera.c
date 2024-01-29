#import "camera.h"
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

void UpdateGameCamera(GameCamera *camera, Vector3 playerPosition) {
    Vector2 mousePositionDelta = GetMouseDelta();

    Vector3 direction = Vector3Subtract(
            camera->camera.target,
            camera->camera.position);

    // Ignore the Y component (pitch angle) - this will prevent the look target angle from restricting the player movement
    direction.y = 0;

    camera->directionToTarget = Vector3Normalize(direction);

    // Calculate the rotation angles to align the player model with the camera
    camera->currentRotationAngle = atan2(camera->directionToTarget.x, camera->directionToTarget.z) * RAD2DEG;

//    if (game->GetInspector()->IsExplorationModeActive()) {
//        UpdateExplorationModeCamera(camera, mousePositionDelta);
//    } else {
//        UpdatePlayerModeCamera(camera, game->GetPlayer()->GetPosition(), mousePositionDelta);
//    }

    UpdatePlayerModeCamera(camera, playerPosition, mousePositionDelta);

}

void UpdatePlayerModeCamera(GameCamera *camera, Vector3 playerPosition, Vector2 mousePositionDelta) {
    camera->camera.target = playerPosition;

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        camera->yaw += mousePositionDelta.x * camera->sensitivity;
        camera->pitch -= mousePositionDelta.y * camera->sensitivity;

        if (camera->pitch > camera->maxPitch) camera->pitch = camera->maxPitch;
        if (camera->pitch < -camera->maxPitch) camera->pitch = -camera->maxPitch;
    }

    camera->distance -= GetMouseWheelMove() * camera->zoomSpeed;
    if (camera->distance < 1.0f) camera->distance = 1.0f;
    if (camera->distance > 15.0f) camera->distance = 15.0f;


    Vector3 front;
    front.x = cosf(camera->yaw * DEG2RAD) * cosf(camera->pitch * DEG2RAD);
    front.y = sinf(camera->pitch * DEG2RAD);
    front.z = sinf(camera->yaw * DEG2RAD) * cosf(camera->pitch * DEG2RAD);

    camera->camera.position.x = playerPosition.x - front.x * camera->distance;
    camera->camera.position.y = playerPosition.y - front.y * camera->distance;
    camera->camera.position.z = playerPosition.z - front.z * camera->distance;

}


