#include "Level.h"
#define LEVEL_WIDTH 26
#define LEVEL_HEIGHT 10
#define LEVEL1_ENEMY_COUNT 10
#define LEVEL1_EGG_COUNT 12
#define LEVEL2_ENEMY_COUNT 15
#define LEVEL2_EGG_COUNT 17
#include <time.h>
unsigned int level1_data[] = {
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 64, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 64, 64, 65, 65, 65, 64, 65, 65,
    65, 65, 65, 65, 64, 64, 64, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 64, 64, 64, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 64, 65, 64, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 64, 64, 64, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 64, 65, 65, 64, 64, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 64, 65, 64, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 64, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
};

GLuint level_1_fontID;
GLuint tileSheetTextureID;
GLuint eggTextureID;
int level = 1;
Entity* eggs;
void Level::Initialize(int level) {
    
    state.nextScene = -1;
    
    if (level == 1) {
        tileSheetTextureID = Util::LoadTexture("farmingTilesheetComplete.png");
        level_1_fontID = Util::LoadTexture("font.png");
        state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, level1_data, tileSheetTextureID, 1.0f, 13, 6);
        eggTextureID = Util::LoadTexture("egg.png");
    }
    
    if (level == 1) {
        eggs = new Entity[LEVEL1_EGG_COUNT];
        
        for (int i = 0; i < LEVEL1_EGG_COUNT; ++i) {
            eggs[i].isActive = false;
            eggs[i].entityType = EGG;
            eggs[i].textureID = eggTextureID;
            eggs[i].speed = 4.0f;
            eggs[i].animIndices = NULL;
        }
        
        state.player = new Entity();
        state.player->eggs = eggs;
        state.player->currentEggsLeft = LEVEL1_EGG_COUNT;
        state.player->height = 0.8;
        state.player->width = 0.5;
        state.player->position = glm::vec3(5, -5, 0);
        state.player->movement = glm::vec3(0);
        state.player->acceleration = glm::vec3(0, 0, 0);
        state.player->speed = 2.0f;
        state.player->textureID = tileSheetTextureID;
        state.player->entityType = PLAYER;
           
        state.player->animRight = new int[4] {11, 9, 10};
        state.player->animLeft = new int[4] {6, 7, 8};
        state.player->animUp = new int[4] {5, 3, 4};
        state.player->animDown = new int[4] {0, 1, 2};

        state.player->animIndices = state.player->animRight;
        state.player->animFrames = 3;
        state.player->animIndex = 0;
        state.player->animTime = 0;
        state.player->animCols = 13;
        state.player->animRows = 6;
        
        // Initialize Enemies
        state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
        GLuint enemyTexID = tileSheetTextureID;
        
        srand((unsigned)time(0));
        for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
            state.enemies[i].textureID = enemyTexID;

            int randX = rand() % 20;
            int randY = rand() % 20;

            int positiveOrNegative = rand() % 4;

            if (positiveOrNegative == 0) {
                randX *= -1;
                randY *= -1;
            } else if (positiveOrNegative == 1) {
                randX *= 1;
                randY *= -1;
            } else if (positiveOrNegative == 2) {
                randX *= -1;
                randY *= 1;
            }

            state.enemies[i].position = glm::vec3(randX, randY, 0);
            state.enemies[i].entityType = ENEMY;
            state.enemies[i].aiType = WALKER;
            state.enemies[i].aiState = SEEKING;
            state.enemies[i].speed = 1.0f;
            
            state.enemies[i].animRight = new int[4] {27, 28};
            state.enemies[i].animLeft = new int[4] {27,28};
            state.enemies[i].animUp = new int[4] {27,28};
            state.enemies[i].animDown = new int[4] {30,31};

            state.enemies[i].animIndices = state.enemies[0].animRight;
            state.enemies[i].animFrames = 2;
            state.enemies[i].animIndex = 0;
            state.enemies[i].animTime = 0;
            state.enemies[i].animCols = 13;
            state.enemies[i].animRows = 6;
        }
    } else if (level == 2) {
//        delete [] eggs;
//        delete [] state.enemies;
        eggs = new Entity[LEVEL2_EGG_COUNT];
        
        for (int i = 0; i < LEVEL2_EGG_COUNT; ++i) {
            eggs[i].isActive = false;
            eggs[i].entityType = EGG;
            eggs[i].textureID = eggTextureID;
            eggs[i].speed = 4.0f;
            eggs[i].animIndices = NULL;
        }
        
        state.player->eggs = eggs;
        state.player->currentEggsLeft = LEVEL2_EGG_COUNT;
        state.player->eggIndex = 0;
        
        // Initialize Enemies
        state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
        GLuint enemyTexID = tileSheetTextureID;
        
        srand((unsigned)time(0));
        for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
            state.enemies[i].textureID = enemyTexID;

            int randX = rand() % 20 + 30;
            int randY = rand() % 20;

            int positiveOrNegative = rand() % 4;

            if (positiveOrNegative == 0) {
                randX *= -1;
                randY *= -1;
            } else if (positiveOrNegative == 1) {
                randX *= 1;
                randY *= -1;
            } else if (positiveOrNegative == 2) {
                randX *= -1;
                randY *= 1;
            }

            state.enemies[i].position = glm::vec3(randX, randY, 0);
            state.enemies[i].entityType = ENEMY;
            state.enemies[i].aiType = WALKER;
            state.enemies[i].aiState = SEEKING;
            state.enemies[i].speed = 1.0f;
            
            state.enemies[i].animRight = new int[4] {27, 28};
            state.enemies[i].animLeft = new int[4] {27,28};
            state.enemies[i].animUp = new int[4] {27,28};
            state.enemies[i].animDown = new int[4] {30,31};

            state.enemies[i].animIndices = state.enemies[0].animRight;
            state.enemies[i].animFrames = 2;
            state.enemies[i].animIndex = 0;
            state.enemies[i].animTime = 0;
            state.enemies[i].animCols = 13;
            state.enemies[i].animRows = 6;
        }
    }
}
bool called = false;
void Level::Update(float deltaTime) {
    state.player->update(deltaTime, state.map, state.enemies, LEVEL1_ENEMY_COUNT, state.player);
    
    if (state.player->position.x > 16 && !called) {
        Level::Initialize(2);
        called = true;
    }
    
    int level_egg_count = 0;
    int level_enemy_count = 0;
    
    if (level == 1) {
        level_egg_count = LEVEL1_EGG_COUNT;
        level_enemy_count = LEVEL1_ENEMY_COUNT;
    } else if (level == 2) {
        level_egg_count = LEVEL2_EGG_COUNT;
        level_enemy_count = LEVEL2_ENEMY_COUNT;
    }
    
    for (int i = 0; i < level_egg_count; i++) {
        state.player->eggs[i].update(deltaTime, NULL, state.enemies, level_enemy_count, NULL);
    }
    
    for (int i = 0; i < level_egg_count; i++) {
        state.enemies[i].update(deltaTime, state.map, state.enemies, level_enemy_count, state.player);
        
    }
     
}
void Level::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->render(program);
    
    if (state.player->position.x > 5 && state.player->position.x < 20 && level != 1) {
        Util::DrawText(program, level_1_fontID, "Eggs: " + std::to_string(state.player->currentEggsLeft), .75,-0.5f,glm::vec3(state.player->position.x+3, -1, 0));
    } else if (state.player->position.x > 20) {
        Util::DrawText(program, level_1_fontID, "Eggs: " + std::to_string(state.player->currentEggsLeft), .75,-0.5f,glm::vec3(23, -1, 0));
    } else {
        Util::DrawText(program, level_1_fontID, "Eggs: " + std::to_string(state.player->currentEggsLeft), .75,-0.5f,glm::vec3(8, -1,0));
    }
    
    int level_egg_count = 0;
    int level_enemy_count = 0;
    
    if (level == 1) {
        level_egg_count = LEVEL1_EGG_COUNT;
        level_enemy_count = LEVEL1_ENEMY_COUNT;
    } else if (level == 2 && state.player->position.x > 17) {
        level_egg_count = LEVEL2_EGG_COUNT;
        level_enemy_count = LEVEL2_ENEMY_COUNT;
    }
   
    for (int i = 0; i < level_egg_count; i++) {
        state.player->eggs[i].render(program);
    }

    for (int i = 0; i < level_enemy_count; i++) {
        state.enemies[i].render(program);
    }
}
