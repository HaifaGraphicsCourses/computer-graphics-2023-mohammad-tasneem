#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 texCoords;

// The model/view/projection matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// These outputs will be available in the fragment shader as inputs
out vec3 orig_fragPos;
out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoords;
out mat3 TBN; // Tangent-Bitangent-Normal matrix for normal mapping

void main()
{
	// Normal matrix is inverse transpose of model matrix
	mat3 normalMatrix = inverse(transpose(mat3(model)));

	// Apply the model transformation to the 'position' and 'normal' properties of the vertex,
	// so the interpolated values of these properties will be available for us in the fragment shader
	orig_fragPos = vec3(vec4(pos, 1.0f));
	fragPos = vec3(model * vec4(pos, 1.0f));
	fragNormal = normalize(normalMatrix * normal);

	// Tangent-Bitangent-Normal matrix for normal mapping
	vec3 T = normalize(normalMatrix * tangent);
	vec3 B = normalize(normalMatrix * bitangent);
	vec3 N = normalize(cross(T, B));
	TBN = mat3(T, B, N);

	// Pass the vertex texture coordinates property as it is. Its interpolated value
	// will be avilable for us in the fragment shader
	fragTexCoords = texCoords;

	// This is an internal OpenGL variable, we must set a value to this variable
	gl_Position = projection * view *  model * vec4(pos, 1.0f);
}
