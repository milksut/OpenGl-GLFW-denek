#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Texture_1;
uniform vec4 delete_color;
uniform vec4 background_color;
uniform float tolerance;

void main()
{
    FragColor = texture(Texture_1, TexCoord);
    if(FragColor.a == 0.01f || distance(FragColor.rgb, delete_color.rgb) < tolerance)
        FragColor = background_color;
}