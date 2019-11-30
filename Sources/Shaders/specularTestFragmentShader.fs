#version 330 core

#define MAX_DIRECTIONAL_LIGHTS 2
#define MAX_POINT_LIGHTS 4
#define MAX_SPOTLIGHTS_LIGHTS 4

#define Material_Diffuse StaticTexture_0
#define Material_Specular StaticTexture_1

struct DirLight {
    vec3 direction;
    vec3 lightColor;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 lightColor;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 lightColor;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};



out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform float Time;
uniform float MaterialShininess;

uniform sampler2D StaticTexture_0;
uniform sampler2D StaticTexture_1;

uniform int DirectionalLightCounts;
uniform int PointLightCounts;
uniform int SpotLightCounts;

uniform DirLight dirLight[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight[MAX_SPOTLIGHTS_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec3 result = vec3(0.f);
    for(int i = 0; i < min(DirectionalLightCounts, MAX_DIRECTIONAL_LIGHTS); i++)
       result += CalcDirLight(dirLight[i], norm, viewDir);
    // phase 2: point light
    for(int i = 0; i < min(PointLightCounts, MAX_POINT_LIGHTS); i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    // phase 3: spot light
    for(int i = 0; i < min(SpotLightCounts, MAX_SPOTLIGHTS_LIGHTS); i++)
        result += CalcSpotLight(spotLight[i], norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), MaterialShininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(Material_Diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(Material_Diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(Material_Specular, TexCoords));
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), MaterialShininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(Material_Diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(Material_Diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(Material_Specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), MaterialShininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(Material_Diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(Material_Diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(Material_Specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
