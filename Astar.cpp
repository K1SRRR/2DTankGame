#include "AStar.h"
#include <queue>
#include <cmath>
#include <algorithm>

struct Node {
    int x, y;
    float gCost;
    float hCost;
    float fCost;
    Node* parent;

    Node(int x, int y, float gCost, float hCost, Node* parent = nullptr)
        : x(x), y(y), gCost(gCost), hCost(hCost), parent(parent) {
        fCost = gCost + hCost;
    }

    bool operator>(const Node& other) const {
        return fCost > other.fCost;
    }
};

float calculateHeuristic(int startX, int startY, int goalX, int goalY) {
    return std::abs(startX - goalX) + std::abs(startY - goalY);
}

std::vector<glm::vec2> AStar::findPath(Map& map, glm::vec2 start, glm::vec2 goal) {
    int startX = static_cast<int>((start.x + 1.0f) * map.MATRIX_WIDTH / 2.0f);
    int startY = static_cast<int>((start.y + 1.0f) * map.MATRIX_HEIGHT / 2.0f);
    int goalX = static_cast<int>((goal.x + 1.0f) * map.MATRIX_WIDTH / 2.0f);
    int goalY = static_cast<int>((goal.y + 1.0f) * map.MATRIX_HEIGHT / 2.0f);

    Node* startNode = new Node(startX, startY, 0.0f, calculateHeuristic(startX, startY, goalX, goalY));
    Node* goalNode = new Node(goalX, goalY, 0.0f, 0.0f);

    std::priority_queue<Node*, std::vector<Node*>, std::greater<Node*>> openList;
    std::vector<std::vector<bool>> closedList(map.MATRIX_HEIGHT, std::vector<bool>(map.MATRIX_WIDTH, false));

    openList.push(startNode);

    std::vector<glm::vec2> directions = {
        glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(0, -1), glm::vec2(-1, 0)
    };

    while (!openList.empty()) {
        Node* currentNode = openList.top();
        openList.pop();

        if (currentNode->x == goalX && currentNode->y == goalY) {
            std::vector<glm::vec2> path;
            Node* temp = currentNode;
            while (temp != nullptr) {
                path.push_back(glm::vec2((temp->x * 2.0f / map.MATRIX_WIDTH) - 1.0f,
                    (temp->y * 2.0f / map.MATRIX_HEIGHT) - 1.0f));
                temp = temp->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        closedList[currentNode->y][currentNode->x] = true;

        for (const auto& direction : directions) {
            int neighborX = currentNode->x + static_cast<int>(direction.x);
            int neighborY = currentNode->y + static_cast<int>(direction.y);

            if (neighborX >= 0 && neighborX < map.MATRIX_WIDTH && neighborY >= 0 && neighborY < map.MATRIX_HEIGHT &&
                map.getTile(neighborX, neighborY) != TileType::WALL && !closedList[neighborY][neighborX]) {

                float gCost = currentNode->gCost + 1.0f;
                float hCost = calculateHeuristic(neighborX, neighborY, goalX, goalY);
                Node* neighborNode = new Node(neighborX, neighborY, gCost, hCost, currentNode);

                openList.push(neighborNode);
            }
        }
    }

    return {};  // Ako nema puta
}
