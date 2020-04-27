#version 330 core
out vec4 FragColor;

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

in vec2 TexCoords;
in vec3 FragPos;  
in vec3 Normal;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform float time;

vec3 FlowUVW(vec2 uv, vec2 flowVector, float time) 
{
    float progress = fract(time);
    vec3 uvw; 
    uvw.xy = uv - flowVector * progress;
    uvw.z = 1;
    return uvw;
}

void main()
{   
	// Remove white pixels on texture (using diffuse for now)
	//vec4 texColor = texture(texture_diffuse1, TexCoords);
	vec4 texColor = texture(texture_diffuse1, TexCoords);
	//texColor.z += texture(texture_normal1, TexCoords);
	if (texColor.a < 0.1)
	{
		discard;
	}

	/*
	An easy way to visualize your normals is to draw them using lines, 
	using {position, position + normal} as the start & end points.
	*/
	vec2 flowVector = texture2D(texture_normal1, TexCoords).rg * 2 - 1;
    float noise = texture2D(texture_normal1, TexCoords).a;
    float newTime = time + noise;
    vec3 uvw = FlowUVW(TexCoords, flowVector, newTime);

	vec4 col = texture2D(texture_diffuse1, uvw.xy) * uvw.z;

	// calculate normals
    // ambient
    vec3 ambient = light.ambient * texture(texture_diffuse1, uvw.xy).rgb;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, uvw.xy).rgb;

	// specular 
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(texture_normal1, uvw.xy).rgb;
	vec3 result = ambient + diffuse + specular;
	vec4 final = vec4(result, 1.0);

    FragColor = col * final;
    //FragColor = col;
}