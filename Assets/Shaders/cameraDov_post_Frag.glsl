#version 330
uniform sampler2D diffuseTex;


in Vertex{
	
	vec2 	texCoord;
	vec3 	position;
} IN;
void main(void) {
	gl_FragColor = vec4(1.0,0,0,1);
}