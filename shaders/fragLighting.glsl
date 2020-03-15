#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
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
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform Light light;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform Material material;

out vec4 FragColor;

void main()
{
    // ambient = color in complete darkness
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    // normalize because we dont care about magnitude
    vec3 norm = normalize(Normal);
    // Direction vector of FragPos -> light.position
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm,lightDir), 0.0);
    // diffuse = directional impace of light
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    // Direction vector of FragPos -> viewPos
    vec3 viewDir = normalize(viewPos - FragPos);
    // reflects along normal axis
    vec3 reflectDir = reflect(-lightDir, norm);
    // calculate strength of specular effect using the dot product
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // specular = bright spot from light source
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));  

    vec3 emission = vec3(texture(material.emission, TexCoords));
    vec4 result = vec4(ambient + diffuse + specular + emission, 1.0);
    FragColor = result;
} 