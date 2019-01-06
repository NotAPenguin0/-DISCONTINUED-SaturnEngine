#version 330 core

uniform int axis;

out vec4 FragColor;

void main()
{
    if (axis == 0) //x axis
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    else if (axis == 1) //y axis
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    else if (axis == 2) //z axis
        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
}