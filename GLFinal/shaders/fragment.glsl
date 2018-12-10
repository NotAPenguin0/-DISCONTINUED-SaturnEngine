#version 330 core

#define pointLightCount 4

in vec3 Normal;
in vec3 FragPos;
in vec2 texCoords;

out vec4 FragColor;

struct Material
{
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	
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
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform DirectionalLight dirLight;
uniform PointLight pointLights[pointLightCount];
uniform Material material;
uniform vec3 viewPos;

vec3 directional_light(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	//diffuse lighting
	float diff = max(dot(normal, lightDir), 0.0);
	//secular lighting
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	//combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
	
	return (ambient + diffuse + specular);
}

vec3 point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
    // diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    // specular lighting
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation (= decrease in intensity over distance)
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + 
  			     light.quadratic * (dist * dist));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = vec3(0.0);
	
	result += directional_light(dirLight, norm, viewDir);
	for(int i = 0; i < pointLightCount; ++i)
		result += point_light(pointLights[i], norm, FragPos, viewDir);
//	result += spot_light();

	FragColor = vec4(result, 1.0);
}