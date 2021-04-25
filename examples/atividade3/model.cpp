#include "model.hpp"

#include <fmt/core.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <filesystem>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

//retirado do exemplo pq não sei c++ o suficiente
namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const& vertex) const noexcept {
    std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
    std::size_t h2{std::hash<glm::vec3>()(vertex.normal)};
    std::size_t h3{std::hash<glm::vec2>()(vertex.texCoord)};
    return h1 ^ h2 ^ h3;
  }
};
} 

Model::~Model() {
  glDeleteTextures(1, &diffuseTexture);
  glDeleteBuffers(1, &EBO);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}

void Model::computeNormals()
{
  for (auto& vertex : vertices)
  {
    vertex.normal = glm::zero<glm::vec3>();
  }
  for (const auto offset : iter::range<int>(0, indices.size(), 3))
  {
    Vertex& a{vertices.at(indices.at(offset + 0))};
    Vertex& b{vertices.at(indices.at(offset + 1))};
    Vertex& c{vertices.at(indices.at(offset + 2))};

    const auto edge1{b.position - a.position};
    const auto edge2{c.position - b.position};
    glm::vec3 normal{glm::cross(edge1, edge2)};

    a.normal += normal;
    b.normal += normal;
    c.normal += normal;
  }

  for (auto& vertex : vertices)
  {
    vertex.normal = glm::normalize(vertex.normal);
  }
  hasNormals = true;
}

void Model::createBuffers()
{

  glDeleteBuffers(1, &EBO);
  glDeleteBuffers(1, &VBO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(vertices[0]) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(indices[0]) * indices.size(),
               indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::loadDiffuseTexture(std::string_view path)
{
  if (!std::filesystem::exists(path)) return;

  glDeleteTextures(1, &diffuseTexture);
  diffuseTexture = abcg::opengl::loadTexture(path);
}

void Model::loadFromFile(std::string_view path, bool standardize)
{
  auto basePath{std::filesystem::path{path}.parent_path().string() + "/"};

  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path = basePath; 

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data(), readerConfig))
  {
    if (!reader.Error().empty())
    {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) 
  {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};
  const auto& materials{reader.GetMaterials()};

  vertices.clear();
  indices.clear();

  hasNormals = false;
  hasTexCoords = false;

  std::unordered_map<Vertex, GLuint> hash{};

  for (const auto& shape : shapes)
  {
    for (const auto offset : iter::range(shape.mesh.indices.size()))
    {
      tinyobj::index_t index{shape.mesh.indices.at(offset)};

      std::size_t startIndex{static_cast<size_t>(3 * index.vertex_index)};
      float vx{attrib.vertices.at(startIndex + 0)};
      float vy{attrib.vertices.at(startIndex + 1)};
      float vz{attrib.vertices.at(startIndex + 2)};

      float nx{};
      float ny{};
      float nz{};
      if (index.normal_index >= 0) {
        hasNormals = true;
        startIndex = 3 * index.normal_index;
        nx = attrib.normals.at(startIndex + 0);
        ny = attrib.normals.at(startIndex + 1);
        nz = attrib.normals.at(startIndex + 2);
      }
      float tu{};
      float tv{};
      if (index.texcoord_index >= 0) {
        hasTexCoords = true;
        startIndex = 2 * index.texcoord_index;
        tu = attrib.texcoords.at(startIndex + 0);
        tv = attrib.texcoords.at(startIndex + 1);
      }

      Vertex vertex{};
      vertex.position = {vx, vy, vz};
      vertex.normal = {nx, ny, nz};
      vertex.texCoord = {tu, tv};

      if (hash.count(vertex) == 0) {
        
        hash[vertex] = vertices.size();
        vertices.push_back(vertex);
      }

      indices.push_back(hash[vertex]);
    }
  }

  if (!materials.empty()) {
    const auto& mat{materials.at(0)}; 
    Ka = glm::vec4(mat.ambient[0], mat.ambient[1], mat.ambient[2], 1);
    Kd = glm::vec4(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1);
    Ks = glm::vec4(mat.specular[0], mat.specular[1], mat.specular[2], 1);
    shininess = mat.shininess;

    if (!mat.diffuse_texname.empty())
      loadDiffuseTexture(basePath + mat.diffuse_texname);
  } else {
  
    Ka = {0.1f, 0.1f, 0.1f, 1.0f};
    Kd = {0.7f, 0.7f, 0.7f, 1.0f};
    Ks = {1.0f, 1.0f, 1.0f, 1.0f};
    shininess = 25.0f;
  }

  if (standardize) {
    this->standardize();
  }

  if (!hasNormals) {
    computeNormals();
  }

  createBuffers();
}

void Model::render(int numTriangles) const {
  glBindVertexArray(VAO);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, diffuseTexture);

 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  GLsizei numIndices = (numTriangles < 0) ? indices.size() : numTriangles * 3;

  glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
}

void Model::setupVAO(GLuint program) {
  
  glDeleteVertexArrays(1, &VAO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  GLint positionAttribute{glGetAttribLocation(program, "inPosition")};
  if (positionAttribute >= 0) {
    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), nullptr);
  }

  GLint normalAttribute{glGetAttribLocation(program, "inNormal")};
  if (normalAttribute >= 0) {
    glEnableVertexAttribArray(normalAttribute);
    GLsizei offset{sizeof(glm::vec3)};
    glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), reinterpret_cast<void*>(offset));
  }

  GLint texCoordAttribute{glGetAttribLocation(program, "inTexCoord")};
  if (texCoordAttribute >= 0) {
    glEnableVertexAttribArray(texCoordAttribute);
    GLsizei offset{sizeof(glm::vec3) + sizeof(glm::vec3)};
    glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), reinterpret_cast<void*>(offset));
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Model::standardize() {
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

  const auto center{(min + max) / 2.0f};
  const auto scaling{2.0f / glm::length(max - min)};
  for (auto& vertex : vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}
