#version 300 es
layout(location = 0) in vec3 aPosition;   
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;


uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;
uniform float u_scale;   

out vec2 TexCoord;
out vec3 Normal;
out vec3 Tangent;
out vec3 FragPos;  


void main()                                
{
    FragPos = mat3(model) * aPosition;
    Normal =  mat3(model) * aNormal;  
    Tangent = aTangent;
    TexCoord = aTexCoord;
    gl_Position =  perspective * view * model * vec4(aPosition,u_scale);   

}