#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Utils.h"

glm::vec3 Utils::Vec3fFromStream(std::istream& issLine)
{
	float x, y, z;
	issLine >> x >> std::ws >> y >> std::ws >> z;
	return glm::vec3(x, y, z);
}

glm::vec2 Utils::Vec2fFromStream(std::istream& issLine)
{
	float x, y;
	issLine >> x >> std::ws >> y;
	return glm::vec2(x, y);
}

void Utils::PrintMeshModel(const std::string& filePath)
{
	std::ifstream ifile(filePath.c_str());

	// while not end of file
	while (!ifile.eof())
	{
		// get line
		std::string curLine;
		std::getline(ifile, curLine);

		// read the type of the line
		std::istringstream issLine(curLine);
		std::string lineType;

		issLine >> std::ws >> lineType;
		std::string str;

		// based on the type parse data
		if (lineType == "v")
		{
			str = issLine.str();
			std::cout << str << std::endl;
		}
		else if (lineType == "f")
		{
			str = issLine.str();
			std::cout << str << std::endl;
		}

	}

}
std::shared_ptr<MeshModel> Utils::LoadMeshModel(const std::string& filePath)
{
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoords;
	std::ifstream ifile(filePath.c_str());

	// while not end of file
	if (ifile)
		while (!ifile.eof())
		{
			// get line
			std::string curLine;
			std::getline(ifile, curLine);

			// read the type of the line
			std::istringstream issLine(curLine);
			std::string lineType;

			issLine >> std::ws >> lineType;

			// based on the type parse data
			if (lineType == "v")
			{
				vertices.push_back(Utils::Vec3fFromStream(issLine));
			}
			else if (lineType == "vn")
			{
				normals.push_back(Utils::Vec3fFromStream(issLine));
			}
			else if (lineType == "vt")
			{
				textureCoords.push_back(Utils::Vec2fFromStream(issLine));
			}
			else if (lineType == "f")
			{
				faces.push_back(Face(issLine));
			}
			else if (lineType == "#" || lineType == "")
			{
				// comment / empty line
			}
			else
			{
				std::cout << "Found unknown line Type \"" << lineType << "\"";
			}
		}

	return std::make_shared<MeshModel>(faces, vertices, normals, textureCoords, Utils::GetFileName(filePath));
}

std::string Utils::GetFileName(const std::string& filePath)
{
	if (filePath.empty()) {
		return {};
	}

	auto len = filePath.length();
	auto index = filePath.find_last_of("/\\");

	if (index == std::string::npos) {
		return filePath;
	}

	if (index + 1 >= len) {

		len--;
		index = filePath.substr(0, len).find_last_of("/\\");

		if (len == 0) {
			return filePath;
		}

		if (index == 0) {
			return filePath.substr(1, len - 1);
		}

		if (index == std::string::npos) {
			return filePath.substr(0, len);
		}

		return filePath.substr(index + 1, len - index - 1);
	}

	return filePath.substr(index + 1, len - index);
}
