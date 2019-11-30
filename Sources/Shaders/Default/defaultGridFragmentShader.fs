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


float LinearizeDepth(float depth);

void main()
{
    FragColor = vec4(normalize(Normal), 1.f);//texture(material.diffuse, FragPos.xz / vec2(10.f, -10.f));
    float CameraDistance = max(max(abs((FragPos - CameraPosition).x), abs((FragPos - CameraPosition).y)), abs((FragPos - CameraPosition).z));
    float ScaleMultiplier = max(1.f, trunc(pow(CameraDistance, .75f) / 10) * 10.f);
    vec3 NormalizedNormal = normalize(Normal);


    //Compute specular
    vec3 lightDir = normalize(LightDirection);//normalize(LightPosition - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, NormalizedNormal);
    float Specular = pow(max(dot(viewDir, reflectDir), 0.0), 5.f);


    vec2 CoordinatesOffset = vec2(1.f, 1.f);

    if (CameraDistance < 100)
    {
      if (abs(NormalizedNormal.y) > .5f)
      {
          FragColor = texture(material.diffuse, FragPos.xz / ScaleMultiplier * CoordinatesOffset);
      }
      else if (abs(NormalizedNormal.x) > .5f)
      {
          FragColor = texture(material.diffuse, FragPos.yz / ScaleMultiplier * CoordinatesOffset) * vec4(0.8f, 0.8f, 1.f, 0.f);
      }
      else
      {
          FragColor = texture(material.diffuse, FragPos.xy / ScaleMultiplier * CoordinatesOffset) * vec4(1.f, 0.8f, .8f, 0.f);
      }
    }
    else
    {
        FragColor = texture(material.diffuse, TexCoords * CoordinatesOffset);
    }

    FragColor += vec4(Specular);


}
