#define SAMPLES_COUNT 32  
#define INV_SAMPLES_COUNT (1.0f / SAMPLES_COUNT)   
uniform sampler2D decal;  // decal texture  uniform sampler2D spot;  
 // projected spotlight image  uniform sampler2DShadow shadowMap;  // shadow map 
uniform float fwidth;
uniform vec2 offsets[SAMPLES_COUNT];    // these are passed down from vertex shader  
varying vec4 shadowMapPos;  
varying vec3 normal; 
varying vec2 texCoord;  
varying vec3 lightVec;  
varying vec3 view; 

void main(void)  { 
    float shadow = 0;   
    float fsize = shadowMapPos.w * fwidth;  
    vec4 smCoord = shadowMapPos;    
    for (int i = 0; i<SAMPLES_COUNT; i++) {  
    smCoord.xy = offsets[i] * fsize + shadowMapPos;  
    shadow += texture2DProj(shadowMap, smCoord) * INV_SAMPLES_COUNT;   
 } 
  vec3 N = normalize(normal);   
 vec3 L = normalize(lightVec); 
   vec3 V = normalize(view);  
    vec3 R = reflect(-V, N);      // calculate diffuse dot product   
   float NdotL = max(dot(N, L), 0);      // modulate lighting with the computed shadow value 
   vec3 color = texture2D(decal, texCoord).xyz;    
  gl_FragColor.xyz = (color * NdotL + pow(max(dot(R, L), 0), 64)) *       
                 shadow * texture2DProj(spot, shadowMapPos) +         
               color * 0.1;
 }