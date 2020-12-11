#include "Entity.h"
#include <vector>
#include <random>
#include "Util.h"
using namespace std;

bool won = false;
bool lost = false;
int lives = 3;
bool wonLevel = false;
bool wonLevel2 = false;

Entity::Entity()
{
    position = glm::vec3(0);
    speed = 0;
    movement = glm::vec3(0);
    velocity = glm::vec3(0);
    acceleration = glm::vec3(0);
    modelMatrix = glm::mat4(1.0f);
}


void Entity::AIWalker(Map* map, Entity* player) {
    switch (aiState) {
        case SEEKING: {
            if (!happened && level == 1) {
                int randIndex = rand() % map->L1Corn.size();
                randCorn = map->L1Corn.at(randIndex);
                happened = true;
            } else if (!happened2 && level == 2 && wonLevel) {
                int randIndex = rand() % map->L2Corn.size();
                randCorn = map->L2Corn.at(randIndex);
                happened2 = true;
            }
            
            movement = randCorn - position;
            
            if (glm::length(movement) > 1.0f) {
                movement = glm::normalize(movement);
            }
            
            if (map->isCorn(position) && level != 3) {
                animIndices = animDown;
            }
            
            if (fabs(player->position.x - position.x) < 2 && fabs(player->position.y - position.y) < 2) {
                aiState = ATTACKING;
                animIndices = animRight;
                speed = 1.25f;
            }
            
            break;
        }
            
        case ATTACKING: {
            movement = player->position - position;
            
            if (level == 3) {
                if (movement.x > 0) animIndices = animRight;
                if (movement.x < 0) animIndices = animLeft;
                if (fabs(player->position.x - position.x) < 2 && fabs(player->position.y - position.y) < 2 && movement.x < 0 ) animIndices = animDown;
                if (fabs(player->position.x - position.x) < 2 && fabs(player->position.y - position.y) < 2 && movement.x > 0 ) animIndices = animUp;
                
                if (entityLives < 15) {
                    speed = 1.0f;
                    int randChance = rand() % 100;
                    
                    if (randChance == 2) {
                        if (entityType == ENEMY) {
                            //movement = glm::vec3(0,0,0);
                            GLuint eggTextureID = Util::LoadTexture("egg.png");
                            Entity* egg = new Entity();
                            egg->entityType = ENEMYEGG;
                            egg->textureID = eggTextureID;
                            egg->speed = 1.0f;
                            egg->animIndices = NULL;
                            egg->width = 0.5f;
                            egg->height = 0.5f;
                            egg->position = position;
                            egg->movement = player->position - position;
                            enemyEggs.emplace_back(egg);
                            
                        }
                    }
                    
                }
                
            }
            
            if (fabs(player->position.x - position.x) > 2 && fabs(player->position.y - position.y) > 2 && !alwaysAttack) {
                aiState = SEEKING;
                speed = 0.75f;
            }
            
            if (glm::length(movement) > 1.0f) {
                movement = glm::normalize(movement);
            }
            break;
        }
    }
    
    
}

void Entity::AI(Map* map, Entity* player) {
    switch (aiType) {
        case WALKER:
            AIWalker(map, player);
            break;
        case JUMPER:
            break;
        case WAITER:
            break;
    }
}

void Entity::throwEgg() {
    if (currentEggsLeft != 0 && entityType == PLAYER) {
        --currentEggsLeft;
        eggs[eggIndex].position = position;
        eggs[eggIndex].isActive = true;
        if (animIndices == animRight) {
            eggs[eggIndex].movement = glm::vec3(1.0f, 0, 0);
        } else if (animIndices == animLeft) {
            eggs[eggIndex].movement = glm::vec3(-1.0f, 0, 0);
        } else if (animIndices == animUp) {
            eggs[eggIndex].movement = glm::vec3(0, 1.0f, 0);
        } else if (animIndices == animDown) {
            eggs[eggIndex].movement = glm::vec3(0, -1.0f, 0);
        }
        ++eggIndex;
    }
}
    

int l1Count;
void Entity::update(float deltaTime, Map* map, Entity* enemies, int enemyCount, Entity* player)
{
    if (isActive == false) return;
    
    if (won || lost) return;
    
    if (entityType == PLAYER) {
        bool allDead = true;
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i].isActive || (!enemies[i].isDead && level == 2)) allDead = false;
        }
        
        if (allDead && level == 1) {
            wonLevel = true;
            l1Count = enemyCount;
            level++;
        } else if (allDead && level == 2 && enemyCount != l1Count) {
            level++;
            wonLevel2 = true;
        } else if (allDead && level == 3 && enemyCount != l1Count) {
            won = true;
        }
        
//        if (!allDead && currentEggsLeft == 0) lost = true;
    }
    
    if (entityType == ENEMY) {
        AI(map, player);
    }
        
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0 || entityType == ENEMY) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    if (thrown && entityType == PLAYER) {
        thrown = false;
        throwEgg();
    }

    velocity = movement * speed;
    //velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;
    //CheckCollisionsY(map);
    //checkCollisionsY(objects, objectCount);
    
    if (entityType == PLAYER || entityType == EGG) {
        checkCollisionsY(enemies, enemyCount);
        if (level == 3)
        checkCollisionsY(NULL, (int)enemies[0].enemyEggs.size(), enemies[0].enemyEggs);
    }
    position.x += velocity.x * deltaTime;
    //CheckCollisionsX(map);
    //checkCollisionsX(objects, objectCount);
    
    if (entityType == PLAYER || entityType == EGG) {
        checkCollisionsX(enemies, enemyCount);
        if (level == 3)
        checkCollisionsX(NULL, (int)enemies[0].enemyEggs.size(), enemies[0].enemyEggs);
    }
    
    modelMatrix = glm::mat4(1.0f);
    
