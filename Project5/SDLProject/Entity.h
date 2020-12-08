#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Map.h"

enum EntityType {PLAYER, PLATFORM, ENEMY};

enum AIType {WALKER, JUMPER, WAITER};
enum AIState {IDLE, WALKING, ATTACKING};

extern bool won;
extern bool lost;
extern int lives;

class Entity {
public:
    
    AIType aiType;
    AIState aiState;
    EntityType entityType;
    int platformIndex = NULL;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    
    float width = 1;
    float height = 1;
    
    bool jump = false;
    float jumpPower = 0;
    
    float speed;
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;

    int *animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;
    
    bool isActive = true;
    
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    
    bool happened = false;
    
    Entity();
    
    bool checkCollision(Entity* other);
    void checkCollisionsY(Entity *objects, int objectCount);
    void checkCollisionsX(Entity *objects, int objectCount);
    void update(float deltaTime, Map* map, Entity* enemies, int enemyCount, Entity* player);
    void render(ShaderProgram *program);
    void drawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    void AI(Entity* player);
    void AIWalker(bool leftPlatform);
    void WaitAndGo(Entity* player);
    void Jumper(Entity* player);
    void CheckCollisionsX(Map *map);
    void CheckCollisionsY(Map *map);

};