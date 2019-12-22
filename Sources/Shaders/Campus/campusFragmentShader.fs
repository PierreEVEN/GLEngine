

#version 330 core
out vec4 FragColor;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Light light;
uniform float Time;

uniform sampler2D StaticTexture_0;
uniform sampler2D DynamicTexture_0;

void main()
{
    FragColor = texture(DynamicTexture_0, vec2(TexCoords.x, TexCoords.y));
}
