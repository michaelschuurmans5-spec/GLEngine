#version 410 core

layout(location = 0) out vec4 fragColor;

in vec2 v_TexCoords;

uniform sampler2D u_ScreenTexture; // Pass 1 scene texture
uniform vec2 u_LightScreenPos;     // Sun projection coordinate
uniform float u_Exposure;          // Dynamic intensity slider / height factor

// God ray constants for screen space radial blur step sampling
const int NUM_SAMPLES = 100;
const float Density = 1.0;
const float Weight = 0.01;
const float Decay = 0.98;

void main()
{
    // 1. Sample your clean, un-blurred base game scene frame
    vec4 baseSceneColor = texture(u_ScreenTexture, v_TexCoords);
    
    // 2. Initialize screen space raymarching loop paths
    vec2 textCoords = v_TexCoords;
    vec2 deltaTextCoords = (textCoords - u_LightScreenPos);
    deltaTextCoords *= 1.0 / float(NUM_SAMPLES) * Density;
    
    float illuminationDecay = 1.0;
    vec4 lightRaysAccumulation = vec4(0.0);
    
    // March a line along the screen toward the light coordinate position
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        textCoords -= deltaTextCoords;
        vec4 colorSample = texture(u_ScreenTexture, textCoords);
        
        colorSample *= Weight;
        colorSample *= illuminationDecay;
        lightRaysAccumulation += colorSample;
        illuminationDecay *= Decay;
    }
    
    // Multiply ONLY the streaks by your dynamic exposure multiplier
    lightRaysAccumulation *= u_Exposure;
    
    // ===================================================================
    // FIXED: ADD THE LIGHT STREAKS ON TOP OF YOUR BASE SCENE GEOMETRY
    // ===================================================================
    // If exposure falls to 0.0, lightRaysAccumulation becomes 0.0, 
    // leaving your base scene completely visible instead of multiplying into black!
    fragColor = baseSceneColor + lightRaysAccumulation;
}
