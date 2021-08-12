#version 330

uniform sampler2D grassSampler;
uniform sampler2D waterSampler;
uniform sampler2D snowSampler;

uniform mat4 mvpMatrix;

in vec2 TexCoord;


void main()
{
    gl_FragColor = texture(grassSampler, TexCoord);
}
