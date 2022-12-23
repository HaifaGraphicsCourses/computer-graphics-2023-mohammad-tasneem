#include "Camera.h"
#include <MeshModel.h>
#define M_PI        3.14159265358979323846264338327950288

Camera::Camera(const glm::vec3& eye, const glm::vec3 &at, const glm::vec3& up, std::shared_ptr<MeshModel>& model, int id, const float aspectRatio) :
	MeshModel(model->faces_, model->vertices_, model->normals_, "Camera " + std::to_string(id)),
	eye(eye),
	at(at),
	up(up)
{
	fovy = glm::pi<float>() / 4.0f;
	height = 5;
	zNear = 0.1f;
	zFar = 200.0f;
	aspect = aspectRatio;
	SetCameraLookAt(eye, at, up);
}

Camera::~Camera()
{
	
}

const glm::mat4x4& Camera::GetProjectionTransformation() const
{
	return projection_transformation_;
}

const glm::mat4x4& Camera::GetViewTransformationold() const
{
	return view_transformation_;
}

const glm::mat4x4& Camera::GetViewTransformation() const
{
	glm::mat4x4 localTrans = local_translate_mat * local_X_rotation_mat * local_Y_rotation_mat * local_Z_rotation_mat;
	glm::mat4x4 worldTrans = world_translate_mat * world_X_rotation_mat * world_Y_rotation_mat * world_Z_rotation_mat;
	glm::mat4x4 lookat = glm::lookAt(eye, at, up);
	glm::mat4x4 ortho = glm::ortho(left, right, bottom, top, zNear, zFar);
	glm::mat4x4 pers = glm::perspective(fovy, aspect, zNearPer, zFarPer);
	if (is_orth)
	{
		return ortho * lookat * inversemat;
	}
	else
	{
		return pers * lookat * inversemat;
	}
}
void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{
	this->at = glm::vec3(at);
	this->eye = glm::vec3(eye);
	this->up = glm::vec3(up);
	glm::vec3 z = normalize(eye - at);
	glm::vec3 x = normalize(glm::cross(up, z));
	glm::vec3 y = normalize(glm::cross(z, x));

	glm::mat4x4 m = glm::transpose(glm::mat4x4({
		x[0], x[1], x[2], 0,
		y[0], y[1], y[2], 0,
		z[0], z[1], z[2], 0,
		0, 0, 0, 1.0f }
	));
	glm::mat4x4 tm = glm::transpose(glm::mat4x4(
		{ 1, 0, 0, -eye[0],
			0, 1, 0, -eye[1],
			0, 0, 1, -eye[2],
			0, 0, 0, 1 }));

	view_transformation_ = m * tm;
}

void Camera::SetPerspective(const float left, const float right, const float bottom, const float top, const float near, const float far)
{
	projection_transformation_ =
		glm::transpose(
			glm::mat4x4{
			(2.0 * near) / (right - left), 0, (right + left) / (right - left), 0,
			0, (2.0 * near) / (top - bottom), (top + bottom) / (top - bottom), 0,
			0, 0, -1 * (far + near) / (far - near),	-1 * ((2 * far * near) / (far - near)),
			0, 0, -1, 0
			}
	);

}

void Camera::SetOrthographicProjection(const float l, const float r, const float b, const float t, const float n, const float f)
{
	projection_transformation_ =
		glm::transpose(
			glm::mat4((2.0f / (r - l)), 0.0f, 0.0f, -((r + l) / (r - l)),
				0.0f, (2.0f / (t - b)), 0.0f, -((t + b) / (t - b)),
				0.0f, 0.0f, (2.0f / (n - f)), -((f + n) / (f - n)),
				0.0f, 0.0f, 0.0f, 1.0f));
}

void Camera::SetPerspectiveProjection(const float fovy, const float aspectRatio, const float near, const float far)
{
	float k = float(M_PI) / float(180);
	float newFovy = fovy * k / 2;
	float height = glm::abs(glm::tan(newFovy / 2.0f) * near);
	float width = aspectRatio * height;
	SetPerspective(-width, width, -height, height, near, far);

}

void Camera::IncremntalTransform(int flag) {
	glm::mat4x4 TranslateTransform = glm::mat4x4(1);
	if (flag) //translate left
	{

		TranslateTransform[3][0] = 1.5;
		TranslateTransform[3][1] = this->local_translation[1];
		TranslateTransform[3][2] = this->local_translation[2];
		TranslateTransform = glm::inverse(TranslateTransform);
		this->local_translation[0] += 1.5;
	}
	else//translate right
	{

		TranslateTransform[3][0] = -1.5;
		TranslateTransform[3][1] = this->local_translation[1];
		TranslateTransform[3][2] = this->local_translation[2];
		TranslateTransform = glm::inverse(TranslateTransform);
		this->local_translation[0] -= 1.5;
	}
	inversemat = inversemat * TranslateTransform;
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
	projection_transformation_ = glm::perspective(fov, aspect, zNear, zFar);
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