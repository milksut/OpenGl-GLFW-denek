#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
}; 

struct Light{
    bool has_a_source;
    vec3 light_pos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
  
uniform Material material;
uniform Light light;
uniform sampler2D Texture_1;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = light.has_a_source ? normalize(light.light_pos - FragPos) : light.light_pos; 
    vec3 diff_map_val = vec3(texture(material.diffuse, TexCoord));

    vec3 ambient_calc = light.ambient * diff_map_val;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diff_calc = light.diffuse * diff * diff_map_val;

    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 spec_val = vec3(texture(material.specular,TexCoord));

    vec3 spec_calc = vec3(0,0,0);
    if(spec_val.x + spec_val.y + spec_val.z > 0)
    {
        float spec = pow(max(dot(normalize(-FragPos), reflectDir), 0.0), material.shininess);
        spec_calc = light.specular * spec_val * spec;
    }

    FragColor = vec4((ambient_calc + diff_calc + spec_calc) * texture(Texture_1, TexCoord).rgb, 1.0f);
}