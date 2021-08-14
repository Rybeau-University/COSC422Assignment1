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

/*
    Calculates the output colour using the lighting vectors passed through from the Geometry Shader.
    If fog is enabled it also calculates and applies the fog effect.
    Specular reflections are applied only to water.
    Water variation with depth is also calculated here.
*/
vec4 calculateOutputColor(vec4 material, bool water){
    vec4 white = vec4(1.0);
    vec4 specColor = vec4(0.4);
    vec4 grey = vec4(0.2);
    float fogMin = 0.0, fogMax = -130;
    vec4 color;
    vec4 ambOut;

    //Water ambient and specular calculations
    if (water){
        ambOut = (1 - min(waterHeight - texture(heightMap, TexCoord).r * 10, 0.7)) * material;
        float shininess = 20.0;
        float specTerm = max(dot(halfVec, normalEye), 0);
        vec4 specOut = specColor * pow(specTerm, shininess);
        color += specOut;
    } else {
        ambOut = grey * material;
    }

    float diffTerm = max(dot(lgtVec, normalEye), 0.1);
    vec4 diffOut = material * diffTerm;

    color += ambOut + diffOut;

    //Fog calculations
    if (fogEnabled){
        if(oPosition.z <= fogMin){
            float t = (oPosition.z - fogMin) / (fogMax - fogMin);
            color = (1 - t) * color + t * white;
        }
    }

    return color;
}


/*
    Calls the output colour with the given texture for the vertex height. This will vary depending upon the
    set snow and water levels.
*/
void main()
{
    if (oPosition.y >= snowHeight){
        //Snow Texturing
        outputColor = calculateOutputColor(texture(snowSampler, TexCoord), false);
    } else if (oPosition.y < snowHeight && oPosition.y > 0.01 + waterHeight){
        if (oPosition.y >= snowHeight - 1.0){
            //Mixing of snow and grass textures for smooth transition
            outputColor = calculateOutputColor(mix(texture(snowSampler, TexCoord), texture(grassSampler, TexCoord), snowHeight - oPosition.y), false);
        } else {
            //Grass texturing
            outputColor = calculateOutputColor(texture(grassSampler, TexCoord), false);
        }
    }else{
        //Water texturing
        outputColor = calculateOutputColor(texture(waterSampler, TexCoord), true);
    }
}
