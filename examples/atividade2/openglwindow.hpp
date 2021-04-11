#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include "abcg.hpp"

//Do exemplo
struct Vertex {
  glm::vec3 position;
  bool operator==(const Vertex& other) const {
    return position == other.position;
  }
};

class OpenGLWindow : public abcg::OpenGLWindow
{
 protected:
  void handleEvent(SDL_Event &event) override; // controle do giro
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint program{};
  GLuint VAO{};
  GLuint VBO{};
  GLuint EBO{};

  float ang{}; //controle do giro
  float angle{};

  int verticesToDraw{};
  int viewportWidth{};
  int viewportHeight{};

  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  void loadModelFromFile(std::string_view path);
  void standardize();
};
#endif