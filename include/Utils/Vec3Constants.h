#include <glm/glm.hpp>

namespace Vec3
{
    constexpr glm::vec3 XAxis = glm::vec3(1, 0, 0);
    constexpr glm::vec3 YAxis = glm::vec3(0, 1, 0);
    constexpr glm::vec3 ZAxis = glm::vec3(0, 0, 1);

    constexpr glm::vec3 UP = YAxis;
    constexpr glm::vec3 FORWARD = ZAxis;
    constexpr glm::vec3 RIGHT = XAxis;
}


