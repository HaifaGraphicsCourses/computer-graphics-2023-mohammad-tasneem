#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <MeshModel.h>
#include <memory>

class Camera : public MeshModel
{
public:
	Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, std::shared_ptr<MeshModel>& model, int id,const float aspectRatio);
	virtual ~Camera();

	float GetzNear();
	float GetzFar();
	float GetLeft();
	float GetRight();
	float GetTop();
	float GetBottom();

	void Camera::SetzNear(float Near);
	void  Camera::SetzFar(float Far);
	void  Camera::SetLeft(float left);
	void  Camera::SetRight(float right);
	void  Camera::SetTop(float top);
	void  Camera::SetBottom(float bottom);
	void  Camera::SetAspect(float Aspect);

	void SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up);
	void Camera::SetPerspectiveProjection(const float fovy, const float aspectRatio, const float near, const float far);
	void Camera::SetOrthographicProjection(const float l, const float r, const float b, const float t, const float n, const float f);
	void SetPerspective(const float left, const float right, const float bottom, const float top, const float near, const float far);
	const glm::mat4x4& GetProjectionTransformation() const;
	const glm::mat4x4& GetViewTransformationold() const;
	const glm::mat4x4& Camera::GetViewTransformation() const;
	void Camera::IncremntalTransform(int flag);
	bool Getorth();
	void Camera::set_perspective(float fov);
	void Camera::Setorth(bool is_orth);


private:
	glm::mat4x4 view_transformation_;
	glm::mat4x4 projection_transformation_;

	glm::vec3 eye = glm::vec3(0, 0, 1);
	glm::vec3 up = glm::vec3(0,1,0);
	glm::vec3 at = glm::vec3(0, 0, 0);

	glm::mat4 inversemat = glm::inverse(world_translate_mat * world_X_rotation_mat
	* world_Y_rotation_mat * world_Z_rotation_mat * local_translate_mat * 
	local_X_rotation_mat * local_Y_rotation_mat * local_Z_rotation_mat);

	float zNear = 80;
	float zFar = -80;
	float zNearPer = 80;
	float zFarPer = -80;
	float left = -80;
	float right = 80;
	float bottom = -80;
	float top = 80;
	float fovy;
	float aspect = 1280 / 640;
	bool is_orth = 1;
	bool perspective = false;
	float height;
};
