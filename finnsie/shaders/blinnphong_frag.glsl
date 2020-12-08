#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float time;

vec2 moveTexture(vec2 uv, vec2 newVector, float time)
{
    return uv - newVector * time;
}

void main()
{
    //vec2 uv = fs_in.TexCoords;
    //vec2 newVector = texture2D(texture_diffuse1, fs_in.TexCoords).rg * 2 - 1;
    //vec2 newUV = moveTexture(uv, newVector, time);

    vec2 uv = fs_in.TexCoords + time;

    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;

    //vec4 red = vec4(1.0, 0.6, 0.2, 1.0);
    // ambient
    //vec3 ambient = (0.05 * color) * red.rgb;
    vec3 ambient = 0.03 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    //vec3 reflectDir = reflect(-lightDir, normal); // TODO(CK): this does nothing?
    float spec = 0.0;
    
    // blinn lighting
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    

    vec3 specular = vec3(0.3) * spec; // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}