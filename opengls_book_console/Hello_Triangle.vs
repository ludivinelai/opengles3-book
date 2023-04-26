#version 300 es                            
layout(location = 0) in vec3 a_position;   
layout(location = 1) in vec4 a_color;      
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

uniform float u_scale;                      
out vec4 v_color;               
out vec2 TexCoord;
void main()                                
{                                          
    v_color = a_color;   
    TexCoord = aTexCoord;
    gl_Position =  perspective * view * model * vec4(a_position,u_scale);              
}