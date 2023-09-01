#pragma once
#include <glm/glm.hpp>

bool triBoxOverlap(glm::vec3 boxcenter, glm::vec3 boxhalfsize, glm::vec3 triverts[3]);