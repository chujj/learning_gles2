precision mediump float;
varying vec2 v_texCoord;
uniform sampler2D s_texture;
varying vec4 v_light_color;
varying float v_diffuse_factor;


void main()
{
     gl_FragColor= (texture2D( s_texture, v_texCoord ) + v_light_color) * v_diffuse_factor;
}


