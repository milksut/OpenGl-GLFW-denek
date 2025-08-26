#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Texture_1;
uniform vec4 delete_color;
uniform vec4 background_color;
uniform float tolerance;

void main()
{
    vec4 texColor = texture(Texture_1, TexCoord);
    
    float distance_rgb = distance(texColor.rgb, delete_color.rgb);
    
    float alpha_mask = smoothstep(0.0, tolerance, distance_rgb);
    
    FragColor = mix(background_color, texColor, alpha_mask);
}