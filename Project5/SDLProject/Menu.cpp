#include "Menu.h"

#define MENU_WIDTH 11
#define MENU_HEIGHT 10

unsigned int menuData[] = {
    1, 2, 1, 3, 2, 3, 3, 1, 1, 1, 3,
    3, 3, 3, 2, 3, 1, 1, 2, 3, 2, 2,
    2, 1, 3, 2, 1, 2, 2, 3, 1, 3, 1,
    3, 2, 3, 2, 1, 2, 3, 2, 1, 2, 2,
    3, 2, 1, 3, 2, 3, 1, 2, 3, 3, 3,
    1, 3, 2, 3, 1, 1, 3, 2, 1, 2, 1,
    2, 3, 3, 3, 3, 1, 2, 3, 3, 1, 1,
    1, 1, 2, 2, 2, 2, 3, 1, 1, 2, 2,
    2, 2, 3, 3, 3, 2, 3, 3, 2, 3, 3,
    3, 2, 1, 3, 1, 3, 1, 2, 2, 3, 1,
};

GLuint fontTextureMenuID;

void Menu::Initialize() {
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menuData, mapTextureID, 1.0f, 4, 1);
    
    fontTextureMenuID = Util::LoadTexture("font1.png");
    
    
}

void Menu::Update(float deltaTime) {
    return;
}
void Menu::Render(ShaderProgram *program) {
    state.map->Render(program);
    
    Util::DrawText(program, fontTextureMenuID, "Don't Touch The Bald Man", .85, -0.5f, glm::vec3(.75, -2.0, 0));
    
    Util::DrawText(program, fontTextureMenuID, "Press enter to begin!", .80, -0.5f, glm::vec3(.75, -6.0, 0));
    
}

