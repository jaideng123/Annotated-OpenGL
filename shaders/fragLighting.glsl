#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
// Incoming from vertex shader
// ALL INPUTS are interpolated from vertex shader
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform Light light;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform Material material;

out vec4 FragColor;

void main()
{
    vec4 baseColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);

    // ambient = color in complete darkness
    vec3 ambient = light.ambient * material.ambient;

    // normalize because we dont care about magnitude
    vec3 norm = normalize(Normal);
    // Direction vector of FragPos -> light.position
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm,lightDir), 0.0);
    // diffuse = directional impace of light
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // Direction vector of FragPos -> viewPos
    vec3 viewDir = normalize(viewPos - FragPos);
    // reflects along normal axis
    vec3 reflectDir = reflect(-lightDir, norm);
    // calculate strength of specular effect using the dot product
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // specular = bright spot from light source
    vec3 specular = light.specular * (spec * material.specular);  

    vec4 result = vec4(ambient + diffuse + specular,1.0) * baseColor;
    FragColor = result;
} 