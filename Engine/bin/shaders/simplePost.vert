#version 410

layout( location = 0 ) in vec4 Position;

out vec2 vTexCoord;

void main() 
{
	vTexCoord = vec2(Position.x, Position.y) * 0.5f + 0.5f;
	gl_Position = vec4(Position.x, Position.y, 0, 1);
}