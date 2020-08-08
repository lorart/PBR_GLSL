#version 330 core
uniform sampler2D mainTex;
uniform sampler2DMS MutiTex;
uniform int nMultiSample;　

in Vertex
{
	//vec4 colour;
	vec2 texCoord;
	vec3 worldPos;
	vec2 Cood;
} IN;

out vec4 fragColor;

void main(void) {
//fragColor=texture(mainTex,IN.texCoord);

	ivec2 texSize = textureSize(MutiTex);
     vec4 fTexCol = vec4(0.0);

	if( nMultiSample==0)
	{
	//FragColor = texelFetch(MutiTex,ivec2(Coord * texSize),0);
	FragColor = texture(mainTex,IN.texCoord);
	}
	else
	{
		for(int i =0; i< nMultiSample;i++)
		{
		fTexCol += texelFetch(MutiTex, ivec2(IN.Coord * texSize), i);　
		}
		fragColor = fTexCol / nMultiSample;
	}
}