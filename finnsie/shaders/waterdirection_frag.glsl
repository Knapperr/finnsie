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
uniform float heightScale2;
uniform float heightScaleModulated2;
uniform bool dualGrid;

// Water controls
uniform float time;
uniform float tiling2;
uniform float speed2;
uniform float flowStrength2;
uniform float gridResolution;
uniform float tilingModulated;

uniform float uJump;
uniform float vJump;
uniform float flowOffset;

vec3 UnpackDerivativeHeight(vec4 textureData)
{
    vec3 dh = textureData.agb;
    dh.xy = dh.xy * 2 - 1;
    return dh;
}

vec2 DirectionalFlowUVW (
    vec2 uv,  vec3 flowVectorAndSpeed, float tiling, float time,
    out mat2 rotation
)
{
    vec2 dir = normalize(flowVectorAndSpeed.xy);
    rotation = mat2(dir.y, dir.x, -dir.x, dir.y);
    uv = mat2(dir.y, -dir.x, dir.x, dir.y) * uv;
    uv.y -= time * flowVectorAndSpeed.z;
    return uv * tiling;
}

vec3 FlowCell (vec2 uv, vec2 offset, float time, bool gridB)
{
    vec2 shift = 1 - offset;
    shift *= 0.5;
    offset *= 0.5;
    if (gridB)
    {
        offset += 0.25;
        shift -= 0.25;
    }
    mat2 derivRotation;
    vec2 uvTiled = (floor(uv * gridResolution + offset) + shift) / gridResolution;
    vec3 flow = texture2D(texture_normal1, uvTiled).rgb;
    flow.xy = flow.xy * 2 - 1;
    flow.z *= flowStrength2; 
    float tiling = flow.z * tilingModulated + tiling2;
    vec2 uvFlow = DirectionalFlowUVW(
        uv + offset, flow, tiling, time,
        derivRotation
    );
    vec3 dh = UnpackDerivativeHeight(texture2D(texture_diffuse1, uvFlow));
    dh.xy = derivRotation * dh.xy;
    dh *= flow.z * heightScaleModulated2 + heightScale2;
    return dh;
}

vec3 FlowGrid(vec2 uv, float time, bool gridB)
{
    vec3 dhA = FlowCell(uv, vec2(0, 0), time, gridB);
    vec3 dhB = FlowCell(uv, vec2(1, 0), time, gridB);
    vec3 dhC = FlowCell(uv, vec2(0, 1), time, gridB);
    vec3 dhD = FlowCell(uv, vec2(1, 1), time, gridB);
    
    vec2 t = uv * gridResolution;
    if (gridB)
    {
        t += 0.25;
    }
    t = abs(2 * fract(t) - 1);
    float wA = (1 - t.x) * (1 - t.y);
    float wB = t.x * (1 - t.y);
    float wC = (1 - t.x) * t.y;
    float wD = t.x * t.y;
    return dhA * wA + dhB * wB + dhC * wC + dhD * wD;
}

void main()
{   
    float newTime = time * speed2;
    vec2 uv = fs_in.TexCoords;
    vec3 dh = FlowGrid(uv, newTime, false);
    if (dualGrid)
    {
        dh = (dh + FlowGrid(uv, newTime, true)) * 0.5;
    }
    //vec4 _color = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 _color = vec4(0.3, 0.6, 0.8, 1.0);
    //vec4 flowCol = vec4(dh, 1.0); // NOTE(CK): visualize the derivatives 
	vec4 flowCol = dh.z * dh.z * _color;    
    
    vec3 normal = normalize(vec3(-dh.xy, 1));

    // TODO(CK): Not sure if we need this?
    normal = normalize(normal * 2.0 - 1.0);

    // ambient
    vec3 ambient = (0.8 * flowCol.rgb);
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