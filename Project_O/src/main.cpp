#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "resources/shader.hpp"

#define SCRN_WIDTH 1366
#define SCRN_HEIGHT 768

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCRN_WIDTH, SCRN_HEIGHT, "Project_O", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -2;
    }

    glViewport(0, 0, SCRN_WIDTH, SCRN_HEIGHT);


    Resources::Shader sh("basic");

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        float fovY = 90.f;
        float near = 0.001f;
        float far = 1000.f;
        vec3 camPos = { 0.f, 0.f, 1.f };
        vec3 camRot = vec3::zo;

        mat4 p = Math3::perspective(fovY, SCRN_WIDTH / (float)SCRN_HEIGHT, near, far);
        mat4 v = Math3::translateMatrix(-camPos) * Math3::rotateXMatrix(camRot.x) * Math3::rotateYMatrix(-camRot.y) * Math3::rotateZMatrix(camRot.z);
        mat4 m = mat4::id;

        sh.use();
        sh.uniformMat4("uProjection", p, false);
        sh.uniformMat4("uView", v, false);
        sh.uniformMat4("uModel", m, false);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}