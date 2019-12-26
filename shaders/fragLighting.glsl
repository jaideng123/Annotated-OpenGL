#version 330 core
// Incoming from vertex shader
// ALL INPUTS are interpolated from vertex shader
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    vec4 baseColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);

    float ambientStrength = 0.1;
    // ambient = color in complete darkness
    vec3 ambient = ambientStrength * lightColor;

    // normalize because we dont care about magnitude
    vec3 norm = normalize(Normal);
    // Direction vector of FragPos -> lightPos
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm,lightDir), 0.0);
    // diffuse = directional impace of light
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    // Direction vector of FragPos -> viewPos
    vec3 viewDir = normalize(viewPos - FragPos);
    // reflects along normal axis
    vec3 reflectDir = reflect(-lightDir, norm);
    int shininess = 32;
    // calculate strength of speculat effect
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // specular = bright spot from light source
    vec3 specular = specularStrength * spec * lightColor;  

    vec4 result = vec4(ambient + diffuse + specular,1.0) * baseColor;
    FragColor = result;
} 