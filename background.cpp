#include "Background.h"
#include <glm/gtc/matrix_transform.hpp>
#include "globalFunctions.h"

const char* vertexBackgroundShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 inPos;
    layout(location = 1) in vec2 inTex;
    out vec2 chTex;
    uniform mat4 uModel;
    void main()
    {
        gl_Position = uModel * vec4(inPos, 0.0, 1.0);
        chTex = inTex;
    }
)";

const char* fragmentBackgroundShaderSource = R"(
    #version 330 core
        in vec2 chTex;
        out vec4 outCol;
        uniform sampler2D uWallTex;
        uniform sampler2D uGrassTex;
        void main()
        {
            // Definišite debljinu zida (npr. 0.1 = 10% od ivice)
            float wallThickness = 0.1;
        
            // Provera da li smo blizu ivica
            bool isNearLeftEdge = chTex.x < wallThickness;
            bool isNearRightEdge = chTex.x > (1.0 - wallThickness);
            bool isNearTopEdge = chTex.y > (1.0 - wallThickness);
            bool isNearBottomEdge = chTex.y < wallThickness;
        
            // Ako smo na ivici, prikaži zid, inače travu
            if (isNearLeftEdge || isNearRightEdge || isNearTopEdge || isNearBottomEdge) {
                outCol = texture(uWallTex, chTex);
            } else {
                outCol = texture(uGrassTex, chTex);
            }
        }
)";

Background::Background() : position(0.0f, 0.0f) {
    unifiedShader = createShader(vertexBackgroundShaderSource, fragmentBackgroundShaderSource);

    // Vertices covering entire normalized screen space
    float vertices[] = {
        // Positions    // Texture Coords
        -1.0f, -1.0f,   0.0f, 0.0f,  // Bottom-left
         1.0f, -1.0f,   1.0f, 0.0f,  // Bottom-right
        -1.0f,  1.0f,   0.0f, 1.0f,  // Top-left
         1.0f,  1.0f,   1.0f, 1.0f   // Top-right
    };

    unsigned int stride = (2 + 2) * sizeof(float);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    wallTexture = loadImageToTexture("wall.jpg");
    grassTexture = loadImageToTexture("grass.jpg");

    // Texture parameters
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

Background::~Background() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &wallTexture);
    glDeleteTextures(1, &grassTexture);
    glDeleteProgram(unifiedShader);
}

void Background::render() {
    if (wallTexture == 0 || grassTexture == 0) {
        std::cout << "Texture loading failed! Wall: "
            << wallTexture << ", Grass: "
            << grassTexture << std::endl;
        return;
    }

    // Ensure shader is valid
    if (unifiedShader == 0) {
        std::cout << "Shader compilation failed!" << std::endl;
        return;
    }
    glUseProgram(unifiedShader);

    // Render walls
    glUniform1i(glGetUniformLocation(unifiedShader, "isWall"), true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, wallTexture);

    // Render wall areas
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Render grass interior
    glUniform1i(glGetUniformLocation(unifiedShader, "isWall"), false);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glUseProgram(0);
    renderDynamicWalls();
}

glm::vec2 Background::resolveWallCollision(glm::vec2 tankPosition, float tankRadius) {
    glm::vec2 resolvedPosition = tankPosition;

    // Clamp tank position to map boundaries
    resolvedPosition.x = glm::clamp(resolvedPosition.x, -1.0f + tankRadius, 1.0f - tankRadius);
    resolvedPosition.y = glm::clamp(resolvedPosition.y, -1.0f + tankRadius, 1.0f - tankRadius);

    return resolvedPosition;
}
void Background::addWall(glm::vec2 position, float width, float height) {
    dynamicWalls.push_back({ position, glm::vec2(width, height) });
}

void Background::renderDynamicWalls() {
    glUseProgram(unifiedShader);
    glUniform1i(glGetUniformLocation(unifiedShader, "isWall"), true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, wallTexture);

    for (const auto& wall : dynamicWalls) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(wall.position, 0.0f));
        model = glm::scale(model, glm::vec3(wall.dimensions, 1.0f));

        glUniformMatrix4fv(glGetUniformLocation(unifiedShader, "uModel"), 1, GL_FALSE, &model[0][0]);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

bool Background::checkWallCollision(glm::vec2 tankPosition, float tankRadius) {
    // Check map boundaries
    if (tankPosition.x - tankRadius < -1.0f ||
        tankPosition.x + tankRadius > 1.0f ||
        tankPosition.y - tankRadius < -1.0f ||
        tankPosition.y + tankRadius > 1.0f) {
        return true;
    }

    // Check dynamic walls
    for (const auto& wall : dynamicWalls) {
        glm::vec2 wallMin = wall.position - wall.dimensions / 2.0f;
        glm::vec2 wallMax = wall.position + wall.dimensions / 2.0f;

        bool collisionX = tankPosition.x + tankRadius > wallMin.x &&
            tankPosition.x - tankRadius < wallMax.x;
        bool collisionY = tankPosition.y + tankRadius > wallMin.y &&
            tankPosition.y - tankRadius < wallMax.y;

        if (collisionX && collisionY) {
            return true;
        }
    }

    return false;
}