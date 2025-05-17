$input a_position, a_normal, a_texcoord0, a_color0
$output v_position, v_normal, v_texcoord0, v_color0

#include "common.sh"

void main()
{
    v_position = mul(u_modelMatrix, vec4(a_position, 1.0)).xyz;
    v_normal = normalize(mul(u_normalMatrix, vec4(a_normal, 0.0)).xyz);
    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;
    
    gl_Position = mul(u_viewProj, vec4(v_position, 1.0));
}
