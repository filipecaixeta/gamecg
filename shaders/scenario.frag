#version 120

struct materialT
{
    sampler2D texture_difuse0;
    sampler2D texture_specular0;
    sampler2D texture_difuse1;
    sampler2D texture_specular1;
    sampler2D texture_difuse2;
    sampler2D texture_specular2;
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
   vec4 ambient;
   vec4 diffuse;
   vec4 specular;
   vec4 position;
}; 

uniform materialT material;
uniform lightSourceT lightS;
uniform vec3 eyePos;
uniform int useTexture;

varying vec3 Normal;
varying vec2 TexCoord;
varying vec3 Color;
varying vec3 VPos;

vec4 light0()
{
    // lightS.position.y=lightS.position.y;
    vec4 color_diffuse;
    if (useTexture==1)
    {
        color_diffuse=texture2D(material.texture_difuse0, TexCoord);
        float a=color_diffuse.a;
        if (a<1.0)
            discard;
        color_diffuse=a*color_diffuse+(1.0-a)*material.color_diffuse;
    }
    else
    {
        color_diffuse=material.color_diffuse;
    }

    vec3 L = normalize(lightS.position.xyz-VPos+vec3(0.0,2,0.0));   
    vec3 E = normalize(eyePos-VPos);
    vec3 R = normalize(-reflect(L,Normal));  

    float dist = length(L);

    //calculate Ambient Term:  
    vec4 Iamb = lightS.ambient*color_diffuse;    

    //calculate Diffuse Term:  
    vec4 Idiff = lightS.diffuse*2* max(dot(Normal,L), 0.0)*color_diffuse;
    Idiff = clamp(Idiff, 0.0, 1.0);

    // calculate Specular Term:
    vec4 Ispec = 0*lightS.specular*pow(max(dot(R,E),0.0),0.3*material.shininess);
    Ispec = clamp(Ispec, 0.0, 1.0); 

    // write Total Color:
    vec4 color = material.color_emissive*0.5+Iamb+(material.color_ambient + Idiff + Ispec);
    color.rgb = clamp(color.rgb,0.0,1.0);
    color.a = material.color_transparent.a;
    return color;
}

void main()
{
    gl_FragColor = light0();
}