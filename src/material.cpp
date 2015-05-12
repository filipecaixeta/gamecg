#include <material.h>

Material::Material(aiColor4D _color_diffuse, aiColor4D _color_specular,
			 aiColor4D _color_ambient, aiColor4D _color_emissive,
			 aiColor4D _color_transparent, float _opacity, float _shininess)
{
	color_diffuse=vec4(_color_diffuse.r,_color_diffuse.g,_color_diffuse.b,_color_diffuse.a);
	color_specular=vec4(_color_specular.r,_color_specular.g,_color_specular.b,_color_specular.a);
	color_ambient=vec4(_color_ambient.r,_color_ambient.g,_color_ambient.b,_color_ambient.a);
	color_emissive=vec4(_color_emissive.r,_color_emissive.g,_color_emissive.b,_color_emissive.a);
	color_transparent=vec4(_color_transparent.r,_color_transparent.g,_color_transparent.b,_color_transparent.a);
	opacity=_opacity;
	shininess=_shininess;
}
Material::Material(aiMaterial* mat)
{
	aiColor4D _color_diffuse(0.f,0.f,0.f,0.0f);
	aiColor4D _color_specular(0.f,0.f,0.f,0.0f);
	aiColor4D _color_ambient(0.f,0.f,0.f,0.0f);
	aiColor4D _color_emissive(0.f,0.f,0.f,0.0f);
	aiColor4D _color_transparent(0.f,0.f,0.f,0.0f);
	float _opacity=1.0f;
	float _shininess=0.0f;
    aiGetMaterialColor(mat,AI_MATKEY_COLOR_DIFFUSE,&_color_diffuse);
    aiGetMaterialColor(mat,AI_MATKEY_COLOR_SPECULAR,&_color_specular);
    aiGetMaterialColor(mat,AI_MATKEY_COLOR_AMBIENT,&_color_ambient);
    aiGetMaterialColor(mat,AI_MATKEY_COLOR_EMISSIVE,&_color_emissive);
    aiGetMaterialColor(mat,AI_MATKEY_COLOR_TRANSPARENT,&_color_transparent);
    aiGetMaterialFloat(mat,AI_MATKEY_OPACITY,&_opacity);
    aiGetMaterialFloat(mat,AI_MATKEY_SHININESS,&_shininess);

	color_diffuse=vec4(_color_diffuse.r,_color_diffuse.g,_color_diffuse.b,_color_diffuse.a);
	color_specular=vec4(_color_specular.r,_color_specular.g,_color_specular.b,_color_specular.a);
	color_ambient=vec4(_color_ambient.r,_color_ambient.g,_color_ambient.b,_color_ambient.a);
	color_emissive=vec4(_color_emissive.r,_color_emissive.g,_color_emissive.b,_color_emissive.a);
	color_transparent=vec4(_color_transparent.r,_color_transparent.g,_color_transparent.b,_color_transparent.a);
	opacity=_opacity;
	shininess=_shininess;
}
void Material::print()
{
	std::cout << "color_diffuse\t\t= " << color_diffuse.r << "\t" << color_diffuse.g << "\t" << color_diffuse.b << "\t" << color_diffuse.a << std::endl;
	std::cout << "color_specular\t\t= " << color_specular.r << "\t" << color_specular.g << "\t" << color_specular.b << "\t" << color_specular.a << std::endl;
	std::cout << "color_ambient\t\t= " << color_ambient.r << "\t" << color_ambient.g << "\t" << color_ambient.b << "\t" << color_ambient.a << std::endl;
	std::cout << "color_emissive\t\t= " << color_emissive.r << "\t" << color_emissive.g << "\t" << color_emissive.b << "\t" << color_emissive.a << std::endl;
	std::cout << "color_transparent\t= " << color_transparent.r << "\t" << color_transparent.g << "\t" << color_transparent.b << "\t" << color_transparent.a << std::endl;
	std::cout << std::endl;	
}
void Material::loadToShader(GLuint program)
{
	glUniform4fv(glGetUniformLocation(program, "material.color_diffuse"), 1, &color_diffuse[0]);
	glUniform4fv(glGetUniformLocation(program, "material.color_specular"), 1, &color_specular[0]);
	glUniform4fv(glGetUniformLocation(program, "material.color_ambient"), 1, &color_ambient[0]);
	glUniform4fv(glGetUniformLocation(program, "material.color_emissive"), 1, &color_emissive[0]);
	glUniform4fv(glGetUniformLocation(program, "material.color_transparent"), 1, &color_transparent[0]);
	glUniform1f(glGetUniformLocation(program, "material.opacity"),opacity);
	glUniform1f(glGetUniformLocation(program, "material.shininess"),shininess);
}
Material::~Material()
{

}