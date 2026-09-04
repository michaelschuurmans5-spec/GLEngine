#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 v_TexCoords; // Used as a 3D direction vector for procedural math

uniform mat4 u_ViewProjection;

void main() {
    v_TexCoords = aPos;
    
    // Convert to clip space
    vec4 pos = u_ViewProjection * vec4(aPos, 1.0);
    
    // CRITICAL ENGINE TRICK: Forcing z to equal w sets the depth to 1.0. 
    // This tells OpenGL that the skybox is infinitely far away, rendering behind everything else!
    gl_Position = pos.xyww;
}