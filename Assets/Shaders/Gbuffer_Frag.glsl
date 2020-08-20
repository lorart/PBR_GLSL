#version 400 core
uniform sampler2D 	mainTex;
uniform sampler2D albedo_map;
uniform sampler2D normal_map;


uniform vec3	cameraPos;

uniform vec3 albedo;


in Vertex
{
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

vec3  caculateNormal(){
mat3 TBN = mat3 (IN.tangent ,IN.binormal,IN .normal);
 vec3 N = normalize ( TBN * ( texture ( normal_map ,IN.texCoord ). rgb * 2.0 - 1.0));
 return N;
}

void main(void)
{
	vec3 N=caculateNormal();
vec3 albedoValue=texture(albedo_map, IN.texCoord).rgb+albedo;
	fragColor.rgb=albedoValue;
}