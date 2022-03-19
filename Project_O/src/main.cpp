#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        static vec3 camPos = { 0.f, 0.f, 1.f };
        static vec3 camRot = vec3::zo;
        static float fovY = 90.f * TORAD;
        static float near = 0.001f;
        static float far = 1000.f;

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        ImGui::Begin("Camera");
        ImGui::DragFloat3("position", camPos.elem);
        ImGui::DragFloat3("rotation", camRot.elem);
        ImGui::SliderAngle("fov y", &fovY, 0.f, 180.f);
        ImGui::DragFloat("near", &near);
        ImGui::DragFloat("far", &far);
        ImGui::End();

        ImGui::Render();


        mat4 p = Math3::perspective(fovY * TODEG, SCRN_WIDTH / (float)SCRN_HEIGHT, near, far);
        mat4 v = Math3::translateMatrix(-camPos) * Math3::rotateXMatrix(camRot.x) * Math3::rotateYMatrix(-camRot.y) * Math3::rotateZMatrix(camRot.z);
        mat4 m = mat4::id;

        glEnable(GL_DEPTH_TEST);

        sh.use();
        sh.uniformMat4("uProjection", p, false);
        sh.uniformMat4("uView", v, false);
        sh.uniformMat4("uModel", m, false);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glDisable(GL_DEPTH_TEST);


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}