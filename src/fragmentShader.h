#pragma once

#include "math.h"

class FragmentShader {
public:
	FragmentShader() {}
	vec3 fragmentShader_phong(Texture* texture, vec3 normal, vec2 texCoords, vec3 worldPos,
		vec3 lightColor, vec3 lightPos, vec3 viewPos) {

		// sample in texture map
		//vec3 texColor = texture->sampleTex(texCoords);
		vec3 texColor = vec3(1.0, 0.5, 0.31);

		// ambient
		float ambientStrength = 0.5;
		vec3 ambient = lightColor * ambientStrength;

		// diffuse
		vec3 norm = normalize(normal);
		vec3 lightDir = lightPos - worldPos;
		lightDir = normalize(lightDir);
		float diff = maxInTwo(dot(norm, lightDir), 0.0);
		vec3 diffuse = lightColor * diff;

		// specular
		float specularStrength = 0.5;
		vec3 viewDir = viewPos - worldPos;
		viewDir = normalize(viewDir);
		//// phong
		//vec3 reflectDir = reflect(-lightDir, norm);
		//reflectDir = normalize(reflectDir);
		//float spec = pow(maxInTwo(dot(viewDir, reflectDir), 0.0), 16);
		// blinn-phong
		vec3 halfwayDir = lightDir + viewDir;
		halfwayDir = normalize(halfwayDir);
		float spec = pow(maxInTwo(dot(norm, halfwayDir), 0.0), 64);
		vec3 specular = lightColor * specularStrength * spec;

		vec3 color = (ambient + diffuse + specular) * texColor;

		return color;
	}
};
