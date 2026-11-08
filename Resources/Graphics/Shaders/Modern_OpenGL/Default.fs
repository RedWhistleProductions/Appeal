#version 330 core
in vec2 v_TexCoord;
in vec3 v_Normal;
in vec4 v_Color;
uniform sampler2D u_Texture;
uniform bool u_Textured;
uniform bool u_Lit;
out vec4 LFragment;
void main() {
    vec4 color=v_Color;
    if(u_Textured) color*=texture(u_Texture,v_TexCoord);
    if(u_Lit) {
        vec3 normal=normalize(v_Normal);
        float diffuse=max(dot(normal,normalize(vec3(2.5,4.0,4.0))),0.0);
        color.rgb*=vec3(0.28,0.28,0.30)+vec3(0.86,0.86,0.78)*diffuse;
    }
    LFragment=color;
}
