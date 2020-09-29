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
glm::mat4 viewMatrix, modelMatrix, projectionMatrix, slimeMatrix;

float playerX = 0.0f;
float playerY = 0.0f;
float playerRotate = 0.0f;
GLuint playerTextureID;
float slimeX = 1.0f;
float slimeY = 0.0f;
float slimeRotate = 0.0f;
GLuint slimeTextureID;

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
    displayWindow = SDL_CreateWindow("Project 1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, 1280, 960);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    modelMatrix = glm::mat4(1.0f);
    slimeMatrix = glm::mat4(1.0f);
    
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
//    program.SetColor(0.3f, 0.3f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
    playerTextureID = loadTexture("ctg.png");
    slimeTextureID = loadTexture("slime.png");
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

void shutdown() {
    SDL_Quit();
}

void processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;

void update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    playerX -= 1.0f * deltaTime;
    playerRotate += -90.0f * deltaTime;
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(playerX, playerY, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(playerRotate), glm::vec3(0.0f, 0.0f, 1.0f));
    
//    if (slimeY >= 3.75f) {
//        slimeY -= 1.0f * deltaTime;
//    } else if (slimeY <= -3.75f) {
//        slimeY += -1.0f;
//    } else if (slimeY >= 0 && slimeY <= 3.75f) {
//        slimeY += 1.0f * deltaTime;
//    } else if (slimeY <= 0 && slimeY >= -3.75f) {
//        slimeY -= 1.0f * deltaTime;
//    }
    
    //slimeY += 1.0f * deltaTime;
    slimeMatrix = glm::mat4(1.0f);
    slimeRotate += -90.0f * deltaTime;
    slimeMatrix = glm::scale(slimeMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
    slimeMatrix = glm::translate(slimeMatrix, glm::vec3(slimeX, 0.0f, 0.0f));
    slimeMatrix = glm::rotate(slimeMatrix, glm::radians(slimeRotate), glm::vec3(0.0f, 0.0f, 1.0f));
}

void render() {
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    program.SetModelMatrix(modelMatrix);
    glBindTexture(GL_TEXTURE_2D, playerTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(slimeMatrix);
    glBindTexture(GL_TEXTURE_2D, slimeTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
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
