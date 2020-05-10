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

void main()
{   
	// Remove white pixels on texture (using diffuse for now)
	vec4 texColor = texture(texture_diffuse1, fs_in.TexCoords);
	if (texColor.a < 0.1)
	{
		discard;
	}


	// calculate normals
    vec3 normal = texture(texture_normal2, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0); // this normal is in tangent space

    // diffuse color 
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    
    // ambient
    vec3 ambient = 0.1 * color;
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