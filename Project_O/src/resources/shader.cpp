#include <iostream>
#include <fstream>

#include "resources/shader.hpp"

Resources::Shader::Shader(const char* shaderName)
{
	std::string filename = shaderName;
	filename = "../shaders/" + filename;

	//vertex shader
	std::ifstream vsStream(filename + ".vs");
	std::ifstream fsStream(filename + ".fs");

	if (!vsStream.is_open() || !fsStream.is_open())
	{
		std::cout << "could not open shader files : " << filename << std::endl;
		return;
	}

	std::string vs((std::istreambuf_iterator<char>(vsStream)), std::istreambuf_iterator<char>());
	std::string fs((std::istreambuf_iterator<char>(fsStream)), std::istreambuf_iterator<char>());

	const char* vertexShaderSource = vs.c_str();
	const char* fragmentShaderSource = fs.c_str();

	//shaders
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	int  success[2];
	char infoLog[2][512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success[0]);

	if (!success[0])
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog[0]);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED : " << filename << ".vs\n" << infoLog[0] << std::endl;
	}

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success[1]);

	if (!success[1])
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog[1]);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED : " << filename << ".fs\n" << infoLog[1] << std::endl;
	}

	if (success[0] && success[1])
	{
		std::cout << "successfully loaded shader files : " << filename << std::endl;
		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	vsStream.close();
	fsStream.close();
}

Resources::Shader::~Shader()
{
    glDeleteShader(program);
}

void Resources::Shader::use() const
{
    glUseProgram(program);
}

void Resources::Shader::uniformBool(const bool& b, const char* name)
{
    int location = glGetUniformLocation(program, name);
    use();
    glUniform1i(location, b);
}

void Resources::Shader::uniformFloat(const float& f, const char* name)
{
    int location = glGetUniformLocation(program, name);
    use();
    glUniform1f(location, f);
}

void Resources::Shader::uniformVec3(const vec3& v, const char* name)
{
    int location = glGetUniformLocation(program, name);
    use();
    glUniform3fv(location, 1, v.elem);
}

void Resources::Shader::uniformMat4(const mat4& m, const bool transpose, const char* name)
{
    int location = glGetUniformLocation(program, name);
    use();
    glUniformMatrix4fv(location, 1, transpose, m.elem);
}