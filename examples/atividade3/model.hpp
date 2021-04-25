#ifndef MODEL_HPP_
#define MODEL_HPP_

#include <string_view>

#include "abcg.hpp"

//retirado do exemplo pq não sei c++ o suficiente
struct Vertex {
  glm::vec3 position{};
  glm::vec3 normal{};
  glm::vec2 texCoord{};

  bool operator==(const Vertex& other) const noexcept {
    static const auto epsilon{std::numeric_limits<float>::epsilon()};
    return glm::all(glm::epsilonEqual(position, other.position, epsilon)) &&
           glm::all(glm::epsilonEqual(normal, other.normal, epsilon)) &&
           glm::all(glm::epsilonEqual(texCoord, other.texCoord, epsilon));
  }
};

class Model {
 public:
  Model() = default;
  virtual ~Model();

  Model(const Model&) = delete;
  Model(Model&&) = default;
  Model& operator=(const Model&) = delete;
  Model& operator=(Model&&) = default;

  void loadDiffuseTexture(std::string_view path);
  void loadFromFile(std::string_view path, bool standardize = true);
  void render(int numTriangles = -1) const;
  void setupVAO(GLuint program);

  [[nodiscard]] int getNumTriangles() const {
    return static_cast<int>(indices.size()) / 3;
  }

  [[nodiscard]] glm::vec4 getKa() const { return Ka; }
  [[nodiscard]] glm::vec4 getKd() const { return Kd; }
  [[nodiscard]] glm::vec4 getKs() const { return Ks; }
  [[nodiscard]] float getShininess() const { return shininess; }

  [[nodiscard]] bool isUVMapped() const { return hasTexCoords; }

 private:
  GLuint VAO{};
  GLuint VBO{};
  GLuint EBO{};

  glm::vec4 Ka;
  glm::vec4 Kd;
  glm::vec4 Ks;
  float shininess;
  GLuint diffuseTexture{};

  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  bool hasNormals{false};
  bool hasTexCoords{false};

  void computeNormals();
  void createBuffers();
  void standardize();
};

#endif