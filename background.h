// Background.h
#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Background {
public:
    glm::vec2 position;
    unsigned int wallTexture;
    unsigned int grassTexture;
    unsigned int VAO, VBO;
    unsigned int unifiedShader;

    const float mapWidth = 2.0f;  // Normalized coordinates
    const float mapHeight = 2.0f;
    struct Wall {
        glm::vec2 position;
        glm::vec2 dimensions;
    };
    std::vector<Wall> dynamicWalls;

    Background();
    ~Background();

    void render();
    bool checkWallCollision(glm::vec2 tankPosition, float tankRadius);
    //glm::vec2 resolveWallCollision(glm::vec2 tankPosition, float tankRadius);
    void addWall(glm::vec2 position, float width, float height);

    // Modified collision check
    glm::vec2 resolveWallCollision(glm::vec2 tankPosition, float tankRadius);
    // New method to render dynamic walls
    void renderDynamicWalls();
};

#endif