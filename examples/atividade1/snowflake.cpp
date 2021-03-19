#include "snowflake.hpp"
#include <glm/gtx/fast_trigonometry.hpp>

void Snowflake::initializeGL(GLuint program)
{
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  randomEngine.seed(seed);
  programSnow = program;
  colorUni = glGetUniformLocation(programSnow, "color");
  rotationUni = glGetUniformLocation(programSnow, "rotation");
  scaleUni = glGetUniformLocation(programSnow, "scale");

  std::array<glm::vec2, 72> vertices
  {
      glm::vec2{0.0f, 15.0f}, glm::vec2{-1.0f, 14.0f}, //1, 2
      glm::vec2{-1.0f, 10.0f}, glm::vec2{-4.0f, 12.0f}, // 3, 4
      glm::vec2{-4.0f, 10.f}, glm::vec2{-01.0f, 8.0f}, // 5, 6
      glm::vec2{-1.0f, 5.0f}, glm::vec2{-4.0f, 7.0f},  // 7, 8
      glm::vec2{-4.0f, 3.0f}, glm::vec2{-6.0f, 4.0f},  // 9, 10
      glm::vec2{-6.0f, 8.0f}, glm::vec2{-7.0f, 9.0f},  // 11, 12
      glm::vec2{-8.0f, 8.0f}, glm::vec2{-8.0f, 5.0f},  // 13, 14
      glm::vec2{-11.0f, 7.0f}, glm::vec2{-13.0f, 7.0f}, //15, 16
      glm::vec2{-12.0f, 5.0f}, glm::vec2{-5.0f, 1.0f},  //17, 18
      glm::vec2{-7.0f, 0.0f}, //19

      glm::vec2{-5.0f, -1.0f}, glm::vec2{-12.0f, -5.0f}, //20, 21
      glm::vec2{-13.0f, -7.0f}, glm::vec2{-11.0f, -7.0f}, //22, 23
      glm::vec2{-8.0f, -5.f}, glm::vec2{-08.0f, -8.0f},  //24, 25
      glm::vec2{-7.0f, -9.0f}, glm::vec2{-6.0f, -8.0f},  //26, 27
      glm::vec2{-6.0f, -4.0f}, glm::vec2{-4.0f, -3.0f},  //28, 29
      glm::vec2{-4.0f, -7.0f}, glm::vec2{-1.0f, -5.0f},  //30, 31
      glm::vec2{-1.0f, -8.0f}, glm::vec2{-4.0f, -10.0f}, //32, 33
      glm::vec2{-4.0f, -12.f}, glm::vec2{-01.0f, -10.0f}, //34, 35
      glm::vec2{-1.0f, -14.0f}, glm::vec2{0.0f, -15.0f}, //36, 37


      glm::vec2{1.0f, -14.0f}, glm::vec2{1.0f, -10.0f}, //38, 39
      glm::vec2{4.0f, -12.0f}, glm::vec2{4.0f, -10.0f}, //40, 41
      glm::vec2{1.0f, -8.0f}, glm::vec2{1.0f, -05.0f},  //42, 43
      glm::vec2{4.0f, -7.0f}, glm::vec2{4.0f, -3.0f},   //44, 45
      glm::vec2{6.0f, -4.0f}, glm::vec2{6.0f, -8.0f},   //46, 47
      glm::vec2{7.0f, -9.0f}, glm::vec2{8.0f, -8.0f},   //48, 49

      glm::vec2{8.0f, -5.0f}, glm::vec2{11.0f, -7.0f},  //50, 51
      glm::vec2{13.0f, -7.0f}, glm::vec2{12.0f, -5.0f}, //52, 53
      glm::vec2{5.0f, -1.0f}, glm::vec2{7.0f, 0.0f}, //54, 55

      glm::vec2{5.0f, 1.0f}, glm::vec2{12.0f, 5.0f}, //56, 57
      glm::vec2{13.0f, 7.0f}, glm::vec2{11.0f, 7.0f}, //58, 59
      glm::vec2{8.0f, 5.0f}, glm::vec2{8.0f, 8.0f}, //60, 61
      glm::vec2{7.0f, 9.0f}, glm::vec2{6.0f, 8.0f}, //62, 63
      glm::vec2{6.0f, 4.f}, glm::vec2{4.0f, 3.0f}, //64, 65
      glm::vec2{4.0f, 7.0f}, glm::vec2{1.0f, 5.0f}, //66, 67
      glm::vec2{1.0f, 8.0f}, glm::vec2{4.0f, 10.0f}, //68, 69
      glm::vec2{4.0f, 12.0f}, glm::vec2{1.0f, 10.0f}, //70, 71
      glm::vec2{1.0f, 14.0f} //72
  };

  for (auto &vertice :vertices)
  {
    vertice /= glm::vec2{15.0f, 15.0f};
  }

  std::array indices
  {
    0, 1, 71,   //triang sup
    35, 36, 37, //triang inf
    1, 35, 37,
    1, 37, 71,   //haste vertical
    2, 3, 4,
    2, 4, 5,      //chifre sup esq
    7, 29, 54,
    18, 43, 65,   //estrela do centro

    10, 11, 12,
    9, 10, 12,
    9, 13, 12,  //chifre vertical sup esq

    14, 15, 16,
    8, 14, 16,
    8, 16, 17,  //haste diagonal sup esq                     
                     
    20, 21, 22, //trian inf esq
    19, 20, 22,  
    19, 22, 28,  //haste inf esq
                     
    24, 25, 26,  //cansei de escrever 
    23, 24, 26,
    23, 26, 27,

    31, 32, 34,
    32, 33, 34,

    38, 39, 40,
    38, 40, 41,
                    
    50, 51, 52,
    44, 50, 52,
    44, 52, 53,

    45, 46, 48,
    45, 48, 49,
    46, 47, 48,

    55, 56, 58,
    56, 57, 58,
    55, 58, 64,

    59, 60, 62,
    60, 61, 62,
    59, 62, 63,

    67, 68, 69,
    67, 69, 70,
                     
    8, 17, 44,
    7, 44, 54,

    19, 28, 55,
    28, 55, 64
  };

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  GLint positionAttribute{glGetAttribLocation(programSnow, "inPosition")};

  glGenVertexArrays(1, &VAO);

  glBindVertexArray(VAO);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  glBindVertexArray(0);
}

void Snowflake::paintGL()
{

  glUseProgram(programSnow);

  glBindVertexArray(VAO);

  glUniform1f(scaleUni, scale);
  glUniform1f(rotationUni, rotation);


  glUniform4fv(colorUni, 1, &color.r);
  glDrawElements(GL_TRIANGLES, 72*3, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);

  glUseProgram(0);
}

void Snowflake::terminateGL()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void Snowflake::update(float deltaTime)
{
  if (input[static_cast<size_t>(Input::Left)])
    rotation = glm::wrapAngle(rotation + 9.0f * deltaTime);
  if (input[static_cast<size_t>(Input::Right)])
    rotation = glm::wrapAngle(rotation - 9.0f * deltaTime);
  if (input[static_cast<size_t>(Input::Up)])
    scale += 3.0f * deltaTime;
  if (input[static_cast<size_t>(Input::Down)])
    scale -= 3.0f * deltaTime;
  if (input[static_cast<size_t>(Input::Space)])
  {
    std::uniform_real_distribution<float> m_randomDist{0.0f, 1.0f};
    color = {m_randomDist(randomEngine), m_randomDist(randomEngine), m_randomDist(randomEngine), 1.0f};
  }
}
