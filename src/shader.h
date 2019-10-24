#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

using namespace std;

class Shader
{
public:
    unsigned int ID;

    // Read + compile shader
    Shader(const string vertexPath, const string fragmentPath);
    // Activate shader
    void use();

    void setBool(const string &name, bool value) const;
    void setInt(const string &name, int value) const;
    void setFloat(const string &name, float value) const;

private:
    string readFileContents(string filename);
    void checkSuccessfulShaderCompilation(int shaderId);
    int generateAndCompileShader(string sourceFileLocation, int shaderType);
    void checkSuccessfulShaderLink(int shaderId);
};

#endif