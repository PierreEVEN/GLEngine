#version 330 core
out vec4 FragColor;


layout (std140) uniform shader_data
{ 
  vec4 camera_position;
  vec4 light_position;
  vec4 light_diffuse;
};

in vec3 FragPos;
in vec3 Color;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
    FragColor = vec4(Color, 1.f);
}
