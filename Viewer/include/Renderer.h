#pragma once
#include "Scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

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


private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void DrawLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color);
	void DrawLineBresenham(int x1, int y1, int x2, int y2, const glm::vec3& Color, bool flip = 0);
	void DrawMeshModel(MeshModel& model, glm::mat4x4 transformation);
	void Renderer::DrawMesh(MeshModel mesh, std::vector<glm::vec3> vertices, std::vector<Face> faces);
	glm::vec2 PointAjdustment(float x, float y);
	void Renderer::DrawLine2(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);


	void CreateBuffers(int w, int h);
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();

	float* color_buffer_;
	int viewport_width_;
	int viewport_height_;
	GLuint gl_screen_tex_;
	GLuint gl_screen_vtc_;
};
