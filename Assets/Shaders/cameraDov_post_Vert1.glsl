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
	vec4 shadowProj;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} OUT;
void main(void) {
	mat4 mvp 		  = (projMatrix * viewMatrix * modelMatrix);
	gl_Position		= mvp * vec4(position, 1.0);
	
	OUT.texCoord	= texCoord;
}

