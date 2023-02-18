#include "MeshModel.h"
#define M_PI        3.14159265358979323846264338327950288
MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> textureCoords, const std::string& model_name) :
	faces_(faces),
	vertices_(vertices),
	normals_(normals),
	textureCoords_(textureCoords),
	model_name_(model_name),
	color(glm::vec3(1, 1, 1)),
	ambient(1, 1, 1),
	diffuse(1, 1, 1),
	specular(1, 1, 1),
	shininess(16)
{
	scaleLoadedMeshModel();
	setBoundingBox();

	modelVertices.reserve(3 * faces_.size());
	for (int i = 0; i < faces_.size(); i++)
	{
		Face currentFace = faces_.at(i);

		// calculate tangent and bitangent vectors for normal mapping
		// (based on https://learnopengl.com/Advanced-Lighting/Normal-Mapping)
		glm::vec3 tangent = glm::vec3(0);
		glm::vec3 bitangent = glm::vec3(0);
		if (vertices_.size() > 0 && textureCoords_.size() > 0)
		{
			// positions of face vertices
			glm::vec3 pos1 = vertices_[currentFace.GetVertexIndex(0) - 1];
			glm::vec3 pos2 = vertices_[currentFace.GetVertexIndex(1) - 1];
			glm::vec3 pos3 = vertices_[currentFace.GetVertexIndex(2) - 1];

			// texture coordinates of face vertices
			glm::vec2 uv1 = textureCoords_[currentFace.GetTextureIndex(0) - 1];
			glm::vec2 uv2 = textureCoords_[currentFace.GetTextureIndex(1) - 1];
			glm::vec2 uv3 = textureCoords_[currentFace.GetTextureIndex(2) - 1];

			// edge vectors
			glm::vec3 edge1 = pos2 - pos1;
			glm::vec3 edge2 = pos3 - pos1;
			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			// tangent and bitangent vectors
			float det = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
			tangent = (deltaUV2.y * edge1 - deltaUV1.y * edge2) / det;
			bitangent = (-deltaUV2.x * edge1 + deltaUV1.x * edge2) / det;
		}

		for (int j = 0; j < 3; j++)
		{
			Vertex vertex;
			vertex.position = glm::vec3(0);
			vertex.normal = glm::vec3(0);
			vertex.tangent = tangent;
			vertex.bitangent = bitangent;
			vertex.textureCoords = glm::vec3(0);

			if (vertices_.size() > 0)
			{
				int vertexIndex = currentFace.GetVertexIndex(j) - 1;
				vertex.position = vertices_[vertexIndex];
			}

			if (normals_.size() > 0)
			{
				int normalIndex = currentFace.GetNormalIndex(j) - 1;
				vertex.normal = normals_[normalIndex];
			}

			if (textureCoords_.size() > 0)
			{
				int textureCoordsIndex = currentFace.GetTextureIndex(j) - 1;
				vertex.textureCoords = textureCoords_[textureCoordsIndex];
			}

			modelVertices.push_back(vertex);
		}
	}

	// load the mesh on the GPU
	if (modelVertices.size() > 0)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), &modelVertices[0], GL_STATIC_DRAW);

		// Vertex Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Normal vectors
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// Tangent vectors
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		// Bitangent vectors
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(9 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);

		// Vertex Texture Coords
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(12 * sizeof(GLfloat)));
		glEnableVertexAttribArray(4);

		// unbind to make sure other code does not change it somewhere else
		glBindVertexArray(0);
	}
}

MeshModel::~MeshModel()
{
}

const Face& MeshModel::GetFace(int index) const
{
	return faces_[index];
}

int MeshModel::GetFacesCount() const
{
	return (int)faces_.size();
}
const std::vector<Face> MeshModel::GetFacesList()const
{
	return this->faces_;
}
const std::vector<glm::vec3> MeshModel::GetVerticesList()const
{
	return this->vertices_;
}

const std::string& MeshModel::GetModelName() const
{
	return model_name_;
}

const glm::vec3& MeshModel::GetVertex(int index) const
{
	return vertices_[index];
}
const glm::vec3& MeshModel::GetNormal(int index) const
{
	return normals_[index];
}

