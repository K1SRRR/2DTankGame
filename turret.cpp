#include "Turret.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "globalFunctions.h"

const char* vertexTurretShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec2 inPos;
        layout(location = 1) in vec2 inTex;
        out vec2 chTex;
        uniform vec2 uPos;
        uniform mat4 uModel;
        void main()
        {
	        gl_Position = uModel * vec4(inPos + uPos, 0.0, 1.0);
	        chTex = inTex;
        }
    )";

const char* fragmentTurretShaderSource = R"(
        #version 330 core
        in vec2 chTex;
        out vec4 outCol;
        uniform sampler2D uTex;
        void main()
        {
	        outCol = texture(uTex, chTex);
        }
    )";

// Konstruktor
Turret::Turret(glm::vec2 initPosition, const char* textureSourceFile)
    : position(initPosition), turretAngle(0.0f), texture(0)
{
    unifiedShader = createShader(vertexTurretShaderSource, fragmentTurretShaderSource);

    float vertices[] =
    {   // X      Y      S      T
        -0.05f, -0.05f,  1.0f, 0.0f,  // prvo tjeme
        -0.05f,  0.15f,  1.0f, 1.0f,  // drugo tjeme
         0.05f, -0.05f,  0.0f, 0.0f,  // trece tjeme
         0.05f,  0.15f,  0.0f, 1.0f   // cetvrto tjeme
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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    texture = loadImageToTexture(textureSourceFile); // Učitavamo teksturu kupole
    glBindTexture(GL_TEXTURE_2D, texture);
    glGenerateMipmap(GL_TEXTURE_2D); 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(unifiedShader);
    unsigned uTexLoc = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc, 0);
    glUseProgram(0);
}

// Renderovanje kupole
void Turret::render() {
    glm::mat4 model = getModelMatrix();

    glUseProgram(unifiedShader);
    unsigned uModelLoc = glGetUniformLocation(unifiedShader, "uModel");
    glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, &model[0][0]);

    unsigned uPosLoc = glGetUniformLocation(unifiedShader, "uPos");
    glUniform2f(uPosLoc, position.x, position.y);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

// Usmeravanje kupole prema mišu
void Turret::aimAtMouse(float mouseX, float mouseY, int windowWidth, int windowHeight) {
    float normalizedX = (mouseX / windowWidth) * 2.0f - 1.0f;
    float normalizedY = 1.0f - (mouseY / windowHeight) * 2.0f;

    turretAngle = glm::degrees(atan2(normalizedY - position.y, normalizedX - position.x)) - 90.0f;
}

// Matrica modela
glm::mat4 Turret::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::rotate(model, glm::radians(turretAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-position, 0.0f));
    return model;
}

// Uništavanje resursa
Turret::~Turret() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(unifiedShader);
    glDeleteTextures(1, &texture);
}
