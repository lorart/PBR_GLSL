#include "OGLMaterial.h"

NCL::Rendering::OGLMaterial::OGLMaterial(std::vector<OGLTexture*> pbrTexArry, Vector3 albedoValue, 
	Vector3 normalValue, int metallicValue, int roughnessValue)
{
	this->pbrTexArry = pbrTexArry;
	this->albedoValue = albedoValue;
	this->normalValue = normalValue;
	this->metallicValue = metallicValue;
	this->roughnessValue = roughnessValue;
	this->aoValue = aoValue;
	this->matShader = new OGLShader("PBR_Vert.glsl", "PBR_Frag.glsl");

}
NCL::Rendering::OGLMaterial::OGLMaterial(std::vector<OGLTexture*> pbrTexArry

	)
{
	this->pbrTexArry = pbrTexArry;
	this->albedoValue = Vector3(0, 0, 0);
	this->normalValue = Vector3(0, 0, 0);
	this->metallicValue = 0;
	this->roughnessValue = 0;
	this->aoValue = 0;
	this->matShader = new OGLShader("PBR_Vert.glsl", "PBR_Frag.glsl");

}

NCL::Rendering::OGLMaterial::~OGLMaterial()
{ 
	delete matShader;

}
