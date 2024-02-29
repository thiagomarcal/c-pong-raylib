#include "entity.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include "game.h"

BoundingBox calculateModelBounds(Model *model, Vector3 position, Vector3 offset, float scale, float expand) {
    Mesh mesh = model->meshes[0];
    int vertexCount = mesh.vertexCount;
    Vector3 *transformedVertices = (Vector3 *)malloc(vertexCount * sizeof(Vector3));

    for (int i = 0; i < vertexCount; i++) {
        Vector3 vertex = {
            mesh.vertices[i * 3],
            mesh.vertices[i * 3 + 1],
            mesh.vertices[i * 3 + 2]
        };

        vertex = Vector3Scale(vertex, scale);
        vertex = Vector3Transform(vertex, model->transform);

        vertex.x += position.x + offset.x;
        vertex.y += position.y + offset.y;
        vertex.z += position.z + offset.z;

        transformedVertices[i] = vertex;

    }

    Vector3 minBound = transformedVertices[0]; Vector3 maxBound = transformedVertices[0];

    for (int i = 1; i < vertexCount; i++) {
        minBound = Vector3Min(minBound, transformedVertices[i]);
        maxBound = Vector3Max(maxBound, transformedVertices[i]);
    }

    free(transformedVertices);

    minBound = Vector3Subtract(minBound, (Vector3){expand, expand, expand});
    maxBound = Vector3Add(maxBound, (Vector3){expand, expand, expand});

    BoundingBox box = {minBound, maxBound};
    return box;
}



Vector3 calculatePlayerNetForce(Game *game, Player *player, float diagonalAngle, float maxSpeed)  {

    // starts with gravity
    Vector3 netForce = {0, -player->base.mass * game->gravity, 0.0f};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        float cameraCurrentRotation = game->gameCamera.currentRotationAngle;
        Vector3 cameraDirectionToTarget = game->gameCamera.directionToTarget;

        player->base.rotation = DEG2RAD * cameraCurrentRotation;

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
            float strafeAngle = IsKeyDown(KEY_A) ? diagonalAngle : -diagonalAngle;
            Vector3 diagonalForce = Vector3Scale(Vector3Scale(cameraDirectionToTarget, maxSpeed), player->base.speed);
            diagonalForce = Vector3RotateByAxisAngle(diagonalForce, (Vector3){0, 1, 0}, strafeAngle * DEG2RAD);
            netForce = Vector3Add(netForce, diagonalForce);
        }

        if (IsKeyDown(KEY_S)) {
            Vector3 backwardForce = Vector3Negate(cameraDirectionToTarget);
            backwardForce = Vector3Scale(Vector3Scale(backwardForce, maxSpeed), player->base.speed);
            netForce = Vector3Add(netForce, backwardForce);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector3 forwardForce = Vector3Scale(cameraDirectionToTarget, maxSpeed * player->base.speed);
            netForce = Vector3Add(netForce, forwardForce);
        }
    } else {
      if (IsKeyDown(KEY_W)) {
        Vector3 forwardForce = Vector3Scale(player->base.direction, maxSpeed * player->base.speed);
        netForce = Vector3Add(netForce, forwardForce);
      }

      if (IsKeyDown(KEY_S)) {
        Vector3 backwardForce = Vector3Negate(player->base.direction);
        backwardForce = Vector3Scale(backwardForce, maxSpeed * player->base.speed);
        netForce = Vector3Add(netForce, backwardForce);
      }

      if (IsKeyDown(KEY_A)) player->base.rotation += player->base.speed * GetFrameTime();
      if (IsKeyDown(KEY_D)) player->base.rotation -= player->base.speed * GetFrameTime();

    }

    return netForce;
}

