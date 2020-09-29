#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
using namespace std;
#include "vector"
#include "iostream"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/mat4x4.hpp"
#include "ShaderProgram.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


SDL_Window* displayWindow;
bool gameIsRunning = true;
ShaderProgram program;
glm::mat4 viewMatrix, playerMatrix, projectionMatrix, rectMatrix;

glm::vec3 playerPosition = glm::vec3(0, 0, 0);
glm::vec3 playerMovement = glm::vec3(0, 0, 0);
float playerSpeed = 1.0f;
GLuint playerTextureID;

int windowHeight = 480;
int windowWidth = 640;
float orthoHeight = 5.0f;
float orthoWidth = 3.75f;

GLuint loadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    
    return textureID;
}

void init() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project 2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, windowWidth, windowHeight);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    playerMatrix = glm::mat4(1.0f);
    rectMatrix = glm::mat4(1.0f);
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-orthoHeight, orthoHeight, -orthoWidth, orthoWidth, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
    playerTextureID = loadTexture("ctg.png");

    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

void shutdown() {
    SDL_Quit();
}

void processInput() {
    
    playerMovement = glm::vec3(0, 0, 0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_RIGHT:
                        break;
                    case SDLK_LEFT:
                        break;
                    case SDLK_SPACE:
                        break;
                }
                break;
        }
    }
    
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT]) {
        playerMovement.x = -1.0f;
        
    } else if (keys[SDL_SCANCODE_RIGHT]) {
        playerMovement.x = 1.0f;
    }
    
    if (keys[SDL_SCANCODE_DOWN]) {
        playerMovement.y = -1.0f;
        
    } else if (keys[SDL_SCANCODE_UP]) {
        playerMovement.y = 1.0f;
    }
    
    if (glm::length(playerMovement) > 1.0f) {
        playerMovement = glm::normalize(playerMovement);
    }
    
}

float lastTicks = 0.0f;

void update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    playerMatrix = glm::mat4(1.0f);
    playerPosition += playerMovement * playerSpeed * deltaTime;
    playerMatrix = glm::translate(playerMatrix, playerPosition);
}

void render() {
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    program.SetModelMatrix(playerMatrix);
    glBindTexture(GL_TEXTURE_2D, playerTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
    float rectVertices[] = { 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, 0.5 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, rectVertices);
    glEnableVertexAttribArray(program.positionAttribute);
    program.SetModelMatrix(rectMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 4);
    glDisableVertexAttribArray(program.positionAttribute);
    
    SDL_GL_SwapWindow(displayWindow);}

int main(int argc, char* argv[]) {
    init();
    
    while (gameIsRunning) {
        processInput();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
