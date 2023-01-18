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
	CreateBuffers(viewport_width, viewport_height);
	for (int i = 0; i < viewport_width * viewport_height; i++)
	{
		z_buffer.push_back(INFINITY);
	}
}
void Renderer::DrawLine2(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
{
	bool switch_flag = false, reflect_flag = false;
	//-------------------------------------
	int x1 = p1.x, x2 = p2.x, y1 = p1.y, y2 = p2.y;
	if (x1 == x2)//straight line case 
	{
		while (y1 != y2)
		{
			PutPixel(x1, y1, color);
			if (y1 > y2)
			{
				y1 -= 1;
			}
			else
			{
				y1 += 1;
			}
		}
		return;
	}
	if (x1 > x2)//we need to swap the cordinates of the starting pint and the ending point of the line
	{
		//swaping the x coordinates
		int temp = x1;
		x1 = x2;
		x2 = temp;
		//------------------------
		//swaping the y coordinates
		temp = y1;
		y1 = y2;
		y2 = temp;
	}
	float dy = y2 - y1, dx = x2 - x1;
	float slope = dy / dx;
	int e = -dx;
	if (slope > 1) {
		switch_flag = true;
		e = -dy;
	}
	if (0 > slope && slope >= -1) {
		reflect_flag = true;
		e = -dx;
	}
	if (slope < -1)
	{
		reflect_flag = true;
		switch_flag = true;
		e = dy;
	}
	int x = x1, y = y1, goal = x2;
	if (switch_flag)
	{
		while (y <= y2 && !reflect_flag || reflect_flag && y >= y2)
		{
			if (e > 0)
			{
				if (reflect_flag) {//m<-1
					x = x + 1;
					e = e + 2 * dy;
				}
				else if (reflect_flag == false)//m>1
				{
					x = x + 1;
					e = e - 2 * dy;
				}
			}
			PutPixel(x, y, color);
			if (reflect_flag) {//m<-1
				y = y - 1;
				e = e + 2 * dx;
			}
			else//m>1
			{
				y = y + 1;
				e = e + 2 * dx;
			}
		}
	}
	else
	{
		while (x <= x2)
		{
			if (e > 0)
			{
				if (reflect_flag) {//-1<m<0
					y = y - 1;
					e = e - 2 * dx;
				}
				else//0<m<1
				{
					y = y + 1;
					e = e - 2 * dx;
				}
			}
			PutPixel(x, y, color);
			if (reflect_flag) {//-1<m<0
				x = x + 1;
				e = e - 2 * dy;
			}
			else//0<m<1
			{
				x = x + 1;
				e = e + 2 * dy;
			}
		}
	}
}

Renderer::~Renderer()
{
	delete[] color_buffer;
}

void Renderer::PutPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewport_width) return;
	if (j < 0) return; if (j >= viewport_height) return;

	color_buffer[INDEX(viewport_width, i, j, 0)] = color.x;
	color_buffer[INDEX(viewport_width, i, j, 1)] = color.y;
	color_buffer[INDEX(viewport_width, i, j, 2)] = color.z;
}


void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenGLBuffer(); //Do not remove this line.
	color_buffer = new float[3 * w * h];
	ClearColorBuffer(glm::vec3(0.0f, 0.0f, 0.0f));

}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::InitOpenGLRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &gl_screen_tex_);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &gl_screen_vtc_);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(gl_screen_vtc_);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[] = {
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[] = {
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1 };

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)sizeof(vtc));

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
}

void Renderer::CreateOpenGLBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex_);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewport_width, viewport_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewport_width, viewport_height);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex_);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport_width, viewport_height, GL_RGB, GL_FLOAT, color_buffer);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(gl_screen_vtc_);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewport_width; i++)
	{
		for (int j = 0; j < viewport_height; j++)
		{
			PutPixel(i, j, color);
		}
	}
}

