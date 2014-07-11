uniform mat4 uModelMatrix;
uniform mat4 uProjectionMatrix;
attribute vec4 vPosition;
attribute vec2 a_texCoord;
attribute vec3 a_normal;

varying vec3 v_normal;
varying vec4 v_position;
varying vec2 v_texCoord;

void main()
{
     v_normal = vec3(uModelMatrix *  vec4(a_normal, 0));
     v_position = uModelMatrix * vPosition;
     v_texCoord = a_texCoord;
     gl_Position = uProjectionMatrix * uModelMatrix * vPosition;
}


