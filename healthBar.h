#ifndef HEALTHBAR_H
#define HEALTHBAR_H

#include <glm/glm.hpp>
#include <GL/glew.h>
class Tank; // Forward declaration

class HealthBar {
public:
    unsigned int VAO, VBO, unifiedShader;
    float duration;  // Trajanje prikaza nakon udarca
    float startTime; // Vreme kada je tenk poslednji put pogođen

    HealthBar();
    ~HealthBar();

    void render(Tank& tank, float currentTime);
    void setLastHitTime(float time);
};

#endif // HEALTHBAR_H
