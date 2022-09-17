#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

void windowInit();
void openglInit();
void openglVertexBuffer();
void openglVertexArray();
void openglCompileShaders();
void drawFrame();

GLFWwindow* window;
GLuint VBO;
GLuint VAO;
GLuint shaderProgram;

float vertices[] = {
	// x, y, z, r, g, b
	0.f, 0.5f, 0.f, 1.f, 0.f, 0.f,
	0.5f, -0.5f, 0.f, 0.f, 1.f, 0.f,
	-0.5f, -0.5f, 0.f, 0.f, 0.f, 1.f
};

const char* vstriangleSrc = R"GLSL(
#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 oFrag;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	oFrag = aColor;
}
)GLSL";

const char* fstriangleSrc = R"GLSL(
#version 450 core

in vec3 oFrag;

out vec4 oColor;

void main()
{
	oColor = vec4(oFrag, 1.0);
}
)GLSL";

void windowInit()
{
	// OpenGL core 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(800, 600, "OpenGL window", nullptr, nullptr);
	glfwMakeContextCurrent(window);
}

void openglInit()
{
	if (!gladLoadGL(glfwGetProcAddress))
		throw std::exception("Failed to load OpenGL functions");
}

void openglVertexBuffer()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void openglVertexArray()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FLOAT, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void openglCompileShaders()
{
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vstriangleSrc, NULL);
	glCompileShader(vs);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fstriangleSrc, NULL);
	glCompileShader(fs);

	int success[2];
	char infoLog[2][512];
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success[0]);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success[1]);

	if (!success[0] || !success[1])
	{
		glGetShaderInfoLog(vs, 512, NULL, infoLog[0]);
		std::cout << infoLog[0] << std::endl;
		glGetShaderInfoLog(fs, 512, NULL, infoLog[1]);
		std::cout << infoLog[1] << std::endl;

		throw std::exception("Failed to compile shaders");
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);

	glDeleteShader(vs);
	glDeleteShader(fs);
}

void drawFrame()
{
	glViewport(0, 0, 800, 600);

	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

int main()
{
	try
	{
		glfwInit();

		windowInit();
		openglInit();
		openglVertexBuffer();
		openglVertexArray();
		openglCompileShaders();

		while (!glfwWindowShouldClose(window))
		{
			glfwSwapBuffers(window);
			glfwPollEvents();
			drawFrame();
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	return EXIT_SUCCESS;
}