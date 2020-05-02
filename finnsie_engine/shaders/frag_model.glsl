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

vec3 FlowUVW(vec2 uv, vec2 flowVector, vec2 jump, float time, bool flowB) 
{
    float phaseOffset = flowB ? 0.5 : 0;
    float progress = fract(time + phaseOffset);
    vec3 uvw; 
    uvw.xy = uv - flowVector * progress + phaseOffset;
    uvw.xy += (time - progress) * jump;
    uvw.z = 1 - abs(1 - 2 * progress);
    return uvw;
}

void main()
{   
    /* NOTE(CK): CANT USE THIS FOR THE WATER SHADER! (MAYBE I CAN)
    ================================================================
	Remove white pixels on texture (using diffuse for now)
	vec4 texColor = texture(texture_diffuse1, TexCoords);
	vec4 texColor = texture(texture_diffuse1, TexCoords);
	//texColor.z += texture(texture_normal1, TexCoords);
	if (texColor.a < 0.1)
	{
		discard;
	}
    ================================================================
    */

	/*
	An easy way to visualize your normals is to draw them using lines, 
	using {position, position + normal} as the start & end points.
	*/
    vec3 greyScale = texture(texture_diffuse1, fs_in.TexCoords).rrr;

    float uJump = 0.24;
    float vJump = 0.24;

	vec2 flowVector = texture2D(texture_normal1, fs_in.TexCoords).rg * 2 - 1;
	float noise = texture2D(texture_normal1, fs_in.TexCoords).a;
	float newTime = time + noise;
    vec2 jump = vec2(uJump, vJump);

	vec3 uvwA = FlowUVW(fs_in.TexCoords, flowVector, jump, newTime, false);
    vec3 uvwB = FlowUVW(fs_in.TexCoords, flowVector, jump, newTime, true);

    // NOTE(CK): Using single channel (GL_RED) have to fill the remaining channels
    vec4 texA = texture2D(texture_diffuse1, uvwA.xy).rrrr * uvwA.z;
    vec4 texB = texture2D(texture_diffuse1, uvwB.xy).rrrr * uvwB.z;

    vec4 _color = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 flowCol = (texA + texB) * _color;
    vec3 color = vec3(flowCol);

	// calculate normals
    //vec3 normal = texture(texture_normal2, fs_in.TexCoords).rgb;
    vec4 normalA = texture(texture_normal2, uvwA.xy) * uvwA.z;
    vec4 normalB = texture(texture_normal2, uvwB.xy) * uvwB.z; 
    //normal = normalize(normal * 2.0 - 1.0); // this normal is in tangent space
    vec3 normal = normalize((normalA.xyz + normalB.xyz) * 2.0 - 1.0);

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