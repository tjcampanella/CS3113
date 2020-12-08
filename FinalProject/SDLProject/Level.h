#include "Scene.h"
class Level : public Scene {
public:
    void Initialize(int level) override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};