//    if (entityType == ENEMY && level == 3) {
//        glm::mat4 scaleMatrix = glm::scale(3.0f, 3.0f, 1.0f);
//        modelMatrix = modelMatrix * scaleMatrix;
//    }
    
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::drawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::render(ShaderProgram *program) {
    
    if (isActive == false) return;
    
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        drawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    vector<float> vertices;
    vector<float> texCoords;
    if (entityType == PLAYER || entityType == ENEMY || entityType == EGG || entityType == ENEMYEGG) {
        vertices = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        texCoords = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    }
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

bool Entity::checkCollision(Entity* other) {
    
    if (other == NULL) return false;
    
    if (isActive == false || other->isActive == false) return false;
    
    float xDist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float yDist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xDist < 0 && yDist < 0) {
        return true;
    }
    
    return false;
}

 void Entity::checkCollisionsY(Entity *objects, int objectCount, std::vector<Entity*> enemyEggs)
{
    
    if (objects == NULL && entityType == PLAYER) {
        for (int i = 0; i < enemyEggs.size(); i++) {
            if (checkCollision(enemyEggs[i])) {
                    lives--;
                    if (lives == 0) {
                        lost = true;
                    } else {
                        enemyEggs[i]->isActive = false;
                        enemyEggs[i]->isDead = true;
                }
                
            }
        }
        return;
    } else if (objects == NULL) return;
    
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (checkCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                if (object->entityType != ENEMY || entityType != ENEMY) {
                    position.y -= penetrationY;
                    velocity.y = 0;
                    collidedTop = true;
                }
                if (object->entityType == ENEMY && entityType == EGG) {
                    if (object->entityLives == 0) {
                        object->isActive = false;
                        object->isDead = true;
                        isActive = false;
                        collidedTop = true;
                    } else {
                        object->entityLives--;
                        isActive = false;
                    }
                }
                
            } else if (velocity.y < 0) {
                if (object->entityType != ENEMY || entityType != ENEMY) {
                    position.y += penetrationY;
                    velocity.y = 0;
                    collidedBottom = true;
                }
                if (object->entityType == ENEMY && entityType == PLAYER) {
                    object->isActive = false;
                } else if (object->entityType == ENEMY && entityType == EGG) {
                    if (object->entityLives == 0) {
                        object->isActive = false;
                        object->isDead = true;
                        isActive = false;
                        collidedBottom = true;
                    } else {
                        object->entityLives--;
                        isActive = false;
                    }
                }
                
            }
            
            if (object->entityType == ENEMY && entityType == PLAYER) {
                lives--;
                if (lives == 0) {
                    lost = true;
                } else {
                    if (object->entityLives == 0) {
                        object->isActive = false;
                        object->isDead = true;
                    } else {
                        object->entityLives--;
                    }
                }
            }
        }
    }
}

 void Entity::checkCollisionsX(Entity *objects, int objectCount, std::vector<Entity*> enemyEggs)
{
    
    if (objects == NULL && entityType == PLAYER) {
        for (int i = 0; i < enemyEggs.size(); i++) {
            if (checkCollision(enemyEggs[i])) {
                    lives--;
                    if (lives == 0) {
                        lost = true;
                    } else {
                        enemyEggs[i]->isActive = false;
                        enemyEggs[i]->isDead = true;
                }
                
            }
        }
        return;
    } else if (objects == NULL) return;
    
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (checkCollision(object))
        {
            if (object->entityType == ENEMY && entityType == PLAYER) {
                lives--;
                if (lives == 0) {
                    lost = true;
                } else {
                    if (object->entityLives == 0) {
                        object->isActive = false;
                        object->isDead = true;
                    } else {
                        object->entityLives--; 
                    }
                }
            } else if (object->entityType == ENEMYEGG && entityType == PLAYER) {
                lives--;
                if (lives == 0) {
                    lost = true;
                } else {
                    object->isActive = false;
                    object->isDead = true;
            }
        }
            
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                if (object->entityType != ENEMY || entityType != ENEMY) {
                    position.x -= penetrationX;
                    velocity.x = 0;
                    collidedRight = true;
                }
                if (object->entityType == ENEMY && entityType == EGG) {
                    if (object->entityLives == 0) {
                        object->isActive = false;
                        object->isDead = true;
                        isActive = false;
                        collidedRight = true;
                    } else {
                        object->entityLives--;
                        isActive = false;
                    }
                }
            } else if (velocity.x < 0) {
                if (object->entityType != ENEMY || entityType != ENEMY) {
                    position.x += penetrationX;
                    velocity.x = 0;
                    collidedLeft = true;
                }
                if (object->entityType == ENEMY && entityType == EGG) {
                    if (object->entityLives == 0) {
                        object->isActive = false;
                        object->isDead = true;
                        isActive = false;
                        collidedLeft = true;
                    } else {
                        object->entityLives--;
                        isActive = false;
                    }
                }
            }
        }
    }
}

void Entity::CheckCollisionsY(Map *map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);

    float penetration_x = 0; float penetration_y = 0;
    
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) { position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y; velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y; velocity.y = 0;
        collidedTop = true;
    }

    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    
    } else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y; velocity.y = 0; collidedBottom = true;
    
    } else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y; velocity.y = 0; collidedBottom = true;
 
    }
}

void Entity::CheckCollisionsX(Map *map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    
    float penetration_x = 0; float penetration_y = 0;
    
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }

    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
    
}
    

