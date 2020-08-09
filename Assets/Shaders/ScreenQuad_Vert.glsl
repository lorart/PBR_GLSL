#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;


out Vertex
{
	//vec4 colour;
	vec2 texCoord;
//	vec3 worldPos;
	vec2 Cood;
} OUT;
void main(void) {
	mat4 mvp 		  = (projMatrix * viewMatrix * modelMatrix);
	gl_Position		= mvp * vec4(position, 1.0);
	OUT.Cood   =texCoord;//?
	OUT.texCoord	= texCoord;
}

