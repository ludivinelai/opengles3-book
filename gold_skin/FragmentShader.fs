#version 300 es
precision mediump float;   
out vec4 o_fragColor;

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;  
in vec3 Tangent;


uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

vec3 CalcBumpedNormal()
{
    vec3 normal = normalize(Normal);
    vec3 tangent = normalize(Tangent);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 Bitangent = cross(tangent, normal);
    vec3 BumpMapNormal = texture(normalMap, TexCoord).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(tangent, Bitangent, normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

//uniform vec3 lightPos; 
//uniform vec3 viewPos; 

void main()                
{    //ambientlight           
    float Ka = 0.2;
    vec3 lightColor = vec3(1.6,1.6,1.6 );
    vec3 lightPos = vec3(0.0,0.0,1.0);

    vec3 viewPos = vec3(0.0,2.0,3.0);
    vec3 ambient = Ka * lightColor;
    vec3 objectColor = texture(diffuseMap, TexCoord).rgb;
    float Specular = texture(specularMap, TexCoord).r;

    //diffuse light
    float Kd = 0.98;
   // vec3 norm = normalize(Normal);
   vec3 norm =CalcBumpedNormal();
    float distance = length (lightPos - FragPos);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.1);
    float Fattenuation =1.0 /(1.0 + 0.22*distance+(0.2 *distance*distance));
    vec3 diffuse = Kd * diff * Fattenuation * lightColor;
    //specular light
    float Ks = 2.2;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spot_factor =max(dot(viewDir, reflectDir), 0.0);
    float spec = pow(spot_factor, 16.0);
    vec3 specular = Ks * spec * Fattenuation * lightColor*Specular;  
  //  vec3 specular = Ks * spec * Fattenuation * lightColor; 
  //  vec3 result = ambient + diffuse +specular;
    vec3 result = ambient + diffuse ;

    o_fragColor = vec4(result,1.0) * vec4(objectColor,1.0)+vec4(specular,1.0); 
  // o_fragColor = vec4(result,1.0) * vec4(objectColor,1.0);
}