#version 440

// The transformation matrix
uniform mat4 MVP;
// The model transformation matrix
uniform mat4 M;
// The normal matrix
uniform mat3 N;

// Incoming position
layout (location = 0) in vec3 position;
// Incoming normal
layout (location = 2) in vec3 normal;
// Incoming binormal
layout (location = 3) in vec3 binormal;
// Incoming tangent
layout (location = 4) in vec3 tangent;
// Incoming texture coordinate
layout (location = 10) in vec2 tex_coord_in;

// Outgoing vertex position
layout (location = 0) out vec3 vertex_position;
// Outgoing texture coordinate
layout (location = 1) out vec2 tex_coord_out;
// Outgoing normal
layout (location = 2) out vec3 transformed_normal;
// Outgoing tangent
layout (location = 3) out vec3 tangent_out;
// Outgoing binormal
layout (location = 4) out vec3 binormal_out;

void main()
{
    // Transform position into screen space
    gl_Position = MVP * vec4(position, 1.0);
    // Transform position into world space
    vertex_position = (M * vec4(position, 1.0)).xyz;
    // Pass through texture coordinate
    tex_coord_out = tex_coord_in;
	// Transform normal
	transformed_normal = N * normal;

	// *****************
	// Transform tangent
	// *****************
	vec3 calcTang = tangent;
	vec3 calcBi = binormal;

	// if the vertex doesn't have a tangent, calculate it and it's binormal.
	if (tangent == vec3(0))
	{
		vec3 c1 = cross(normal, vec3(0, 0, 1));
		vec3 c2 = cross(normal, vec3(0, 1, 0));

		if (length(c1) > length(c2))
			calcTang = normalize(c1);
		else
			calcTang = normalize(c2);

		calcBi = normalize(cross(normal, calcTang));

	}

	// transform tangent & binormal
	tangent_out = N * calcTang;
	binormal_out = N * calcBi;
	
}