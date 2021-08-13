#version 330

uniform sampler2D grassSampler;
uniform sampler2D waterSampler;
uniform sampler2D snowSampler;
uniform sampler2D heightMap;

uniform mat4 mvpMatrix;

in vec2 TexCoord;
in float height;

void main()
{
    if (height >= 6.0){
        gl_FragColor = texture(snowSampler, TexCoord);
//        gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    } else if (height < 6.0 && height > 2.0){
        gl_FragColor = texture(grassSampler, TexCoord);
//        gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }else{
        gl_FragColor = texture(waterSampler, TexCoord);
//        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }

}
