#version 330 core
uniform sampler2D mainTex;

in Vertex
{	
	vec2 texCoord;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void) {

	fragColor = texture(mainTex,IN.texCoord);
	
	fragColor=fragColor*vec4(1,0.8,0.8,1);
}
