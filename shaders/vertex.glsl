#version 330 core
// Input Vector3 as aPos at Location 0
// Can also omit layout and use glGetAttribLocation()
layout (location = 0) in vec3 aPos;
// will be available in frag shader
out vec4 vertexColor;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);

    vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
}