#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include <GL/glew.h>
#include "tank.h"

class Crosshair {
public:
    unsigned int VAO, VBO, unifiedShader;
    float x, y;

    Crosshair();
    ~Crosshair();
    void render(const Tank& tank, float currentTime);
    void setPosition(float nx, float ny);
};

#endif
