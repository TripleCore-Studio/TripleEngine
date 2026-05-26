#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec3 a_Color;
layout(location = 5) in vec2 a_UV;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out vec2 v_UV;
out vec3 v_Color;
out vec3 v_Normal;
out vec3 v_FragPos;

void main()
{
    v_UV = a_UV;
    v_Color = a_Color;
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    
    gl_Position = u_MVP * vec4(a_Position, 1.0);
}