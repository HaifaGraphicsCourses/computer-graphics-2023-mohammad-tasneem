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
