#pragma once

#include <vector>
#include <memory>

#include "Camera.h"
#include "MeshModel.h"
#include "Light.h"

class Scene {
public:
	Scene();

	std::shared_ptr<MeshModel> camera_model;

	void AddModel(const std::shared_ptr<MeshModel>& mesh_model);
	int GetModelCount() const;
	MeshModel& GetModel(int index) const;
	MeshModel& GetActiveModel() const;

	void AddCamera(glm::vec3 eye, glm::vec3 at, glm::vec3 up);
	int GetCameraCount() const;
	Camera& GetCamera(int index);
	Camera& GetActiveCamera();

	void SetActiveCameraIndex(int index);
	int GetActiveCameraIndex() const;

	void SetActiveModelIndex(int index);
	int GetActiveModelIndex() const;

	MeshModel& GetCameraMesh() const;
	void SetDrawCamera(bool flag);
	bool GetDrawCamera()const;
	void Scene::AddCameranew(const std::shared_ptr<Camera>& camera);

	int GetLightCount() const;
	Light& GetLight(int index);
	Light& GetActiveLight();
	MeshModel& GetLightModel() const;
	void SetActiveLightIndex(int index);
	void AddLight(const std::shared_ptr<Light>& light);
	bool useAmbientLighting = true;
	bool useDiffuseLighting = true;
	bool useSpecularLighting = true;
	bool& GetUseAmbientLighting();
	bool& GetUseDiffuseLighting();
	bool& GetUseSpecularLighting();
	int& GetMappingType();
	bool& GetUseToonShading();
	glm::vec3& GetToonShadingColor();
	int& GetToonShadingLevels();

private:
	std::vector<std::shared_ptr<MeshModel>> mesh_models_;
	std::vector<std::shared_ptr<Camera>> cameras_;
	std::vector<std::shared_ptr<Light>> lights_;
	std::shared_ptr<MeshModel> light_model_;

	int active_camera_index_;
	int active_model_index_;
	int active_light_index_;

	bool draw_camera = false;
	int mappingType = 0;
	bool useToonShading = false;
	glm::vec3 toonShadingColor = glm::vec3(1.0, 0.8, 0.7);
	int toonShadingLevels = 3;
};