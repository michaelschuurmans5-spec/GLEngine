#version 330 core
// collect vertex data from meshes
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

// store the vertex data in these variables 
out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

// create variables type u get mesh data store in uniform 
uniform mat4 u_ViewProjection; // Camera data
uniform mat4 u_Transform; // mesh data


void main() {

	v_TexCoord = aTexCoord;

	// Transform normals into world space 
	v_Normal = mat3(u_Transform) * aNormal;

	 // Calculate the fragment's position in world space for light direction
	 v_FragPos = vec3(u_Transform * vec4(aPos, 1.0));

	 gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);

}