#version 330 core
out vec4 FragColor;

in vec3 LocalFragPos;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 VertexColor;

uniform int state;

void main()
{
  FragColor = vec4(VertexColor * .5);
  if (state == 1)
  {
    FragColor *= vec4(2, 1, 1, 1);
  }
  else if (state == 2)
  {
    FragColor *= vec4(1, 2, 1, 1);
  }
  else if (state == 3)
  {
    FragColor *= vec4(1, 1, 2, 0);
  }
}
