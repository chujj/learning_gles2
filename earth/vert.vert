struct directional_light 
{
     vec3 direction;
     vec3 halfplane;
     vec4 ambient_color;
     vec4 diffuse_color;
     vec4 specular_color;
};

struct material_properties 
{
     vec4 ambient_color;
     vec4 diffuse_color;
     vec4 specular_color;
     float specular_exponent;
};

const float c_zero = 0.0;
const float c_one = 1.0;

/* uniform material_properties material; */
/* uniform directional_light light; */
const material_properties material = material_properties(
     vec4(1, 1, 1, 0),
     vec4(0.5, 0.5, 0.5, 0),
     vec4(0, 0, 0, 0),
     0.0);

const directional_light light = directional_light(
     vec3(-1, -0, 0.5),
     vec3(0, 0, 0),
     vec4(0, 0, 0, 0),
     vec4(1, 1, 1, 0),
     vec4(0, 0, 0, 0));

varying float v_diffuse_factor;

// normal has been transformal into eye space and is a normalized
// value returns the computed color
vec4 calc_directional_light(vec3 normal) 
{
     vec4 computed_color = vec4(c_zero, c_zero, c_zero, c_zero);
     float ndotl; // dot product of normal & light direction
     float ndoth; // dot product of normal & half-plane vector
     ndotl = max(c_zero, dot(normal, normalize( light.direction)));
     v_diffuse_factor = ndotl;
     /* ndoth = max(c_zero, dot(normal, light.halfplane)); */

     computed_color += (light.ambient_color * material.ambient_color);
     computed_color += (ndotl * light.diffuse_color * material.diffuse_color);
     /* if (ndoth > c_zero) { */
     /* 	  computed_color += (pow(ndoth, material.specular_exponent) * */
     /* 			     light.specular_color * material.specular_color); */
     /* } */
     return computed_color;
}


uniform mat4 uModelMatrix;
uniform mat4 uProjectionMatrix;
attribute vec4 vPosition;
attribute vec2 a_texCoord;
attribute vec3 a_normal;
varying vec2 v_texCoord;
varying vec4 v_light_color;


void main()
{
     gl_Position = uProjectionMatrix * uModelMatrix * vPosition;
     v_texCoord = a_texCoord;
     v_light_color = calc_directional_light (
	  normalize(
	       vec3(
		    uModelMatrix * (vec4(a_normal, 0))
		    )
	       )
	  );
}


