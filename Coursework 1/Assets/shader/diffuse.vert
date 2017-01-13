#version 430

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 textureCoords;
layout (location = 2) in vec3 vertexNormal;

out vec3 vertNormal;
out vec3 vertPos;
out vec2 texCoords;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
	texCoords = textureCoords;
    vertPos = vec3(M * vec4(vertexPosition, 1.0f));
    vertNormal = mat3(transpose(inverse(M))) * vertexNormal;  
	gl_Position = P * V *  M * vec4(vertexPosition, 1.0f);
} 