#version 330 core
layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _color;

out vec2 TexCoord;
out vec4 colorIntensity;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//On va de la face vers la source lumineuse
//const vec3 lightDirection = vec3(0.0, 0.0, 1.0);

//const vec4 color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
void main()
{
	//pseudo normale exprimee dans le repere LOCAL
	//vec3 pseudoNormal = normalize(_position.xyz);
	//vec3 worldNormal = mat3(model) * pseudoNormal;

	//Vertex lighting
	//float intensity = dot(worldNormal, lightDirection);
	//colorIntensity = color * intensity;
	colorIntensity = vec4(_color, 1.0f);
	//normal = worldNormal;

    //gl_Position = projection * view * model * vec4(position, 1.0f);


    gl_Position = vec4(_position, 1.0f);
    gl_PointSize = 6;
    //TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}