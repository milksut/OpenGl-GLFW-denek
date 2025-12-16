#version 330 core
#define MAX_TEX_PER_TYPE 16

in vec2 TexCoord;

uniform int TEX_COUNTS[];

uniform sampler2D DIFFUSE[MAX_TEX_PER_TYPE];
uniform sampler2D NORMAL[MAX_TEX_PER_TYPE];
uniform sampler2D SPECULAR[MAX_TEX_PER_TYPE];

out vec4 FragColor;
void main()
{
	vec4 Diffuse_col = texture(DIFFUSE[0], TexCoord);
	//for(int i = 1; i < TEX_COUNTS[0]; i++)
	//{
	//	Diffuse_col += texture(DIFFUSE[i], TexCoord);
	//}

	FragColor = Diffuse_col;
}