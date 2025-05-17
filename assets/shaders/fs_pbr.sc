$input v_position, v_normal, v_texcoord0, v_color0

#include "common.sh"

// Material parameters
uniform vec4 u_baseColor;
uniform vec4 u_materialParams; // x: metallic, y: roughness, z: unused, w: unused
uniform vec4 u_emissive;       // xyz: emissive color, w: intensity

// Light data
uniform vec4 u_lightPosRange[4];    // xyz: position, w: range
uniform vec4 u_lightColorIntensity[4]; // xyz: color, w: intensity
uniform vec4 u_lightDirSpot[4];    // xyz: direction, w: spot params
uniform vec4 u_ambientColor;        // xyz: ambient color, w: ambient intensity

// Texture samplers
SAMPLER2D(s_texColor, 0);
SAMPLER2D(s_texNormal, 1);
SAMPLER2D(s_texMetallicRoughness, 2);
SAMPLER2D(s_texEmissive, 3);

// PBR functions
float distribution_ggx(float ndoth, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float ndoth2 = ndoth * ndoth;
    float denom = ndoth2 * (a2 - 1.0) + 1.0;
    return a2 / (3.14159 * denom * denom);
}

float geometry_schlick_ggx(float ndotv, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return ndotv / (ndotv * (1.0 - k) + k);
}

float geometry_smith(float ndotv, float ndotl, float roughness)
{
    return geometry_schlick_ggx(ndotv, roughness) * geometry_schlick_ggx(ndotl, roughness);
}

vec3 fresnel_schlick(float hdotv, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - hdotv, 5.0);
}

void main()
{
    // Sample textures
    vec4 albedo = u_baseColor * v_color0;
    
#ifdef ALBEDO_TEXTURE
    albedo *= texture2D(s_texColor, v_texcoord0);
#endif
    
    float metallic = u_materialParams.x;
    float roughness = u_materialParams.y;
    
#ifdef METALROUGH_TEXTURE
    vec4 metallicRoughness = texture2D(s_texMetallicRoughness, v_texcoord0);
    metallic *= metallicRoughness.b;
    roughness *= metallicRoughness.g;
#endif
    
    vec3 emissive = u_emissive.xyz * u_emissive.w;
    
#ifdef EMISSIVE_TEXTURE
    emissive *= texture2D(s_texEmissive, v_texcoord0).rgb;
#endif
    
    // Normal mapping
    vec3 normal = normalize(v_normal);
    
#ifdef NORMAL_TEXTURE
    mat3 TBN = /* Calculate TBN matrix */;
    vec3 n = texture2D(s_texNormal, v_texcoord0).rgb * 2.0 - 1.0;
    normal = normalize(TBN * n);
#endif
    
    // View direction
    vec3 view_dir = normalize(u_viewPos - v_position);
    
    // Surface properties
    vec3 f0 = vec3(0.04);
    f0 = mix(f0, albedo.rgb, metallic);
    
    // Final lighting
    vec3 Lo = vec3(0.0);
    
    // Ambient lighting
    vec3 ambient = u_ambientColor.xyz * u_ambientColor.w * albedo.rgb * (1.0 - metallic);
    
    // Directional/point/spot lighting
    for (int i = 0; i < 4; ++i) {
        vec3 light_pos = u_lightPosRange[i].xyz;
        float light_range = u_lightPosRange[i].w;
        vec3 light_color = u_lightColorIntensity[i].xyz;
        float light_intensity = u_lightColorIntensity[i].w;
        vec3 light_dir = u_lightDirSpot[i].xyz;
        float spot_params = u_lightDirSpot[i].w;
        
        // Skip invalid lights
        if (light_intensity <= 0.0) continue;
        
        vec3 L;
        float attenuation = 1.0;
        
        if (light_range > 0.0) {
            // Point or spot light
            L = light_pos - v_position;
            float distance = length(L);
            
            // Early exit if outside of range
            if (distance > light_range) continue;
            
            L = normalize(L);
            
            // Distance attenuation
            attenuation = max(0.0, 1.0 - (distance / light_range));
            attenuation *= attenuation;
            
            // Spot light cone
            if (spot_params > 0.0) {
                float spot_effect = dot(normalize(light_dir), -L);
                float spot_cutoff = 1.0 - spot_params;
                
                // Early exit if outside spot cone
                if (spot_effect < spot_cutoff) continue;
                
                // Spot attenuation
                attenuation *= smoothstep(spot_cutoff, 1.0, spot_effect);
            }
        } else {
            // Directional light
            L = normalize(-light_dir);
        }
        
        // PBR lighting
        vec3 H = normalize(view_dir + L);
        float NdotL = max(dot(normal, L), 0.0);
        float NdotV = max(dot(normal, view_dir), 0.0);
        float NdotH = max(dot(normal, H), 0.0);
        float HdotV = max(dot(H, view_dir), 0.0);
        
        if (NdotL > 0.0) {
            // Cook-Torrance BRDF
            float D = distribution_ggx(NdotH, roughness);
            float G = geometry_smith(NdotV, NdotL, roughness);
            vec3 F = fresnel_schlick(HdotV, f0);
            
            vec3 specular = (D * G * F) / max(0.0001, 4.0 * NdotV * NdotL);
            
            // Diffuse BRDF
            vec3 diffuse = (1.0 - F) * (1.0 - metallic) * albedo.rgb / 3.14159;
            
            // Add light contribution
            Lo += (diffuse + specular) * light_color * light_intensity * NdotL * attenuation;
        }
    }
    
    // Final color
    vec3 color = ambient + Lo + emissive;
    
    // Tone mapping and gamma correction would be applied here in a full implementation
    
    gl_FragColor = vec4(color, albedo.a);
}
