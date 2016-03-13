#version 440

#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light
{
    vec4 ambient_intensity;
    vec4 light_colour;
    vec3 light_dir;
};
#endif

// A material structure
#ifndef MATERIAL
#define MATERIAL
struct material
{
    vec4 emissive;
    vec4 diffuse_reflection;
    vec4 specular_reflection;
    float shininess;
};
#endif

// Forward declarations of used functions
vec4 calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
vec3 calc_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in vec3 sampled_normal, in vec2 tex_coord);

// Direction light being used in the scene
uniform directional_light light;
// Material of the object being rendered
uniform material mat;
// Position of the eye
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex[2];
// Normal map to sample from
//uniform sampler2D normal_map;

// Incoming vertex position
layout (location = 0) in vec3 position;
// Incoming texture coordinate
layout (location = 1) in vec2 tex_coord;
// Incoming normal
layout (location = 2) in vec3 normal;
// Incoming tangent
layout (location = 3) in vec3 tangent;
// Incoming binormal
layout (location = 4) in vec3 binormal;

// Outgoing colour
layout (location = 0) out vec4 colour;

void main()
{
	colour = vec4(0.0, 0.0, 0.0, 1.0);
    // **************
    // Sample texture
    // **************
	vec4 text_sample = texture2D(tex[0], tex_coord);
    
	// ************************
    // Calculate view direction
    // ************************
	vec3 view_dir = normalize(eye_pos - position); // eye - world
    
	// sample normal
	vec3 samp_norm = texture(tex[1], tex_coord).xyz;

	vec3 transN = calc_normal(normal, tangent, binormal, samp_norm, tex_coord);

    // ***************************
    // Calculate directional light
    // ***************************
	colour = calculate_direction(light, mat, transN, view_dir, text_sample);

}