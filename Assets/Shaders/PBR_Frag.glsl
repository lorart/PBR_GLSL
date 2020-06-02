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
uniform float metallicValue;
uniform float roughnessValue;
uniform float aoValue;


uniform sampler2D albedo_map;
uniform sampler2D metallic_map;
uniform sampler2D roughness_map;
uniform sampler2D ao_map;
uniform sampler2D normal_map;
const float PI = 3.14159265359;

in Vertex
{                                               
	
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

vec3  caculateNormal(){
mat3 TBN = mat3 ( IN . tangent , IN . binormal , IN . normal );
 vec3 N = normalize ( TBN * ( texture ( normal_map ,IN . texCoord ). 
 rgb * 2.0 - 1.0));
 return N;
}



float NoramlDistirbution_GGX(float roughnessValue,vec3 Normal,vec3 halfDir){//H is a vector


    roughnessValue=roughnessValue+1;//do sth to make sure the roughness value >0
	float roughnessValue2=roughnessValue*roughnessValue;
	float up=roughnessValue2;
	float NdotH=dot(Normal,halfDir);
	float down= NdotH*NdotH*(roughnessValue2-1)+1;      //if  roughnessValue is from [0,1] roughnessValue2-1 will be negitive
	down=down*down*PI;
	return up/down;
}

void main(void)
{
	//all the value
vec3 N=caculateNormal();
vec3 diffuse=texture(albedo_map, IN.texCoord).rgb+albedoValue;
vec3 metallicValue=texture(metallic_map,IN.texCoord).rgb+metallicValue;
float roughnessValue=texture(roughness_map,IN.texCoord).a+roughnessValue;
float aoValue=texture(ao_map,IN.texCoord).a+aoValue;

vec3 incident = normalize(lightPos - IN.worldPos);
vec3 viewDir = normalize(cameraPos - IN.worldPos);
vec3 halfDir = normalize(incident + viewDir);

float NorD_ggx=NoramlDistirbution_GGX(roughnessValue,N,halfDir);



//float lambert = max(0.0, dot(-incident, N));

//float dist = length(lightPos - IN.worldPos);
//float atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

//float rFactor = max(0.0, dot(halfDir, N)); // Different !
//float sFactor = pow(rFactor, 33.0);
vec3 seeNorD_gg = vec3  (NorD_ggx,NorD_ggx,NorD_ggx);
fragColor.rgb=diffuse+seeNorD_gg;
//fragColor.rgb=seeNorD_gg;

}