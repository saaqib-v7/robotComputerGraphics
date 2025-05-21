#version 430

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 textureCoords;

out vec4 outColor;

uniform sampler2D textureImage;
uniform vec3 cameraPosition;
uniform int isTabletop; // 1 for tabletop, 0 for others
uniform sampler2D shineTexture;
uniform int laserLightActive;
uniform vec3 laserLightPos;
uniform int debugView;

// If you want to use these, declare them as uniforms
// uniform sampler2D diffuseTexture;
// uniform sampler2D shininessTexture;

const vec3 lightDir = normalize(vec3(1.0, 2.0, 2.0));
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const vec3 ambientColor = vec3(0.1, 0.1, 0.1);

void main()
{
    float shininess = 32.0;
    vec3 N = normalize(worldNormal);
    vec3 L = normalize(lightDir);
    vec3 V = normalize(cameraPosition - worldPosition);
    vec3 R = reflect(-L, N);

    // Ambient
    vec3 ambient = ambientColor;

    // Diffuse
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float spec = pow(max(dot(R, V), 0.0), shininess);
    vec3 specular = spec * lightColor;

    // Texture
    vec3 texColor = texture(textureImage, textureCoords).rgb;

    // Checkerboard pattern for tabletop only
    if (isTabletop == 1) {
        float u = textureCoords.x;
        float v = textureCoords.y;
        int val = int(floor(8.0 * u) + floor(8.0 * v));
        if (val % 2 == 1) {
            texColor *= 0.8;
        }
    }
    if (isTabletop == 0) { // Only for robot and not the table
        float shineVal = texture(shineTexture, textureCoords).r;
        shininess = shineVal == 0.0 ? 0.0 : shineVal * 16.0;
    }

    // Calculate base color first
    vec3 finalColor = (ambient + diffuse + specular) * texColor;

    // Add laser light if active
    if (laserLightActive == 1 && (isTabletop == 1 || isTabletop == 0)) {
        vec3 laserColor = vec3(1, 0, 0);
        float dist = length(laserLightPos - worldPosition);
        float attenuation = 1.0 / (1.0 + dist * dist * 0.05); // tweak as needed
        vec3 laserDir = normalize(laserLightPos - worldPosition);
        float NdotL = max(dot(N, laserDir), 0.0);
        finalColor += laserColor * NdotL * attenuation * 10.0; 
    }

    // Debug views
    if (debugView == 1) {
        // Normal as color
        outColor = vec4(worldNormal * 0.5 + 0.5, 1.0); // FIXED: use worldNormal
        return;
    }
    // The following debug views require additional uniforms/inputs.
    // Commented out to avoid errors. Uncomment and provide uniforms if needed.
    
    else if (debugView == 2) {
        // Shading only (no texture)
        vec3 shading = ambient + diffuse + specular;
        outColor = vec4(shading, 1.0);
        return;
    }
    else if (debugView == 3) {
        // Diffuse texture only
        outColor = vec4(texColor, 1.0);
        return;
    }
    else if (debugView == 4) {
        // Shininess texture only
        float shineVal = texture(shineTexture, textureCoords).r;
        outColor = vec4(shineVal, shineVal, shineVal, 1.0);
        return;
    }
    
    outColor = vec4(finalColor, 1.0);
}
