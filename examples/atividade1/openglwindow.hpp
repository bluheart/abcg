#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include "abcg.hpp"
#include "snowflake.hpp"

class OpenGLWindow : public abcg::OpenGLWindow
{
 protected:
  void handleEvent(SDL_Event& event) override;
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint program{};
  int viewportWidth{};
  int viewportHeight{};
  Snowflake snow;

  void update();
};
#endif