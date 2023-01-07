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
	std::vector<glm::vec3> boundingBoxPoints;
	void MeshModel::setBoundingBox();

	// Helper functions to calculate the matrices according to the variables
	glm::mat4x4 ScaleMatrix(glm::vec3 v);
	glm::mat4x4 TranslationMatrix(glm::vec3 v);
	glm::mat4x4 X_RotationMatrix(glm::vec3 v);
	glm::mat4x4 Y_RotationMatrix(glm::vec3 v);
	glm::mat4x4 Z_RotationMatrix(glm::vec3 v);

	const bool MeshModel::GetWorldLocal()const;
	void MeshModel::SetWorldLocal(bool flag);

	const glm::mat4 GetScaleMatLoc()const;//
	const glm::mat4 GetTranslateMatwor()const;//
	const glm::mat4 GetRotatexMatwor()const;//
	const glm::mat4 GetRotateyMatwor()const;//
	const glm::mat4 GetRotatezMatwor()const;//
	const glm::mat4 GetTranslateMatLoc()const;//

	void SetLocalScaleMatrix(float scale_x, float scale_y, float scale_z);//
	void SetLocalTranslateMatrix(float translate_x, float  translate_y, float  translate_z);//
	void SetLocalRotateMatrix(float rotate_x, float  rotate_y, float  rotate_z);//
	void SetWorldScaleMatrix(float scale_x, float scale_y, float scale_z);//
	void SetWorldTranslateMatrix(float translate_x, float  translate_y, float  translate_z);//
	void SetWorldRotateMatrix(float rotate_x, float  rotate_y, float  rotate_z);//
	void SetShowAxis(bool flag);
	bool GetShowAxis();
	void SetBoundedBox(bool flag); 
	bool GetBoundedBox();
	void SetShowFaceNormals(bool flag);
	bool GetShowFaceNormals();
	void SetShowVertexNormals(bool flag);
	bool GetShowVertexNormals();
	glm::vec3 GetNormal(int index);


	void MeshModel::setWorldTransformation();
	glm::mat4x4& MeshModel::GetWorldTransformation();

	glm::mat4x4& MeshModel::GetLocalTransformation();
	void MeshModel::setLocalTransformation();

	const glm::mat4 MeshModel::GetTranformationMat();

	glm::mat4x4 worldTransform;
	glm::mat4x4 localTransform;

	

	std::vector<Face> faces_;
	std::vector<glm::vec3> vertices_;
	std::vector<glm::vec3> normals_;
	std::string model_name_;

	////////////////////////////// part 3 ////////////////////////////////////
	float min_x;
	float min_y;
	float min_z;
	float max_x;
	float max_y;
	float max_z;
	glm::vec3 color_b = glm::vec3(0,0,0);

	/* local matricies */
	//glm::mat4 local_scale_mat = glm::mat4(1);
	glm::mat4 local_scale_mat = { 50,  0,  0,  0,
		0,  50,  0,  0,
		0,  0,  25,  0,
		0,  0,  0,  1 };
	glm::mat4 local_translate_mat = glm::mat4(1);
	glm::mat4 local_X_rotation_mat = glm::mat4(1);
	glm::mat4 local_Y_rotation_mat = glm::mat4(1);
	glm::mat4 local_Z_rotation_mat = glm::mat4(1);

	/* world matricies */
	glm::mat4 world_scale_mat = glm::mat4(1);
	glm::mat4 world_translate_mat = glm::mat4(1);
	glm::mat4 world_X_rotation_mat = glm::mat4(1);
	glm::mat4 world_Y_rotation_mat = glm::mat4(1);
	glm::mat4 world_Z_rotation_mat = glm::mat4(1);

	glm::mat4 Trasformation;
	bool world_local = false;

	/* local variables of the matriecies */
	glm::vec3 local_translation = glm::vec3(0,0,0); // (translate_x,translate_y,translate_z)
	glm::vec3 local_rotation = glm::vec3(0,0,0); // (rotate_x,rotate_y,rotate_z)
	glm::vec3 local_scale = glm::vec3(20,20,25); // (scale_x,scale_y,scale_z)

	/* world variables of the matriecies */
	glm::vec3 world_translation = glm::vec3(0, 0, 0); 
	glm::vec3 world_rotation = glm::vec3(0, 0, 0); 
	glm::vec3 world_scale = glm::vec3(1, 1, 1); 


	bool show_axis = false;
	bool bounding_box = false;
	bool show_face_normals = false;
	bool show_vertex_normals = false;

	//////////////////////////////// part 4 ///////////////////////
	bool faces_bound = false;
	bool& GetFacesBound();
	//void setNormals();
	////std::vector<glm::vec3> verticesNormals;
	//std::vector<glm::vec3> FacesNormals;
	//std::vector<int> facesPerVertex;

};
