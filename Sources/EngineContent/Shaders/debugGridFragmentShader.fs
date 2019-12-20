#version 330 core
out vec4 FragColor;

layout (std140) uniform shader_data
{ 
  mat4 viewMatrix;
  mat4 worldProjection;
  vec3 cameraLocation;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
uniform mat4 view;

float largeLineSize = 0.05f;

void main()
{
  vec3 res = FragPos.xyz;
  res = ceil(mod(abs(res), 1.f) - 1 + largeLineSize);

  FragColor = vec4(1 - res, 1.f);
}
