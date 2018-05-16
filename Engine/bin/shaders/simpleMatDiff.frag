#version 410

uniform vec3 Kd;

out vec4 FragColour;

void main()
{
	FragColour = vec4(Kd, 1);
}