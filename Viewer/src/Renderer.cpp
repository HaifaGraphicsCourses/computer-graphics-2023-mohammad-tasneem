#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

#include "Renderer.h"
#include "InitShader.h"
#include <iostream>
using namespace std;


#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))

Renderer::Renderer(int viewport_width, int viewport_height) :
	viewport_width(viewport_width),
	viewport_height(viewport_height)
{
	InitOpenGLRendering();
}

Renderer::~Renderer()
{
}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::InitOpenGLRendering()
{
	// Loads and compiles a sheder.
	program = InitShader("vshader.glsl", "fshader.glsl");

	// Make this program the current one.
	glUseProgram(program);
}

void Renderer::Render(Scene& scene)
{
	// set uniforms for each light
	for (int i = 0; i < scene.GetLightCount(); i++)
	{
		Light& light = scene.GetLight(i);
		glUniform3fv(glGetUniformLocation(program, ("lights[" + std::to_string(i) + "].position").c_str()), 1, glm::value_ptr(light.GetPosition()));
		glUniform3fv(glGetUniformLocation(program, ("lights[" + std::to_string(i) + "].ambient").c_str()), 1, glm::value_ptr(light.GetAmbient()));
		glUniform3fv(glGetUniformLocation(program, ("lights[" + std::to_string(i) + "].diffuse").c_str()), 1, glm::value_ptr(light.GetDiffuse()));
		glUniform3fv(glGetUniformLocation(program, ("lights[" + std::to_string(i) + "].specular").c_str()), 1, glm::value_ptr(light.GetSpecular()));
	}

	// set uniforms for all lights
	glUniform1i(glGetUniformLocation(program, "numLights"), scene.GetLightCount());
	glUniform1i(glGetUniformLocation(program, "useAmbientLighting"), scene.GetUseAmbientLighting());
	glUniform1i(glGetUniformLocation(program, "useDiffuseLighting"), scene.GetUseDiffuseLighting());
	glUniform1i(glGetUniformLocation(program, "useSpecularLighting"), scene.GetUseSpecularLighting());
	glUniform3fv(glGetUniformLocation(program, "cameraPos"), 1, glm::value_ptr(scene.GetActiveCamera().GetWorldPos()));

	// set uniforms for current mapping
	glUniform1i(glGetUniformLocation(program, "textureMap"), 0);
	glUniform1i(glGetUniformLocation(program, "normalMap"), 1);
	glUniform1i(glGetUniformLocation(program, "envMap"), 2);
	glUniform1i(glGetUniformLocation(program, "mappingType"), scene.GetMappingType());

	// set uniforms for toon shading
	glUniform1i(glGetUniformLocation(program, "useToonShading"), scene.GetUseToonShading());
	glUniform3fv(glGetUniformLocation(program, "toonShadingColor"), 1, glm::value_ptr(scene.GetToonShadingColor()));
	glUniform1i(glGetUniformLocation(program, "toonShadingLevels"), scene.GetToonShadingLevels());

	// set active texture at slots #0-#2
	textureMap.bind(0);
	normalMap.bind(1);
	envMap.bind(2);

	// set uniform variables
	const Camera& camera = scene.GetActiveCamera();
	glm::mat4 view = camera.GetViewTransformation();
	glm::mat4 projection = camera.GetProjectionTransformation();
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// draw models
	if (scene.GetModelCount() > 0)
	{
		int modelCount = scene.GetModelCount();

		for (int meshIndex = 0; meshIndex < modelCount; meshIndex++)
		{
			MeshModel& mesh = scene.GetModel(meshIndex);

			// set uniform variables
			glm::mat4 model = mesh.GetTranformationMat();
			glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));

			// set uniforms for material
			glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, glm::value_ptr(mesh.GetAmbient()));
			glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, glm::value_ptr(mesh.GetDiffuse()));
			glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, glm::value_ptr(mesh.GetSpecular()));
			glUniform1f(glGetUniformLocation(program, "material.shininess"), mesh.GetShininess());
			glUniform1i(glGetUniformLocation(program, "material.isLight"), false);

			// draw mesh triangles
			glBindVertexArray(mesh.GetVAO());
			glDrawArrays(GL_TRIANGLES, 0, (int)mesh.modelVertices.size());
			glBindVertexArray(0);
		}
	}

	// unset active texture at slot #0-#2
	textureMap.unbind(0);
	normalMap.unbind(1);
	envMap.unbind(2);

	// draw light sources
	for (int i = 0; i < scene.GetLightCount(); i++)
	{
		Light& light = scene.GetLight(i);
		MeshModel& mesh = scene.GetLightModel();

		// copy light position to light source model position
		glm::vec3 pos = light.GetPosition();
		scene.GetLightModel().SetWorldTranslateMatrix(pos.x, pos.y, pos.z);

		// set light source model size
		float size = 0.3f;
		scene.GetLightModel().SetWorldScaleMatrix(size, size, size);

		// set uniform variables
		glm::mat4 model = mesh.GetTranformationMat();
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// set uniforms for material
		glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, glm::value_ptr(light.GetAmbient()));
		glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, glm::value_ptr(light.GetDiffuse()));
		glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, glm::value_ptr(light.GetSpecular()));
		glUniform1f(glGetUniformLocation(program, "material.shininess"), 1.0f);
		glUniform1i(glGetUniformLocation(program, "material.isLight"), true);

		// draw mesh triangles
		glBindVertexArray(mesh.GetVAO());
		glDrawArrays(GL_TRIANGLES, 0, (int)mesh.modelVertices.size());
		glBindVertexArray(0);
	}
}

int Renderer::GetViewportWidth() const
{
	return viewport_width;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height;
}

void Renderer::SetViewportWidth(int ViewportWidth)
{
	this->viewport_width = ViewportWidth;

}

void Renderer::SetViewportHeight(int ViewportHeight)
{
	this->viewport_height = ViewportHeight;
}

void Renderer::LoadTexture(Texture2D& texture, std::string fileName)
{
	// try different folders to locate texture
	if (texture.loadTexture("bin/Debug/" + fileName, true))
		return;
	if (texture.loadTexture("bin/Release/" + fileName, true))
		return;
	if (texture.loadTexture("../Data/" + fileName, true))
		return;
	if (texture.loadTexture("../../Data/" + fileName, true))
		return;
	if (texture.loadTexture("../../../Data/" + fileName, true))
		return;

	std::cerr << "Error loading texture '" << fileName << "'" << std::endl;
}

void Renderer::LoadTextures()
{
	LoadTexture(textureMap, "texture_map.jpg");
	LoadTexture(normalMap, "normal_map.png");
	LoadTexture(envMap, "env_map.jpg");
}
