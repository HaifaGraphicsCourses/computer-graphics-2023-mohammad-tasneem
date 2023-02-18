#pragma once
#include "Scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Utils.h"
#include <vector>
#include "Texture2D.h"
class Renderer
{
public:
	Renderer(int viewportWidth, int viewportHeight);
	virtual ~Renderer();
	void Render(Scene& scene);
	void SwapBuffers();
	int GetViewportWidth() const;
	int GetViewportHeight() const;
	void LoadTexture(Texture2D& texture, std::string fileName);
	void LoadTextures();

private:
	void InitOpenGLRendering();
	void SetViewportWidth(int ViewportWidth);
	void SetViewportHeight(int ViewportHeight);

	int viewport_width;
	int viewport_height;
	float maxz = -1 * INFINITY;
	float minz = INFINITY;

	GLuint program = 0;
	Texture2D textureMap, normalMap, envMap;
};
