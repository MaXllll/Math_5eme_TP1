#version 330 core
in vec2 TexCoord;
in vec4 colorIntensity;
in vec3 normal;

out vec4 color;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

const vec3 lightDirection = vec3(0.0, 0.0, 1.0);

void main()
{
    //color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2);

	//Vertex lighting
	//color = colorIntensity;
	//float intensity = max(dot(normalize(normal), lightDirection), 0.0);
	color = colorIntensity;// * intensity;
}