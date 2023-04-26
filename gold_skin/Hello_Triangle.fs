#version 300 es            
precision mediump float;   
in vec4 v_color;           
out vec4 o_fragColor;      
in vec2 TexCoord;
uniform sampler2D texture1;
void main()                
{                          
    o_fragColor =  mix(texture(texture1, TexCoord), v_color, 0.2); 
   
}