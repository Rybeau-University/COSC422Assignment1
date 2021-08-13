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

out vec4 outputColor;

void main()
{
    if (height >= snowHeight){
        outputColor = texture(snowSampler, TexCoord);
    } else if (height < snowHeight && height > 0.1 + waterHeight){
        if (height >= snowHeight - 1.0){
            outputColor = mix(texture(snowSampler, TexCoord), texture(grassSampler, TexCoord), snowHeight - height);
        } else {
            outputColor = texture(grassSampler, TexCoord);
        }
    }else{
        outputColor = texture(waterSampler, TexCoord);
    }

}
