#ifndef SNOWFLAKE_HPP_
#define SNOWFLAKE_HPP_

#include "abcg.hpp"
#include <bitset>
#include <random>

class OpenGLWindow;
enum class Input { Right, Left, Down, Up, Space};
class Snowflake {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  void update(float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint programSnow{};
  GLint colorUni{};
  GLint scaleUni{};
  GLint rotationUni{};

  GLuint VAO{};
  GLuint VBO{};
  GLuint EBO{};
  glm::vec4 color{0.0f, 0.0f, 0.0f, 1.0f};
  std::bitset<5> input;
  std::default_random_engine randomEngine;
  float rotation{0.0f};
  float scale{0.5f};
};
#endif