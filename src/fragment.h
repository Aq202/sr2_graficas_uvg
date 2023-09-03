#pragma once
#include <glm/glm.hpp>
#include <cstdint>
#include "color.h"

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};

struct Fragment {
  uint16_t x;      
  uint16_t y;      
  double z;
  Color color; 
  float intensity; 
};

struct FragColor {
  Color color;
  double z; 
};
