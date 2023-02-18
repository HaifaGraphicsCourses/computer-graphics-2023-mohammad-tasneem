#include "Camera.h"

Camera::Camera()
{
	zNear = 4;
	zFar = 10;
	zNearPer = 100;
	zFarPer = -100;
	right = 100;
	left = -100;
	top = 100;
	bottom = -100;
	view_transformation = glm::mat4x4(1.0f);
	eye.x = 0;
	eye.y = 0;
	eye.z = 1;
	up = glm::vec3(0.0f);
	at = glm::vec3(0.0f);
	up.y = 1;
}
Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, const float aspectRatio) :
	fovy(glm::pi<float>() / 4.0f),
	height(5),
	zNear(0.1f),
	zFar(200.0f),
	aspect(aspectRatio),
	viewTransformation(1),
	eye(eye),
	at(at),
	up(up)
{
	viewTransformation = glm::lookAt(eye, at, up);
}


Camera::~Camera()
{

}

glm::mat4x4 Camera::GetProjectionTransformation() const
{
	if (is_orth)
		return glm::ortho(left, right, bottom, top, zNear, zFar);
	else
		return glm::perspective(fovy, aspect, zNear, zFar);
}

glm::mat4x4 Camera::GetViewTransformation() const
{
	glm::mat4x4 lookat = glm::lookAt(eye, at, up);
	return lookat * inversemat;
}

