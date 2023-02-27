
# assignment 3 


## 1. Explain Renderer::OpenGLRendering() 
The glGenTextures function is used to create a unique texture name, which is returned and stored in the GLuint pointer, gl_screen_tex.in addition, a vertex array object (VAO) is generated and a pointer to it called gl_screen_vtc.  This VAO is then bound to create a buffer, which allows for the initialization of two arrays: vtc, which contains coordinate data for the vertices, and tex, which contains coordinate data for texture mapping. After these arrays are initialized, the buffer is bound to the target (GL_ARRAY_BUFFER), which allocates memory for the data and enables it to be passed to OpenGL. Next, the fragment and vertex shaders are initialized, specifying that OpenGL should use them. The vertex coordinates and texture coordinates are then passed to the shaders, which allows for the rendering of the objects on the screen.

## 2.Update the MeshModel
When constructing the MeshModel object, we included the vertex buffer object (VBO) and vertex array object (VAO) as attributes. We generated the VAO to point to the VBO and bound it to ensure that OpenGL can locate it. We then created a buffer and assigned it the reference for the VBO, which we subsequently bound. We stored the vertices in the VBO before using the glDrawArrays function in the renderer class to draw triangles.


## 3.vertex shader
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


## 4.Fragment shader
#version 330 core

// material properties
struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	bool isLight; // set to true when rendering light sources
};

// light source properties
struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;        // material properties of the current object
uniform Light lights[10];         // array of all lights
uniform int numLights;            // number of active lights
uniform bool useAmbientLighting;  // enable/disable ambient lighting
uniform bool useDiffuseLighting;  // enable/disable diffuse lighting
uniform bool useSpecularLighting; // enable/disable specular lighting
uniform vec3 cameraPos;           // camera position
uniform sampler2D textureMap;     // current image for texture mapping
uniform sampler2D normalMap;      // current image for normal mapping
uniform sampler2D envMap;         // current image for environment mapping
uniform int mappingType;          // 0(None), 1(Tex-Plane), 2(Tex-Cylinder), 3(Tex-Sphere), 4(Tex-Coords), 5(Normal mapping), 6(Env mapping)
uniform bool useToonShading;      // enable/disable toon shading
uniform vec3 toonShadingColor;    // color to use for toon shading
uniform int toonShadingLevels;    // number of levels for toon shading

// Inputs from vertex shader (after interpolation was applied)
in vec3 orig_fragPos;
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;
in mat3 TBN; // Tangent-Bitangent-Normal matrix for normal mapping

// The final color of the fragment (pixel)
out vec4 frag_color;

vec4 PhongShading(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 normal)
{
	vec3 lighting = vec3(0);

	// get contributions from all light sources
	for (int i = 0; i < numLights; i++)
	{
		// ambient contribution
		if (useAmbientLighting)
			lighting += ambientColor * lights[i].ambient;

		// direction to light source 
		vec3 lightDir = normalize(lights[i].position - fragPos);

		// diffuse contribution
		if (useDiffuseLighting)
		{
			float diffuse = dot(lightDir, normal);
			if (diffuse > 0.0)
				lighting += diffuseColor * diffuse * lights[i].diffuse;
		}

		// specular contribution
		if (useSpecularLighting)
		{
			// direction of reflected light ray and direction to camera
			vec3 reflectedLightDir = reflect(-lightDir, normal);
			vec3 cameraDir = normalize(cameraPos - fragPos);

			float specular = dot(reflectedLightDir, cameraDir);
			if (specular > 0)
				lighting += specularColor * pow(specular, material.shininess) * lights[i].specular;
		}
	}

	return vec4(lighting, 1);
}

