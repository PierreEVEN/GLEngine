#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
uniform vec3 CameraPosition;
uniform vec3 LightDirection;

uniform vec3 viewPos;
uniform Material material;

uniform mat4 view;
uniform mat4 projection;

float largeLineSize = 0.05f;

void main()
{
  vec3 res = FragPos.xyz;
  res = ceil(mod(abs(res), 1.f) - 1 + largeLineSize);

  FragColor = vec4(1 - res, 1.f);
}
