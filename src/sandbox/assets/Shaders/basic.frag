#version 330 core 
out vec4 FragColor;

// Recieved from vertex shader
in vec2 v_TexCoord;

// A Sampler2D special uniform that binds to GPU texture slot
uniform sampler2D u_Texture;

void main() {
	// Sample the exact color pixel from the image at the active UV spot
	FragColor = texture(u_Texture, v_TexCoord);
}