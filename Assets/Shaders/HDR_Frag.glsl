#version 400 core

uniform vec4 		objectColour;
uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;

uniform vec3	lightPos;
uniform float	lightRadius;
uniform vec4	lightColour;

uniform vec3	cameraPos;

uniform bool hasTexture;

in Vertex
{
vec3 localPos;
} IN;

out vec4 fragColor;


const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)   /*reference:https://learnopengl.com/PBR/IBL/Diffuse-irradiance*/  //From Equirectangular to Cubemap
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{       
    vec2 uv = SampleSphericalMap(normalize(IN.localPos)); // make sure to normalize localPos
    vec3 color = texture(mainTex, uv).rgb;

    fragColor = vec4(color, 1.0);
}
