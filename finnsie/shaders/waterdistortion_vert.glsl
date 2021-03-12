#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

// water
uniform float time;
uniform float waveLength;

#define PI 3.1415926538

void main()
{
    // =========================
    // MORE UNIFORMS
    float amplitude = 2.0;
    float speed = 5.0;
    // =========================

    // Vertex Position
    vec3 p = vec3(model * vec4(aPos, 1.0));
    float k = 2 * PI / waveLength;
    float f = k * (p.x - speed * time);
    p.y += amplitude * sin(k * (p.x - speed * time));
    vs_out.FragPos = p;
    

    vs_out.TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(vec3(1, k * amplitude * cos(f), 0)) * normalize(normalMatrix * aTangent);
    vec3 N = vec3(-T.y, T.x, 0) * normalize(normalMatrix * aNormal);
    //vec3 T = normalize(normalMatrix * aTangent);
    //vec3 N = normalize(normalMatrix * aNormal);
    
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPos  = TBN * lightPos;
    vs_out.TangentViewPos   = TBN * viewPos;
    vs_out.TangentFragPos   = TBN * vs_out.FragPos;

    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0); 
 
    // NOTE(ck): This is for waves i was multiplying the model again when
    // it was already getting added to vs_out.FragPos.
    //gl_Position = projection * view * model * vec4(vs_out.FragPos, 1.0); 
    
}