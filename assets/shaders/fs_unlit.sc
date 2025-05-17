$input v_position, v_normal, v_texcoord0, v_color0

#include "common.sh"

// Material parameters
uniform vec4 u_color;

// Texture samplers
SAMPLER2D(s_texColor, 0);

void main()
{
    vec4 color = u_color * v_color0;
    
#ifdef TEXTURING
    color *= texture2D(s_texColor, v_texcoord0);
#endif
    
    gl_FragColor = color;
}
