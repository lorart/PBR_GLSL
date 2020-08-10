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



vec3  caculateNormal(){
mat3 TBN = mat3 ( IN . tangent , IN . binormal , IN . normal );
 vec3 N = normalize ( TBN * ( texture ( normal_map ,IN . texCoord ). rgb * 2.0 - 1.0));
 return N;
}



/*********************************************/
float NoramlDistirbution_GGX(float roughnessValue,float NdotH){ //trowbridge-Reitz

	float roughnessValue2=roughnessValue*roughnessValue;
	float roughnessValue4=roughnessValue2*roughnessValue2;
	float nom=roughnessValue4;
	float denom= NdotH*NdotH*(roughnessValue4-1.0)+1.0;      
	denom=denom*denom*PI;
	return nom/denom;
}





/*********************************************/
float Geometry_lmplicit(float NdotL,float NdotV){
	return NdotL*NdotV;
}
/*
float Geometry_SchlickGGX(float roughnessValue,float NdotV)
{
	float k=(roughnessValue+1)*(roughnessValue+1)/8.0;
	float nom=NdotV;
	float denom=NdotV*(1-k)+k;
	return nom/denom;

}
*/

float Geometry_Geometry_SchlickGGX(float roughnessValue,float NdotV){
	float k=roughnessValue*roughnessValue/2;
	float nom=NdotV;
	float denom=NdotV*(1-k)+k;
		return nom/denom;
}



/*********************************************/

vec3 Fresnel_Schlick(vec3 F0,float HdotV){  //which didn't look right
	
	return F0+(1-F0)*pow((1-HdotV),5.0);    
}



vec3 Cook_Torrance(float NorD,float Geo,vec3 Fre,float NdotV,float NdotL ){
	vec3 nom=NorD*Geo*Fre;
	float denom=4.0*NdotL*NdotV+0.00001;
	return nom/denom;
}




float CaculateShadow(){
		float shadow = 1.0; // New !
	
	if( IN . shadowProj . w > 0.0) { // New !
		shadow = textureProj ( shadowTex , IN . shadowProj ) * 0.5f;
	}
	return shadow;
}
void main(void)
{

vec3 N=caculateNormal();
vec3 albedoValue=texture(albedo_map, IN.texCoord).rgb+albedoValue;
float metallicValue=texture(metallic_map,IN.texCoord).r+metallicValue;
float roughnessValue=texture(roughness_map,IN.texCoord).r+roughnessValue;
float aoValue=texture(ao_map,IN.texCoord).r+aoValue;

vec3 incident = normalize(lightPos - IN.worldPos);
vec3 viewDir = normalize(cameraPos - IN.worldPos);
vec3 halfDir = normalize(incident + viewDir);

float NdotH=max(dot(N,halfDir),0.0);
float NdotV=max(dot(N,viewDir),0.0);
float HdotV=max(dot(halfDir,viewDir),0.0);
float NdotL=max(dot(N,incident),0.0);

//todo: get F0
vec3 F0=vec3(0.04);
F0=mix(F0,albedoValue,metallicValue);

/*********************************/

float NorD=Geometry_Geometry_SchlickGGX( roughnessValue,NdotV);


vec3 Fre=Fresnel_Schlick(F0, HdotV);//?
//float Geo=  Geometry_Geometry_SchlickGGX( roughnessValue, NdotV);
float Geo= Geometry_lmplicit( NdotL, NdotV);

vec3 specular=Cook_Torrance( NorD,Geo, Fre, NdotV, NdotL );//strange


float distance=length(lightPos-IN.worldPos);
float attenuation=1.0-clamp(distance/lightRadius,0.0,1.0);
vec3 radiance=lightColour.rgb*attenuation;//ok

vec3 ks=Fre;
vec3 kd=vec3(1.0)-ks;
//
 kd *= 1.0 - metallicValue;	
float shadow =CaculateShadow();

vec3 colour=(kd*albedoValue/PI+ specular)*radiance*NdotL;




//fragColor.rgb=NorD*Geo*Fre;

//fragColor.rgb=vec3(Geo);
//fragColor.rgb= specular;
//fragColor.rgb= specular
fragColor.rgb=vec3(shadow);
//fragColor.rgb=colour;


}