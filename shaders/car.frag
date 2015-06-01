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
uniform mat4 View;

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
        color_diffuse=a*color_diffuse+(1.0-a)*material.color_diffuse;
    }
    else
    {
        color_diffuse=material.color_diffuse;
    }

    vec3 L = normalize(lightS.position.xyz-VPos+vec3(0.0,2,0.0))*100;   
    vec3 E = normalize(eyePos-VPos);
    vec3 R = normalize(-reflect(L,Normal));  

    float dist = length(L);

    //calculate Ambient Term:  
    vec4 Iamb = 1.5*lightS.ambient*color_diffuse;    

    //calculate Diffuse Term:  
    vec4 Idiff = lightS.diffuse*max(dot(Normal,L), 0.0)*color_diffuse;
    Idiff = clamp(Idiff, 0.0, 1.0);

    // calculate Specular Term:
    vec4 Ispec = lightS.specular*pow(max(dot(R,E),0.0),0.3*material.shininess);
    Ispec = clamp(Ispec, 0.0, 1.0); 

    // write Total Color:
    vec4 color = material.color_emissive*0.5+Iamb+(material.color_ambient + Idiff + Ispec);
    color.rgb = clamp(color.rgb,0.0,1.0);
    color.a = material.color_transparent.a;
    return color;
}

vec4 light1()
{
    vec4 lightPosition=vec4(0,0,0,1);
    vec4 Ia=lightS.ambient;
    vec4 Id=lightS.diffuse;
    vec4 Is=lightS.specular;  

    vec4 Kd;
    if (useTexture==1)
    {
        Kd=texture2D(material.texture_difuse0, TexCoord);
        float a=Kd.a;
        if (a<1.0)
            discard;
        Kd=a*Kd+(1.0-a)*material.color_diffuse;
    }
    else
    {
        Kd=material.color_diffuse;
    }

    vec4 Ka=Kd*0.7;
    vec4 Ks=material.color_specular;
    float shininess=material.shininess;

    vec4 lPos = transpose(View)*lightS.position; // light position in the eye-space coordinate

    vec3 n = normalize(Normal); // norm interpolated via rasterizer should be normalized again here
    vec3 p = VPos.xyz;         // 3D position of this fragment
    vec3 l = normalize(lPos.xyz-(lPos.a==0.0?vec3(0):p));   // lPos.a==0 means directional light
    vec3 v = normalize(-p);                                 // eye-ecPos = vec3(0)-ecPos
    vec3 h = normalize(l+v);    // the halfway vector

    vec4 Ira = Ka*Ia;                                   // ambient reflection
    vec4 Ird = max(Kd*dot(l,n)*Id,0.0);                 // diffuse reflection
    vec4 Irs = max(Ks*pow(dot(h,n),shininess)*Is,0.0);  // specular reflection

    vec4 color = vec4(Ira*2 + Ird*5.5 + Irs*4.5);
    color.a = material.color_transparent.a;
    return color;
}

void main()
{
    gl_FragColor = light1();
}