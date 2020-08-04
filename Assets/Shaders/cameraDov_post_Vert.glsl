#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;


in vec3 position;
in vec2 texCoord;

out Vertex{
	vec2 texCoord;
	vec3 position;

} OUT;

void main(void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);
	OUT.texCoord = OUT.texCoord	= texCoord;
}

