#version 330 core

layout(location = 0) in vec3 iPos;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = iTexCoords;
    Normal = normalize(vec3(transpose(inverse(model)) * vec4(iNormal, 0.0)));
	FragPos = vec3(model * vec4(iPos, 1.0));
    gl_Position = projection * view  * model * vec4(iPos, 1.0);
}