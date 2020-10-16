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

void Entity::update(float deltaTime, Entity* objects, int objectCount, Entity* goal)
{
    if (isActive == false) return;
    
    if (won || lost) return;
    
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
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
    
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;               // Move on Y
    checkCollisionsY(objects, objectCount);         // Fix if needed
    
    if (goal) {checkCollisionsY(goal, 1);}
    
    position.x += velocity.x * deltaTime;               // Move on X
    checkCollisionsX(objects, objectCount);         // Fix if needed
    
    if (goal) {checkCollisionsX(goal, 1);}
    
    acceleration.x = 0;
    
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
    if (entityType == PLAYER) {
        vertices = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        texCoords = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    
    } else if (entityType == PLATFORM) {
        if (platformIndex == 0) {
            vertices  = {-5, -0.5, 5, -0.5, 5, 0.5, -5, -0.5, 5, 0.5, -5, 0.5};
            texCoords = {0.0, 1.0, 10.0, 1.0, 10.0, 0.0, 0.0, 1.0, 10.0, 0.0, 0.0, 0.0 };
        
        } else if (platformIndex == 1) {
            vertices  = {-0.5, -3.5, 0.5, -3.5, 0.5, 3.5, -0.5, -3.5, 0.5, 3.5, -0.5, 3.5};
            texCoords = {0.0, 7.0, 1.0, 7.0, 1.0, 0.0, 0.0, 7.0, 1.0, 0.0, 0.0, 0.0};
        } else if (platformIndex == 2) {
            vertices  = {-1, -0.5, 1, -0.5, 1, 0.5, -1, -0.5, 1, 0.5, -1, 0.5};
            texCoords = {0.0, 1.0, 2.0, 1.0, 2.0, 0.0, 0.0, 1.0, 2.0, 0.0, 0.0, 0.0};
        }
        
    } else if (entityType == GOAL) {
        vertices  = {-1, -0.5, 1, -0.5, 1, 0.5, -1, -0.5, 1, 0.5, -1, 0.5};
        texCoords = {0.0, 1.0, 2.0, 1.0, 2.0, 0.0, 0.0, 1.0, 2.0, 0.0, 0.0, 0.0};
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
                if (object->entityType == GOAL) {won = true;}
                if (object->entityType == PLATFORM) {lost = true;}
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
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                if (object->entityType == PLATFORM) {lost = true;}
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                if (object->entityType == PLATFORM) {lost = true;}
            }
        }
    }
}


    

