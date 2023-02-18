#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Camera
{
public:
	Camera();
	Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, const float aspectRatio);
	virtual ~Camera();
	void SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up);
	void SetLocalTranslateMatrix(float translate_x, float  translate_y, float  translate_z);
	void SetLocalRotateMatrix(float rotate_x, float  rotate_y, float  rotate_z);
	void SetWorldTranslateMatrix(float translate_x, float  translate_y, float  translate_z);
	void SetWorldRotateMatrix(float rotate_x, float  rotate_y, float  rotate_z);
	glm::mat4x4 GetProjectionTransformation() const;
	glm::mat4x4 GetViewTransformation() const;
	glm::vec3 GetWorldPos();
	void Camera::set_perspective(float fov);
	float GetzNear();
	float GetzFar();
	float GetLeft();
	float GetRight();
	float GetTop();
	float GetBottom();
	void SetzNear(float Near);
	void SetzFar(float Far);
	void SetLeft(float left);
	void SetRight(float right);
	void SetTop(float top);
	void SetBottom(float bottom);
	void SetAspect(float Aspect);
	void Camera::Setorth(bool is_orth);
	bool Camera::Getorth();
	glm::mat4x4 GetWorldLocal();
	void IncremntalTransform(int flag);
	float GetTranslateLocx();
	float GetTranslateLocy();
	float GetTranslateLocz();
	float GetRotateLocx();
	float GetRotateLocy();
	float GetRotateLocz();
	float GetTranslateWorx();
	float GetTranslateWory();
	float GetTranslateWorz();
	float GetRotateWorx();
	float GetRotateWory();
	float GetRotateWorz();
private:
	glm::mat4x4 view_transformation = glm::mat4x4(1.0f);
	glm::mat4x4 projection_transformation;
	glm::mat4x4 translate_mat_loc = glm::mat4(1.0f);
	glm::mat4x4 rotation_mat_locX = glm::mat4(1.0f);
	glm::mat4x4 rotation_mat_locY = glm::mat4(1.0f);
	glm::mat4x4 rotation_mat_locZ = glm::mat4(1.0f);
	glm::mat4x4 translate_mat_wor = glm::mat4(1.0f);
	glm::mat4x4 rotation_mat_worX = glm::mat4(1.0f);
	glm::mat4x4 rotation_mat_worY = glm::mat4(1.0f);
	glm::mat4x4 rotation_mat_worZ = glm::mat4(1.0f);
	glm::vec3 eye;
	glm::vec3 up;
	glm::vec3 at;
	glm::mat4 inversemat = glm::inverse(translate_mat_wor * rotation_mat_worX * rotation_mat_worY * rotation_mat_worZ * translate_mat_loc * rotation_mat_locX * rotation_mat_locY * rotation_mat_locZ);
	bool perspective = false;
	float zNear;
	float zFar;
	float zNearPer;
	float zFarPer;
	float left;
	float right;
	float bottom;
	float top;
	float fovy;
	float aspect = 1280 / 640;
	float is_orth = 1;
	float height;
	glm::mat4x4 viewTransformation;
	//-------------------------
	float translate_loc_x = 0;
	float translate_loc_y = 0;
	float translate_loc_z = 0;
	float rotate_loc_x = 0;
	float rotate_loc_y = 0;
	float rotate_loc_z = 0;
	//--------------------------
	float translate_wor_x = 0;
	float translate_wor_y = 0;
	float translate_wor_z = 0;
	float rotate_wor_x = 0;
	float rotate_wor_y = 0;
	float rotate_wor_z = 0;
};
