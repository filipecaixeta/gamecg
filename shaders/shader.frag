#version 120

struct materialT
{
    vec4 texture_difuse0;
    vec4 texture_specular0;
    vec4 color_diffuse;
    vec4 color_specular;
    vec4 color_ambient;
    vec4 color_emissive;
    vec4 color_transparent;
    float opacity;
    float shininess;
};

struct lightSourceT
{   
   vec4 ambient;              // Aclarri   
   vec4 diffuse;              // Dcli   
   vec4 specular;             // Scli   
   vec4 position;             // Ppli   
}; 

uniform materialT material;
uniform lightSourceT lightS;
uniform sampler2D texUnit;
 
varying vec3 Normal;
varying vec2 TexCoord;
varying vec3 Color;
varying vec3 VPos;

void main()
{
    // vec4 color=vec4(Color,1.0);
    // vec4 amb=vec4(0.2f, 0.2f, 0.8f, 1.0f);
    // float intensity;
    // vec3 lightDir;
    // vec3 n;
 
    // lightDir = normalize(vec3(1.0,1.0,1.0));
    // n = normalize(Normal);
    // intensity = max(dot(lightDir,n),0.0);
 
    // // color = texture2D(texUnit, TexCoord);
    // gl_FragColor = vec4(material.color_specular,1.0f);
    // // amb = color;
    // // gl_FragColor = (color * intensity) + amb;

    // // gl_FragColor = intensity + amb;

    vec3 L = normalize(lightS.position.xyz-VPos);   
    vec3 E = normalize(-VPos); // we are in Eye Coordinates, so EyePos is (0,0,0)  
    vec3 R = normalize(-reflect(L,Normal));  

    //calculate Ambient Term:  
    vec4 Iamb = lightS.ambient;    

    //calculate Diffuse Term:  
    vec4 Idiff = lightS.diffuse * max(dot(Normal,L), 0.0);
    Idiff = clamp(Idiff, 0.0, 1.0);     

    // calculate Specular Term:
    vec4 Ispec = lightS.specular*pow(max(dot(R,E),0.0),material.shininess);
    Ispec = clamp(Ispec, 0.0, 1.0); 

    vec4 sceneColor=vec4(0.1,0.1,0.1,1);

    // write Total Color:  
    vec4 c=material.color_ambient+Iamb+Idiff+Ispec;
    gl_FragColor = clamp(material.color_diffuse*c, 0.0, 1.0);   

}