void handlePlayerStateMachine(Game *game, Player *player) {
    switch (player->state) {
        case NORMAL:
            player->base.speed = 4.0f;
            player->initialJumpVelocity = 1.4f;

            if (IsKeyDown(KEY_SPACE)) {
                player->state = JUMPING;
            }
            else if (IsKeyDown(KEY_LEFT_SHIFT)) {
                player->state = SPRINTING;
            }
            break;
        case SPRINTING:

            player->base.speed = 8.0f;

            if (IsKeyDown(KEY_SPACE)) {
                player->state = JUMPING;
            }

            if (IsKeyUp(KEY_LEFT_SHIFT)) {
                player->state = NORMAL;
            }

            break;
        case JUMPING:
            if (IsKeyDown(KEY_SPACE) && player->base.velocity.y < 0.0f) {
                player->base.velocity.y = player->initialJumpVelocity * (player->base.speed / 1.2f);
            }

            if (IsKeyPressed(KEY_SPACE) && player->base.position.y > 0.3f) {
                player->state = DOUBLE_JUMPING;
            }

            if (player->base.velocity.y < 0.0f) {
                player->state = FALLING;
            }

            break;
        case DOUBLE_JUMPING:
            if (IsKeyDown(KEY_SPACE)) {
                player->base.velocity.y = player->initialJumpVelocity * (player->base.speed / 1.2f);
            }

            if (player->base.velocity.y < 0.0f) {
                player->state = FALLING;
            }
            break;
        case FALLING:
            if (IsKeyPressed(KEY_SPACE) && player->base.position.y > 0.3f) {
                player->state = DOUBLE_JUMPING;
            }

            if (player->base.position.y <= 0.3f) {
                player->base.position.y = 0.3f;
                player->state = NORMAL;
            }
            break;
        case ON_TOP:
            if (IsKeyDown(KEY_SPACE)) {
                player->base.velocity.y = player->initialJumpVelocity * (player->base.speed / 1.2f);
                player->state = JUMPING;
            }
            break;
        case ROLLING:
            break;
        case SHOOTING:
            break;
        }
}

void handlePlayerPhysics(Game *game, Player *player, Vector3 *netForce, float damping, float maxSpeed) {

    // Apply model rotation
    player->base.model.transform = MatrixRotateY(player->base.rotation);

    // Apply force-based acceleration
    Vector3 accelerationVector = Vector3Scale(*netForce, 1.0f / player->base.mass);
    Vector3 velocityVector = Vector3Scale(accelerationVector, GetFrameTime());
    player->base.velocity = Vector3Add(player->base.velocity, velocityVector);

    // Apply velocity based position
    Vector3 positionVector = Vector3Scale(player->base.velocity, GetFrameTime());
    player->base.position = Vector3Add(player->base.position, positionVector);

    // Apply damping
    player->base.velocity = Vector3Scale(player->base.velocity, damping);

    // Limit the player's speed
    if (Vector3Length(player->base.velocity) > maxSpeed) {
        player->base.velocity = Vector3Normalize(player->base.velocity);
        player->base.velocity = Vector3Scale(player->base.velocity, maxSpeed);
    }

    // Update model direction based on player's rotation
    player->base.direction = (Vector3) {
        sinf(player->base.rotation),
        0,
        cosf(player->base.rotation)
    };

    // TODO: see if this have any effects later
    player->base.position.y += player->base.velocity.y * GetFrameTime();

}

void updatePlayerMovement(Game *game, Player *player, float diagonalAngle) {
    const float maxSpeed = 180.f;
    const float acceleration = 0.5f;
    const float damping = 0.95f;

    Vector3 netForce = calculatePlayerNetForce(game, player, diagonalAngle, maxSpeed);

    handlePlayerStateMachine(game, player);
    handlePlayerPhysics(game, player, &netForce, damping, maxSpeed);

    // TODO: Implement collision detection
    //

    if (player->base.position.y <= 0.3f) {
        player->base.position.y = 0.3f;
        /* player->state = GROUNDED; */
    }

}

void updatePlayer(Game *game, Player *player) {
    player->base.box = calculateModelBounds(&player->base.model, player->base.position, (Vector3){0, 0, 0}, 0.3f, 0.05f);
    player->base.direction = Vector3Transform((Vector3){0, 0, 1}, player->base.model.transform);
    player->base.velocity.y += (game->gravity * player->base.mass * 0.04f) * GetFrameTime();

    updatePlayerMovement(game, player, 45.0f);

}


void UpdateEntity(Game *game, Entity *entity) {
    switch (entity->type) {
        case ENTITY_PLAYER:
            updatePlayer(game, &entity->player);
            break;
        case ENTITY_ENEMY:
            // update enemy
            break;
    }
}


void RenderEntity(Game *game, Entity *entity) {
    switch (entity->type) {
        case ENTITY_PLAYER:
            DrawModel(entity->player.base.model, entity->player.base.position, 0.3f, entity->player.base.color);
            DrawModelWires(entity->player.base.model, entity->player.base.position, 0.3f, BLACK);
            DrawBoundingBox(entity->player.base.box, GREEN);
            break;
        case ENTITY_ENEMY:
            // render enemy
            break;
    }
}
