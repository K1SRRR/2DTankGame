#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include "TextRenderer.h"

class PauseMenu {
private:
    bool isPaused;
    unsigned int VAO, VBO;
    unsigned int unifiedShader;
    unsigned int windowWidth, windowHeight;
    TextRenderer& textRenderer;

    struct Button {
        float x, y, width, height;
        bool isHovered;

        bool isMouseOver(double mouseX, double mouseY);
    } quitButton, resumeButton;

public:
    PauseMenu(unsigned int windowWidth, unsigned int windowHeight, TextRenderer& textRenderer);
    ~PauseMenu();

    void togglePause();
    bool isGamePaused() const;
    void renderOverlay(unsigned int textRenderingShader, double mouseX, double mouseY);
    void renderButtons(unsigned int unifiedShader);
    bool handleQuitClick(double mouseX, double mouseY);
    bool handleResumeClick(double mouseX, double mouseY);

};

#endif // PAUSE_MENU_H