vec3 TextureColor()
{
	// canonical projection: plane
	if (mappingType == 1)
	{
		// planar coordinates
		vec2 uv = orig_fragPos.xy;
		return texture(textureMap, uv).rgb;
	}

	// canonical projection: cylinder
	if (mappingType == 2)
	{
		// cylindrical coordinates
		float theta = atan(orig_fragPos.x, orig_fragPos.z) / radians(180.0);
		float height = orig_fragPos.y;
		return texture(textureMap, vec2(theta, height)).rgb;
	}

	// canonical projection: sphere
	if (mappingType == 3)
	{
		// spherical coordinates 
		float theta = atan(orig_fragPos.x, orig_fragPos.z) / radians(180.0);
		float phi = atan(orig_fragPos.y, length(orig_fragPos.xz)) / radians(90.0);
		return texture(textureMap, vec2(theta, phi)).rgb;
	}

	// use texture coordinates
	if (mappingType == 4)
		return texture(textureMap, fragTexCoords).rgb;

	return vec3(1);
}

vec4 EnvironmentMapping(vec3 dir)
{
	vec2 uv = vec2(0);

	// use positive/negative x side
	if (abs(dir.x) >= abs(dir.y) && abs(dir.x) >= abs(dir.z))
		uv = dir.zy;

	// use positive/negative y side
	if (abs(dir.y) >= abs(dir.x) && abs(dir.y) >= abs(dir.z))
		uv = dir.xz;

	// use positive/negative z side
	if (abs(dir.z) >= abs(dir.x) && abs(dir.z) >= abs(dir.y))
		uv = dir.xy;

	// project to plane
	float maxAxis = max(max(abs(dir.x), abs(dir.y)), abs(dir.z));
	uv /= maxAxis;

	// remap from -1..1 to 0..1
	uv = (uv + 1.0) * 0.5;
	return texture(envMap, uv);
}

vec4 ToonShading(vec4 color)
{
	// get brightness by averaging color components
	color = clamp(color, 0.0, 1.0);
	float brightness = (color.r + color.g + color.b) / 3.0;

	// quantize brightness
	brightness *= toonShadingLevels;
	brightness = ceil(brightness);
	brightness /= toonShadingLevels;

	// apply brightness to toon shading color
	return vec4(toonShadingColor * brightness, 1);
}

void main()
{
	vec3 normal = normalize(fragNormal);
	frag_color = vec4(0, 0, 0, 1);

	if (mappingType == 0)
	{
		// no texture mapping: use material colors
		frag_color = PhongShading(material.ambient, material.diffuse, material.specular, normal);
	}
	else if(mappingType <= 4)
	{
		// texture mapping: use texture colors
		vec3 color = TextureColor();
		frag_color = PhongShading(color, color, color, normal);
	}
	else if(mappingType == 5)
	{
		// normal mapping: transform normal vector from texture space to world space
		vec3 textureNormal = texture(normalMap, fragTexCoords).rgb * 2.0 - 1.0;
		vec3 transformedNormal = normalize(TBN * textureNormal);
		frag_color = PhongShading(material.ambient, material.diffuse, material.specular, transformedNormal);
	}
	else if(mappingType == 6)
	{
		// environment mapping: reflect viewing direction and look up one of cube sides
		vec3 viewDir = normalize(fragPos - cameraPos);
		vec3 reflectedDir = reflect(viewDir, normal);
		frag_color = EnvironmentMapping(reflectedDir);
	}

	// toon shading: use small number of discrete shades
	if (useToonShading)
		frag_color = ToonShading(frag_color);

	// light source should keep its color
	if (material.isLight)
		frag_color = vec4((material.ambient + material.diffuse) / 2.0, 1);
}

## 5.update renderer
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


## 6.Phong Shading 

with one light
![](Part6_LargeMesh_1Light.PNG)


with two lights
![](Part6_LargeMesh_2Lights.PNG)


## 7.Texture mapping

![](Part7_TextureMapping_Coordinates.PNG)

![](Part7_TextureMapping_Cylinder.gif)

![](Part7_TextureMapping_Plane.PNG)

![](Part7_TextureMapping_Sphere.PNG)

## 8.ToonShading,NormalMapping,EnvironmentMapping

** Normal Mapping**

![](Part8_NormalMapping.PNG)

** Enviroment Mapping**
![](Part8_EnvironmenMapping.PNG)

** Toon Shading **

![](Part8_ToonShading.PNG)