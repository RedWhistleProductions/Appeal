Set_GL_Version 3 3
Init_Shader GL_VERTEX_SHADER "Vertex Shader" "Default.vs"
Init_Shader GL_FRAGMENT_SHADER "Fragment Shader" "Default.fs"
Set_Attribute_Data LVertexPos3D 0 3 "GLfloat"
Set_Attribute_Data texCoord 1 2 "GLfloat"
Set_Attribute_Data a_Normal 2 3 "GLfloat"
Set_Attribute_Data a_Color 3 4 "GLfloat"
Build
Get_Uniform_Location u_MVP
Done
