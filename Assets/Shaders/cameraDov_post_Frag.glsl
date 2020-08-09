#version 330 core
uniform sampler2D mainTex;
uniform sampler2DMS MutiTex;
uniform int nMultiSample;

in Vertex
{	
	vec2 texCoord;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void) {


	ivec2 texSize = textureSize(MutiTex);
  vec4 fTexCol = vec4(0.0);
   ivec2 Coord= ivec2(texSize*IN.texCoord);

	if( nMultiSample==0)
	{

	fragColor = texture(mainTex,IN.texCoord);
	}
	else
	{
		for(int i =0; i< nMultiSample;i++)
		{
		fTexCol+=texelFetch(MutiTex,ivec2(Coord*texSize),i);
		}
		fragColor = fTexCol / nMultiSample;

		fragColor=texelFetch(MutiTex,ivec2(Coord*texSize),1);
	//	fragColor = vec4(0.8);
	}
	fragColor=fragColor*vec4(1,0.8,0.8,1);
}
