#version 330 core
// Input Vector3 as aPos at Location 0
// Can also omit layout and use glGetAttribLocation()
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
// will be available in frag shader
out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    // Calculate Position in world space
    FragPos = vec3(model * vec4(aPos,1.0));
    TexCoords = aTexCoord;
    // generate normal matrix for transforming normals to world space
    // NOTE: inversing matrices is not performant in shader code and should be done on CPU
    Normal = mat3(transpose(inverse(model))) * aNormal;
}