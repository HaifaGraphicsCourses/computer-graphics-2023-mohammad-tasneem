#include "Light.h"

Light::Light() : position(0, 0, 0), ambient(1, 1, 1), diffuse(1, 1, 1), specular(1, 1, 1)
{
}

Light::~Light()
{
}

glm::vec3& Light::GetPosition()
{
	return position;
}

glm::vec3& Light::GetAmbient()
{
	return ambient;
}

glm::vec3& Light::GetDiffuse()
{
	return diffuse;
}

glm::vec3& Light::GetSpecular()
{
	return specular;
}
