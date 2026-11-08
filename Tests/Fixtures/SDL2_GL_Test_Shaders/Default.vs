#version 140

in vec3 LVertexPos3D;
in vec2 texCoord;

out vec2 v_TexCoord;

uniform mat4 u_MVP;

void main() 
{
	gl_Position = u_MVP * vec4(LVertexPos3D, 1.0f);
	v_TexCoord = texCoord; 
}