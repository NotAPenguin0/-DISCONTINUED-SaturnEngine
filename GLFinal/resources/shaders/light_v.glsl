#version 330 core

layout(location = 0) in vec3 iPos;
layout(location = 2) in vec3 iNormal;
layout(location = 3) in vec2 iTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main()
{
	gl_Position = projection * view * model * vec4(iPos, 1.0);
	Normal = mat3(transpose(inverse(model))) * iNormal;
	FragPos = vec3(model * vec4(iPos, 1.0));
	TexCoords = iTexCoords;
}