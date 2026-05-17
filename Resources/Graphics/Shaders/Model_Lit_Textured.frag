uniform sampler2D Appeal_Texture;
uniform bool Appeal_Has_Texture;
uniform vec3 Appeal_Light_Direction;
uniform vec3 Appeal_Light_Color;
uniform vec3 Appeal_Ambient_Color;

varying vec3 Appeal_Normal;
varying vec2 Appeal_UV;
varying vec3 Appeal_Position;

void main()
{
    vec4 Material_Color = gl_Color;
    if(Appeal_Has_Texture)
    {
        Material_Color *= texture2D(Appeal_Texture, Appeal_UV);
    }

    vec3 Normal = normalize(Appeal_Normal);
    vec3 Light = normalize(Appeal_Light_Direction);
    float Diffuse = max(dot(Normal, Light), 0.0);
    vec3 Lit_Color = Material_Color.rgb * (Appeal_Ambient_Color + Appeal_Light_Color * Diffuse);
    gl_FragColor = vec4(Lit_Color, Material_Color.a);
}
