#include "shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <cstdio>

std::string readSourceFile(const std::string fileName)
{
    std::ifstream sc;
    try
    {
        sc.open(fileName.c_str());
    }
    catch (...)
    {
        std::cerr << "ERROR: Loading source file "<<fileName<<std::endl;
    }
    std::stringstream buffer;
    buffer << sc.rdbuf();
    std::string str = buffer.str();
    return str;
}

Shader::Shader(const std::string vertexShaderFile, const std::string fragmentShaderFile)
{
    this->vertexShaderFile=vertexShaderFile;
    this->fragmentShaderFile=fragmentShaderFile;
    struct stat attr;
    stat(vertexShaderFile.c_str(), &attr);
    vsmodified=attr.st_mtime;
    stat(fragmentShaderFile.c_str(), &attr);
    fsmodified=attr.st_mtime;

    createProgram();
}

void Shader::createProgram()
{
    // Create and compile Vertex Shader
    GLint compile_ok = GL_FALSE;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertexShaderSource = readSourceFile(vertexShaderFile);
    const char *vertexShaderSourcePointer = vertexShaderSource.c_str();

    glShaderSource(vertexShader, 1, &vertexShaderSourcePointer, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compile_ok);
    if (!compile_ok)
    {
        std::cerr << "Error Vertex shader compilation"<<std::endl; 
    }
    GLint infologLength = 0;
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH,&infologLength);
    if (infologLength > 0)
    {
        char *infoLog = new char[infologLength];
        int charsWritten  = 0;
        glGetShaderInfoLog(vertexShader, infologLength, &charsWritten, infoLog);
        if (*infoLog)
            std::cerr << "Vertex shader compilation: " <<infoLog << std::endl;
        if (!compile_ok)
            return;
    }

    // Create and compile Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragmentShaderSource = readSourceFile(fragmentShaderFile);
    const char *fragmentShaderSourcePointer = fragmentShaderSource.c_str();

    glShaderSource(fragmentShader, 1, &fragmentShaderSourcePointer, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compile_ok);
    if (!compile_ok) 
    {
        std::cerr << "Error: Fragment shader compilation"<<std::endl; 
        return;
    }
    infologLength = 0;
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH,&infologLength);
    if (infologLength > 0)
    {
        char *infoLog = new char[infologLength];
        int charsWritten  = 0;
        glGetShaderInfoLog(fragmentShader, infologLength, &charsWritten, infoLog);
        if (*infoLog)
            std::cerr << "Fragment shader compilation: " <<infoLog << std::endl;
        if (!compile_ok)
            return;
    }

    // Create program and link
    GLint link_ok = GL_FALSE;
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) 
    {
        std::cerr << "Error: Program link"<<std::endl; 
        return;
    }
    infologLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH,&infologLength);
    if (infologLength > 0)
    {
        char *infoLog = (char *)malloc(infologLength);
        int charsWritten  = 0;
        glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
        if (*infoLog)
            std::cerr << "Program link : " <<infoLog << std::endl;
        free(infoLog);
    }
}

void Shader::use(bool b)
{
    if (b==true)
        glUseProgram(program);
    else
        glUseProgram(0);
}

GLuint Shader::getProgram()
{
    return program;
}

void Shader::reloadShader()
{
    struct stat attr1;
    struct stat attr2;
    stat(vertexShaderFile.c_str(), &attr1);
    stat(fragmentShaderFile.c_str(), &attr2);
    if (vsmodified!=attr1.st_mtime || fsmodified!=attr2.st_mtime)
    {
        vsmodified=attr1.st_mtime;
        fsmodified=attr2.st_mtime;
        std::cout << "Reloading shaders" <<std::endl;
        glUseProgram(0);
        glDetachShader(program,vertexShader);
        glDetachShader(program,fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        createProgram();
    }   
}

Shader::~Shader()
{
    glDetachShader(program,vertexShader);
    glDetachShader(program,fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(program);
}