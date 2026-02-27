#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
}; 

struct Light{
    bool has_a_source;
    vec3 light_pos;
    vec3 light_target;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cos_soft_cut_off_angle;
    float cos_hard_cut_off_angle;

    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
  
uniform Material material;
uniform int num_of_lights;
uniform Light[16] lights;
uniform sampler2D Texture_1;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 total_light = vec3(0.0, 0.0, 0.0);
    vec3 diff_map_val = vec3(texture(material.diffuse, TexCoord));
    vec3 spec_val = vec3(texture(material.specular,TexCoord));

    for(int i = 0; i < max(num_of_lights,16); i++)
    {
        vec3 lightDir = lights[i].has_a_source ? normalize(lights[i].light_pos - FragPos) : lights[i].light_target; 

        float light_power = 1.0f;
        if(lights[i].has_a_source)
        {
            float cos_theta = dot(-lightDir, normalize(lights[i].light_target)); 
            if(cos_theta > lights[i].cos_hard_cut_off_angle)
            {    
                float distance  = length(lights[i].light_pos - FragPos);
                light_power = 1.0 / (lights[i].constant + lights[i].linear * distance + 
                            lights[i].quadratic * (distance * distance));
                if(cos_theta < lights[i].cos_soft_cut_off_angle)
                {
                    float epsilon   = lights[i].cos_soft_cut_off_angle - lights[i].cos_hard_cut_off_angle;
                    light_power *= clamp((cos_theta - lights[i].cos_hard_cut_off_angle) / epsilon, 0.0, 1.0); 
                }
            }
            else 
            {             
                light_power = 0.0;
            }
        }

        vec3 ambient_calc = lights[i].ambient * diff_map_val;

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diff_calc = lights[i].diffuse * diff * diff_map_val;


        vec3 spec_calc = vec3(0,0,0);
        if(spec_val.x + spec_val.y + spec_val.z > 0)
        {
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(normalize(-FragPos), reflectDir), 0.0), material.shininess);
            spec_calc = lights[i].specular * spec_val * spec;
        }

        total_light += (ambient_calc + (diff_calc + spec_calc)*light_power); 

    }

    FragColor = vec4(total_light * texture(Texture_1, TexCoord).rgb, 1.0f);
}