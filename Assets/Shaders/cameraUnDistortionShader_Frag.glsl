#version 330 core
uniform sampler2D mainTex;
uniform int texWide;
uniform int texHight;

in Vertex
{	
	vec2 texCoord;
	vec3 worldPos;
} IN;

out vec4 fragColor;

vec2 LensDistortion(){
/* 
	Alpha1 and alpha2 are constants for the transformation 
	equations, they will be small positive numbers
eg: for test image seen so far
	Lens    alphax  alphay
	35mm     0.02   0.075
   40mm     0.016  0.059
   50mm     0.006  0.03
*/  
float alphax = 0.02;
float alphay = 0.08;
//tranform to NDC
vec4 NDC_Cood=vec4(gl_FragCoord.x/texWide* 2.0 - 1.0,
				 gl_FragCoord.y/texHight* 2.0 - 1.0  ,
				 gl_FragCoord.z * 2.0 - 1.0, 1.0);

float r=NDC_Cood.x*NDC_Cood.x+NDC_Cood.y*NDC_Cood.y;

vec2 DisNDC_Cood=vec2(NDC_Cood.x*(1-alphax*r),
					NDC_Cood.y*(1-alphay*r));

vec2 DisTex_Cood=vec2((DisNDC_Cood.x+1)*texWide/2,
					(DisNDC_Cood.y+1)*texHight/2);

DisTex_Cood=vec2(DisTex_Cood.x/texWide,
				DisTex_Cood.y/texHight);

DisTex_Cood.x=clamp(DisTex_Cood.x,0,1);
DisTex_Cood.y=clamp(DisTex_Cood.y,0,1);


//DisTex_Cood=DisNDC_Cood;
return DisTex_Cood;
}

void main(void) {
vec2 disTex_Cood=LensDistortion();
fragColor = texture(mainTex, disTex_Cood);
//fragColor.rg=disTex_Cood;
}
