#include "openglwindow.hpp"
#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event)
{
  if (event.type == SDL_KEYDOWN)
  {
    if (event.key.keysym.sym == SDLK_SPACE)
      snow.input.set(static_cast<size_t>(Input::Space));
    if (event.key.keysym.sym == SDLK_UP)
      snow.input.set(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN)
      snow.input.set(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT)
      snow.input.set(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT)
      snow.input.set(static_cast<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP)
  {
    if (event.key.keysym.sym == SDLK_SPACE)
      snow.input.reset(static_cast<size_t>(Input::Space));
    if (event.key.keysym.sym == SDLK_UP)
      snow.input.reset(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN)
      snow.input.reset(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT)
      snow.input.reset(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT)
      snow.input.reset(static_cast<size_t>(Input::Right));
  }
}

void OpenGLWindow::initializeGL()
{
  program = createProgramFromFile(getAssetsPath() + "object.vert",
                                  getAssetsPath() + "object.frag");
  glClearColor(205/255, 0.7f, 0.7f, 0.5f);
  snow.initializeGL(program);
}


void OpenGLWindow::update()
{
  float deltaTime{static_cast<float>(getDeltaTime())};
  snow.update(deltaTime);
}

void OpenGLWindow::paintGL()
{
  update();
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, viewportWidth, viewportHeight);
  snow.paintGL();
}

void OpenGLWindow::resizeGL(int width, int height)
{
  viewportWidth = width;
  viewportHeight = height;
  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL()
{
  glDeleteProgram(program);
  snow.terminateGL();
}
