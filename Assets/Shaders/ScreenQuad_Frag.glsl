#version 330 core
uniform sampler2D mainTex;
uniform sampler2DMS MutiTex;
uniform int nMultiSample;　

in Vertex
{                                               
	//vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void) {
	fragColor=texture(mainTex,IN.texCoord);

	 ivec2 texSize = textureSize(MutiTex);
 
     vec4 fTexCol = vec4(0.0);
 
    if( 0 == nMultiSample )
     {
         FragColor = texelFetch(MutiTex,ivec2(Coord * texSize),0);
     }
     else
     {
         for( int i = 0 ; i < nMultiSample ; i++ )
         {
             fTexCol += texelFetch(MutiTex, ivec2(Coord * texSize), i);　
         }
         FragColor = fTexCol / nMultiSample;
     }

	