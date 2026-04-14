#version 330 core
struct Material 
{
    sampler2D DiffuseMap;
    bool bUseDiffuseMap;
    
    bool bUseSpecularMap;
    sampler2D SpecularMap;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirectionalLight 
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

struct PointLight 
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight 
{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
    float innerCutOff;
    float outerCutOff;
};

#define NR_LIGHTS 4

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TextureCoords;

out vec4 FragColor;

uniform vec3 viewPos;

uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight pointLights[NR_LIGHTS];
uniform SpotLight spotLights[NR_LIGHTS];

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
    vec3 result = CalculateDirectionalLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // phase 3: Spot light
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    FragColor = vec4(result, 1.0);

}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDirection = normalize(-light.direction);  
    float diff = max(dot(normal, lightDirection), 0.0);  
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * (material.bUseDiffuseMap ? vec3(texture(material.DiffuseMap, TextureCoords)) : material.ambient);
    vec3 diffuse = light.diffuse * diff * (material.bUseDiffuseMap ? vec3(texture(material.DiffuseMap, TextureCoords)) : material.diffuse);
    vec3 specular = light.specular * spec * (material.bUseSpecularMap ? vec3(texture(material.SpecularMap, TextureCoords)) : material.specular);

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDirection = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / 
        (
            light.constant + 
            light.linear * distance + 
            light.quadratic * (distance * distance)
        );

    vec3 ambient = light.ambient * (material.bUseDiffuseMap ? vec3(texture(material.DiffuseMap, TextureCoords)) : material.ambient);
    vec3 diffuse = light.diffuse * diff * (material.bUseDiffuseMap ? vec3(texture(material.DiffuseMap, TextureCoords)) : material.diffuse);
    vec3 specular = light.specular * spec * (material.bUseSpecularMap ? vec3(texture(material.SpecularMap, TextureCoords)) : material.specular);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;    
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / 
        (
            light.constant + 
            light.linear * distance + 
            light.quadratic * (distance * distance)
        );
        
    // ambient
    vec3 ambient =  light.ambient * (material.bUseDiffuseMap ? vec3(texture(material.DiffuseMap, TextureCoords)) : material.ambient);
  	
    // diffuse 
    vec3 norm = normalize(Normal);

    // NOTE: We are passing these directions in  as coming FROM the light source., So we need to negate them.
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * (material.bUseDiffuseMap ? vec3(texture(material.DiffuseMap, TextureCoords)) : material.diffuse);
    
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);  // -ve since we want to point FROM the light source TO the fragment
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * (material.bUseSpecularMap ? vec3(texture(material.SpecularMap, TextureCoords)) : material.specular);  
    
    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);    

    diffuse  *= intensity;
    specular *= intensity;

    if(theta > light.outerCutOff)
    {
        return vec3(ambient + diffuse + specular);
    }
    else
    {
        return vec3(ambient);
    }
  
}