glm::vec3 Camera::GetWorldPos()
{
	glm::mat4x4 lookat = glm::lookAt(eye, at, up);
	return glm::inverse(lookat * inversemat) * glm::vec4(0, 0, 0, 1);
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{
	this->at = glm::vec3(at);
	this->eye = glm::vec3(eye);
	this->up = glm::vec3(up);
}
void Camera::SetLocalTranslateMatrix(float translate_x, float  translate_y, float  translate_z)
{
	translate_loc_x = translate_x;
	translate_loc_y = translate_y;
	translate_loc_z = translate_z;
	translate_mat_loc[3][0] = translate_x;
	translate_mat_loc[3][1] = translate_y;
	translate_mat_loc[3][2] = translate_z;
	inversemat = glm::inverse(translate_mat_wor * rotation_mat_worX * rotation_mat_worY * rotation_mat_worZ * translate_mat_loc * rotation_mat_locX * rotation_mat_locY * rotation_mat_locZ);
}
void Camera::SetLocalRotateMatrix(float rotate_x, float  rotate_y, float  rotate_z)
{
	rotate_loc_x = rotate_x;
	rotate_loc_y = rotate_y;
	rotate_loc_z = rotate_z;
	float radians = glm::radians(rotate_x);
	rotation_mat_locX[1][1] = cos(radians);
	rotation_mat_locX[2][1] = -sin(radians);
	rotation_mat_locX[1][2] = sin(radians);
	rotation_mat_locX[2][2] = cos(radians);
	radians = glm::radians(rotate_y);
	rotation_mat_locY[0][0] = cos(radians);
	rotation_mat_locY[2][0] = -sin(radians);
	rotation_mat_locY[0][2] = sin(radians);
	rotation_mat_locY[2][2] = cos(radians);
	radians = glm::radians(rotate_z);
	rotation_mat_locZ[0][0] = cos(radians);
	rotation_mat_locZ[1][0] = -sin(radians);
	rotation_mat_locZ[0][1] = sin(radians);
	rotation_mat_locZ[1][1] = cos(radians);
	inversemat = glm::inverse(translate_mat_wor * rotation_mat_worX * rotation_mat_worY * rotation_mat_worZ * translate_mat_loc * rotation_mat_locX * rotation_mat_locY * rotation_mat_locZ);
}
void Camera::SetWorldTranslateMatrix(float translate_x, float  translate_y, float  translate_z)
{
	translate_wor_x = translate_x;
	translate_wor_y = translate_y;
	translate_wor_z = translate_z;
	translate_mat_wor[3][0] = translate_x;
	translate_mat_wor[3][1] = translate_y;
	translate_mat_wor[3][2] = translate_z;
	inversemat = glm::inverse(translate_mat_wor * rotation_mat_worX * rotation_mat_worY * rotation_mat_worZ * translate_mat_loc * rotation_mat_locX * rotation_mat_locY * rotation_mat_locZ);
}
void Camera::SetWorldRotateMatrix(float rotate_x, float  rotate_y, float  rotate_z)
{
	rotate_wor_x = rotate_x;
	rotate_wor_y = rotate_y;
	rotate_wor_z = rotate_z;
	float radians = glm::radians(rotate_x);
	rotation_mat_worX[1][1] = cos(radians);
	rotation_mat_worX[2][1] = -sin(radians);
	rotation_mat_worX[1][2] = sin(radians);
	rotation_mat_worX[2][2] = cos(radians);
	radians = glm::radians(rotate_y);
	rotation_mat_worY[0][0] = cos(radians);
	rotation_mat_worY[2][0] = -sin(radians);
	rotation_mat_worY[0][2] = sin(radians);
	rotation_mat_worY[2][2] = cos(radians);
	radians = glm::radians(rotate_z);
	rotation_mat_worZ[0][0] = cos(radians);
	rotation_mat_worZ[1][0] = -sin(radians);
	rotation_mat_worZ[0][1] = sin(radians);
	rotation_mat_worZ[1][1] = cos(radians);
	inversemat = glm::inverse(translate_mat_wor * rotation_mat_worX * rotation_mat_worY * rotation_mat_worZ * translate_mat_loc * rotation_mat_locX * rotation_mat_locY * rotation_mat_locZ);
}
float Camera::GetzNear()
{
	return this->zNear;
}
float Camera::GetzFar()
{
	return this->zFar;
}
float Camera::GetLeft()
{
	return this->left;
}
float Camera::GetRight()
{
	return this->right;
}
float Camera::GetTop()
{
	return this->top;
}
float Camera::GetBottom()
{
	return this->bottom;
}
void Camera::SetzNear(float Near)
{
	this->zNear = Near;
}
void Camera::SetzFar(float Far)
{
	this->zFar = Far;
}
void Camera::SetLeft(float left)
{
	this->left = left;
}
void Camera::SetRight(float right)
{
	this->right = right;
}
void Camera::SetTop(float top)
{
	this->top = top;
}
void Camera::SetBottom(float bottom)
{
	this->bottom = bottom;
}
void Camera::set_perspective(float fov) {
	fovy = fov;
	projection_transformation = glm::perspective(fov, aspect, zNear, zFar);
}
void Camera::Setorth(bool is_orth)
{
	this->is_orth = is_orth;
}
bool Camera::Getorth()
{
	return (this->is_orth);
}
void Camera::SetAspect(float Aspect)
{
	this->aspect = Aspect;
}
glm::mat4x4 Camera::GetWorldLocal()
{
	glm::mat4x4 localTrans = translate_mat_loc * rotation_mat_locX * rotation_mat_locY * rotation_mat_locZ;
	glm::mat4x4 worldTrans = translate_mat_wor * rotation_mat_worX * rotation_mat_worY * rotation_mat_worZ;
	return  worldTrans * localTrans;
}
void Camera::IncremntalTransform(int flag)
{
	glm::mat4x4 TranslateTransform = glm::mat4x4(1);
	if (flag == 1) //translate left
	{

		TranslateTransform[3][0] = 1.5;
		TranslateTransform[3][1] = translate_loc_y;
		TranslateTransform[3][2] = translate_loc_z;
		TranslateTransform = glm::inverse(TranslateTransform);
		this->translate_loc_x += 1.5;
	}
	else//translate right
	{

		TranslateTransform[3][0] = -1.5;
		TranslateTransform[3][1] = translate_loc_y;
		TranslateTransform[3][2] = translate_loc_z;
		TranslateTransform = glm::inverse(TranslateTransform);
		this->translate_loc_x -= 1.5;
	}
	inversemat = inversemat * TranslateTransform;
}
float Camera::GetTranslateLocx()//returning the local x translation  value
{
	return this->translate_loc_x;
}
float Camera::GetTranslateLocy()//returning the local y translation  value
{
	return this->translate_loc_y;
}
float Camera::GetTranslateLocz()//returning the local z translation  value
{
	return this->translate_loc_z;
}
float Camera::GetTranslateWorx()//returning the world x translation  value
{
	return this->translate_wor_x;
}
float Camera::GetTranslateWory()//returning the world y translation  value
{
	return this->translate_wor_y;
}
float Camera::GetTranslateWorz()//returning the world z translation  value
{
	return this->translate_wor_z;
}
float Camera::GetRotateLocx()//returning the local x rotation  value
{
	return this->rotate_loc_x;
}
float Camera::GetRotateLocy()//returning the local y rotation  value
{
	return this->rotate_loc_y;
}
float Camera::GetRotateLocz()//returning the local z rotation  value
{
	return this->rotate_loc_z;
}
float Camera::GetRotateWorx()//returning the world x rotation  value
{
	return this->rotate_wor_x;
}
float Camera::GetRotateWory()//returning the world y rotation  value
{
	return this->rotate_wor_y;
}
float Camera::GetRotateWorz()//returning the world z rotation  value
{
	return this->rotate_wor_z;
}