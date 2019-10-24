#include "shader.h"
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

using namespace std;

Shader::Shader(const string vertexPath, const string fragmentPath)
{
    int vertexShader = generateAndCompileShader(vertexPath, GL_VERTEX_SHADER);
    checkSuccessfulShaderCompilation(vertexShader);

    int fragmentShader = generateAndCompileShader(fragmentPath, GL_FRAGMENT_SHADER);
    checkSuccessfulShaderCompilation(fragmentShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    checkSuccessfulShaderLink(shaderProgram);

    ID = shaderProgram;

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

int Shader::generateAndCompileShader(string sourceFileLocation, int shaderType)
{
    unsigned int shaderId;
    // Create Shader Object
    shaderId = glCreateShader(shaderType);

    // Read source code into a C string
    const char *vertexShaderSource = readFileContents(sourceFileLocation).c_str();
    // Read source code into shader object
    glShaderSource(shaderId, 1, &vertexShaderSource, NULL);

    // Compile shader
    glCompileShader(shaderId);

    return shaderId;
}

string Shader::readFileContents(string filename)
{
    ifstream filestream(filename);
    string output((istreambuf_iterator<char>(filestream)),
                  istreambuf_iterator<char>());
    return output;
}

void Shader::checkSuccessfulShaderCompilation(int shaderId)
{
    // Check if shader compilation was successful
    int success;
    char infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        throw runtime_error("Shader Compilation Unsuccessful");
    }
}

void Shader::checkSuccessfulShaderLink(int shaderProgramId)
{
    // Check if shader compilation was successful
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
                  << infoLog << std::endl;
        throw runtime_error("Shader Linking Unsuccessful");
    }
}
