#include "OGLMaterial.h"

NCL::Rendering::OGLMaterial::OGLMaterial(std::vector<OGLTexture*> pbrTexArry, Vector3 albedoValue, Vector3 normalValue, int metallicValue,
	int roughnessValue, int aoValue)
{
	this->pbrTexArry = pbrTexArry;
	this->albedoValue = albedoValue;
	this->normalValue = normalValue;
	this->metallicValue = metallicValue;
	this->roughnessValue = roughnessValue;
	this->aoValue = aoValue;

}

NCL::Rendering::OGLMaterial::~OGLMaterial()
{

}
