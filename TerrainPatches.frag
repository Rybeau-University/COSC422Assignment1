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

void main()
{
    if (height >= snowHeight){
        gl_FragColor = texture(snowSampler, TexCoord);
    } else if (height < snowHeight && height > 0.1 + waterHeight){
        if (height >= snowHeight - 1.0){
            gl_FragColor = mix(gl_FragColor = texture(snowSampler, TexCoord), texture(grassSampler, TexCoord), snowHeight - height);
        } else {
            gl_FragColor = texture(grassSampler, TexCoord);
        }
    }else{
        gl_FragColor = texture(waterSampler, TexCoord);
    }

}
