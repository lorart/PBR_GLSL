#include "OGLLight.h"

NCL::Rendering::OGLLight::OGLLight()
{
	this->lightColour = Vector4();
	this->lightPosition = Vector3();
	this->lightRadius = 100;
}

NCL::Rendering::OGLLight::~OGLLight()
{

}

NCL::Rendering::OGLLight::OGLLight(Vector4 lightColour, float lightRadius, Vector3 lightPosition)
{
	this->lightColour = lightColour;
	this->lightPosition = lightPosition;
	this->lightRadius = lightRadius;

}
