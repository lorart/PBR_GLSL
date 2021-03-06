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

		float ViewDepth;
} IN;

out vec4 fragColor;

//normal distribution function Trowbridge-Reitz GGX normal distribution
//float NorDistribution_GGX(){

//}

void main(void)
{
 mat3 TBN = mat3 ( IN . tangent , IN . binormal , IN . normal );
 vec3 N = normalize ( TBN * ( texture ( normal_map ,IN . texCoord ). 
 rgb * 2.0 - 1.0));


//vec3 diffuse=texture(albedo_map, IN.texCoord).rgb+albedoValue;
vec3 objColour=texture(albedo_map, IN.texCoord).rgb+albedoValue;

float ambientStrength = 0.1;
vec3 ambient = ambientStrength * lightColour.rgb;
vec3 ambientResult = ambient * objColour;

vec3 incident = normalize(lightPos - IN.worldPos);
float diffuse = max(0.0, dot(N,incident));
vec3 diffuseResult = diffuse * lightColour.rgb;

float dist = length(lightPos - IN.worldPos);
float atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

vec3 viewDir = normalize(cameraPos - IN.worldPos);
vec3 halfDir = normalize(incident + viewDir);

	float rFactor = max(0.0, dot(halfDir, N)); // Different !
	float sFactor = pow(rFactor, 33.0);//specular light
	float specularStrength=0.5;
vec3 specularResult=	sFactor*lightColour.rgb*specularStrength;
/*
vec3 colour = (diffuse.rgb * lightColour.rgb);
	colour += (lightColour.rgb * sFactor) * 0.33;
	fragColor = vec4(colour * atten * lambert, 1);
	fragColor.rgb += (diffuse.rgb * lightColour.rgb) * 0.1;
	fragColor*=1.1;
	//fragColor.rgb= N;
	*/
fragColor.rgb = (ambientResult + diffuseResult + specularResult) *
 objColour;	


//camera projection
   float blur = 0;
 
    float near_distance = 10.0; // 近平面的模糊衰减范围
    float far_distance = 10.0; // 远平面的模糊衰减范围
 
    float near_plane = -20.0; // 近平面
    float far_plane = -25.0; // 远平面
 
    // 根据深度计算模糊因子
    if(IN.ViewDepth <= near_plane && IN.ViewDepth >= far_plane)
    {
        blur = 0;
    }
    else if(IN.ViewDepth > near_plane)
    {
        blur = clamp(IN.ViewDepth, near_plane, near_plane + near_distance);
        blur = (blur - near_plane) / near_distance;
    }
    else if(IN.ViewDepth < far_plane)
    {
        blur = clamp(IN.ViewDepth, far_plane - far_distance, far_plane);
        blur = (far_plane - blur) / far_distance;
    }
 
    // 将模糊因子写入alpha通道
    fragColor.a= blur;
}