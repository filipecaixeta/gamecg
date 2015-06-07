#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <shader.h>
using namespace glm;
using namespace std;

class Hud
{
public:
	Hud(string imageName);
	~Hud();
	void draw(Shader *shader);
	TextureFromFile(string filename);
	void setSpeed(float _speed);
	GLuint vertexArrayObjectPlane;
	GLuint vertexBufferObjectPlane;
	GLuint vertexArrayObjectLine;
	GLuint vertexBufferObjectLine;
	vector<vec3> verticesPlane;
	vector<vec3> verticesLine;
	GLint textureId;
	vec3 color;
	float aspect;
	float speed;
};