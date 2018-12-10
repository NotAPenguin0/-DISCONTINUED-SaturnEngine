#version 330 core

#define MAX_LIGHT_ARRAY_SIZE 50

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

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
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

struct Material
{
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	
	float shininess;
};

uniform vec3 viewPos;

uniform Material material;
uniform DirectionalLight directional[MAX_LIGHT_ARRAY_SIZE];
uniform PointLight point[MAX_LIGHT_ARRAY_SIZE];
uniform SpotLight spot[MAX_LIGHT_ARRAY_SIZE];
uniform int directional_size;
uniform int point_size;
uniform int spot_size;

vec3 directional_light(DirectionalLight light, vec3 normal, vec3 vDir)
{
	vec3 lightDir = normalize(-light.direction);
	//diffuse lighting
	float diff = max(dot(normal, lightDir), 0.0);
	//secular lighting
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(vDir, reflectDir), 0.0), material.shininess);
	//combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	
	return (ambient + diffuse + specular);
}

vec3 point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 vDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
    // diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    // specular lighting
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(vDir, reflectDir), 0.0), material.shininess);
    // attenuation (= decrease in intensity over distance)
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 spot_light(SpotLight light, vec3 normal, vec3 fragPos, vec3 vDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(vDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

void main()
{
	vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
	
	vec3 result = vec3(0.0);
	
	//apply directional lights
	for(int i = 0; i < directional_size; ++i)
	{
		result += directional_light(directional[i], norm, viewDir);
	}
	
	//apply point lights
	for(int i = 0; i < point_size; ++i)
	{
		result += point_light(point[i], norm, FragPos, viewDir);
	}
	
	//apply spot lights
	for(int i = 0; i < spot_size; ++i)
	{
		result += spot_light(spot[i], norm, FragPos, viewDir);
	}
	
	FragColor = vec4(result, 1.0);
}