void Renderer::Render(Scene& scene)
{
	int half_width = viewport_width / 2;
	int half_height = viewport_height / 2;
	//DrawWorldAxis(scene, half_width, half_height);
	if (scene.GetModelCount() > 0)
	{

		int modles_num = scene.GetModelCount();
		for (int i = 0; i < modles_num; i++)
		{
			MeshModel curr_mesh = scene.GetModel(i);
			//DrawMeshModel(curr_mesh,curr_mesh.GetTranformationMat(),  scene);
			DrawMesh(curr_mesh, scene, half_width, half_height, i);
		}

		//MeshModel curr_mesh = scene.GetActiveModel();
		//DrawMeshModel(curr_mesh, curr_mesh.GetTranformationMat(), scene);
		//DrawMesh(curr_mesh, curr_mesh.GetVerticesList(), curr_mesh.GetFacesList(), scene, half_width, half_height,2);
	}
	//glm::vec3 randomcolor = glm::vec3(1,0,0);
	Scene scene1 = scene;
	if (scene1.getiscolored())
	{
		for (int i = 0; i < viewport_width; i++)
		{
			for (int j = 0; j < viewport_height; j++)
			{
				if (z_buffer[Z_INDEX(viewport_width, i, j)] != INFINITY)
				{
					z_buffer[Z_INDEX(viewport_width, i, j)] = INFINITY;
				}

			}
		}
		maxz = -1 * INFINITY;
		minz = INFINITY;
	}
	if (scene.GetDrawCamera() == true)
	{
		Scene scene1 = scene;
		for (int i = 0; i < scene.GetCameraCount(); i++)
		{
			MeshModel& camera_ver = *(scene.camera_model);
			Camera& currCamera = scene1.GetCamera(i);
			Camera& activeCamera = scene1.GetActiveCamera();
			std::vector<glm::vec3> camera_vertices = camera_ver.GetVerticesList();
			glm::mat4x4 trans = activeCamera.GetViewTransformation();

			if (i != scene.GetActiveCameraIndex())
			{
				for (int j = 0; j < camera_ver.GetFacesCount(); j++) {

					const Face& model_face = camera_ver.GetFace(j);
					glm::vec3 u0 = camera_vertices[model_face.GetVertexIndex(0) - 1];
					glm::vec3 u1 = camera_vertices[model_face.GetVertexIndex(1) - 1];
					glm::vec3 u2 = camera_vertices[model_face.GetVertexIndex(2) - 1];
					u0 *= 200;
					u1 *= 200;
					u2 *= 200;
					glm::vec4 v0 = trans * glm::vec4(u0, 1);
					glm::vec4 v1 = trans * glm::vec4(u1, 1);
					glm::vec4 v2 = trans * glm::vec4(u2, 1);
					v0 /= v0.w ? v0.w : 1;
					v1 /= v1.w ? v1.w : 1;
					v2 /= v2.w ? v2.w : 1;
					v0.x += viewport_width / 2;
					v1.x += viewport_width / 2;
					v2.x += viewport_width / 2;
					v0.y += viewport_height / 2;
					v1.y += viewport_height / 2;
					v2.y += viewport_height / 2;
					DrawLine2(v0, v1, glm::vec3(0, 0, 0));
					DrawLine2(v0, v2, glm::vec3(0, 0, 0));
					DrawLine2(v2, v1, glm::vec3(0, 0, 0));

				}
			}
		}

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
///////////////////////////////////////////////////////////////////////////////////////////////////
void Renderer::DrawMeshModel(MeshModel& model, glm::mat4x4 transformation, Scene& scene)
{
	for (int i = 0; i < model.GetFacesCount(); ++i)
	{
		// get verticies  of each face and draw line between each pair
		Face face = model.GetFace(i);

		glm::vec4 v1 = transformation * glm::vec4(model.GetVertex(face.GetVertexIndex(0) - 1), 1);
		glm::vec4 v2 = transformation * glm::vec4(model.GetVertex(face.GetVertexIndex(1) - 1), 1);
		glm::vec4 v3 = transformation * glm::vec4(model.GetVertex(face.GetVertexIndex(2) - 1), 1);
		v1 = v1 / v1[3];
		v2 = v2 / v2[3];
		v3 = v3 / v3[3];

		glm::vec3 p1(v1[0], v1[1], v1[2]);
		glm::vec3 p2(v2[0], v2[1], v2[2]);
		glm::vec3 p3(v3[0], v3[1], v3[2]);

		DrawLine(p1, p2, model.color);
		DrawLine(p2, p3, model.color);
		DrawLine(p1, p3, model.color);

	}

	if (model.GetBoundedBox()) {
		DrawBoundingBox(model);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////

glm::vec2 Renderer::PointAjdustment(float x, float y)
{
	if (viewport_width > viewport_height)
	{
		x = x * (viewport_height / 4.0f)
			+ (viewport_width * 0.5f);
		y = y * (viewport_height / 4.0f)
			+ (viewport_height * 0.5f);
	}
	else
	{
		x = x * (viewport_width / 4.0f)
			+ (viewport_width * 0.5f);
		y = y * (viewport_width / 4.0f)
			+ (viewport_height * 0.5f);
	}
	return glm::vec2(x, y);

}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Renderer::DrawLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color)
{
	glm::vec2 v1 = PointAjdustment(p1.x, p1.y);
	glm::vec2 v2 = PointAjdustment(p2.x, p2.y);

	int x1 = v1.x;
	int x2 = v2.x;
	int y1 = v1.y;
	int y2 = v2.y;


	DrawLineBresenham(x1, y1, x2, y2, color);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//new version of draw line function
void Renderer::DrawLineBresenham(int x1, int y1, int x2, int y2, const glm::vec3& Color, bool flip)
{

	if (x1 > x2)
	{
		DrawLineBresenham(x2, y2, x1, y1, Color);
	}
	else if (x1 == x2)
	{
		int yMin = (y2 > y1 ? y1 : y2), yMax = (y2 > y1 ? y2 : y1);
		while (yMin <= yMax)
		{
			PutPixel(x1, yMin, Color);
			yMin++;
		}
	}
	else
	{
		int dx = (x2 - x1);
		int dy = abs(y2 - y1);
		float a = ((float)dy / (float)dx);
		int x = x1, y = y1;

		if (a == 0.0f)
		{
			while (x <= x2)
			{
				PutPixel(x, y1, Color);
				x++;
			}
		}
		else if ((a > 1.0f))
		{
			if (y1 <= y2)
				DrawLineBresenham(y1, x1, y2, x2, Color, 1);
			else
			{
				int dy = y1 - y2;
				int dx = x2 - x1;
				int x = x1, y = y1;
				int e = -dy;
				while (y >= y2)
				{
					if (e > 0)
					{
						x++;
						e -= (dy + dy);
					}
					PutPixel(x, y, Color);
					y--;
					e += (dx + dx);
				}
			}
		}
		else
		{
			int e = -dx;
			while (x <= x2)
			{
				if (e > 0)
				{
					y++;
					e -= (dx + dx);
				}
				if (y2 > y1)
				{
					if (flip)
						PutPixel(y, x, Color);
					else
						PutPixel(x, y, Color);
				}
				else
				{
					PutPixel(x, y1 - (y - y1), Color);
				}
				x++;
				e += (dy + dy);
			}
		}
	}
}

void Renderer::DrawBoundingBox(MeshModel& model)
{
	glm::mat4x4 transformation = model.GetTranformationMat();
	std::vector<glm::vec3> PB;
	for (int i = 0; i < 8; ++i)
	{
		glm::vec4 ve = transformation * glm::vec4(model.boundingBoxPoints[i], 1);
		ve = ve / ve[3];
		PB.push_back(glm::vec3(ve[0], ve[1], ve[2]));
	}

	DrawLine(PB[0], PB[1], model.color_b);
	DrawLine(PB[0], PB[2], model.color_b);
	DrawLine(PB[0], PB[4], model.color_b);
	DrawLine(PB[1], PB[3], model.color_b);
	DrawLine(PB[1], PB[5], model.color_b);
	DrawLine(PB[2], PB[3], model.color_b);
	DrawLine(PB[2], PB[6], model.color_b);
	DrawLine(PB[3], PB[7], model.color_b);
	DrawLine(PB[4], PB[5], model.color_b);
	DrawLine(PB[4], PB[6], model.color_b);
	DrawLine(PB[5], PB[7], model.color_b);
	DrawLine(PB[6], PB[7], model.color_b);

}
void Renderer::DrawColoredTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	float minX = min(p1.x, min(p2.x, p3.x));
	float minY = min(p1.y, min(p2.y, p3.y));
	float maxX = max(p1.x, max(p2.x, p3.x));
	float maxY = max(p1.y, max(p2.y, p3.y));
	int flag;
	glm::vec3 point;
	glm::vec3 randomcolor = glm::vec3(rand() / 10000, rand() / 10000, rand() / 10000);
	for (int i = minX; i <= maxX; i++)
	{
		for (int j = minY; j <= maxY; j++)
		{
			if (i < viewport_width && j < viewport_height && i > 0 && j > 0)
				if (i < viewport_width && j < viewport_height && i > 0 && j > 0)
				{
					point.x = i;
					point.y = j;
					flag = Utils::IsInsideTraingle(point, p1, p2, p3);
					if (flag)
					{
						PutPixel(i, j, randomcolor);
					}
				}
		}
	}
}

glm::vec3 Renderer::ComputeAmbientLighting(Scene& scene, MeshModel& mesh, glm::vec3 position, glm::vec3 normal)
{
	glm::vec3 lighting(0, 0, 0);

	// add contribution from all lights
	for (int j = 0; j < scene.GetLightCount(); j++)
	{
		// ambient lighting
		Light& light = scene.GetLight(j);
		lighting += light.GetAmbient() * mesh.GetAmbient();
	}

	return lighting;
}

glm::vec3 Renderer::ComputeDiffuseLighting(Scene& scene, MeshModel& mesh, glm::vec3 position, glm::vec3 normal)
{
	glm::vec3 lighting(0, 0, 0);

	// add contribution from all lights
	for (int j = 0; j < scene.GetLightCount(); j++)
	{
		// diffuse lighting
		Light& light = scene.GetLight(j);
		glm::vec3 lightDirection = glm::normalize(light.GetPosition() - position);
		float diffuse = glm::dot(lightDirection, normal);
		if (diffuse > 0)
		{
			lighting += light.GetDiffuse() * mesh.GetDiffuse() * diffuse;
		}
	}

	return lighting;
}

glm::vec3 Renderer::ComputeSpecularLighting(Scene& scene, MeshModel& mesh, glm::vec3 position, glm::vec3 normal, glm::vec3 cameraPosition)
{
	glm::vec3 lighting(0, 0, 0);

	// add contribution from all lights
	for (int j = 0; j < scene.GetLightCount(); j++)
	{
		// specular lighting
		Light& light = scene.GetLight(j);
		glm::vec3 lightDirection = glm::normalize(light.GetPosition() - position);
		glm::vec3 reflectedLightDirection = glm::reflect(-lightDirection, normal);
		glm::vec3 cameraDirection = glm::normalize(cameraPosition - position);
		float specular = glm::dot(reflectedLightDirection, cameraDirection);
		if (specular > 0)
		{
			lighting += light.GetSpecular() * mesh.GetSpecular() * 
				glm::vec3(glm::pow(specular, 16), glm::pow(specular, 16), glm::pow(specular, 16));
		}
	}

	return lighting;
}

void Renderer::DrawMesh(MeshModel& mesh, Scene& scene, int width, int height, int index)
{
	// tranformation matrices for positions and normals
	glm::mat4 matrix = mesh.GetTranformationMat();
	glm::mat3 normal_matrix = glm::inverse(glm::transpose(matrix));
	glm::mat4x4 camera_transform = scene.GetActiveCamera().GetViewTransformation();

	// camera position in world space
	glm::vec3 cameraPosition = scene.GetActiveCamera().GetWorldPos();

	// shade vertices
	std::vector<glm::vec3> faceColors(mesh.faces_.size());
	std::vector<glm::vec3> vertexPositions(mesh.vertices_.size());
	std::vector<glm::vec3> vertexNormals(mesh.vertices_.size());
	std::vector<glm::vec3> vertexColors(mesh.vertices_.size());
	if (scene.getiscolored())
	{
		// compute shading for each face
		for (int i = 0; i < faceColors.size(); i++)
		{
			// face center and normal in world space
			glm::vec3 center = matrix * glm::vec4(mesh.facesCenters[i], 1);
			glm::vec3 normal = glm::normalize(normal_matrix * mesh.facesNormals[i]);

			faceColors[i] = glm::vec3(0, 0, 0);
			if (scene.GetUseAmbientLighting())
				faceColors[i] += ComputeAmbientLighting(scene, mesh, center, normal);
			if (scene.GetUseDiffuseLighting())
				faceColors[i] += ComputeDiffuseLighting(scene, mesh, center, normal);
		}

		// compute shading for each vertex
		for (int i = 0; i < vertexColors.size(); i++)
		{
			// vertex position and normal in world space
			vertexPositions[i] = matrix * glm::vec4(mesh.vertices_[i], 1);
			vertexNormals[i] = glm::normalize(normal_matrix * mesh.verticesNormals[i]);

			vertexColors[i] = glm::vec3(0, 0, 0);
			if (scene.GetUseAmbientLighting())
				vertexColors[i] += ComputeAmbientLighting(scene, mesh, vertexPositions[i], vertexNormals[i]);
			if (scene.GetUseDiffuseLighting())
				vertexColors[i] += ComputeDiffuseLighting(scene, mesh, vertexPositions[i], vertexNormals[i]);
		}
	}

	// Draw Faces and implement operation according to the assigment

	int face_count = mesh.faces_.size();
	glm::vec3 p1, p2, p3, facecolor;
	glm::mat4 T_mat = matrix;
	glm::mat4x4 worldaxesmodel_transform = camera_transform * mesh.GetTranslateMatwor() * mesh.GetRotatexMatwor() * mesh.GetRotateyMatwor() * mesh.GetRotatezMatwor() * mesh.world_scale_mat * mesh.GetTranslateMatLoc() * mesh.GetScaleMatLoc();
	T_mat = camera_transform * T_mat;
	for (int i = 0; i < face_count; i++)
	{
		Face face = mesh.faces_[i];
		p1 = mesh.vertices_[face.GetVertexIndex(0) - 1];
		p2 = mesh.vertices_[face.GetVertexIndex(1) - 1];
		p3 = mesh.vertices_[face.GetVertexIndex(2) - 1];

		glm::vec4 temp_vec = T_mat * glm::vec4(p1, 1);
		p1.x = temp_vec.x / temp_vec.w;
		p1.y = temp_vec.y / temp_vec.w;
		p1.z = temp_vec.z / temp_vec.w;

		temp_vec = T_mat * glm::vec4(p2, 1);
		p2.x = temp_vec.x / temp_vec.w;
		p2.y = temp_vec.y / temp_vec.w;
		p2.z = temp_vec.z / temp_vec.w;

		temp_vec = T_mat * glm::vec4(p3, 1);
		p3.x = temp_vec.x / temp_vec.w;
		p3.y = temp_vec.y / temp_vec.w;
		p3.z = temp_vec.z / temp_vec.w;

		p1.x = (p1.x + 1) * width;
		p1.y = (p1.y + 1) * height;
		p2.x = (p2.x + 1) * width;;
		p2.y = (p2.y + 1) * height;
		p3.x = (p3.x + 1) * width;;
		p3.y = (p3.y + 1) * height;

		int max_x = std::max(p1.x, std::max(p2.x, p3.x));
		int max_y = std::max(p1.y, std::max(p2.y, p3.y));
		int min_x = std::min(p1.x, std::min(p2.x, p3.x));
		int min_y = std::min(p1.y, std::min(p2.y, p3.y));

		//draw faces of the mesh 
		if (!scene.getiscolored())
		{
			DrawLine2(glm::vec2(p1.x, p1.y), glm::vec2(p2.x, p2.y), glm::vec3(0, 0, 0));
			DrawLine2(glm::vec2(p1.x, p1.y), glm::vec2(p3.x, p3.y), glm::vec3(0, 0, 0));
			DrawLine2(glm::vec2(p2.x, p2.y), glm::vec2(p3.x, p3.y), glm::vec3(0, 0, 0));
		}

		// if scene is colored , color each mesh with appropriate color to look different
		if (scene.getiscolored())
		{
			glm::vec3 v1 = vertexPositions[face.GetVertexIndex(0) - 1];
			glm::vec3 v2 = vertexPositions[face.GetVertexIndex(1) - 1];
			glm::vec3 v3 = vertexPositions[face.GetVertexIndex(2) - 1];
			glm::vec3 normal1 = vertexNormals[face.GetVertexIndex(0) - 1];
			glm::vec3 normal2 = vertexNormals[face.GetVertexIndex(1) - 1];
			glm::vec3 normal3 = vertexNormals[face.GetVertexIndex(2) - 1];

			if (scene.GetGouraudShading())
			{
				// Gouraud shading: each vertex has its color
				glm::vec3 color1 = vertexColors[face.GetVertexIndex(0) - 1];
				glm::vec3 color2 = vertexColors[face.GetVertexIndex(1) - 1];
				glm::vec3 color3 = vertexColors[face.GetVertexIndex(2) - 1];
				Scan(mesh, scene, p1, p2, p3, min_x, max_x, min_y, max_y, v1, v2, v3, color1, color2, color3, normal1, normal2, normal3, cameraPosition);
			}
			else
			{
				// flat shading: single color for entire face
				glm::vec3 color = faceColors[i];
				Scan(mesh, scene, p1, p2, p3, min_x, max_x, min_y, max_y, v1, v2, v3, color, color, color, normal1, normal2, normal3, cameraPosition);
			}
		}

		// show light directions as short lines
		if (mesh.GetShowLightDirections() && scene.GetLightCount() > 0)
		{
			// face center and normal in world space
			glm::vec3 center = matrix * glm::vec4(mesh.facesCenters[i], 1);
			glm::vec3 normal = glm::normalize(normal_matrix * mesh.facesNormals[i]);

			// check if facing light
			glm::vec3 lightDirection = glm::normalize(scene.GetActiveLight().GetPosition() - center);
			if (glm::dot(lightDirection, normal) > 0)
			{
				glm::vec3 reflectedLightDirection = glm::reflect(-lightDirection, normal);

				// face center in screen space
				glm::vec4 temp_vec = camera_transform * glm::vec4(center, 1);
				glm::vec2 a;
				a.x = (temp_vec.x / temp_vec.w + 1) * width;
				a.y = (temp_vec.y / temp_vec.w + 1) * height;

				// light direction endpoint in screen space
				temp_vec = camera_transform * glm::vec4(center + lightDirection * 0.25f, 1);
				glm::vec2 b;
				b.x = (temp_vec.x / temp_vec.w + 1) * width;
				b.y = (temp_vec.y / temp_vec.w + 1) * height;

				// reflected light direction endpoint in screen space
				temp_vec = camera_transform * glm::vec4(center + reflectedLightDirection * 0.25f, 1);
				glm::vec2 c;
				c.x = (temp_vec.x / temp_vec.w + 1) * width;
				c.y = (temp_vec.y / temp_vec.w + 1) * height;

				// show light direction and reflected light direction as red and green line
				DrawLine2(a, b, glm::vec3(1, 0, 0));
				DrawLine2(a, c, glm::vec3(0, 1, 0));
			}
		}

		// for part 1 of the assignment 
		if (mesh.GetFacesBound())
		{
			//facecolor = glm::vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
			//float avg = (p1.z + p2.z + p3.z) / 3;
			//facecolor = glm::vec3(avg,avg,avg);
			glm::vec2 left_top = glm::vec2(min_x, max_y);
			glm::vec2 left_bot = glm::vec2(min_x, min_y);
			glm::vec2 right_top = glm::vec2(max_x, max_y);
			glm::vec2 right_bot = glm::vec2(max_x, min_y);
			DrawLine2(right_top, right_bot, facecolor);
			DrawLine2(right_top, left_top, facecolor);
			DrawLine2(left_bot, right_bot, facecolor);
			DrawLine2(left_top, left_bot, facecolor);
		}


		if (mesh.GetShowAxis() == true)
		{

			glm::vec3 px_max = glm::vec3(mesh.max_x, (mesh.max_y + mesh.min_y) / 2, (mesh.max_z + mesh.min_z) / 2);
			glm::vec3 py_max = glm::vec3((mesh.max_x + mesh.min_x) / 2, mesh.max_y, (mesh.max_z + mesh.min_z) / 2);
			glm::vec3 pz_max = glm::vec3((mesh.max_x + mesh.min_x) / 2, (mesh.max_y + mesh.min_y) / 2, mesh.max_z);
			glm::vec3 center = glm::vec3((mesh.max_x + mesh.min_x) / 2, (mesh.max_y + mesh.min_y) / 2, (mesh.max_z + mesh.min_z) / 2);
			if (mesh.GetWorldLocal() == false)
			{
				//----------------------------------------
				glm::vec4 temp_vec = glm::vec4(px_max, 1);
				temp_vec = T_mat * temp_vec;
				px_max = temp_vec;
				//--------------------------------
				temp_vec = glm::vec4(py_max, 1);
				temp_vec = T_mat * temp_vec;
				py_max = temp_vec;
				//-------------------------------------
				temp_vec = glm::vec4(pz_max, 1);
				temp_vec = T_mat * temp_vec;
				pz_max = temp_vec;
				//-------------------------------------------
				temp_vec = glm::vec4(center, 1);
				temp_vec = T_mat * temp_vec;
				center = temp_vec;
				//--------------------------
			}
			else
			{
				glm::vec4 temp_vec = glm::vec4(px_max, 1);
				temp_vec = worldaxesmodel_transform * temp_vec;
				px_max = temp_vec;
				//--------------------------------
				temp_vec = glm::vec4(py_max, 1);
				temp_vec = worldaxesmodel_transform * temp_vec;
				py_max = temp_vec;
				//-------------------------------------
				temp_vec = glm::vec4(pz_max, 1);
				temp_vec = worldaxesmodel_transform * temp_vec;
				pz_max = temp_vec;
				//-------------------------------------------
				temp_vec = glm::vec4(center, 1);
				temp_vec = worldaxesmodel_transform * temp_vec;
				center = temp_vec;
				//--------------------------
			}
			px_max.x = (px_max.x + 1) * width;
			px_max.y = (px_max.y + 1) * height;
			py_max.x = (py_max.x + 1) * width;;
			py_max.y = (py_max.y + 1) * height;
			pz_max.x = (pz_max.x + 1) * width;;
			pz_max.y = (pz_max.y + 1) * height;
			center.x = (center.x + 1) * width;;
			center.y = (center.y + 1) * height;

			DrawLine2(glm::vec2(px_max.x, px_max.y), center, glm::vec3(1, 0, 0));
			DrawLine2(glm::vec2(py_max.x, py_max.y), center, glm::vec3(0, 1, 0));
			DrawLine2(glm::vec2(pz_max.x, pz_max.y), center, glm::vec3(0, 0, 1));

		}

	}
}

void Renderer::DrawWorldAxis(Scene scene, int width, int height)
{
	glm::vec3 origin = glm::vec3(0, 0, 0);
	glm::vec3 x_axis = glm::vec3(width, 0, 0.0f);
	glm::vec3 y_axis = glm::vec3(0, height, 0.0f);
	glm::vec3 z_axis = glm::vec3(0, 0, 300);
	glm::mat4x4 camera_transform = scene.GetActiveCamera().GetViewTransformation();
	glm::vec4 temp_vec = glm::vec4(origin, 1);
	temp_vec = camera_transform * temp_vec;
	origin = temp_vec;
	temp_vec = glm::vec4(x_axis, 1);
	temp_vec = camera_transform * temp_vec;
	x_axis = temp_vec;
	temp_vec = glm::vec4(y_axis, 1);
	temp_vec = camera_transform * temp_vec;
	y_axis = temp_vec;
	temp_vec = glm::vec4(z_axis, 1);
	temp_vec = camera_transform * temp_vec;
	z_axis = temp_vec;
	origin.x = (origin.x + 1) * width;
	origin.y = (origin.y + 1) * height;
	x_axis.x = (x_axis.x + 1) * width;
	x_axis.y = (x_axis.y + 1) * height;
	y_axis.x = (y_axis.x + 1) * width;
	y_axis.y = (y_axis.y + 1) * height;
	z_axis.x = (z_axis.x + 1) * width;
	z_axis.y = (z_axis.y + 1) * height;
	DrawLine2(origin, x_axis, glm::vec3(1, 0, 0));
	DrawLine2(origin, y_axis, glm::vec3(0, 1, 0));
	DrawLine2(origin, z_axis, glm::vec3(0, 0, 1));
}

////////////////////////
void Renderer::Scan(MeshModel& mesh, Scene& scene, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int minx, int maxx, int miny, int maxy, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 color1, glm::vec3 color2, glm::vec3 color3, glm::vec3 normal1, glm::vec3 normal2, glm::vec3 normal3, glm::vec3 cameraPosition)
{
	for (int i = minx; i < maxx; i++)
	{
		for (int j = miny; j < maxy; j++)
		{
			if (Utils::IsInsideTraingle(glm::vec2(i, j), p1, p2, p3))
			{
				float z1 = p1.z, z2 = p2.z, z3 = p3.z, A, A1, A2, A3, z;

				// fixed version
				A1 = (p2.y - p3.y) * i + (j - p2.y) * p3.x - (j - p3.y) * p2.x;
				A2 = (p3.y - p1.y) * i + (j - p3.y) * p1.x - (j - p1.y) * p3.x;
				A3 = (p1.y - p2.y) * i + (j - p1.y) * p2.x - (j - p2.y) * p1.x;
				// same as in lecture 
				A = 1 / (A1 + A2 + A3);
				A1 *= A;
				A2 *= A;
				A3 *= A;
				z = A1 * z1 + A2 * z2 + A3 * z3;
				if (z > maxz) {
					maxz = z;
				}
				if (z < minz)
				{
					minz = z;
				}
				if ((i < viewport_width && j < viewport_height && i>0 && j>0) && z <= z_buffer[Z_INDEX(viewport_width, i, j)])
				{
					z_buffer[Z_INDEX(viewport_width, i, j)] = z;

					// interpolate color
					glm::vec3 color = A1 * color1 + A2 * color2 + A3 * color3;

					// specular lighting using Phong shading
					if (scene.GetUseSpecularLighting())
					{
						// interpolate position in world space
						glm::vec3 v = A1 * v1 + A2 * v2 + A3 * v3;

						// interpolate normal in world space
						glm::vec3 normal = glm::normalize(A1 * normal1 + A2 * normal2 + A3 * normal3);

						color += ComputeSpecularLighting(scene, mesh, v, normal, cameraPosition);
					}

					PutPixel(i, j, color);
				}
			}
		}
	}
}
void Renderer::SetViewportWidth(int ViewportWidth)
{
	this->viewport_width = ViewportWidth;

}

void Renderer::SetViewportHeight(int ViewportHeight)
{
	this->viewport_height = ViewportHeight;
}
