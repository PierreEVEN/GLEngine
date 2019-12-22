

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform float Time;

layout (std140) uniform shader_data
{
  mat4 viewMatrix;
  mat4 worldProjection;
  vec3 cameraLocation;
};

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = worldProjection * viewMatrix * vec4(FragPos, 1.0);
}
