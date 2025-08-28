#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D Texture_1;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float ambientStrength;
uniform float specularStrength;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); 

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, norm); 
    float spec = pow(max(dot(normalize(-FragPos), reflectDir), 0.0), 32);

    FragColor = vec4((ambientStrength + diff + (spec*specularStrength)) * lightColor * texture(Texture_1, TexCoord).rgb,1.0f);
}