#version 400
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform vec4 lightPos;
uniform mat4 mvpMatrix;
uniform vec3 cameraPos;
out vec4 oColor;
out vec2 TexCoord;


void main(){

    for(int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;
        switch (i) {
            case 0:
                TexCoord = vec2(0.5,1);
                break;
            case 1:
                TexCoord = vec2(0,0);
                break;
            case 2:
                TexCoord = vec2(1,0);
                break;
        }
        EmitVertex();
    }
    EndPrimitive();
}