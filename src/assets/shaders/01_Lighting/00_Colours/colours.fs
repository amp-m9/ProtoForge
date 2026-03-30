#version 330 core
struct Material {
    sampler2D DiffuseMap;
    bool bUseDiffuseMap;
    
    bool bUseSpecularMap;
    sampler2D SpecularMap;
    
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

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TextureCoords;

out vec4 FragColor;

uniform vec3 viewPos;

uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient =  light.ambient * (material.bUseDiffuseMap ? vec3(texture(material.DiffuseMap, TextureCoords)) : material.ambient);
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * (material.bUseDiffuseMap ? vec3(texture(material.DiffuseMap, TextureCoords)) : material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  // -ve since we want to point FROM the light source TO the fragment
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * (material.bUseSpecularMap ? vec3(texture(material.SpecularMap, TextureCoords)) : material.specular);  
        
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}