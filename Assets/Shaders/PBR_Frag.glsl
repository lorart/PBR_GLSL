#version 400 core

uniform vec4 		objectColour;
uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;

uniform vec3	lightPos;
uniform float	lightRadius;                    
uniform vec4	lightColour;

uniform vec3	cameraPos;

uniform bool hasTexture;

uniform vec3 albedoValue;
uniform vec3 metallicValue;
uniform float roughnessValue;
uniform float aoValue;
uniform float normalValue;//todo

uniform sampler2D albedo_map;
uniform sampler2D metallic_map;
uniform sampler2D roughness_map;
uniform sampler2D ao_map;
uniform sampler2D normal_map;
const float PI = 3.14159265359;
//value+ map;

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

//normal distribution function Trowbridge-Reitz GGX normal distribution
//float NorDistribution_GGX(){

//}

void main(void)
{
 mat3 TBN = mat3 ( IN . tangent , IN . binormal , IN . normal );
 vec3 N = normalize ( TBN * ( texture ( normal_map ,IN . texCoord ). rgb * 2.0 - 1.0));


vec3 colour=texture(albedo_map, IN.texCoord).rgb;

fragColor.rgb= N;
//fragColor.rgb= vec3(0,1,1);
//fragColor.rgb = IN.normal;

	//fragColor = IN.colour;
}