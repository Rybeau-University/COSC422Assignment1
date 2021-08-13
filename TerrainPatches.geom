#version 400
layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 mvpMatrix;
uniform vec3 cameraPos;

out float height;
out vec4 oColor;
out vec2 TexCoord;

void main(){
    float xmin = -45, xmax = +45, zmin = 0, zmax = -100;
    for(int i = 0; i < gl_in.length(); i++)
    {
        TexCoord.s = (gl_in[i].gl_Position.x -  xmin)/(xmax - xmin);
        TexCoord.t = (gl_in[i].gl_Position.z - zmin)/ (zmax - zmin);
        height = gl_in[i].gl_Position.y;
        gl_Position = mvpMatrix * gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}