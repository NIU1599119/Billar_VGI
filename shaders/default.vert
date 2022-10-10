#version 330 core

layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
layout (location = 2) in vec2 aTexCoord; // the texture variable has attribute position 2

uniform vec2 dPos;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos.x + dPos.x, aPos.y + dPos.y, aPos.z, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
