#ifndef MAP_H
#define MAP_H

#include <vector>
#include <glm/glm.hpp>

enum class TileType {
    GRASS,
    WALL,
    BUSH
};

class Map {
public:
    std::vector<std::vector<TileType>> mapMatrix;
    std::vector<std::vector<bool>> bushMatrix;
    unsigned int wallTexture, bushTexture, grassTexture;
    unsigned int VAO, VBO;
    unsigned int shader;

    const int MATRIX_WIDTH = 15;
    const int MATRIX_HEIGHT = 15;
    const float TILE_SIZE = 2.0f / MATRIX_WIDTH;

    Map();
    void initializeMap();
    void render();
    bool checkCollision(glm::vec2 position, float radius);
    void setTile(int x, int y, TileType type);
    TileType getTile(int x, int y);
    float clamp(float value, float min, float max);
    void placeBush(int x, int y);
    void removeBush(int x, int y);
    bool hasBush(int x, int y) const;
    void clearAll();
};

#endif