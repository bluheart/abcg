#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <string_view>

#include "abcg.hpp"
#include "model.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  int viewportWidth{};
  int viewportHeight{};

  Model model;
  int trianglesToDraw{};

  float zoom{};

  float ang{}; //controle do giro
  float angle{};

  glm::mat4 modelMatrix{1.0f};
  glm::mat4 viewMatrix{1.0f};
  glm::mat4 projMatrix{1.0f};

  GLuint program;
  
  glm::vec4 Ia{1.0f};
  glm::vec4 Id{1.0f};
  glm::vec4 Is{1.0f};
  glm::vec4 Ka;
  glm::vec4 Kd;
  glm::vec4 Ks;
  float shininess{};

  void loadModel(std::string_view path);
  void update();
};
#endif