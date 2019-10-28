#version 330 core
// Input Vector3 as aPos at Location 0
// Can also omit layout and use glGetAttribLocation()
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
// will be available in frag shader
out vec3 vertexColor;
out vec2 TexCoord;

uniform mat4 transform;


void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    vertexColor = aColor;
    TexCoord = aTexCoord;
}