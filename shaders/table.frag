#version 330 core

out vec4 FragColor;


struct Material {
    sampler2D texture_diffuse0;
    sampler2D texture_specular0;
    sampler2D texture_normal0;
    sampler2D texture_roughness0;
    float shininess;
    // sampler2D emission;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 5


in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

uniform vec3 viewPos;
// uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int n_pointLights;
// uniform SpotLight spotLight;
uniform Material material;

#define MAX_BALLS 16
uniform vec3 ballPositions[MAX_BALLS];
uniform float ballShadowRadius;
uniform int n_balls;

// function prototypes
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // FragColor = vec4(texture(material.texture_diffuse0, TexCoords).rgb, 1.0);
    // FragColor = FragColor + vec4(texture(material.texture_specular0, TexCoords).rgb, 0.0);

    // FragColor = vec4(normalize(Normal), 1.0);

    // return;
    // calculate normal with normal mapping
    // vec3 norm = normalize(Normal);
    vec3 norm = texture(material.texture_normal0, TexCoords).rgb;
    norm = norm * 2.0 - 1.0;
    norm = normalize(TBN * norm);

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    // directional light (the sun)
    // result += calcDirLight(dirLight, norm, viewDir);

    // // point lights (lights in a room)
    for(int i = 0; i < n_pointLights; i++)
    {
        result += calcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    // spot light (flashlight)
    // result += calcSpotLight(spotLight, norm, FragPos, viewDir);

    // shadow calculation (very simple shadow xd)
    // for (int i = 0; i < n_balls; i++)
    // {
    //     float shadow = min(1, max(0, (1/ballShadowRadius)*length(ballPositions[i]-FragPos)));
    //     result *= vec3(shadow);
    // }


    FragColor = vec4(result, 1.0);
}



vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // combine results
    vec3 ambient = light.ambient * texture(material.texture_diffuse0, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse0, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular0, TexCoords).rgb;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}


vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec3 ambient = light.ambient * texture(material.texture_diffuse0, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse0, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular0, TexCoords).rgb;
    // vec3 specular = vec3(0.0);

    return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient * texture(material.texture_diffuse0, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse0, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular0, TexCoords).rgb;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}
