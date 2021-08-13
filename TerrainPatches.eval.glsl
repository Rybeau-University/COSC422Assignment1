#version 400

layout(quads, equal_spacing, ccw) in;

uniform mat4 mvpMatrix;
uniform sampler2D heightMap;
uniform float waterHeight;

void main()
{
    float xmin = -45, xmax = +45, zmin = 0, zmax = -100;
    vec4 posn;
    vec2 tcoord;

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    //Complete the following statement
    posn = (1-u)* (1-v) * gl_in[0].gl_Position
    + u * (1-v) * gl_in[1].gl_Position
    + u * v * gl_in[2].gl_Position
    + (1-u) * v * gl_in[3].gl_Position;

    tcoord.s = (posn.x -  xmin)/(xmax - xmin);
    tcoord.t = (posn.z - zmin)/ (zmax - zmin);

    float height = texture(heightMap, tcoord).r;
    posn.y = height * 10.0;
    if (posn.y <= waterHeight) {
        posn.y = waterHeight;
    }
    gl_Position = posn;
}