#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

layout (std140) uniform shader_data
{
  mat4 viewMatrix;
  mat4 worldProjection;
  vec4 cameraLocation;
};
uniform samplerCube Cubemap_0;
uniform mat4 model;

void main()
{
    TexCoords = aPos;

    vec3 FragPos = vec3(mat3(model) * aPos);
    mat4 view = mat4(mat3(viewMatrix));
    gl_Position = worldProjection * view * vec4(FragPos * -1, 1.0);
}
