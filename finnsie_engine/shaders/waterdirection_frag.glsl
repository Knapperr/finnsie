#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 
 
uniform vec3 viewPos;
uniform Material material;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_normal2;

// Water controls
uniform float time;
uniform float tiling2;
uniform float speed2;

uniform float uJump;
uniform float vJump;
uniform float flowStrength;
uniform float flowOffset;
uniform float heightScale;
uniform float heightScaleModulated;


vec3 FlowUVW(vec2 uv, vec2 flowVector, vec2 jump, float flowOffset, float tiling, float time, bool flowB) 
{
    float phaseOffset = flowB ? 0.5 : 0;
    float progress = fract(time + phaseOffset);
    vec3 uvw; 
    uvw.xy = uv - flowVector * (progress + flowOffset);
    uvw.xy *= tiling2;
    uvw.xy += phaseOffset;
    uvw.xy += (time - progress) * jump;
    uvw.z = 1 - abs(1 - 2 * progress);
    return uvw;
}

vec2 DirectionalFlowUVW (
    vec2 uv,  vec2 flowVector, float tiling, float time
)
{
    vec2 dir = normalize(flowVector.xy);
    uv = mat2(dir.y, dir.x, -dir.x, dir.y) * uv;
    uv.y -= time;
    return uv * tiling;
}

vec3 UnpackDerivativeHeight(vec4 textureData)
{
    vec3 dh = textureData.agb;
    dh.xy = dh.xy * 2 - 1;
    return dh;
}

void main()
{   

    //vec2 uv = fs_in.TexCoords * tiling2;
    float newTime = time * speed2;
    vec2 uvFlow = DirectionalFlowUVW(fs_in.TexCoords, vec2(1, 1), tiling2, newTime);
    vec3 dh = UnpackDerivativeHeight(texture2D(texture_diffuse1, uvFlow));
    
    vec4 _color = vec4(1.0, 1.0, 1.0, 1.0);
	//vec4 flowCol = dh.z * dh.z * _color;    
    vec4 flowCol = vec4(dh, 1.0); // visualize the derivatives 
    vec3 normal = normalize(vec3(-dh.xy, 1));

    // ambient
    vec3 ambient = (0.2 * flowCol.rgb);
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * flowCol.rgb;
    // specular 
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
	FragColor = vec4(ambient + diffuse + specular, 1.0);
}