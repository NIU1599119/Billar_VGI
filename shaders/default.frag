#version 330 core


/////// PREVIOUS SHADER WITH TEXTURES AND THINGS
// out vec4 FragColor;

// // in vec3 ourColor; //
// in vec2 TexCoord;

// uniform sampler2D ourTexture;


// void main()
// {
//     FragColor = texture(ourTexture, TexCoord); // vec4(ourColor, 1.0f); // vec4(1.0f, 0.5f, 0.2f, 1.0f);
// }



out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    FragColor = vec4(lightColor * objectColor, 1.0);
}

