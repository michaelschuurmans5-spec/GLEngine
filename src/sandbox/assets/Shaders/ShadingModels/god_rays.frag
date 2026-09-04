#version 330 core
out vec4 FragColor;

in vec2 v_TexCoords;

uniform sampler2D u_ScreenTexture; // Your rendered game scene
uniform vec2 u_LightScreenPos;  // Light x y converted to screen coords 

// Raymarching Parameters
const int NUM_SAMPLES = 32;       // Higher = smoother rays, lower = faster performance
uniform float u_Density = 0.5;    // How long the rays are
uniform float u_Weight = 0.05;    // How bright the rays are
uniform float u_Decay = 0.95;     // How fast the rays fade out over distance
uniform float u_Exposure = 0.3;   // Overall intensity multiplier

void main() {
    // Calculate the vector pointing from this pixel to the light source
    vec2 textCoords = v_TexCoords;
    vec2 deltaTextCoords = (textCoords - u_LightScreenPos);

     // Divide the distance by the number of samples to get our step size
    deltaTextCoords *= 1.0 / float(NUM_SAMPLES) * u_Density;

     // Sample the initial pixel color
    vec4 color = texture(u_ScreenTexture, textCoords);
    
    // Track illumination decay over the ray length
    float illuminationDecay = 1.0;
    
    // MARCH! Walk along the line toward the light source
    for (int i = 0; i < NUM_SAMPLES; i++) {
        // Step closer to the light source
        textCoords -= deltaTextCoords;
        
        // Sample the scene at this step
        vec4 sampleColor = texture(u_ScreenTexture, textCoords);
        
        // Multiply by weight and current decay factor
        sampleColor *= u_Weight * illuminationDecay;
        
        // Accumulate the light ray brightness
        color += sampleColor;
        
        // Exponentially decay the light intensity for the next step
        illuminationDecay *= u_Decay;
    }
    
    // Output the final scene combined with the calculated volumetric rays
    FragColor = color * u_Exposure;
}