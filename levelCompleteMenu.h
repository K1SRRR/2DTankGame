// LevelCompleteMenu.h
#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "TextRenderer.h"

class LevelCompleteMenu {
public:
    struct Button {
        float x;
        float y;
        float width;
        float height;
        bool isHovered;

        bool isMouseOver(double mouseX, double mouseY);
    };

    LevelCompleteMenu(unsigned int windowWidth, unsigned int windowHeight, TextRenderer& textRenderer);
    ~LevelCompleteMenu();

    void show(bool isLastLevel = false);
    void hide();
    bool isShown() const;
    void renderOverlay(unsigned int textRenderingShader, double mouseX, double mouseY);
    bool handleNextLevelClick(double mouseX, double mouseY);

private:
    void renderButtons(unsigned int textRenderingShader);

    bool isVisible;
    bool isLastLevelComplete;
    unsigned int VAO, VBO;
    unsigned int unifiedShader;
    unsigned int windowWidth;
    unsigned int windowHeight;
    TextRenderer& textRenderer;
    Button nextLevelButton;
};