#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
};

in vec3 FragPos;

uniform vec3 viewPos;
uniform Material material;

void main()
{
    FragColor = texture(material.diffuse, FragPos.xz / vec2(10.f, -10.f));
}
