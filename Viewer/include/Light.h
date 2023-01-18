#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Light
{
public:
	Light();
	virtual ~Light();

	glm::vec3& GetPosition();
	glm::vec3& GetAmbient();
	glm::vec3& GetDiffuse();
	glm::vec3& GetSpecular();

private:
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};
