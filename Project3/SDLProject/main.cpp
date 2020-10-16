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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#include <vector>

#define PLATFORM_COUNT 4

struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* goal;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
GLuint fontTextureID;

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text,
              float size, float spacing, glm::vec3 position) {
    
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    
    std::vector<float> vertices;
    std::vector<float> texCoords;
    
    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        
        texCoords.insert(texCoords.end(), { u, v,
                u, v + height,
                u + width, v,
                u + width, v + height,
                u + width, v,
                u, v + height,
            });
    }
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
  
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}


void initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project 3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    // Initialize Game Objects
    
    fontTextureID = LoadTexture("font1.png");
    
    state.goal = new Entity();
    GLuint goalTextureID = LoadTexture("platformPack_tile053.png");
    state.goal->textureID = goalTextureID;
    state.goal->position = glm::vec3(2, -2.25, 0);
    state.goal->entityType = GOAL;
    state.goal->width = 2;
    state.goal->height = .125;
    state.goal->update(1, NULL, 0, NULL);
    
    
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("platformPack_tile041.png");
    EntityType platformType = PLATFORM;
    EntityType playerType = PLAYER;
    
    //Bottom Platforms
    state.platforms[0].textureID = platformTextureID;
    state.platforms[0].entityType = platformType;
    state.platforms[0].position = glm::vec3(0, -3.25f, 0);
    state.platforms[0].width = 10;
    state.platforms[0].platformIndex = 0;
    
    //Left Platforms
    state.platforms[1].textureID = platformTextureID;
    state.platforms[1].entityType = platformType;
    state.platforms[1].position = glm::vec3(-4.5, .5, 0);
    state.platforms[1].height = 7;
    state.platforms[1].platformIndex = 1;
    
    //Right Platforms
    state.platforms[2].textureID = platformTextureID;
    state.platforms[2].entityType = platformType;
    state.platforms[2].position = glm::vec3(4.5, .5, 0);
    state.platforms[2].height = 7;
    state.platforms[2].platformIndex = 1;
    
    state.platforms[3].textureID = platformTextureID;
    state.platforms[3].entityType = platformType;
    state.platforms[3].position = glm::vec3(0, 0, 0);
    state.platforms[3].width = 2;
    state.platforms[3].platformIndex = 2;
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].update(1, NULL, 0, NULL);
    }
    
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(0, 4.75, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -0.1, 0);
    state.player->speed = 1.0f;
    state.player->textureID = LoadTexture("playerShip2_green.png");
    state.player->entityType = playerType;
}

void processInput() {
    
    state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        // Some sort of action
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->acceleration.x = -0.5f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->acceleration.x = 0.5f;
        
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
        
    }
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, state.goal);
        //state.goal->update(FIXED_TIMESTEP, state.player, 1, NULL);
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
    
}



void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].render(&program);
    }
    
    state.goal->render(&program);
    state.player->render(&program);
    
    if (lost) {
        DrawText(&program, fontTextureID, "Mission Failed", 1, -0.5f, glm::vec3(-3.25, 0, 0));
    } else if (won) {
        DrawText(&program, fontTextureID, "Mission Successful", 1, -0.5f, glm::vec3(-4.0, 0, 0));
    }
    
    SDL_GL_SwapWindow(displayWindow);
}


void shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    initialize();
    
    while (gameIsRunning) {
        processInput();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
