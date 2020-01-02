#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

layout (std140) uniform shader_data
{
  mat4 viewMatrix;
  mat4 worldProjection;
  vec4 cameraLocation;
};

uniform samplerCube Cubemap_0;
void main()
{
    FragColor = texture(Cubemap_0, TexCoords);
}
