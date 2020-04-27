#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;  
in vec3 Normal;

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
    FragColor = col;
}