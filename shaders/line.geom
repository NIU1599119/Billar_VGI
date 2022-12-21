#version 330 core

// in gl_Vertex
// {
//     vec4  gl_Position;
//     float gl_PointSize;
//     float gl_ClipDistance[];
// } gl_in[];



layout (lines) in;
layout (line_strip, max_vertices = 2) out;

void main() {
    gl_Position = gl_in[0].gl_Position;
    // gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    // gl_Position = gl_in[0].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
    return;

    // // passthrough
    // for (int i = 0; i < 2; i++)
    // {
    //     gl_Position = gl_in[i].gl_Position; 
    //     EmitVertex();
    // }
    // EndPrimitive();
}    