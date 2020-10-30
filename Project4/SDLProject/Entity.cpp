#include "Entity.h"
#include <vector>
using namespace std;

bool won = false;
bool lost = false;

Entity::Entity()
{
    position = glm::vec3(0);
    speed = 0;
    movement = glm::vec3(0);
    velocity = glm::vec3(0);
    acceleration = glm::vec3(0);
    modelMatrix = glm::mat4(1.0f);
}

bool jumped = false;
void Entity::Jumper(Entity* player) {
    switch (aiState) {
        case IDLE:
            break;
        case WALKING:
            AIWalker(true);
            if (glm::distance(position.x, player->position.x) < 2.0f && glm::distance(position.y, player->position.y) < 1.0f) {
                aiState = ATTACKING;
            }
            break;
        case ATTACKING:
            if (!jumped) {
                jump = true;
                jumped = true;
            }
            if (player->position.x < position.x) {
                movement = glm::vec3(-1, 0, 0);
            } else {
                movement = glm::vec3(1, 0, 0);
            }
            break;
    }
}

void Entity::WaitAndGo(Entity* player) {
    switch (aiState) {
        case IDLE:
            if (glm::distance(position.x, player->position.x) < 3.0f) aiState = WALKING;
            break;
        case WALKING:
            if (glm::distance(position, player->position) > 3.0f) {
                aiState = IDLE;
                movement = glm::vec3(0);
            } else {
                if (player->position.x < position.x) {
                    movement = glm::vec3(-1, 0, 0);
                } else {
                    movement = glm::vec3(1, 0, 0);
                }
             
            }
            break;
        case ATTACKING:
            break;
    }
}

void Entity::AIWalker(bool leftPlatform) {
    if (!happened) {
        movement = glm::vec3(1,0,0);
        happened = true;
    }
    
    if (leftPlatform) {
        if (position.x >= -2.0f) {
            movement *= -1;
        } else if (position.x <= -4.50f) {
            movement *= -1;
        }
    }else {
        if (position.x <= 2.0f) {
            movement *= -1;
        } else if (position.x >= 4.50f) {
            movement *= -1;
        }
    }
    
    
}

void Entity::AI(Entity* player) {
    switch (aiType) {
        case WALKER:
            AIWalker(false);
            break;
        case JUMPER:
            Jumper(player);
            break;
        case WAITER:
            WaitAndGo(player);
    }
}

void Entity::update(float deltaTime, Entity* objects, int objectCount, Entity* enemies, int enemyCount, Entity* player)
{
    if (isActive == false) return;
    
    int deadEnemyCount = 0;
    for (int i = 0; i < enemyCount; i++) {
        
        if (enemies[i].isActive == false) {
            deadEnemyCount += 1;
        }
    }
    
    if (deadEnemyCount == 3) {
        won = true;
    }
    
    if (won || lost) return;
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if (entityType == ENEMY) {
        AI(player);
    }
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
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
    
    if (jump) {
        jump = false;
        velocity.y += jumpPower;
    }

    checkCollisionsY(objects, objectCount);
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;
    checkCollisionsY(objects, objectCount);
    
    if (entityType == PLAYER) {
        checkCollisionsY(enemies, enemyCount);
    }
    
    position.x += velocity.x * deltaTime;
    checkCollisionsX(objects, objectCount);
    
    if (entityType == PLAYER) {
        checkCollisionsX(enemies, enemyCount);
    }
    
    modelMatrix = glm::mat4(1.0f);
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
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
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
    if (entityType == PLAYER || entityType == ENEMY) {
        vertices = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        texCoords = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    
    } else if (entityType == PLATFORM) {
        if (platformIndex == 0) {
            vertices  = {-5, -0.5, 5, -0.5, 5, 0.5, -5, -0.5, 5, 0.5, -5, 0.5};
            texCoords = {0.0, 1.0, 10.0, 1.0, 10.0, 0.0, 0.0, 1.0, 10.0, 0.0, 0.0, 0.0 };
        
        } else if (platformIndex == 1 || platformIndex == 2) {
            vertices  = {-1.5, -0.5, 1.5, -0.5, 1.5, 0.5, -1.5, -0.5, 1.5, 0.5, -1.5, 0.5};
            texCoords = {0.0, 1.0, 3.0, 1.0, 3.0, 0.0, 0.0, 1.0, 3.0, 0.0, 0.0, 0.0};
        } 
        
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
    
    if (isActive == false || other->isActive == false) return false;
    
    float xDist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float yDist = fabs(position.y - other->position.y) - ((height + other->height)) / 2.0f;
    
    if (xDist < 0 && yDist < 0) {
        return true;
    }
    
    return false;
}

 void Entity::checkCollisionsY(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (checkCollision(object))
        {
            
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                
            } else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                if (object->entityType == ENEMY) {
                    object->isActive = false;
                }
            }
            
            if (object->entityType == ENEMY && entityType == PLAYER && collidedBottom == false) {
                lost = true;
            }
        }
    }
}

 void Entity::checkCollisionsX(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (checkCollision(object))
        {
            if (object->entityType == ENEMY && entityType == PLAYER) {
                lost = true;
            }
            
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}


    

