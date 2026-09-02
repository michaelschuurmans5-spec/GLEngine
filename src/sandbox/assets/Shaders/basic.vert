#version 330 core 
layout(location = 0) in vec3 aPos;

// 4x4 mathematical matrix passed dynamically from c++
uniform mat4 u_Transform;

// ViewProjection 
uniform mat4 u_ViewProjection;

void main() {
// multiply the matrix by the position vector
gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);

}