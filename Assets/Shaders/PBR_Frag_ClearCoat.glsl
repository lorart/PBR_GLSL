#version 400 core

uniform vec4 		objectColour;
uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform vec3	lightPos;
uniform float	lightRadius;                    
uniform vec4	lightColour;


uniform vec3	cameraPos;

uniform float	cameraFocusDistance;
uniform float	lens;

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

  
uniform int isUseClearcoat;
uniform float clearCoat;
uniform float clearCoatPerceptualRoughness;




in Vertex
{                                               
	
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;

    
	float ViewDepth;
} IN;

out vec4 fragColor;



vec3  caculateNormal(){
mat3 TBN = mat3 ( IN . tangent , IN . binormal , IN . normal );
 vec3 N = normalize ( TBN * ( texture ( normal_map ,IN . texCoord ). rgb * 2.0 - 1.0));
 return N;
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}

// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
float F_Schlick(float f0, float f90, float VoH) {
    return f0 + (f90 - f0) * pow(1.0 - VoH,5.0);
}

vec3 F_Schlick(float u, vec3 f0) {
    float f = pow(1.0 - u, 5.0);
    return f + f0 * (1.0 - f);
}
// ----------------------------------------------------------------------------
float V_Kelemen(float LoH) {
    return 0.25 / (LoH * LoH);
}




void main()
{		
vec3 N=caculateNormal();
vec3 albedoValue=texture(albedo_map, IN.texCoord).rgb+albedoValue;
float metallicValue=texture(metallic_map,IN.texCoord).r+metallicValue;
float roughnessValue=texture(roughness_map,IN.texCoord).r+roughnessValue;
float aoValue=texture(ao_map,IN.texCoord).r+aoValue;

vec3 L = normalize(lightPos - IN.worldPos);
vec3 V = normalize(cameraPos - IN.worldPos);
vec3 H = normalize(L+V );

float NdotH=max(dot(N,H),0.0);
float NdotV=max(dot(N,V),0.0);
float HdotV=max(dot(H,V),0.0);
float NdotL=max(dot(N,L),0.0);
float LdotH=max(dot(L,H),0.0);


 
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedoValue, metallicValue);

    vec3 Lo = vec3(0.0);
   // for(int i = 0; i < 4; ++i) 
   // {
   // calculate per-light radiance
    float distance = length(lightPos - IN.worldPos);
      float attenuation=1.0-clamp(distance / lightRadius,0.0,1.0);
      vec3 radiance=lightColour.rgb*attenuation;//ok

      //  float attenuation = 1.0 / (distance * distance);
      //  vec3 radiance = lightColour.rgb * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughnessValue);   
        float G   = GeometrySmith(N, V, L, roughnessValue);      
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
       
           
        vec3 nominator    = NDF * G * F; 
      //  float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
       // vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0
      vec3  specular=nominator;
        
        
        // kS is equal to Fresnel
         vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallicValue;
    
        // add to outgoing radiance Lo
        Lo += (kD * albedoValue / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
 //   }   

//clearcoat
    float  clearCoat_NoV= NdotV;
    vec3 color ;
if(isUseClearcoat !=0){
    
    float Fc = F_Schlick(0.04, 1.0, clearCoat_NoV) * clearCoat;
  
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedoValue;
    //clear coat
    diffuse*=1.0 - Fc;

    const float MAX_REFLECTION_LOD = 4.0;
     vec3 R = reflect(-V, N); 
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughnessValue * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughnessValue)).rg;
    vec3 specular1 = prefilteredColor * (F * brdf.x + brdf.y);

    specular1 *=1.0 - Fc;
        float clearCoatPerceptualRoughness1 = mix(0.089, 1.0, clearCoatPerceptualRoughness);
    //  float  clearCoatPerceptualRoughness1 = clamp(clearCoatPerceptualRoughness, 0.089, 1.0);
     vec3 prefilteredColor_c = textureLod(prefilterMap, R,  clearCoatPerceptualRoughness1 * MAX_REFLECTION_LOD).rgb; 
    specular1 +=prefilteredColor_c* (F * brdf.x + brdf.y) * Fc;
    
   //vec3 ambient = (kD * diffuse + specular) * ao;
    vec3 ambient = (kD * diffuse + specular1) ;
    color = ambient + Lo;

  //  float clearCoatPerceptualRoughness1 = mix(0.089, 1.0, clearCoatPerceptualRoughness);
    float clearCoatRoughness2 = clearCoatPerceptualRoughness1*clearCoatPerceptualRoughness1;
    float  Dc = DistributionGGX(N,H,clearCoatRoughness2);
    float  Vc = V_Kelemen(LdotH );

    float Frc = (Dc * Vc) * Fc;
      color =color * ((ambient + specular * (1.0 - Fc)) * (1.0 - Fc) + Frc);
     // color=vec3(Fc);

}
else{
  
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedoValue;
   
   
    const float MAX_REFLECTION_LOD = 4.0;
     vec3 R = reflect(-V, N); 
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughnessValue * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughnessValue)).rg;
    vec3 specular1 = prefilteredColor * (F * brdf.x + brdf.y);

//vec3 ambient = (kD * diffuse + specular) * ao;
    vec3 ambient = (kD * diffuse + specular1) ;
    color = ambient + Lo;
}

   




    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 


  // fragColor.rgb=  color;
   fragColor.rgb=color;


//camera projection
   float blur = 0;
    float lens_m=lens*0.001;
	float Coc = 0.029;//Circle of confusion 35mm  0.029
	float N1 = 64;
	float lens_aperture = lens / N1;

	float CFD = cameraFocusDistance  + 0.001;

	
	float HDis = (lens * lens / (N1 * Coc)+ lens)*0.001;
	//float H = 6.25;


	float near_Fplane = (HDis * CFD) / (HDis + CFD - lens_m);

	float far_Fplane = (HDis * CFD) / (HDis -CFD - lens_m);


	float near_distance = 0.7 * (0 - near_Fplane); // 
	float far_distance = 0.7 * (0 - far_Fplane); //
	
 
 
    // 根据深度计算模糊因子
    if(IN.ViewDepth <= near_Fplane && IN.ViewDepth >= far_Fplane)
    {
        blur = 0;
    }
    else if(IN.ViewDepth > near_Fplane)
    {
        blur = clamp(IN.ViewDepth, near_Fplane, near_Fplane + near_distance);
        blur = (blur - near_Fplane) / near_distance;
    }
    else if(IN.ViewDepth < far_Fplane)
    {
        blur = clamp(IN.ViewDepth, far_Fplane - far_distance, far_Fplane);
        blur = (far_Fplane - blur) / far_distance;
    }
 
    // 将模糊因子写入alpha通道
    fragColor.a= blur;


}
