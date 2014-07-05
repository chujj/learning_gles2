uniform mat4 uModelMatrix;
attribute vec4 vPosition;
attribute vec2 a_texCoord;
varying vec2 v_texCoord;

void main()
{
     gl_Position = uModelMatrix * vPosition;
     v_texCoord = a_texCoord;
}


