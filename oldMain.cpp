//// Autor: Bogdan Davinic
//
//#define _CRT_SECURE_NO_WARNINGS
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <GL/glew.h>   
//#include <GLFW/glfw3.h>
////Biblioteke za pokretanje generatora podataka
//#include "TestBed.h"
//#include "Tank.h"
//
//
//const float TARGET_FPS = 60.0f;
//const float FRAME_TIME = 1.0f / TARGET_FPS;
//
//unsigned int compileShader(GLenum type, const char* source);
//unsigned int createShader(const char* vsSource, const char* fsSource);
//
//int main(void)
//{
//    Tank tank(glm::vec2(0.0f, 0.0f), 2.0f, 90.0f); // Početna pozicija, brzina, rotaciona brzina
//
//    //Inicijalizacija objekta tipa Car
//    Car car = getCar();
//
//    if (!glfwInit())
//    {
//        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
//        return 1;
//    }
//
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    GLFWwindow* window;
//    unsigned int wWidth = 500;
//    unsigned int wHeight = 500;
//    const char wTitle[] = "[Generic Title]";
//    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
//    if (window == NULL)
//    {
//        std::cout << "Prozor nije napravljen! :(\n";
//        glfwTerminate();
//        return 2;
//    }
//    glfwMakeContextCurrent(window);
//
//    if (glewInit() != GLEW_OK)
//    {
//        std::cout << "GLEW nije mogao da se ucita! :'(\n";
//        return 3;
//    }
//
//
//    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");
//
//    float vertices[] =
//    {
//        0.25, 0.0,    1.0, 0.0, 0.0, 0.0,
//        -0.25, 0.0,   0.0, 0.0, 1.0, 0.0,
//        0.0, -0.5,     1.0, 1.0, 0.0, 0.0
//    };
//    unsigned int stride = (2 + 4) * sizeof(float);
//    unsigned int VAO;
//    glGenVertexArrays(1, &VAO);
//    glBindVertexArray(VAO);
//
//    unsigned int VBO;
//    glGenBuffers(1, &VBO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
//    glEnableVertexAttribArray(1);
//
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
//
//    //Pokretanje simulacije, kako bi se menjalo stanje automobila kroz vreme
//    startSimulation(&car);
//
//    float previousTime = 0.0f;
//    float deltaTime = 0.0f;
//    while (!glfwWindowShouldClose(window))
//    {
//        float currentTime = glfwGetTime();
//        deltaTime += (currentTime - previousTime) / FRAME_TIME;
//        previousTime = currentTime;
//
//        if (deltaTime >= 1.0f)
//        {
//            deltaTime--;
//
//            // Tvoja logika za renderovanje
//            glClearColor(0.5, 0.5, 0.5, 1.0);
//            glClear(GL_COLOR_BUFFER_BIT);
//
//            glUseProgram(unifiedShader);
//            glBindVertexArray(VAO);
//            glDrawArrays(GL_TRIANGLES, 0, 3);
//
//            glfwSwapBuffers(window);
//            glfwPollEvents();
//        }
//
//
//        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        {
//            glfwSetWindowShouldClose(window, GL_TRUE);
//        }
//        glClearColor(0.5, 0.5, 0.5, 1.0);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        glUseProgram(unifiedShader);
//        glBindVertexArray(VAO);
//        glDrawArrays(GL_TRIANGLES, 0, 3);
//
//        glfwSwapBuffers(window);
//
//        glfwPollEvents();
//    }
//
//    // Zaustavljanje simulacije
//    endSimulation(&car);
//    glDeleteBuffers(1, &VBO);
//    glDeleteVertexArrays(1, &VAO);
//    glDeleteProgram(unifiedShader);
//    glfwTerminate();
//    return 0;
//}
//
//
//void processInput(GLFWwindow* window, Tank& tank, float deltaTime) {
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) tank.moveForward(deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) tank.moveBackward(deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) tank.rotateBodyLeft(deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) tank.rotateBodyRight(deltaTime);
//}
//
//
//
//
//
////------------------------------------------------------------------------------------------------------------------//
////------------------------------------------------------------------------------------------------------------------//
////------------------------------------------------------------------------------------------------------------------//
//unsigned int compileShader(GLenum type, const char* source)
//{
//    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
//    //Citanje izvornog koda iz fajla
//    std::string content = "";
//    std::ifstream file(source);
//    std::stringstream ss;
//    if (file.is_open())
//    {
//        ss << file.rdbuf();
//        file.close();
//        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
//    }
//    else {
//        ss << "";
//        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
//    }
//    std::string temp = ss.str();
//    const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"
//
//    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)
//
//    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
//    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
//    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
//    glCompileShader(shader); //Kompajliraj sejder
//
//    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
//    if (success == GL_FALSE)
//    {
//        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
//        if (type == GL_VERTEX_SHADER)
//            printf("VERTEX");
//        else if (type == GL_FRAGMENT_SHADER)
//            printf("FRAGMENT");
//        printf(" sejder ima gresku! Greska: \n");
//        printf(infoLog);
//    }
//    return shader;
//}
//unsigned int createShader(const char* vsSource, const char* fsSource)
//{
//    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource
//
//    unsigned int program; //Objedinjeni sejder
//    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
//    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)
//
//    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program
//
//    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
//    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder
//
//    //Zakaci verteks i fragment sejdere za objedinjeni program
//    glAttachShader(program, vertexShader);
//    glAttachShader(program, fragmentShader);
//
//    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
//    glValidateProgram(program); //Izvrsi provjeru novopecenog programa
//
//    int success;
//    char infoLog[512];
//    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
//    if (success == GL_FALSE)
//    {
//        glGetShaderInfoLog(program, 512, NULL, infoLog);
//        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
//        std::cout << infoLog << std::endl;
//    }
//
//    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
//    glDetachShader(program, vertexShader);
//    glDeleteShader(vertexShader);
//    glDetachShader(program, fragmentShader);
//    glDeleteShader(fragmentShader);
//
//    return program;
//}
