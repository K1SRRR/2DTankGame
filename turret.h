#ifndef TURRET_H
#define TURRET_H

#include <glm/glm.hpp> // Za pozicije, rotaciju i transformacije
#include <glm/gtc/matrix_transform.hpp> // Za matrice transformacija
#include <GL/glew.h>

class Turret {
public:
    // Osnovni atributi kupole
    glm::vec2 position;     // Pozicija kupole (x, y) u odnosu na tenk
    float turretAngle;            // Ugao rotacije kupole
    unsigned int texture;
    unsigned int VAO, VBO;
    unsigned int unifiedShader;

    // Konstruktor
    Turret(glm::vec2 initPosition, const char* textureSourceFile);
    ~Turret();

    // Funkcije
    void render();
    void aimAtMouse(float mouseX, float mouseY, int windowWidth, int windowHeight);

    // Matrica modela za rendering
    glm::mat4 getModelMatrix() const;
};

#endif // TURRET_H
