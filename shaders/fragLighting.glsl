#version 330 core
// Incoming from vertex shader
// ALL INPUTS are interpolated from vertex shader
in vec3 vertexColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 lightColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(lightColor,1.0) * mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
} 