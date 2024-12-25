#ifndef ASTAR_H
#define ASTAR_H

#include "Map.h"
#include <glm/glm.hpp>
#include <vector>

class AStar {
public:
    std::vector<glm::vec2> findPath(Map& map, glm::vec2 start, glm::vec2 goal);
};

#endif
