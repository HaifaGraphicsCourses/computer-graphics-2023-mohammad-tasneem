#include "Scene.h"
#include "MeshModel.h"
#include <string>
#include "Utils.h"

Scene::Scene() :
	active_camera_index_(0),
	active_model_index_(0)
{

}

void Scene::AddModel(const std::shared_ptr<MeshModel>& mesh_model)
{
	mesh_models_.push_back(mesh_model);
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
	std::string path = "..\\Data\\camera.obj";
	std::shared_ptr<MeshModel>& model = Utils::LoadMeshModel(path);

	int id = cameras_.size();

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