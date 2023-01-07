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
	void Renderer::DrawMesh(MeshModel mesh, std::vector<glm::vec3> vertices, std::vector<Face> faces, Scene scene, int width, int height, int index);
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
	void Renderer::Scan(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int minx, int maxx, int miny, int maxy, glm::vec3 color);
	void Renderer::SetViewportWidth(int ViewportWidth);
	void Renderer::SetViewportHeight(int ViewportHeight);

	float* color_buffer;
	float* Z_buffer;
	std::vector<glm::vec3> temp_color_buffer;
	std::vector<float> z_buffer;
	float* color_buffer_;
	int viewport_width;
	int viewport_height;
	GLuint gl_screen_tex_;
	GLuint gl_screen_vtc_;
	float maxz = -1 * INFINITY;
	float minz = INFINITY;
};
