#version 330 core
// Input Vector3 as aPos at Location 0
// Can also omit layout and use glGetAttribLocation()
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
// will be available in frag shader
out vec3 vertexColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);

    vertexColor = aColor;
}