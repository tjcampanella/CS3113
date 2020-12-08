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
#include "SDL_mixer.h"

#include "Util.h"
#include "Map.h"
#include <vector>
#include "Level.h"
#include "Menu.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
Mix_Music* music;
Mix_Chunk* whooshSound;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;

Scene* currentScene;
Scene* sceneList[4];

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize(1);
    std::cout<< "Called" << std::endl;
}

void initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Final Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    
    fontTextureID = Util::LoadTexture("font.png");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("farm.mp3");
    Mix_PlayMusic(music, -1);
    
    whooshSound = Mix_LoadWAV("whoosh.wav");
    
    sceneList[0] = new Menu();
    sceneList[1] = new Level();
    SwitchToScene(sceneList[0]);
}

void processInput() {
    
    if ((currentScene != sceneList[0])) {
        currentScene->state.player->movement = glm::vec3(0);
    }
    
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
                        if ((currentScene != sceneList[0]) && currentScene->state.player->currentEggsLeft != 0) {
                            currentScene->state.player->thrown = true;
                            Mix_PlayChannel(-1, whooshSound, 0);
                        }
                        break;
                    case SDLK_RETURN:
                        if (currentScene == sceneList[0]) {
                            currentScene->state.nextScene = 1;
                        }
                        break;
                }
                break; // SDL_KEYDOWN
            default:
                break;
        }
    }

    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (currentScene != sceneList[0]) {
        if (keys[SDL_SCANCODE_LEFT]) {
            currentScene->state.player->movement.x = -1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            currentScene->state.player->movement.x = 1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
        } else if (keys[SDL_SCANCODE_UP]) {
            currentScene->state.player->movement.y = 1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animUp;
            
        } else if (keys[SDL_SCANCODE_DOWN]) {
            currentScene->state.player->movement.y = -1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animDown;
        }
        
        if ((currentScene != sceneList[0])) {
            if (glm::length(currentScene->state.player->movement) > 1.0f) {
                currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
            }
        }
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
        currentScene->Update(FIXED_TIMESTEP);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
    
    viewMatrix = glm::mat4(1.0f);
    
    if (currentScene != sceneList[0]) {
        
        if (currentScene->state.player->position.x > 5 && currentScene->state.player->position.x < 20 && wonLevel) {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
        } else {
            if (currentScene->state.player->position.x >= 20) {
                viewMatrix = glm::translate(viewMatrix, glm::vec3(-20, 3.75, 0));
            } else {
                viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
            }
       }
    } else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }
}



void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetViewMatrix(viewMatrix);
    
    currentScene->Render(&program);
    
    if (currentScene != sceneList[0]) {

        if (currentScene->state.player->position.x > 5 && currentScene->state.player->position.x < 20 && level != 1) {
            Util::DrawText(&program, fontTextureID, "Lives: " + std::to_string(lives), .75,-0.5f,glm::vec3(currentScene->state.player->position.x-4, -1, 0));
        } else if (currentScene->state.player->position.x >= 20) {
            Util::DrawText(&program, fontTextureID, "Lives: " + std::to_string(lives), .75,-0.5f,glm::vec3(16, -1, 0));
        } else {
            Util::DrawText(&program, fontTextureID, "Lives: " + std::to_string(lives), .75,-0.5f,glm::vec3(0.5, -1, 0));
        }

        if (lost) {
            Util::DrawText(&program, fontTextureID, "You Lose", 1, -0.5f, glm::vec3(currentScene->state.player->position.x-2, -3.0f, 0));
        } else if (won) {
            Util::DrawText(&program, fontTextureID, "You Win", 1, -0.5f, glm::vec3(currentScene->state.player->position.x-2, -3.0f, 0));
        } else if (wonLevel && currentScene->state.player->position.x < 9) {
            Util::DrawText(&program, fontTextureID, "Continue ->", 1, -0.5f, glm::vec3(currentScene->state.player->position.x-2, -3.0f, 0));
        }
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
        if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene]);
        render();
    }
    shutdown();
    return 0;
}
