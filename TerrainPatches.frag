#version 330

uniform sampler2D grassSampler;
uniform sampler2D waterSampler;
uniform sampler2D snowSampler;
uniform sampler2D heightMap;
uniform float snowHeight;
uniform float waterHeight;

uniform mat4 mvpMatrix;

in vec2 TexCoord;
in float height;
in vec4 lgtVec;
in vec4 normalEye;
in vec4 halfVec;

out vec4 outputColor;

vec4 calculateOutputColor(vec4 material, bool water){
    vec4 white = vec4(1.0);
    vec4 grey = vec4(0.2);

    vec4 color;
    vec4 ambOut;
    if (water){
        ambOut = (1 - min(waterHeight - texture(heightMap, TexCoord).r * 10, 0.7)) * material;
    } else {
        ambOut = grey * material;
    }
    float shininess = 100.0;
    float diffTerm = max(dot(lgtVec, normalEye), 0.1);
    vec4 diffOut = material * diffTerm;
    float specTerm = max(dot(halfVec, normalEye), 0.0);
    vec4 specOut = white *  pow(specTerm, shininess);

    color = ambOut + diffOut;
    if (water){
        color += specOut;
    }
    return color;
}


void main()
{

    if (height >= snowHeight){
        outputColor = calculateOutputColor(texture(snowSampler, TexCoord), false);
    } else if (height < snowHeight && height > 0.01 + waterHeight){
        if (height >= snowHeight - 1.0){
            outputColor = calculateOutputColor(mix(texture(snowSampler, TexCoord), texture(grassSampler, TexCoord), snowHeight - height), false);
        } else {
            outputColor = calculateOutputColor(texture(grassSampler, TexCoord), false);
        }
    }else{
        outputColor = calculateOutputColor(texture(waterSampler, TexCoord), true);
    }

}
