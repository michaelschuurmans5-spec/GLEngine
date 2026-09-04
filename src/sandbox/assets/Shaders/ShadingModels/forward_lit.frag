#version 330 core
out vec4 FragColor;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;

uniform sampler2D u_Texture;

// Lighting Uniforms
uniform vec3 u_LightPos; // light source
uniform vec3 u_LightColor; // color & intensity
uniform vec3 u_ViewPos; // Camera position

void main() {
    // 1. Ambient lighting
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * u_LightColor;
    
    // 2. Diffuse lighting
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPos - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;
    
    // 3. Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * u_LightColor;
    
    // Combine lighting maps with object texture
    vec4 texColor = texture(u_Texture, v_TexCoord);
    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    
    FragColor = vec4(result, texColor.a);
}