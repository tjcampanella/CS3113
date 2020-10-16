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
#include <cstdlib>
#include <stdlib.h>


SDL_Window* displayWindow;
bool gameIsRunning = true;
ShaderProgram program, programTexture;
glm::mat4 viewMatrix, paddleLeftMatrix, paddleRightMatrix, ballMatrix, projectionMatrix;

glm::vec3 paddleLeftPosition = glm::vec3(-4.6f, 0, 0);
glm::vec3 paddleLeftMovement = glm::vec3(0, 0, 0);
glm::vec3 paddleRightPosition = glm::vec3(4.6f, 0, 0);
glm::vec3 paddleRightMovement = glm::vec3(0, 0, 0);
glm::vec3 ballPosition = glm::vec3(0, 0, 0);
glm::vec3 ballMovement = glm::vec3(0, 0, 0);
float paddleSpeed = 2.0f;

int windowHeight = 960;
int windowWidth = 1280;
float orthoHeight = 5.0f;
float orthoWidth = 3.75f;

bool gameStarted = false;
bool gameOver = false;

GLuint loadTexture(const char* filePath) {
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

    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

    paddleLeftMatrix = glm::mat4(1.0f);
    paddleRightMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-orthoHeight, orthoHeight, -orthoWidth, orthoWidth, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

void shutdown() {
    SDL_Quit();
}

void processInput() {
    
    paddleLeftMovement = glm::vec3(0, 0, 0);
    paddleRightMovement = glm::vec3(0, 0, 0);
    
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
                        if (!gameStarted) {
                            ballMovement.x = 1;//(rand() % 4) - 2;
                            ballMovement.y = 1;//(rand() % 4) - 2;
                            gameStarted = true;
                        }
                        break;
                }
                break;
        }
    }
    
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_W]) {
        paddleLeftMovement.y = 1.0f;
        
    } else if (keys[SDL_SCANCODE_S]) {
        paddleLeftMovement.y = -1.0f;
    }
    
    if (keys[SDL_SCANCODE_DOWN]) {
        paddleRightMovement.y = -1.0f;
        
    } else if (keys[SDL_SCANCODE_UP]) {
        paddleRightMovement.y = 1.0f;
    }
    
    if (glm::length(paddleRightMovement) > 1.0f) {
        paddleRightMovement = glm::normalize(paddleRightMovement);
    }
    
    if (glm::length(paddleLeftMovement) > 1.0f) {
        paddleLeftMovement = glm::normalize(paddleLeftMovement);
    }
    
}

bool detectCollision(glm::vec3 ball, glm::vec3 paddle) {
    
    float ballWidth = 0.4;
    float ballHeight = 0.4;
    
    float paddleWidth = 0.6;
    float paddleHeight = 2.0;
    
    float xDiff = abs(ball.x - paddle.x);
    float yDiff = abs(ball.y - paddle.y);
    
    float xDistance = xDiff - (ballWidth + paddleWidth)/2;
    float yDistance = yDiff - (ballHeight + paddleHeight)/2;
    
    if (xDistance < 0 && yDistance < 0) return true;
    
    return false;
}

float lastTicks = 0.0f;

void update() {
    if (!gameOver) {
        
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float deltaTime = ticks - lastTicks;
        lastTicks = ticks;
        
        if(ballPosition.x >= 4.75f || ballPosition.x <= -4.75f) {
            gameOver = true;
        } else if (ballPosition.y >= 3.5 || ballPosition.y <= -3.5) {
            ballMovement.y *= -1;
        }
        
        if (paddleLeftPosition.y > 2.8) {
            paddleLeftPosition.y = 2.8;
        } else if (paddleLeftPosition.y < -2.8) {
            paddleLeftPosition.y = -2.8;
        }
        
        if (paddleRightPosition.y > 2.8) {
            paddleRightPosition.y = 2.8;
        } else if (paddleRightPosition.y < -2.8) {
            paddleRightPosition.y = -2.8;
        }
        
        if (detectCollision(ballPosition, paddleLeftPosition)) {
            ballMovement.x *= -1;
        }
        
        if (detectCollision(ballPosition, paddleRightPosition)) {
            ballMovement.x *= -1;
        }
        
        paddleLeftMatrix = glm::mat4(1.0f);
        paddleLeftPosition += paddleLeftMovement * paddleSpeed * deltaTime;
        paddleLeftMatrix = glm::translate(paddleLeftMatrix, paddleLeftPosition);

        paddleRightMatrix = glm::mat4(1.0f);
        paddleRightPosition += paddleRightMovement * paddleSpeed * deltaTime;
        paddleRightMatrix = glm::translate(paddleRightMatrix, paddleRightPosition);
        
        ballMatrix = glm::mat4(1.0f);
        ballPosition += ballMovement * paddleSpeed * deltaTime;
        ballMatrix = glm::translate(ballMatrix, ballPosition);
    }
    
}

void render() {

    glClear(GL_COLOR_BUFFER_BIT);

    float paddleVertices[]  = { -0.3, -1.0, 0.3, -1.0, 0.3, 1.0, -0.3, -1.0, 0.3, 1.0, -0.3, 1.0 };
    float ballVertices[]  = { -0.2, -0.2, 0.2, -0.2, 0.2, 0.2, -0.2, -0.2, 0.2, 0.2, -0.2, 0.2 };

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddleVertices);
    glEnableVertexAttribArray(program.positionAttribute);

    program.SetModelMatrix(paddleLeftMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddleVertices);
    glEnableVertexAttribArray(program.positionAttribute);

    program.SetModelMatrix(paddleRightMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ballVertices);
    glEnableVertexAttribArray(program.positionAttribute);

    program.SetModelMatrix(ballMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program.positionAttribute);

    SDL_GL_SwapWindow(displayWindow);

}



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
