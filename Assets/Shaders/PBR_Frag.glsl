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
//value+ map;

in Vertex
{
	//vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void)
{
	
vec3 normal=texture(normal_map, IN.texCoord).rgb;
vec3 colour=texture(albedo_map, IN.texCoord).rgb;

fragColor.rgb= lightColour.rgb;
//fragColor.rgb= vec3(0,1,1);
//fragColor.rgb = IN.normal;

	//fragColor = IN.colour;
}