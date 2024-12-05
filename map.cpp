#include "Map.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "globalFunctions.h"

const char* vertexMapShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;

        uniform mat4 uModel;
        //uniform mat4 uProjection;

        out vec2 TexCoord;

        void main() {
            gl_Position = uModel * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

// Fragment shader source
const char* fragmentMapShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D uTexture;

        void main() {
            FragColor = texture(uTexture, TexCoord);
        }
    )";

Map::Map() {
    // Initialize map matrix
    mapMatrix.resize(MATRIX_HEIGHT, std::vector<TileType>(MATRIX_WIDTH, TileType::GRASS));

    // Default walls around edges
    for (int x = 0; x < MATRIX_WIDTH; x++) {
        mapMatrix[0][x] = TileType::WALL;
        mapMatrix[MATRIX_HEIGHT - 1][x] = TileType::WALL;
    }
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
        mapMatrix[y][0] = TileType::WALL;
        mapMatrix[y][MATRIX_WIDTH - 1] = TileType::WALL;
    }

    // Shader setup (similar to previous examples)
    shader = createShader(vertexMapShaderSource, fragmentMapShaderSource);

    // Setup VAO and VBO for rendering tiles
    float vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f,  // Bottom-left
        1.0f, 0.0f, 1.0f, 0.0f,  // Bottom-right
        0.0f, 1.0f, 0.0f, 1.0f,  // Top-left
        1.0f, 1.0f, 1.0f, 1.0f   // Top-right
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load textures and setup rendering
    wallTexture = loadImageToTexture("wall.jpg");
    grassTexture = loadImageToTexture("grass.jpg");
    bushTexture = loadImageToTexture("bush.png");

    glBindTexture(GL_TEXTURE_2D, wallTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, bushTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    bushMatrix.resize(MATRIX_HEIGHT, std::vector<bool>(MATRIX_WIDTH, false));

    glBindVertexArray(0);
    glUseProgram(0);
}

void Map::render() {
    glUseProgram(shader);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);

    for (int y = 0; y < MATRIX_HEIGHT; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
            // Calculate tile position in normalized coordinates
            float posX = (x * TILE_SIZE) - 1.0f;
            float posY = (y * TILE_SIZE) - 1.0f;

            // Create model matrix for positioning
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(posX, posY, 0.0f));
            model = glm::scale(model, glm::vec3(TILE_SIZE, TILE_SIZE, 1.0f));

            // Set uniform for model matrix
            glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, &model[0][0]);

            // Select texture based on tile type
            if (mapMatrix[y][x] == TileType::WALL) {
                glBindTexture(GL_TEXTURE_2D, wallTexture);
            }
            else {
                glBindTexture(GL_TEXTURE_2D, grassTexture);
            }

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            if (hasBush(x, y)) {
                glBindTexture(GL_TEXTURE_2D, bushTexture);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
        }
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

bool Map::checkCollision(glm::vec2 position, float radius) {
    // More precise coordinate conversion
    float normalizedX = position.x + 1.0f;
    float normalizedY = position.y + 1.0f;

    // Calculate the tile coordinates with more precision
    int tileX = static_cast<int>(std::floor(normalizedX / TILE_SIZE));
    int tileY = static_cast<int>(std::floor(normalizedY / TILE_SIZE));

    // Expanded bounds check
    if (tileX < 0 || tileX >= MATRIX_WIDTH || tileY < 0 || tileY >= MATRIX_HEIGHT) {
        return true;  // Outside map boundaries
    }

    // Check a wider area around the player
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int checkX = tileX + dx;
            int checkY = tileY + dy;

            // Skip if out of bounds
            if (checkX < 0 || checkX >= MATRIX_WIDTH || checkY < 0 || checkY >= MATRIX_HEIGHT)
                continue;

            // If it's a wall tile, perform detailed collision
            if (mapMatrix[checkY][checkX] == TileType::WALL) {
                // Calculate tile world position (center of the tile)
                glm::vec2 tileCenter((checkX * TILE_SIZE) - 1.0f + (TILE_SIZE / 2.0f),
                                     (checkY * TILE_SIZE) - 1.0f + (TILE_SIZE / 2.0f));

                // More robust collision detection
                float closestX = clamp(position.x,
                    tileCenter.x - (TILE_SIZE / 2.0f),
                    tileCenter.x + (TILE_SIZE / 2.0f));
                float closestY = clamp(position.y,
                    tileCenter.y - (TILE_SIZE / 2.0f),
                    tileCenter.y + (TILE_SIZE / 2.0f));

                // Distance between the circle's center and the closest point on the tile
                float distanceX = position.x - closestX;
                float distanceY = position.y - closestY;

                // Check if the distance is less than the circle's radius
                float distanceSquared = sqrt((distanceX * distanceX) + (distanceY * distanceY)); //euklidska distanca
                if (distanceSquared < radius) {
                    return true;  // Collision detected
                }
            }
        }
    }

    return false;  // No collision
}
float Map::clamp(float value, float min, float max) {
    return std::max(min, std::min(value, max));
}

void Map::setTile(int x, int y, TileType type) {
    if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
        mapMatrix[y][x] = type;
    }
}

TileType Map::getTile(int x, int y) {
    if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
        return mapMatrix[y][x];
    }
    return TileType::WALL;  // Default to wall for out-of-bounds
}


// Dodajte nove metode:
void Map::placeBush(int x, int y) {
    if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
        // Postavljamo žbunje samo na travu
        if (mapMatrix[y][x] == TileType::GRASS) {
            bushMatrix[y][x] = true;
        }
    }
}

void Map::removeBush(int x, int y) {
    if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT && hasBush(x, y)) {
        bushMatrix[y][x] = false;
    }
}

bool Map::hasBush(int x, int y) const {
    if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
        return bushMatrix[y][x];
    }
    return false;
}