#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 normalRotation;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    Normal = normalize(vec3(normalRotation * vec4(aNormal, 0.0)));
    vec3 Tangent = normalize(vec3(normalRotation * vec4(aTangent, 0.0)));

    vec3 Bitangent = normalize(cross(Normal, Tangent));
    TBN = mat3(Tangent, Bitangent, Normal);

    gl_Position = projection * view * vec4(FragPos, 1.0);
}