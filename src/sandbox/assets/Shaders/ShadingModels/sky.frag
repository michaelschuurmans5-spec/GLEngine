#version 330 core
out vec4 FragColor;

in vec3 v_TexCoords;

uniform vec3 u_DynamicSunDir;
uniform float u_Time; // Received from C++ engine clock loop

// Standard high-performance pseudo-random noise generator function
float hash(vec2 p) {
    p = fract(sin(p * mat2(127.1, 311.7, 269.5, 183.3)) * 43758.5453);
    return p.x;
}

// Generates smooth, organic blending gradients out of raw grid lines
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f); // Smoothstep interpolation
    
    return mix(mix(hash(i + vec2(0.0,0.0)), hash(i + vec2(1.0,0.0)), f.x),
               mix(hash(i + vec2(0.0,1.0)), hash(i + vec2(1.0,1.0)), f.x), f.y);
}

// Layered Fractal Brownian Motion: Stacks multiple noise layers for fluffy cloud details
float fbm(vec2 p) {
    float v = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 3; i++) { // 3 octaves creates clean detail boundaries
        v += amplitude * noise(p);
        p *= 2.0;
        amplitude *= 0.5;
    }
    return v;
}

void main() {
    vec3 viewDir = normalize(v_TexCoords);
    
    // 1. Core Sky Gradients (Day vs Night Setup)
    vec3 skyDayColor = vec3(0.2, 0.5, 0.8);      
    vec3 horizonDayColor = vec3(0.6, 0.7, 0.8);  
    vec3 skyNightColor = vec3(0.02, 0.02, 0.05); 
    
    float heightFactor = max(viewDir.y, 0.0);
    vec3 standardDaySky = mix(horizonDayColor, skyDayColor, heightFactor);
    
    float sunHeightFactor = clamp(u_DynamicSunDir.y, 0.0, 1.0);
    vec3 finalSkyColor = mix(skyNightColor, standardDaySky, sunHeightFactor);
    
    // 2. Horizon Sunset Glow (Golden Hour)
    if (u_DynamicSunDir.y > -0.1 && u_DynamicSunDir.y < 0.3) {
        float sunsetFactor = clamp(dot(viewDir, u_DynamicSunDir), 0.0, 1.0);
        sunsetFactor = pow(sunsetFactor, 8.0);
        vec3 sunsetGlowColor = vec3(1.0, 0.4, 0.1);
        finalSkyColor = mix(finalSkyColor, sunsetGlowColor, sunsetFactor * (1.0 - sunHeightFactor));
    }
    
    // 3. >>> NEW: PROCEDURAL SCROLLING CLOUDS SYSTEM <<<
    // Only draw cloud layouts overhead in the upper hemisphere sky grid (Y > 0)
    if (viewDir.y > 0.0) {
        // Project 3D sky view coordinates down into a flat 2D planar map overhead
        vec2 skyUV = viewDir.xz / (viewDir.y + 0.1); 
        
        // Apply wind speed factor scrolling over time to your coordinate layout lanes
        vec2 windOffset = vec2(u_Time * 0.05, u_Time * 0.02);
        vec2 movingUV = skyUV * 0.5 + windOffset;
        
        // Generate cloud layout thickness mapping
        float cloudDensity = fbm(movingUV);
        
        // Smooth out density edge thresholds so clouds look soft and fluffy instead of like solid shapes
        float cloudThreshold = 0.5;
        float cloudFactor = smoothstep(cloudThreshold, cloudThreshold + 0.2, cloudDensity);
        
        // Cloud coloration: bright white during day, dim grey-blue during night
        vec3 cloudColor = mix(vec3(0.1, 0.1, 0.15), vec3(0.95, 0.95, 1.0), sunHeightFactor);
        
        // If it's sunset, tint the edges of the clouds orange from the sun flare angles
        if (u_DynamicSunDir.y > -0.1 && u_DynamicSunDir.y < 0.3) {
            float sunProximity = clamp(dot(viewDir, u_DynamicSunDir), 0.0, 1.0);
            cloudColor = mix(cloudColor, vec3(1.0, 0.6, 0.3), pow(sunProximity, 3.0) * (1.0 - sunHeightFactor));
        }
        
        // Blend your procedural cloud maps smoothly on top of your atmospheric sky gradient
        finalSkyColor = mix(finalSkyColor, cloudColor, cloudFactor * viewDir.y);
    }
    
    // 4. Draw the actual Sun Sphere disk
    float sunGlow = dot(viewDir, u_DynamicSunDir);
    if (sunGlow > 0.995) { 
        finalSkyColor = vec3(1.0, 1.0, 0.9); 
    } else if (sunGlow > 0.98) {
        float halo = (sunGlow - 0.98) / (0.995 - 0.98);
        finalSkyColor = mix(finalSkyColor, vec3(1.0, 0.7, 0.3), halo);
    }
    
    FragColor = vec4(finalSkyColor, 1.0);
}
