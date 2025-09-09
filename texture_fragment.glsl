#version 330 core
in vec3 ourColor;
in vec2 texCoord;

out vec4 FragColor;

// texture sampler
uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, texCoord) * vec4(ourColor, 1.0);
}