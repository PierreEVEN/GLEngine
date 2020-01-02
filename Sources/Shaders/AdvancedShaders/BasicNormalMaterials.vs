

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;


#define MAX_DIRECTIONAL_LIGHTS 16
#define MAX_POINT_LIGHTS 512
#define MAX_SPOTLIGHTS_LIGHTS 64

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model;

struct LightParameters {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 lightColor;
};

struct DirectionalLightParameters
{
  vec4 direction;
};

struct PointLightParameters
{
	float constant;
	float linear;
	float quadratic;
  vec4 lightPos;
};

struct SpotLightParameters
{
  vec4 direction;
  vec4 lightPos;
  float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};

layout (std140) uniform shader_data
{
  mat4 viewMatrix;
  mat4 worldProjection;
  vec4 cameraLocation;
  int directionalLightCount;
  int pointLightCount;
  int spotLightCount;
  LightParameters dirLightbaseParams[MAX_DIRECTIONAL_LIGHTS];
  DirectionalLightParameters dirLightParams[MAX_DIRECTIONAL_LIGHTS];
  LightParameters pointLightBaseParams[MAX_POINT_LIGHTS];
  PointLightParameters pointLightParams[MAX_POINT_LIGHTS];
  LightParameters spotLightBaseParams[MAX_SPOTLIGHTS_LIGHTS];
  SpotLightParameters spotLightParams[MAX_SPOTLIGHTS_LIGHTS];
};

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    TBN = mat3(aTangent, aBiTangent, aNormal);
    gl_Position = worldProjection * viewMatrix * vec4(FragPos, 1.0);
}
