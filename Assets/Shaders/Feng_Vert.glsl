#version 400 core

uniform mat4 modelMatrix 	= mat4(1.0f);
uniform mat4 viewMatrix 	= mat4(1.0f);
uniform mat4 projMatrix 	= mat4(1.0f);
uniform mat4 shadowMatrix 	= mat4(1.0f);

layout(location = 0) in vec3 position;
//layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 binormal;

uniform vec4 		objectColour = vec4(1,1,1,1);

uniform bool hasVertexColours = false;

out Vertex
{
	//vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;

		float ViewDepth;
} OUT;

void main(void)
{
	
	mat4 mvp 		  = (projMatrix * viewMatrix * modelMatrix);
	mat3 normalMatrix = transpose ( inverse ( mat3 ( modelMatrix )));

	OUT.shadowProj 	=  shadowMatrix * vec4 ( position,1);
	OUT.worldPos 	= ( modelMatrix * vec4 ( position ,1)). xyz ;

	
	OUT.texCoord	= texCoord;
	
	
	 OUT.normal = normalize(normalMatrix * normalize(normal));

	 OUT.tangent = normalize(normalMatrix * normalize(tangent));
	 OUT.binormal = normalize(normalMatrix * normalize(binormal));
	//OUT.binormal = normalize(normalMatrix *normalize(cross(normal, tangent)));
	
	
	gl_Position		= mvp * vec4(position, 1.0);

	//caculate camera DOV burring

	

	
	gl_Position		= mvp * vec4(position, 1.0);
	OUT.ViewDepth=gl_Position.z;

}