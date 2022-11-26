#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Face.h"

class MeshModel
{
public:
	MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name);
	virtual ~MeshModel();
	glm::vec3 color = glm::vec3(0, 0, 1);
	int GetFacesCount() const;
	const Face& GetFace(int index) const;
	const std::vector<Face> MeshModel::GetFacesList()const;
	const std::vector<glm::vec3> MeshModel::GetVerticesList()const;
	const std::string& GetModelName() const;
	const glm::vec3& GetVertex(int index) const;
	const glm::vec3& GetNormal(int index) const;

	void scaleLoadedMeshModel();
	glm::mat4x4 MeshModel::ScaleMatrix(glm::vec3 v);
	glm::mat4x4 MeshModel::TranslationMatrix(glm::vec3 v);
	glm::mat4x4 MeshModel::X_RotationMatrix(glm::vec3 v);
	glm::mat4x4 MeshModel::Y_RotationMatrix(glm::vec3 v);
	glm::mat4x4 MeshModel::Z_RotationMatrix(glm::vec3 v);

	const bool MeshModel::GetWorldLocal()const;
	void MeshModel::SetWorldLocal(bool flag);

	void MeshModel::setWorldTransformation();
	glm::mat4x4& MeshModel::GetWorldTransformation();
	glm::mat4x4& MeshModel::GetLocalTransformation();
	void MeshModel::setLocalTransformation();
	const glm::mat4 MeshModel::GetTranformationMat();



	std::vector<Face> faces_;
	std::vector<glm::vec3> vertices_;
	std::vector<glm::vec3> normals_;
	std::string model_name_;

	// Fields to support rotations/scaling/transformation
	glm::mat4x4 worldTransform;
	glm::mat4x4 localTransform;

	glm::vec3 worldTranslation;
	glm::vec3 worldRotation;
	glm::vec3 worldScale;
	//glm::vec3 worldTranslate;
	glm::vec3 localTranslation;
	glm::vec3 localRotation;
	glm::vec3 localScale;
	float local_scale;
	float world_scale;

	glm::mat4x4 MeshModel::scale(float a);

	int world_local = 0;


};
