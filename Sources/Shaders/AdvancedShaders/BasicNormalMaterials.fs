#version 330 core

#define MAX_DIRECTIONAL_LIGHTS 16
#define MAX_POINT_LIGHTS 512
#define MAX_SPOTLIGHTS_LIGHTS 64

#define Material_Diffuse StaticTexture_0
#define Material_Specular StaticTexture_1
#define Material_Normal StaticTexture_2

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

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

uniform vec3 viewPos;
float MaterialShininess = 1.f;

uniform sampler2D StaticTexture_0;
uniform sampler2D StaticTexture_1;
uniform sampler2D StaticTexture_2;

vec3 CalcDirLight(LightParameters lightBaseParams, DirectionalLightParameters dirLightParams, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(LightParameters lightBaseParams, PointLightParameters pointLightParams, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(LightParameters lightBaseParams, SpotLightParameters spotLightParams, vec3 normal, vec3 fragPos, vec3 viewDir);


void main()
{

    // obtain normal from normal map in range [0,1]
    vec3  norm = texture(Material_Normal, TexCoords).rgb;
    // transform normal vector to range [-1,1]
    norm = normalize(norm * 2.0 - 1.0);

    norm = normalize(TBN * norm);


    // properties
     norm = normalize(Normal);
    vec3 viewDir = normalize(cameraLocation.xyz - FragPos);

    vec3 result = vec3(0.f);
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    for(int i = 0; i < min(directionalLightCount, MAX_DIRECTIONAL_LIGHTS); i++)
    {
       result += CalcDirLight(dirLightbaseParams[i], dirLightParams[i], norm, viewDir);
    }
    // phase 2: point light
    for(int i = 0; i < min(pointLightCount, MAX_POINT_LIGHTS); i++)
    {
        result += CalcPointLight(pointLightBaseParams[i], pointLightParams[i], norm, FragPos, viewDir);
    }
    // phase 3: spot light
    for(int i = 0; i < min(spotLightCount, MAX_SPOTLIGHTS_LIGHTS); i++)
    {
        //result += CalcSpotLight(spotLightBaseParams[i], spotLightParams[i], norm, FragPos, viewDir);
        //result = spotLightParams[i].direction.xyz;
    }
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(LightParameters lightBaseParams, DirectionalLightParameters dirLightParams, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-dirLightParams.direction.xyz);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), MaterialShininess);
    // combine results
    vec3 ambient = lightBaseParams.ambient.xyz * vec3(texture(Material_Diffuse, TexCoords));
    vec3 diffuse = lightBaseParams.diffuse.xyz * diff * vec3(texture(Material_Diffuse, TexCoords));
    vec3 specular = lightBaseParams.specular.xyz * spec * vec3(texture(Material_Specular, TexCoords)).r;
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(LightParameters lightBaseParams, PointLightParameters pointLightParams, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float distance = length(pointLightParams.lightPos.xyz - fragPos);

    vec3 lightDir = normalize(pointLightParams.lightPos.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), MaterialShininess);
    // attenuation
    float attenuation = 1.0 / (pointLightParams.constant + pointLightParams.linear * distance + pointLightParams.quadratic * (distance));
    // combine results
    vec3 ambient = lightBaseParams.ambient.xyz * vec3(texture(Material_Diffuse, TexCoords));
    vec3 diffuse = lightBaseParams.diffuse.xyz * diff * vec3(texture(Material_Diffuse, TexCoords));
    vec3 specular = lightBaseParams.specular.xyz * spec * vec3(texture(Material_Specular, TexCoords)).r;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(LightParameters lightBaseParams, SpotLightParameters spotLightParams, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(spotLightParams.lightPos.xyz - fragPos);
    return lightDir;
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), MaterialShininess);
    // attenuation
    float distance = length(spotLightParams.lightPos.xyz - fragPos);
    float attenuation = 1.0 / (spotLightParams.constant + spotLightParams.linear * distance + spotLightParams.quadratic * (distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-spotLightParams.direction.xyz));
    float epsilon = spotLightParams.cutOff - spotLightParams.outerCutOff;
    float intensity = clamp((theta - spotLightParams.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = lightBaseParams.ambient.xyz * vec3(texture(Material_Diffuse, TexCoords));
    vec3 diffuse = lightBaseParams.diffuse.xyz * diff * vec3(texture(Material_Diffuse, TexCoords));
    vec3 specular = lightBaseParams.specular.xyz * spec * vec3(texture(Material_Specular, TexCoords)).r;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
