#include "Scene.h"
#include "MeshModel.h"
#include <string>
#include "Utils.h"

Scene::Scene() :
	active_camera_index_(0),
	active_model_index_(0),
	active_light_index_(0)
{
	// load mesh model for rendering light sources
	std::string path = "..\\Data\\demo.obj";
	light_model_ = Utils::LoadMeshModel(path);
	if (light_model_->vertices_.empty())
		std::cout << std::endl << "Error loading light source model: " << path << std::endl;
	else light_model_->setNormals();
}

void Scene::AddModel(const std::shared_ptr<MeshModel>& mesh_model)
{
	mesh_models_.push_back(mesh_model);
	mesh_models_.back()->setNormals();

	// default materials for 1st model (blue)
	if (mesh_models_.size() == 1)
	{
		glm::vec3 color(0.5f, 0.5f, 1.0f);
		mesh_models_.back()->GetAmbient() = color;
		mesh_models_.back()->GetDiffuse() = color;
		mesh_models_.back()->GetSpecular() = color;
	}

	// default materials for 2nd model (green)
	if (mesh_models_.size() == 2)
	{
		glm::vec3 color(0.5f, 1.0f, 0.5f);
		mesh_models_.back()->GetAmbient() = color;
		mesh_models_.back()->GetDiffuse() = color;
		mesh_models_.back()->GetSpecular() = color;
	}
}

int Scene::GetModelCount() const
{
	return mesh_models_.size();
}

MeshModel& Scene::GetModel(int index) const
{
	return *mesh_models_[index];
}

MeshModel& Scene::GetActiveModel() const
{
	return *mesh_models_[active_model_index_];
}

void Scene::AddCamera(glm::vec3 eye, glm::vec3 at, glm::vec3 up)
{


}
void Scene::AddCameranew(const std::shared_ptr<Camera>& camera)
{
	cameras_.push_back(camera);
}


int Scene::GetCameraCount() const
{
	return cameras_.size();
}

Camera& Scene::GetCamera(int index)
{
	return *cameras_[index];
}

Camera& Scene::GetActiveCamera()
{
	return *cameras_[active_camera_index_];
}

void Scene::SetActiveCameraIndex(int index)
{
	active_camera_index_ = index;
}

int Scene::GetActiveCameraIndex() const
{
	return active_camera_index_;
}

void Scene::SetActiveModelIndex(int index)
{
	active_model_index_ = index;
}

int Scene::GetActiveModelIndex() const
{
	return active_model_index_;
}

void Scene::SetDrawCamera(bool flag)
{
	this->draw_camera = flag;
}
bool Scene::GetDrawCamera() const
{
	return this->draw_camera;
}
MeshModel& Scene::GetCameraMesh() const
{
	return *this->camera_model;
}
int Scene::getiscolored()
{
	return this->is_colored;
}
void Scene::setiscolored(int iscolored)
{
	this->is_colored = iscolored;
}

int Scene::GetLightCount() const
{
	return lights_.size();
}

Light& Scene::GetLight(int index)
{
	return *lights_[index];
}

Light& Scene::GetActiveLight()
{
	return *lights_[active_light_index_];
}

MeshModel& Scene::GetLightModel() const
{
	return *light_model_;
}

void Scene::SetActiveLightIndex(int index)
{
	active_light_index_ = index;
}

void Scene::AddLight(const std::shared_ptr<Light>& light)
{
	lights_.push_back(light);

	// default properties for 1st light (red)
	if (lights_.size() == 1)
	{
		lights_.back()->GetPosition() = glm::vec3(-1, 1, 0);
		glm::vec3 color(1.0f, 0.5f, 0.5f);
		lights_.back()->GetAmbient() = color * 0.5f;
		lights_.back()->GetDiffuse() = color * 1.0f;
		lights_.back()->GetSpecular() = glm::vec3(1, 1, 1);
	}

	// default properties for 2nd light (yellow)
	if (lights_.size() == 2)
	{
		lights_.back()->GetPosition() = glm::vec3(1, 1, 0);
		glm::vec3 color(1.0f, 1.0f, 0.5f);
		lights_.back()->GetAmbient() = color * 0.5f;
		lights_.back()->GetDiffuse() = color * 1.0f;
		lights_.back()->GetSpecular() = glm::vec3(1, 1, 1);
	}
}

bool& Scene::GetGouraudShading()
{
	return gouraudShading;
}

bool& Scene::GetUseAmbientLighting()
{
	return useAmbientLighting;
}

bool& Scene::GetUseDiffuseLighting()
{
	return useDiffuseLighting;
}

bool& Scene::GetUseSpecularLighting()
{
	return useSpecularLighting;
}
