#include "Level2.h"
#define LEVEL2_WIDTH 23
#define LEVEL2_HEIGHT 8
#define LEVEL2_ENEMY_COUNT 1

unsigned int level2_data[] = {
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


void Level2::Initialize() {
    
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);
    
    state.player = new Entity();
    state.player->height = 0.8;
    state.player->width = 0.5;
    state.player->position = glm::vec3(5, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.0f;
    state.player->jumpPower = 7.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    state.player->entityType = PLAYER;
       
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    // Initialize Enemies
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
    GLuint enemyTexID = Util::LoadTexture("ctg.png");
    

    state.enemies[0].textureID = enemyTexID;
    state.enemies[0].position = glm::vec3(1.0, -5.0, 0);
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].aiType = JUMPER;
    state.enemies[0].aiState = WALKING;
    state.enemies[0].speed = 1.5;
    state.enemies[0].acceleration = glm::vec3(0, -9.81, 0);
    state.enemies[0].jumpPower = 7.0f;
    state.enemies[0].width = 0.8;
    
}

void Level2::Update(float deltaTime) {
    state.player->update(deltaTime, state.map, state.enemies, LEVEL2_ENEMY_COUNT, state.player);
    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].update(deltaTime, state.map, NULL, NULL, state.player);
    }
    
    if (state.player->position.x > 22) {
        state.nextScene = 3;
    }
     
}
void Level2::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->render(program);

    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].render(program);
    }
}
