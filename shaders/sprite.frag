#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;

void main()
<<<<<<< HEAD
{
    vec4 texColor = texture(image, TexCoords);
    if (texColor.a < 0.5)
        discard;

    color = vec4(spriteColor, 1.0) * texColor;
=======
{    
    color = vec4(spriteColor, 1.0) * texture(image, TexCoords);
>>>>>>> Edicion-de-modelos
}