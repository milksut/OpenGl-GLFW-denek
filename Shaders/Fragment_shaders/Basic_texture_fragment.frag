#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Texture_1;

void main()
{
    FragColor = texture(Texture_1, TexCoord);
}