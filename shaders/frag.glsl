#version 330 core
// Incoming from vertex shader
// ALL INPUTS are interpolated from vertex shader
in vec3 vertexColor;

// Uniforms are global for each shader program
// Will keep values till reset/updated
// set with glUniform<Type>(loc,...data)
// Useful for exchanging data between render iterations (time, color, etc.)
// uniform vec4 ourColor

out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0);
} 