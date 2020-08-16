#version 330 core
uniform sampler2D mainTex;


in Vertex
{	
	vec2 texCoord;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void) {
	fragColor.rgb = vec3(texture(mainTex,IN.texCoord).a);
	
}
