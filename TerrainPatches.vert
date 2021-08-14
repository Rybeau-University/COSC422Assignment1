#version 330

layout (location = 0) in vec4 position;

/*
  Pass through vertex shader
*/
void main()
{
  gl_Position = position;
}
