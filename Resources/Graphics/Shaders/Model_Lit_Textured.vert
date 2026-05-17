varying vec3 Appeal_Normal;
varying vec2 Appeal_UV;
varying vec3 Appeal_Position;

void main()
{
    Appeal_Normal = normalize(gl_NormalMatrix * gl_Normal);
    Appeal_UV = gl_MultiTexCoord0.xy;
    vec4 View_Position = gl_ModelViewMatrix * gl_Vertex;
    Appeal_Position = View_Position.xyz;
    gl_Position = gl_ProjectionMatrix * View_Position;
}
