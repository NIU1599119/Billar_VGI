#version 330 core

out vec4 FragColor;

uniform vec3 color;

void main()
{
    float xm2 = mod( gl_FragCoord.x, 2.0 );
    float ym2 = mod( gl_FragCoord.y, 2.0 );
    if ( int(xm2) != int(ym2) )
            discard;
    FragColor = vec4(color, 1.0);

}
