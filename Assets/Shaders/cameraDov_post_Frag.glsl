#version 330 core
uniform sampler2D mainTex;

in Vertex
{                                               
	//vec4 colour;
	vec2 texCoord;
	vec3 worldPos;
	vec2 Cood;
} IN;

out vec4 fragColor;

void main(void) {
	fragColor=texture(mainTex,IN.texCoord)*vec4(0.1,0,0,1);

	//fragColor= vec4(1.0,0,0,1);
}