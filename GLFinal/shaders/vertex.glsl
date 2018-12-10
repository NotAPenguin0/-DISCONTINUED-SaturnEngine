#version 330 core

layout(location = 0) in vec3 iPos;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iTexCoord;

uniform mat4 model;
uniform mat4 proj;
uniform mat4 view;

out vec3 Normal;
out vec3 FragPos;
out vec2 texCoords;

void main()
{
	gl_Position = proj * view * model * vec4(iPos, 1.0);
	Normal = iNormal;
	Normal = mat3(transpose(inverse(model))) * iNormal;  
	FragPos = vec3(model * vec4(iPos, 1.0));
	texCoords = iTexCoord;
}
