#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 ATangent;
layout (location = 4) in vec4 ABitTangent;
layout (location = 5) in vec4 aVertexColor;

layout (std140) uniform shader_data
{
  mat4 viewMatrix;
  mat4 worldProjection;
  vec3 cameraLocation;
};

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 VertexColor;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    VertexColor = aVertexColor;
    Normal = aNormal;
    TexCoords = aTexCoords;
    FragPos = aPos;
    gl_Position = worldProjection * viewMatrix * vec4(FragPos, 1.0);
}
