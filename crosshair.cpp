#include "Crosshair.h"
#include "globalFunctions.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

const char* vertexCrosshairShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        uniform mat4 modelMat;
        void main() {
            gl_Position = modelMat * vec4(aPos, 0.0, 1.0);
        }
    )";

const char* fragmentCrosshairShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform float Ured;
        uniform float Ugreen;
        void main() {
            FragColor = vec4(Ured, Ugreen, 0.0, 1.0);
        }
    )";

Crosshair::Crosshair() : x(0.0f), y(0.0f), unifiedShader(0) {
    unifiedShader = createShader(vertexCrosshairShaderSource, fragmentCrosshairShaderSource);

    // Crosshair geometry for "X"
    float vertices[] = {
        -0.05f, -0.05f, // Bottom-left
         0.05f,  0.05f, // Top-right
        -0.05f,  0.05f, // Top-left
         0.05f, -0.05f  // Bottom-right
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

Crosshair::~Crosshair() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(unifiedShader);
}

void Crosshair::render(const Tank& tank, float currentTime) {
    if (x < -1.0f) x = -1.0f;
    if (x > 1.0f) x = 1.0f;
    if (y < -1.0f) y = -1.0f;
    if (y > 1.0f) y = 1.0f;
    //std::cout << "Crosshair position: " << x << ", " << y << std::endl;
    bool canShoot = tank.canShoot(currentTime);
    //std::cout << "CAN SHOOT: " << canShoot << std::endl;

    unsigned int uredLoc = glGetUniformLocation(unifiedShader, "Ured");
    unsigned int ugreenLoc = glGetUniformLocation(unifiedShader, "Ugreen");
    unsigned int modelLoc = glGetUniformLocation(unifiedShader, "modelMat");

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));


    glUseProgram(unifiedShader);

    if (canShoot) {
        // Zelena kad moze
        glUniform1f(uredLoc, 0.0f);
        glUniform1f(ugreenLoc, 1.0f);
    }
    else {
        // Crvena kad ne moze da puca
        glUniform1f(uredLoc, 1.0f);
        glUniform1f(ugreenLoc, 0.0f);
    }
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]); //Ovo dobija adresu tog prvog člana, tj. početnu adresu celokupnog niza 4x4 elemenata.

    glBindVertexArray(VAO);
    glLineWidth(5.0f);
    glDrawArrays(GL_LINES, 0, 4);  // X oblik
    glBindVertexArray(0);
    glUseProgram(0);
}

void Crosshair::setPosition(float nx, float ny) { //vec normalizone u mainu
    x = nx;
    y = ny;
}
