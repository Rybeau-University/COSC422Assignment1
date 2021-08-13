#version 400
layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 mvpMatrix;
uniform vec3 cameraPos;

out float height;
out vec4 oColor;
out vec2 TexCoord;

void main(){

    for(int i = 0; i < gl_in.length(); i++)
    {
        height = gl_in[i].gl_Position.y;
        switch (i) {
            case 0:
                TexCoord = vec2(0,1);
                break;
            case 1:
                TexCoord = vec2(0,0);
                break;
            case 2:
                TexCoord = vec2(1,1);
                break;
            case 3:
                TexCoord = vec2(1,0);
                break;
        }
        gl_Position = mvpMatrix * gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}