/******** ----------------------------- ********/

const bool MeshModel::GetWorldLocal()const
{
	return this->world_local;	//true = world, false=local
}
void MeshModel::SetWorldLocal(bool flag)
{
	this->world_local = flag;
}
void MeshModel::scaleLoadedMeshModel()
{
	if (vertices_.empty())
		return;

	float
		xMin = vertices_[0].x,
		xMax = vertices_[0].x,
		yMin = vertices_[0].y,
		yMax = vertices_[0].y,
		zMin = vertices_[0].z,
		zMax = vertices_[0].z;
	for (int i = 1; i < vertices_.size(); i++)
	{
		xMin = glm::min(vertices_[i].x, xMin);
		xMax = glm::max(vertices_[i].x, xMax);

		yMin = glm::min(vertices_[i].y, yMin);
		yMax = glm::max(vertices_[i].y, yMax);

		zMin = glm::min(vertices_[i].z, zMin);
		zMax = glm::max(vertices_[i].z, zMax);
	}
	float xMid = (xMax + xMin) / 2.0f;
	float yMid = (yMax + yMin) / 2.0f;
	float zMid = (zMax + zMin) / 2.0f;

	float xScale = xMax - xMin;
	float yScale = yMax - yMin;
	float zScale = zMax - zMin;

	float finalScale = glm::max(xScale, glm::max(yScale, zScale));
	if (finalScale == 0.00f)
		finalScale = 1;
	else
		finalScale = 1.0f / finalScale;

	for (int i = 0; i < vertices_.size(); i++)
	{
		this->vertices_[i].x = (vertices_[i].x - xMid) * finalScale;
		this->vertices_[i].y = (vertices_[i].y - yMid) * finalScale;
		this->vertices_[i].z = (vertices_[i].z - zMid) * finalScale;
	}
}
void MeshModel::setWorldTransformation()
{
	glm::mat4x4 tm = glm::transpose(world_translate_mat);
	glm::mat4x4 xrm = world_X_rotation_mat;
	glm::mat4x4 yrm = world_Y_rotation_mat;
	glm::mat4x4 zrm = world_Z_rotation_mat;
	glm::mat4x4 sm = world_scale_mat;
	this->worldTransform = tm * xrm * yrm * zrm * sm;
}
void MeshModel::setLocalTransformation()
{
	glm::mat4x4 tm = glm::transpose(local_translate_mat);
	glm::mat4x4 xrm = local_X_rotation_mat;
	glm::mat4x4 yrm = local_Y_rotation_mat;
	glm::mat4x4 zrm = local_Z_rotation_mat;
	glm::mat4x4 sm = local_scale_mat;
	this->localTransform = tm * xrm * yrm * zrm * sm;
}

glm::mat4x4& MeshModel::GetLocalTransformation()
{
	this->setLocalTransformation();
	return this->localTransform;
}

glm::mat4x4& MeshModel::GetWorldTransformation()
{
	this->setWorldTransformation();
	return this->worldTransform;
}
const glm::mat4x4 MeshModel::GetTranformationMat()
{
	glm::mat4x4 mat = GetWorldTransformation() * GetLocalTransformation();
	this->Trasformation = mat;
	return mat;
}
glm::mat4x4 MeshModel::ScaleMatrix(glm::vec3 v) // v=(scale_x,scale_y,scale_z)
{
	return glm::mat4x4
	{ v[0],  0,  0,  0,
		0,  v[1],  0,  0,
		0,  0,  v[2],  0,
		0,  0,  0,  1 };
}

