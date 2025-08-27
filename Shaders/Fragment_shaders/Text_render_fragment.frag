#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Texture_1;
uniform vec4 delete_colors[8];
uniform vec4 replace_colors[8];
uniform float tolerances[8];
uniform int num_colors;

void main()
{
    vec4 texColor = texture(Texture_1, TexCoord);
    vec4 finalColor = texColor;
    
   for(int i = 0; i < num_colors && i < 8; i++)
    {
        float dist = distance(texColor.rgb, delete_colors[i].rgb);
        float alpha_mask = smoothstep(0.0, tolerances[i], dist);
        
        finalColor = mix( replace_colors[i], finalColor, alpha_mask);
    }
    
    FragColor = finalColor;
}