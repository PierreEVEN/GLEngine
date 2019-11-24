#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec4 Color;
uniform float UseTexture;
void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)

	FragColor = UseTexture > 0 ? texture(texture2, TexCoord) :  texture(texture1, TexCoord);//vec4(1.0);//mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}