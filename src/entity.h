#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"

// forward declaration
typedef struct Game Game;

typedef enum {
   ENTITY_PLAYER,
   ENTITY_ENEMY,
} EntityType;

typedef struct {
    Vector3 position;
    Model model;
    BoundingBox box;
    float width;
    float height;
    float depth;
    Vector3 direction;
    Vector3 velocity;
    Color color;
    float mass;
    float rotation;
    float speed;
} EntityBase;

typedef enum {
    NORMAL,
    JUMPING,
    DOUBLE_JUMPING,
    FALLING,
    ON_TOP,
    ROLLING,
    SPRINTING,
    SHOOTING,
} PlayerState;

typedef struct {
    EntityBase base;
    int health;
    PlayerState state;
    float initialJumpVelocity;
} Player;

typedef struct {
    EntityBase base;
    int health;
} Enemy;

typedef struct {
    EntityType type;
    bool isInitialized;
    union {
        Player player;
        Enemy enemy;
    };
} Entity;


void UpdateEntity(Game *game, Entity *entity);
void RenderEntity(Game *game, Entity *entity);

#endif
