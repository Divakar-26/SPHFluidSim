
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
  GLuint ID;
  Shader(const char *vertexPath, const char *fragmentPath);

  void use() const;
  void destroy() const;

  void setFloat(const std::string &name, float value) const;
  void setVec2(const std::string &name, const glm::vec2 &value) const;
  void setScale(const std::string &name,  float value) const;
  void setMat4(const std::string &name, const glm::mat4 &value) const;

private:
  std::string readFile(const char *filePath);
  void checkCompileErrors(GLuint shader, std::string type);
};

#endif // !SHADER_H
