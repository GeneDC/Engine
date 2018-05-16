#version 410

layout( location = 0 ) in vec4 Position;

uniform mat4 ProjectionViewModel;
uniform float elapsedTime;

void main() 
{
	gl_Position = ProjectionViewModel * (Position);
}