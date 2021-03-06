#include "OGLMaterial.h"

NCL::Rendering::OGLMaterial::OGLMaterial(std::vector<OGLTexture*> pbrTexArry, Vector3 albedoValue, 
	Vector3 normalValue, float metallicValue, float roughnessValue)
{
	this->pbrTexArry = pbrTexArry;
	this->albedoValue = albedoValue;
	this->normalValue = normalValue;
	this->metallicValue = metallicValue;
	this->roughnessValue = roughnessValue;
	this->aoValue = aoValue;
	this->matShader = nullptr;
	this->FengShader = nullptr;

	this->isUseClearcoat=0;
	this->clearCoat=0;
	this->clearCoatPerceptualRoughness=0;

}


NCL::Rendering::OGLMaterial::OGLMaterial(std::vector<OGLTexture*> pbrTexArry

)
{
	this->pbrTexArry = pbrTexArry;
	this->albedoValue = Vector3(0, 0, 0);
	this->normalValue = Vector3(0, 0, 0);
	this->metallicValue = 0.01;
	this->roughnessValue = 0.01;
	this->aoValue = 0;
	this->matShader = nullptr;
	this->FengShader = nullptr;

	this->isUseClearcoat = 0;
	this->clearCoat = 0;
	this->clearCoatPerceptualRoughness = 0;


}

NCL::Rendering::OGLMaterial::~OGLMaterial()
{ 
	delete matShader;
	delete FengShader;

}
