#pragma once
#include "Scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Utils.h"
#include <vector>

class Renderer
{
public:
	Renderer(int viewportWidth, int viewportHeight);
	virtual ~Renderer();
	void Render(Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	int GetViewportWidth() const;
	int GetViewportHeight() const;
	//void Renderer::DrawMeshExtra(MeshModel mesh, std::vector<glm::vec3> vertices, Face& facec, Scene scene, int width, int height);
	glm::vec3 ComputeAmbientLighting(Scene& scene, MeshModel& mesh, glm::vec3 position, glm::vec3 normal);
	glm::vec3 ComputeDiffuseLighting(Scene& scene, MeshModel& mesh, glm::vec3 position, glm::vec3 normal);
	glm::vec3 ComputeSpecularLighting(Scene& scene, MeshModel& mesh, glm::vec3 position, glm::vec3 normal, glm::vec3 cameraPosition);
	void Renderer::DrawMesh(MeshModel& mesh, Scene& scene, int width, int height, bool isLight);
	void DrawWorldAxis(Scene scene, int width, int height);
	void Renderer::DrawLocalAxis(MeshModel mesh, Scene scene, int width, int height);

	void Renderer::DrawFacesNormals(MeshModel mesh, Scene scene, int width, int height);
	void Renderer::DrawVertexNormals(MeshModel mesh, Scene scene, int width, int height);
	void Renderer::DrawBoundingBoxnew(MeshModel mesh, Scene scene, int width, int height);
	void Renderer::DrawBoundingBox(MeshModel& model);
	void DrawColoredTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void DrawLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color); //vec3
	void DrawLine2(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void DrawLineBresenham(int x1, int y1, int x2, int y2, const glm::vec3& Color, bool flip = 0);
	void DrawMeshModel(MeshModel& model, glm::mat4x4 transformation, Scene& scene);
	glm::vec2 PointAjdustment(float x, float y);


	void CreateBuffers(int w, int h);
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	void Renderer::Scan(MeshModel& mesh, Scene& scene, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int minx, int maxx, int miny, int maxy, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 color1, glm::vec3 color2, glm::vec3 color3, glm::vec3 normal1, glm::vec3 normal2, glm::vec3 normal3, glm::vec3 cameraPosition);
	void Renderer::SetViewportWidth(int ViewportWidth);
	void Renderer::SetViewportHeight(int ViewportHeight);

	float* color_buffer;
	std::vector<float> z_buffer;
	int viewport_width;
	int viewport_height;
	GLuint gl_screen_tex_;
	GLuint gl_screen_vtc_;
	float maxz = -1 * INFINITY;
	float minz = INFINITY;
};
