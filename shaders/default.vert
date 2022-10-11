#version 330 core

layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
// layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
layout (location = 1) in vec2 aTexCoord; // the texture variable has attribute position 1

// uniform vec2 dPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    // gl_Position = vec4(aPos.x + dPos.x, aPos.y + dPos.y, aPos.z, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    // ourColor = aColor;
    TexCoord = aTexCoord;
}
