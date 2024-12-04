#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class Explosion {
public:
    Explosion(glm::vec2 position, float frameTime);

    void update(float deltaTime);
    void render();

    bool isFinished() const { return currentFrame >= totalFrames; }

private:
    glm::vec2 position;
    unsigned int texture;
    unsigned int VAO, VBO;
    unsigned int shader;

    float animationSpeed;
    int currentFrame;
    int totalFrames;
    float frameTimer;
    float scale;

    void initialize();
};

#endif