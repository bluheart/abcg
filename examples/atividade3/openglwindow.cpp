#include "openglwindow.hpp"
#include <glm/gtx/fast_trigonometry.hpp>
#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>

void OpenGLWindow::handleEvent(SDL_Event& event)
{
  if (event.type == SDL_MOUSEWHEEL)
  {
    zoom += (event.wheel.y > 0 ? 1.0f : -1.0f) / 5.0f;
    zoom = glm::clamp(zoom, -1.5f, 1.0f);
  }
  if (event.type == SDL_KEYDOWN)
  {
    if (event.key.keysym.sym == SDLK_LEFT)
       ang = 100;
    if (event.key.keysym.sym == SDLK_RIGHT)
       ang = -100;
  }
  if (event.type == SDL_KEYUP)
  {
    if (event.key.keysym.sym == SDLK_LEFT)
      ang = 100;
    if (event.key.keysym.sym == SDLK_RIGHT)
      ang = -100;
  }
}

void OpenGLWindow::initializeGL()
{
  glClearColor(0.82, 0.80, 0.79, 1);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  program = createProgramFromFile(getAssetsPath() + "garfield.vert",
                                  getAssetsPath() + "garfield.frag");

  loadModel(getAssetsPath() + "james-garfield-bronze-sculpture-150k.obj");
}

void OpenGLWindow::loadModel(std::string_view path)
{
  model.loadDiffuseTexture(getAssetsPath() + "james-garfield-bronze-sculpture-150k-4096-diffuse.jpg");
  model.loadFromFile(path);
  model.setupVAO(program);
  trianglesToDraw = model.getNumTriangles();

  Ka = model.getKa();
  Kd = model.getKd();
  Ks = model.getKs();
  shininess = model.getShininess();
}

void OpenGLWindow::paintGL()
{
  update();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, viewportWidth, viewportHeight);

  auto aspect{static_cast<float>(viewportWidth) /
              static_cast<float>(viewportHeight)};
  projMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);
  glUseProgram(program);

  GLint viewMatrixUni{glGetUniformLocation(program, "viewMatrix")};
  GLint projMatrixUni{glGetUniformLocation(program, "projMatrix")};
  GLint modelMatrixUni{glGetUniformLocation(program, "modelMatrix")};
  GLint normalMatrixUni{glGetUniformLocation(program, "normalMatrix")};
  GLint shininessUni{glGetUniformLocation(program, "shininess")};
  GLint IaUni{glGetUniformLocation(program, "Ia")};
  GLint IdUni{glGetUniformLocation(program, "Id")};
  GLint IsUni{glGetUniformLocation(program, "Is")};
  GLint KaUni{glGetUniformLocation(program, "Ka")};
  GLint KdUni{glGetUniformLocation(program, "Kd")};
  GLint KsUni{glGetUniformLocation(program, "Ks")};
  GLint diffuseTexUni{glGetUniformLocation(program, "diffuseTex")};

  glUniformMatrix4fv(viewMatrixUni, 1, GL_FALSE, &viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixUni, 1, GL_FALSE, &projMatrix[0][0]);
  glUniform1i(diffuseTexUni, 0);

  glUniform4fv(IaUni, 1, &Ia.x);  
  glUniform4fv(IdUni, 1, &Id.x);
  glUniform4fv(IsUni, 1, &Is.x);

  glUniformMatrix4fv(modelMatrixUni, 1, GL_FALSE, &modelMatrix[0][0]);

  auto modelViewMatrix{glm::mat3(viewMatrix * modelMatrix)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  glUniformMatrix3fv(normalMatrixUni, 1, GL_FALSE, &normalMatrix[0][0]);

  glUniform1f(shininessUni, shininess);
  glUniform4fv(KaUni, 1, &Ka.x);
  glUniform4fv(KdUni, 1, &Kd.x);
  glUniform4fv(KsUni, 1, &Ks.x);

  model.render(trianglesToDraw);
  glUseProgram(0);
}

void OpenGLWindow::resizeGL(int width, int height)
{
  viewportWidth = width;
  viewportHeight = height;
}

void OpenGLWindow::terminateGL()
{
    glDeleteProgram(program);
}


void OpenGLWindow::update()
{
  glm::vec3 axis{0.0f, 1.0f, 0.0f};
  float delta{static_cast<float>(getDeltaTime())};
  angle = -glm::wrapAngle(angle + glm::radians(ang) * delta);
  modelMatrix =  glm::rotate(modelMatrix, angle, axis);
  angle = 0;
  viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f + zoom),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}