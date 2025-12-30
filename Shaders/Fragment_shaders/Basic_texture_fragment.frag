#version 330 core
out vec4 FragColor;


#define MAX_TEX_PER_TYPE 16

uniform sampler2D DIFFUSE[MAX_TEX_PER_TYPE];

in vec2 TexCoord;

void main()
{
    FragColor = texture(DIFFUSE[0],TexCoord);
}