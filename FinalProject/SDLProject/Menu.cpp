#include "Menu.h"

#define MENU_WIDTH 11
#define MENU_HEIGHT 10

unsigned int menuData[] = {
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 64, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
};

GLuint fontTextureMenuID;

void Menu::Initialize(int level) {
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("farmingTilesheetComplete.png");
    
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menuData, mapTextureID, 1.0f, 13, 6);
    
    fontTextureMenuID = Util::LoadTexture("font.png");
    
    
}

void Menu::Update(float deltaTime) {
    return;
}

void Menu::Render(ShaderProgram *program) {
    state.map->Render(program);
    
    Util::DrawText(program, fontTextureMenuID, "Attack of the Chickens", .85, -0.5f, glm::vec3(.75, -2.0, 0));
    
    Util::DrawText(program, fontTextureMenuID, "Press enter to begin!", .80, -0.5f, glm::vec3(.75, -6.0, 0));
    
}

