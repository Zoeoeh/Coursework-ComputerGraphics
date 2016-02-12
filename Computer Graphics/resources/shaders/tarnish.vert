#version 440

// MVP transformation matrix
uniform mat4 MVP;
// M transformation matrix
uniform mat4 M;
// N transformation matrix
uniform mat3 N;
// Eye position
uniform vec3 eye_pos;

// Incoming position
layout (location = 0) in vec3 position;
// Incoming normal
layout (location = 2) in vec3 normal;
// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord;

// Outgoing texture coordinate
layout (location = 0) out vec2 vertex_tex_coord;
// Outgoing environment map texture coordinate
layout (location = 1) out vec3 map_tex_coord;

void main()
{
	// Calculate screen position
	gl_Position = MVP * vec4(position, 1.0);
	// ************************
	// Calculate world position
	// ************************
	vec3 worldPos = (M * vec4(position, 1.0)).xyz;
	
	// ********************
	// Transform the normal
	// ********************
	vec3 transN = N * normal;
	

	vec3 eye_dir = worldPos - eye_pos;

	vec3 reflection = reflect(eye_dir, transN);
	// ****************************************************************
	// Calculate tex_coord using world position and transformed normal
	// ****************************************************************
	map_tex_coord = normalize(reflection);


	// *******************************
	// Pass through texture coordinate
	// *******************************
	vertex_tex_coord = tex_coord;
}