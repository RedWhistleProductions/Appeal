#version 330 core
in vec3 LVertexPos3D;
in vec2 texCoord;
in vec3 a_Normal;
in vec4 a_Color;
uniform mat4 u_MVP;
uniform mat4 u_ModelView;
out vec2 v_TexCoord;
out vec3 v_Normal;
out vec4 v_Color;
void main() {
    gl_Position=u_MVP*vec4(LVertexPos3D,1.0);
    v_TexCoord=texCoord;
    v_Normal=transpose(inverse(mat3(u_ModelView)))*a_Normal;
    v_Color=a_Color;
}
