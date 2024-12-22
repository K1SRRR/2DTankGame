#ifndef TURRET_H
#define TURRET_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

class Turret {
public:
    glm::vec2 position;     // Pozicija kupole (x, y) u odnosu na tenk
    float turretAngle;      // Ugao rotacije kupole
    unsigned int texture;
    unsigned int VAO, VBO;
    unsigned int unifiedShader;

    Turret(glm::vec2 initPosition, const char* textureSourceFile);
    ~Turret();

    void render();
    void aimAtMouse(float mouseX, float mouseY, int windowWidth, int windowHeight);
    void aimAtPlayer(float playerPositionX, float playerPositionY, int windowWidth, int windowHeight);

    glm::mat4 getModelMatrix() const;
};

#endif // TURRET_H
