#version 330

layout (location = 0) in vec4 position;

uniform vec3 mvpMatrix;

void main()
{
  gl_Position = position;
}
