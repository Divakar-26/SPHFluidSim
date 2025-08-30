#include "shader.h"
#include <fstream>
#include <ostream>
#include <sstream>
#include <iostream>


std::string Shader::readFile(const char * filePath){
  std::ifstream file(filePath);
    if(!file.is_open()){
      std::cerr << "ERROR: Could not open shader file: "<<filePath<<std::endl;
      return "";
    }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

Shader::Shader(const char * vertexPath, const char * fragmentPath){
  std::string vertexCode = readFile(vertexPath);
  std::string fragmentCode = readFile(fragmentPath);

  const char * vCode = vertexCode.c_str();
  const char * fCode = fragmentCode.c_str();

  GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex,1,&vCode, nullptr);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");

  GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment,1,&fCode, nullptr);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");

  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  checkCompileErrors(ID, "PROGRAM");

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::setFloat(const std::string &name, float value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cerr << "Warning: Uniform '" << name << "' not found!" << std::endl;
        return;
    }
    glUniform1f(loc, value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cerr << "Warning: Uniform '" << name << "' not found!" << std::endl;
        return;
    }
    glUniform2f(loc, value.x, value.y);
}

void Shader::setScale(const std::string &name, float value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cerr << "Warning: Uniform '" << name << "' not found!" << std::endl;
        return;
    }
    glUniform1f(loc, value);
}

void Shader::use() const{
  glUseProgram(ID);
}

void Shader::destroy() const{
  glDeleteProgram(ID);
}


void Shader::checkCompileErrors(GLuint shader, std::string type){
  GLint success;
  GLchar infoLog[1024];

  if(type != "PROGRAM"){
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success){
      glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
      std::cerr<<type<<" SHADER COMPILATION FAILED:\n"<<infoLog<<std::endl;
    }
  }
  else{ // This should be outside the if statement!
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if(!success){
      glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
      std::cerr<<"SHADER PROGRAM LINKING FAILED:\n"<<infoLog<<std::endl;
    }
  }
}


