#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;  
in vec3 Normal;  

uniform sampler2D texture_diffuse1;

void main()
{   
	// Remove white pixels on texture
	vec4 texColor = texture(texture_diffuse1, TexCoords);
	if (texColor.a < 0.1)
	{
		discard;
	}

	/*
	An easy way to visualize your normals is to draw them using lines, 
	using {position, position + normal} as the start & end points.
	*/
	
	
	FragColor = texColor;
    //FragColor = texture(texture_diffuse1, TexCoords);
}