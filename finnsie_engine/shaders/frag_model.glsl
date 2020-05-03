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

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
 
in vec3 Normal;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_normal2;
uniform float time;

vec3 FlowUVW(vec2 uv, vec2 flowVector, vec2 jump, float flowOffset, float tiling, float time, bool flowB) 
{
    float phaseOffset = flowB ? 0.5 : 0;
    float progress = fract(time + phaseOffset);
    vec3 uvw; 
    uvw.xy = uv - flowVector * (progress + flowOffset);
    uvw.xy *= tiling;
    uvw.xy += phaseOffset;
    uvw.xy += (time - progress) * jump;
    uvw.z = 1 - abs(1 - 2 * progress);
    return uvw;
}

vec3 UnpackDerivativeHeight(vec4 textureData)
{
    vec3 dh = textureData.agb;
    dh.xy = dh.xy * 2 - 1;
    return dh;
}

void main()
{   
    vec3 greyScale = texture(texture_diffuse1, fs_in.TexCoords).rrr;

    // TODO(CK): Make these all uniforms
    float uJump = 0.24;
    float vJump = 0.24;
    float tiling = 2;
    float speed = 0.5;
    float flowStrength = 0.25;
    float flowOffset = -0.5;
    // ----------------------------------

	vec2 flowVector = texture2D(texture_normal1, fs_in.TexCoords).rg * 2 - 1;
    flowVector *= flowStrength;
	float noise = texture2D(texture_normal1, fs_in.TexCoords).a;
	float newTime = time * speed + noise;
    vec2 jump = vec2(uJump, vJump);

	vec3 uvwA = FlowUVW(fs_in.TexCoords, flowVector, jump, flowOffset, tiling, newTime, false);
    vec3 uvwB = FlowUVW(fs_in.TexCoords, flowVector, jump, flowOffset, tiling, newTime, true);

    // NOTE(CK): Using single channel (GL_RED) have to fill the remaining channels with rrrr
    vec4 texA = texture2D(texture_diffuse1, uvwA.xy).rrrr * uvwA.z;
    vec4 texB = texture2D(texture_diffuse1, uvwB.xy).rrrr * uvwB.z;

    vec4 _color = vec4(0.5, 0.5, 0.5, 1.0);
	vec4 flowCol = (texA + texB) * _color;
    // NOTE(CK): This is the diffuse colour for the normals
    vec3 color = vec3(flowCol);

	// calculate normals
    /* Old code to calculate normals in tangent space
        vec3 normal = texture(texture_normal2, fs_in.TexCoords).rgb;
        vec4 normalA = texture(texture_normal2, uvwA.xy) * uvwA.z;
        vec4 normalB = texture(texture_normal2, uvwB.xy) * uvwB.z; 
        normal = normalize(normal * 2.0 - 1.0); // this normal is in tangent space
        vec3 normal = normalize((normalA.xyz + normalB.xyz) * 2.0 - 1.0);
    */

    // NOTE(CK): Water uses a derivative map instead of a normal map
    // the X derivative is stored in the A channel and the Y derivative
    // stored in the G channel. The B channle contians the original height map
    // derivatives are calculated by scaling the height by 0.1
    vec3 dhA = UnpackDerivativeHeight(texture2D(texture_normal2, uvwA.xy)) * uvwA.z;
    vec3 dhB = UnpackDerivativeHeight(texture2D(texture_normal2, uvwB.xy)) * uvwB.z;
    vec3 normal = normalize(vec3(-(dhA.xy + dhB.xy), 1));


    // NOTE(CK): This doesn't need to be used the diffuse color we just use the flowCol we found above
    // diffuse color
    //vec3 color = texture(texture_diffuse1, flowCoords.xy * flowCoords.z).rgb;
    
    // ambient
    vec3 ambient = (0.1 * color);
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular 
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
	FragColor = vec4(ambient + diffuse + specular, 1.0);
}