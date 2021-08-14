#version 330

uniform sampler2D grassSampler;
uniform sampler2D waterSampler;
uniform sampler2D snowSampler;
uniform sampler2D heightMap;
uniform float snowHeight;
uniform float waterHeight;
uniform bool fogEnabled;

in vec2 TexCoord;
in vec3 oPosition;
in vec4 lgtVec;
in vec4 normalEye;
in vec4 halfVec;

out vec4 outputColor;

vec4 calculateOutputColor(vec4 material, bool water){
    vec4 white = vec4(1.0);
    vec4 grey = vec4(0.2);
    float fogMin = 0.0, fogMax = -130;
    vec4 color;
    vec4 ambOut;
    if (water){
        ambOut = (1 - min(waterHeight - texture(heightMap, TexCoord).r * 10, 0.7)) * material;
    } else {
        ambOut = grey * material;
    }
    float diffTerm = max(dot(lgtVec, normalEye), 0.1);
    vec4 diffOut = material * diffTerm;

    color = ambOut + diffOut;

    if (fogEnabled){
        if(oPosition.z <= fogMin){
            float t = (oPosition.z - fogMin) / (fogMax - fogMin);
            color = (1 - t) * color + t * white;
        }
    }

    return color;
}


void main()
{

    if (oPosition.y >= snowHeight){
        outputColor = calculateOutputColor(texture(snowSampler, TexCoord), false);
    } else if (oPosition.y < snowHeight && oPosition.y > 0.01 + waterHeight){
        if (oPosition.y >= snowHeight - 1.0){
            outputColor = calculateOutputColor(mix(texture(snowSampler, TexCoord), texture(grassSampler, TexCoord), snowHeight - oPosition.y), false);
        } else {
            outputColor = calculateOutputColor(texture(grassSampler, TexCoord), false);
        }
    }else{
        outputColor = calculateOutputColor(texture(waterSampler, TexCoord), true);
    }

}
