#version 400

layout(vertices = 4) out;
uniform vec3 eyePos;
uniform bool highTessellation;

void main()
{
    float highLevel, lowLevel;
    if (highTessellation){
        highLevel = 100.0;
        lowLevel = 30.0;
    } else {
        highLevel = 30.0;
        lowLevel = 10.0;
    }
    float distanceMin = 20.0, distanceMax = distance(vec3(150.0, 20.0, 120.0), vec3(-50, 0.0, -100.0));
    vec3 avg_position = vec3((gl_in[0].gl_Position +
    + gl_in[1].gl_Position
    + gl_in[2].gl_Position
    + gl_in[3].gl_Position)) * 0.25;
    float dist = distance(eyePos, avg_position);

    int level = int((dist - distanceMin) / (distanceMax - distanceMin) * (lowLevel - highLevel) + highLevel);
    gl_out[gl_InvocationID].gl_Position
      = gl_in[gl_InvocationID].gl_Position;
    gl_TessLevelOuter[0] = level;
    gl_TessLevelOuter[1] = level;
    gl_TessLevelOuter[2] = level;
    gl_TessLevelOuter[3] = level;
    gl_TessLevelInner[0] = level;
    gl_TessLevelInner[1] = level;
}