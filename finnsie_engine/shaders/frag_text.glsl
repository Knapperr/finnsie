#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	vec4 iceBlueColor = vec4(0.1, 0.3, 0.4, 0.0);
	//vec4 iceBlueColor = vec4(0.0, 0.0, 0.0, 0.0);
	FragColor = mix(texture(texture1, TexCoord), iceBlueColor, 0.6);
	//FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}