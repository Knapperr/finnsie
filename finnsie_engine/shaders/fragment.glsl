#version 420
out vec4 FragColor;
in vec3 color;
uniform vec3 triColor;
uniform float newRed;
uniform float newGreen;
uniform float newBlue;
void main()
{
	FragColor = vec4(newRed, newGreen, newBlue, 1.0);
}