glm::mat4x4 MeshModel::TranslationMatrix(glm::vec3 v) // v=(translate_x,translate_y,translate_z)
{
	return glm::mat4x4
	{ 1,  0,  0,  v[0],
		0,  1,  0,  v[1],
		0,  0,  1,  v[2],
		0,  0,  0,  1 };
}
glm::mat4x4 MeshModel::X_RotationMatrix(glm::vec3 v)
{
	float k = float(M_PI) / float(180);
	float a = v[0];
	return glm::mat4x4
	{ 1,          0,              0,            0,
			0, glm::cos(a * k), -1 * glm::sin(a * k), 0,
			0, glm::sin(a * k),      glm::cos(a * k), 0,
			0,        0,                      0     , 1 };
}
glm::mat4x4 MeshModel::Y_RotationMatrix(glm::vec3 v)
{
	float k = float(M_PI) / float(180);
	float a = v[1];
	return glm::mat4x4
	{ glm::cos(a * k), 0, glm::sin(a * k), 0,
	   0,              1, 0, 0,
		-1 * glm::sin(a * k), 0, glm::cos(a * k), 0,
		0, 0, 0, 1
	};
}
glm::mat4x4 MeshModel::Z_RotationMatrix(glm::vec3 v)
{
	float k = float(M_PI) / float(180);
	float a = v[2];
	return glm::mat4x4{
		glm::cos(a * k), -1 * glm::sin(a * k), 0, 0,
		glm::sin(a * k), glm::cos(a * k), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
}
void MeshModel::setBoundingBox()
{
	if (vertices_.empty())
		return;

	float min_x, max_x, min_y, max_y, min_z, max_z;
	min_x = vertices_[0].x; max_x = vertices_[0].x;
	min_y = vertices_[0].y; max_y = vertices_[0].y;
	min_z = vertices_[0].z; max_z = vertices_[0].z;
	for (int i = 1; i < vertices_.size(); i++)
	{
		if (vertices_[i].x < min_x)
			min_x = vertices_[i].x;
		if (vertices_[i].x > max_x)
			max_x = vertices_[i].x;


		if (vertices_[i].y < min_y)
			min_y = vertices_[i].y;
		if (vertices_[i].y > max_y)
			max_y = vertices_[i].y;


		if (vertices_[i].z < min_z)
			min_z = vertices_[i].z;
		if (vertices_[i].z > max_z)
			max_z = vertices_[i].z;
	}

	boundingBoxPoints.push_back(glm::vec3(min_x, min_y, min_z));
	boundingBoxPoints.push_back(glm::vec3(min_x, min_y, max_z));
	boundingBoxPoints.push_back(glm::vec3(min_x, max_y, min_z));
	boundingBoxPoints.push_back(glm::vec3(min_x, max_y, max_z));
	boundingBoxPoints.push_back(glm::vec3(max_x, min_y, min_z));
	boundingBoxPoints.push_back(glm::vec3(max_x, min_y, max_z));
	boundingBoxPoints.push_back(glm::vec3(max_x, max_y, min_z));
	boundingBoxPoints.push_back(glm::vec3(max_x, max_y, max_z));
}

void MeshModel::SetBoundedBox(bool flag)
{
	this->bounding_box = flag;
}
bool MeshModel::GetBoundedBox()
{
	return this->bounding_box;
}
void MeshModel::SetShowAxis(bool flag)
{
	this->show_axis = flag;
}
bool MeshModel::GetShowAxis()
{
	return this->show_axis;
}
void MeshModel::SetShowFaceNormals(bool flag)
{
	this->show_face_normals = flag;
}
bool MeshModel::GetShowFaceNormals()
{
	return this->show_face_normals;
}
void MeshModel::SetShowVertexNormals(bool flag)
{
	this->show_vertex_normals = flag;
}
bool MeshModel::GetShowVertexNormals()
{
	return this->show_vertex_normals;
}
glm::vec3 MeshModel::GetNormal(int index)
{
	return this->normals_[index];
}
void MeshModel::SetLocalScaleMatrix(float scale_x, float scale_y, float scale_z)
{
	glm::vec3 scale_vec = glm::vec3(scale_x, scale_y, scale_z);
	this->local_scale = scale_vec;
	this->local_scale_mat = ScaleMatrix(scale_vec);
}
const glm::mat4 MeshModel::GetScaleMatLoc()const
{
	return this->local_scale_mat;
}
void MeshModel::SetLocalTranslateMatrix(float translate_x, float  translate_y, float  translate_z) {
	glm::vec3 translate_vec = glm::vec3(translate_x, translate_y, translate_z);
	this->local_translation = translate_vec;
	this->local_translate_mat = TranslationMatrix(translate_vec);
}
const glm::mat4 MeshModel::GetTranslateMatLoc()const
{
	return this->local_translate_mat;
}

void MeshModel::SetLocalRotateMatrix(float rotate_x, float  rotate_y, float  rotate_z)
{
	this->local_rotation = glm::vec3(rotate_x, rotate_y, rotate_z);
	this->local_X_rotation_mat = X_RotationMatrix(local_rotation);
	this->local_Y_rotation_mat = Y_RotationMatrix(local_rotation);
	this->local_Z_rotation_mat = Z_RotationMatrix(local_rotation);
}
void MeshModel::SetWorldScaleMatrix(float scale_x, float scale_y, float scale_z)
{
	glm::vec3 scale_vec = glm::vec3(scale_x, scale_y, scale_z);
	this->world_scale = scale_vec;
	this->world_scale_mat = ScaleMatrix(scale_vec);

}
void MeshModel::SetWorldTranslateMatrix(float translate_x, float translate_y, float translate_z)
{
	glm::vec3 translate_vec = glm::vec3(translate_x, translate_y, translate_z);
	this->world_translation = translate_vec;
	this->world_translate_mat = TranslationMatrix(translate_vec);

}
const glm::mat4 MeshModel::GetTranslateMatwor()const
{
	return this->world_translate_mat;
}

void MeshModel::SetWorldRotateMatrix(float rotate_x, float rotate_y, float rotate_z)
{
	this->world_rotation = glm::vec3(rotate_x, rotate_y, rotate_z);
	this->world_X_rotation_mat = X_RotationMatrix(world_rotation);
	this->world_Y_rotation_mat = Y_RotationMatrix(world_rotation);
	this->world_Z_rotation_mat = Z_RotationMatrix(world_rotation);
}
const glm::mat4 MeshModel::GetRotateyMatwor()const
{
	return this->world_Z_rotation_mat;
}
const glm::mat4 MeshModel::GetRotatezMatwor()const
{
	return this->world_Y_rotation_mat;
}
const glm::mat4 MeshModel::GetRotatexMatwor()const
{
	return this->world_X_rotation_mat;
}

void MeshModel::setNormals()
{
	int i = 0;
	for (; i < vertices_.size(); i++)
	{
		verticesNormals.push_back(glm::vec3(0, 0, 0));
	}

	for (i = 0; i < faces_.size(); i++)
	{
		verticesNormals[faces_[i].GetVertexIndex(0) - 1] += normals_[faces_[i].GetNormalIndex(0) - 1];
		verticesNormals[faces_[i].GetVertexIndex(1) - 1] += normals_[faces_[i].GetNormalIndex(1) - 1];
		verticesNormals[faces_[i].GetVertexIndex(2) - 1] += normals_[faces_[i].GetNormalIndex(2) - 1];

		facesCenters.push_back(glm::vec3(0, 0, 0));
		facesNormals.push_back(glm::vec3(0, 0, 0)); /// this one is for normal per face
	}
	for (i = 0; i < vertices_.size(); i++)
	{
		verticesNormals[i] = glm::normalize(verticesNormals[i]);
	}

	for (i = 0; i < faces_.size(); i++)
	{
		facesCenters[i] += vertices_[faces_[i].GetVertexIndex(0) - 1];
		facesCenters[i] += vertices_[faces_[i].GetVertexIndex(1) - 1];
		facesCenters[i] += vertices_[faces_[i].GetVertexIndex(2) - 1];
		facesCenters[i] /= 3.0f;

		facesNormals[i] += normals_[faces_[i].GetNormalIndex(0) - 1];
		facesNormals[i] += normals_[faces_[i].GetNormalIndex(1) - 1];
		facesNormals[i] += normals_[faces_[i].GetNormalIndex(2) - 1];
		facesNormals[i] = glm::normalize(facesNormals[i]);
	}
	return;
}

glm::vec3& MeshModel::GetAmbient()
{
	return ambient;
}

glm::vec3& MeshModel::GetDiffuse()
{
	return diffuse;
}

glm::vec3& MeshModel::GetSpecular()
{
	return specular;
}

float& MeshModel::GetShininess()
{
	return shininess;
}

GLuint MeshModel::GetVAO() const
{
	return vao;
}
