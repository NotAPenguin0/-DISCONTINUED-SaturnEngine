#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 color = texture(texture1, TexCoords).rgb;
    // ambient
    vec3 ambient = 0.10 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);

    vec3 specular = vec3(0.3) * spec; // assuming bright white light color

    //simple attenuation
    float constant = 1.0;
    float linear = 0.045;
    float quadratic = 0.0075;

    float dist    = length(lightPos - FragPos);
    float attenuation = 1.0 / (constant + linear * dist +  quadratic * (dist * dist));   

    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
//    FragColor = vec4(normal, 1.0);
}