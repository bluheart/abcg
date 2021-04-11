#include "openglwindow.hpp"
#include <glm/gtx/fast_trigonometry.hpp>

#include <unordered_map>
#include <tiny_obj_loader.h>
#include <cppitertools/itertools.hpp>
#include <glm/gtx/hash.hpp>
#include <fmt/core.h>

//Do exemplo
namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const& vertex) const noexcept {
    std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};
}

void OpenGLWindow::handleEvent(SDL_Event &event)
{
  if (event.type == SDL_KEYDOWN)
  {
    if (event.key.keysym.sym == SDLK_LEFT)
       ang = 20;
    if (event.key.keysym.sym == SDLK_RIGHT)
       ang = -20;
  }
  if (event.type == SDL_KEYUP)
  {
    if (event.key.keysym.sym == SDLK_LEFT)
      ang = 20;
    if (event.key.keysym.sym == SDLK_RIGHT)
      ang = -20;
  }
}

void OpenGLWindow::initializeGL()
{
  program = createProgramFromFile(getAssetsPath() + "garfield.vert",
                                  getAssetsPath() + "garfield.frag");
  loadModelFromFile(getAssetsPath() + "garfield.obj");

  glClearColor(0.82, 0.80, 0.79, 1);

  angle =  glm::wrapAngle(glm::radians(150.0f));

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);

  standardize();

  verticesToDraw = indices.size();

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

  GLint positionAttribute{glGetAttribLocation(program, "inPosition")};
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void OpenGLWindow::paintGL()
{
  float delta{static_cast<float>(getDeltaTime())};
  angle = glm::wrapAngle(angle + glm::radians(ang) * delta);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, viewportWidth, viewportHeight);

  glUseProgram(program);
  glBindVertexArray(VAO);

  glUniform1f(glGetUniformLocation(program, "angle"), angle);
  glDrawElements(GL_TRIANGLES, verticesToDraw, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
  glUseProgram(0);
}

void OpenGLWindow::resizeGL(int width, int height)
{
  viewportWidth = width;
  viewportHeight = height;
}

void OpenGLWindow::terminateGL()
{ 
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(program);
}

//================================================================================================
//Do exemplo
void OpenGLWindow::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path =
      getAssetsPath() + "mtl/";  // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data(), readerConfig)) {
    if (!reader.Error().empty()) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};

  vertices.clear();
  indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (const auto& shape : shapes) {
    // Loop over faces(polygon)
    size_t indexOffset{0};
    for (const auto faceNumber :
         iter::range(shape.mesh.num_face_vertices.size())) {
      // Number of vertices composing face f
      std::size_t numFaceVertices{shape.mesh.num_face_vertices[faceNumber]};
      // Loop over vertices in the face
      std::size_t startIndex{};
      for (const auto vertexNumber : iter::range(numFaceVertices)) {
        // Access to vertex
        tinyobj::index_t index{shape.mesh.indices[indexOffset + vertexNumber]};

        // Vertex coordinates
        startIndex = 3 * index.vertex_index;
        tinyobj::real_t vx = attrib.vertices[startIndex + 0];
        tinyobj::real_t vy = attrib.vertices[startIndex + 1];
        tinyobj::real_t vz = attrib.vertices[startIndex + 2];

        Vertex vertex{};
        vertex.position = {vx, vy, vz};

        // If hash doesn't contain this vertex
        if (hash.count(vertex) == 0) {
          // Add this index (size of m_vertices)
          hash[vertex] = vertices.size();
          // Add this vertex
          vertices.push_back(vertex);
        }

        indices.push_back(hash[vertex]);
      }
      indexOffset += numFaceVertices;
    }
  }
}

//Do exemplo
void OpenGLWindow::standardize() {

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (const auto& vertex : vertices) {
    max.x = std::max(max.x, vertex.position.x);
    max.y = std::max(max.y, vertex.position.y);
    max.z = std::max(max.z, vertex.position.z);
    min.x = std::min(min.x, vertex.position.x);
    min.y = std::min(min.y, vertex.position.y);
    min.z = std::min(min.z, vertex.position.z);
  }

  // Center and scale
  const auto center{(min + max) / 2.0f};
  const auto scaling{2.0f / glm::length(max - min)};
  for (auto& vertex : vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}