#include "MeshModel.h"
#define M_PI        3.14159265358979323846264338327950288
MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
	faces_(faces),
	vertices_(vertices),
	normals_(normals),
	model_name_(model_name),
	localRotation(0, 0, 0),
	localScale(1, 1, 1),
	localTranslation(2, 1, 1),
	worldRotation(3, 3, 3),
	worldScale(1, 1, 1),
	worldTranslation(0, 0, 0),
	color(glm::vec3(1, 1, 1)),
	local_scale(2),
	world_scale(1)
{

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
	return faces_.size();
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
	float xMid = (xMax + xMin) / 2.0;
	float yMid = (yMax + yMin) / 2.0;
	float zMid = (zMax + zMin) / 2.0;

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
	glm::mat4x4 tm = glm::transpose(MeshModel::TranslationMatrix(this->worldTranslation));
	glm::mat4x4 xrm = MeshModel::X_RotationMatrix(this->worldRotation);
	glm::mat4x4 yrm = MeshModel::Y_RotationMatrix(this->worldRotation);
	glm::mat4x4 zrm = MeshModel::Z_RotationMatrix(this->worldRotation);
	glm::mat4x4 sm = MeshModel::ScaleMatrix(glm::vec3(world_scale, world_scale, world_scale));
	this->worldTransform = tm * xrm * yrm * zrm * sm;
}
void MeshModel::setLocalTransformation()
{
	glm::mat4x4 tm = glm::transpose(MeshModel::TranslationMatrix(this->localTranslation));
	glm::mat4x4 xrm = MeshModel::X_RotationMatrix(this->localRotation);
	glm::mat4x4 yrm = MeshModel::Y_RotationMatrix(this->localRotation);
	glm::mat4x4 zrm = MeshModel::Z_RotationMatrix(this->localRotation);
	glm::mat4x4 sm = MeshModel::ScaleMatrix(glm::vec3(local_scale, local_scale, local_scale));
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
	//return translate_mat_loc * rotation_mat_locX*rotation_mat_locY*rotation_mat_locZ*scaling_mat_loc;
	glm::mat4x4 mat = GetWorldTransformation() * GetLocalTransformation();
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
glm::mat4x4 MeshModel::scale(float a)
{
	return glm::mat4x4
	{ a,	0,	0,	0,
		0,	a,	0,	0,
		0,	0,	a,	0,
		0,	0,	0,	1
	};
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


