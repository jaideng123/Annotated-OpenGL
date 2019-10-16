#version 330 core
// Incoming from vertex shader
in vec4 vertexColor;

out vec4 FragColor;

void main()
{
    FragColor = vertexColor;
} 