#version 330 core
uniform sampler2D mainTex;

in Vertex
{                                               
	//vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void) {
	fragColor=texture(mainTex,IN.texCoord);

	//fragColor= vec4(1.0,0,0,1);
}