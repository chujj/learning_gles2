// Attributes
attribute float aTheta;
// Uniforms
uniform mat4 uProjectionMatrix;
uniform float uK;

void main(void)
{
     float x = cos(uK*aTheta)*sin(aTheta);
     float y = cos(uK*aTheta)*cos(aTheta);
     gl_Position = uProjectionMatrix * vec4(x, y, -4.0, 1.0);
     gl_PointSize = 10.0;
}
