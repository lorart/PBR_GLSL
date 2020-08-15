#version 330 core
uniform sampler2D mainTex;
uniform int texWide;
uniform int texHight;
uniform int lens;

in Vertex
{	
	vec2 texCoord;
	vec3 worldPos;
} IN;

out vec4 fragColor;

/* 
	Alpha1 and alpha2 are constants for the transformation 
	equations, they will be small positive numbers

	 reference:http://paulbourke.net/miscellaneous/lens/distort.c
*/  
/* 
	Alpha1 and alpha2 are constants for the transformation 
	equations, they will be small positive numbers
	Determined for a particular lens by trial and error
	If the lines are bowed inwards (concave) then increase
   the appropriate constant. If they are bowed out (convex)
   then decrease the corresponding constant.
	eg: for test image seen so far
	Lens    alphax  alphay
	35mm     0.02   0.075
   40mm     0.016  0.059
   50mm     0.006  0.03
*/
float caculateAlphaX(){
	//appropriate by the given alphaX Y 
	
	//35-40
	if(lens<35){
		return 0.02;

	}
	else if(lens>=35&&lens<=40){
		return 0.02+(lens-35)/5*(0.016-0.02);

	}
	//40-50
	else if(lens>40&&lens<=50){
		return 0.016+(lens-40)/10*( 0.006- 0.16);
	}
	else{
		return 0.006;
	}
}
/*
	Lens    alphax  alphay
	35mm     0.02   0.075
   40mm     0.016  0.059
   50mm     0.006  0.03
*/
float caculateAlphaY(){
	//appropriate by the given alphaX Y 
	if(lens<35){
		return 0.075;

	}
	//35-40
	else if(lens>=35&&lens<=40){
		return 0.075+(lens-35)/5*(0.059-0.075);

	}

	//40-50
	else if(lens>40&&lens<=50){
		return 0.059+(lens-40)/10*(0.03-0.059);
	}
	else{
		return 0.03;
	}
}

vec2 LensDistortion(){

float alphax = caculateAlphaX();
float alphay = caculateAlphaY();
//tranform to NDC



vec4 NDC_Cood=vec4(gl_FragCoord.x/texWide* 2.0 - 1.0,
				 gl_FragCoord.y/texHight* 2.0 - 1.0  ,
				 gl_FragCoord.z * 2.0 - 1.0, 1.0);

float p=NDC_Cood.x*NDC_Cood.x+NDC_Cood.y*NDC_Cood.y;

vec2 PX=NDC_Cood.xy;
float x_non=PX.x;
float y_non=PX.y;
float x2=PX.x/(1-alphax*p);
float y2=PX.y/(1-alphay*p);
float x_denon=1-alphax*(x2*x2+y2*y2);
float y_denon=1-alphay*(x2*x2+y2*y2);

vec2 DisTex_Cood=vec2( x_non/x_denon,y_non/y_denon);
DisTex_Cood=vec2((DisTex_Cood.x+1)/2,
                  (DisTex_Cood.y+1)/2 );
return DisTex_Cood;
}

void main(void) {
vec2 disTex_Cood=LensDistortion();
fragColor = texture(mainTex, disTex_Cood);
//fragColor.rg =disTex_Cood;

}
