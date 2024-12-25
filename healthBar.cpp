#include "HealthBar.h"
#include "Tank.h"
#include "globalFunctions.h"
#include <glm/gtc/matrix_transform.hpp>

HealthBar::HealthBar() : duration(5.0f), startTime(-5.0f) {
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        uniform mat4 modelMat;
        void main() {
            gl_Position = modelMat * vec4(aPos, 0.0, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec3 color;
        void main() {
            FragColor = vec4(color, 1.0);
        }
    )";
    
    unifiedShader = createShader(vertexShaderSource, fragmentShaderSource);

    // Vertices for the health bar
    float vertices[] = {
        -0.5,  0.1f,  // Left top
        -0.5f, -0.1f,  // Left bottom
         0.5f, -0.1f,  // Right bottom
         0.5f,  0.1f   // Right top
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

HealthBar::~HealthBar() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(unifiedShader);
}

void HealthBar::render(Tank& tank, float currentTime) {
    if (currentTime - startTime > duration) return; // Prikaz istekao

    float healthPercentage = tank.hitpoints / 100.0f;

    float totalWidth = 0.2f; // Konstantna ukupna širina health bara
    float greenWidth = healthPercentage * totalWidth; // Širina zelene trake
    float redWidth = (1.0f - healthPercentage) * totalWidth; // Širina crvene trake

    glUseProgram(unifiedShader);
    glBindVertexArray(VAO);

    // Render zelene boje (preostalo zdravlje)
    glm::mat4 greenModel = glm::mat4(1.0f);
    greenModel = glm::translate(greenModel,
        glm::vec3(tank.position.x - totalWidth / 2.0f + greenWidth / 2.0f, tank.position.y + 0.2f, 0.0f));
    greenModel = glm::scale(greenModel, glm::vec3(greenWidth, 0.1f, 1.0f));
    unsigned int modelLoc = glGetUniformLocation(unifiedShader, "modelMat");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &greenModel[0][0]);

    unsigned int colorLoc = glGetUniformLocation(unifiedShader, "color");
    glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f); // Zelena boja
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Render crvene boje (izgubljeno zdravlje)
    glm::mat4 redModel = glm::mat4(1.0f);
    redModel = glm::translate(redModel,
        glm::vec3(tank.position.x - totalWidth / 2.0f + greenWidth + redWidth / 2.0f - 0.001f, tank.position.y + 0.2f, 0.0f));
    redModel = glm::scale(redModel, glm::vec3(redWidth, 0.1f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &redModel[0][0]);

    glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); // Crvena boja
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);
    glUseProgram(0);
}


void HealthBar::setLastHitTime(float time) {
    startTime = time;
}
