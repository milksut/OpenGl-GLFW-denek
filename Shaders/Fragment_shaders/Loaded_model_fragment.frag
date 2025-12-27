#version 330 core
#define MAX_TEX_PER_TYPE 16

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

/* ================= MATERIAL ================= */

struct Material
{
    vec3 diffuse;    // Kd (MTL)
    vec3 specular;   // Ks (MTL)
    float shininess; // Ns (MTL)
};

uniform Material material;

/* ================= TEXTURES ================= */

uniform sampler2D DIFFUSE[MAX_TEX_PER_TYPE];
uniform sampler2D SPECULAR[MAX_TEX_PER_TYPE];
uniform sampler2D NORMAL_MAP[MAX_TEX_PER_TYPE];

uniform int DIFFUSE_COUNT;
uniform int SPECULAR_COUNT;
uniform int NORMAL_COUNT;

/* ================= LIGHT ================= */

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 ambientLight;

void main()
{
    /* -------- BASE COLOR -------- */
    vec3 baseColor = material.diffuse;

    if (DIFFUSE_COUNT > 0)
    {
        baseColor *= texture(DIFFUSE[0], TexCoord).rgb;
    }

    /* -------- NORMAL -------- */
    vec3 normal = normalize(Normal);
    if (NORMAL_COUNT > 0)
    {
        normal = texture(NORMAL_MAP[0], TexCoord).rgb;
        normal = normalize(normal * 2.0 - 1.0);
    }

    /* -------- AMBIENT -------- */
    vec3 ambient = ambientLight * baseColor;

    /* -------- DIFFUSE -------- */
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * baseColor * lightColor;

    /* -------- SPECULAR -------- */
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 specColor = material.specular;
    if (SPECULAR_COUNT > 0)
    {
        specColor *= texture(SPECULAR[0], TexCoord).rgb;
    }

    vec3 specular = spec * specColor * lightColor;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
