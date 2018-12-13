#ifndef SATURN_FPS_CAMERA_HPP_
#define SATURN_FPS_CAMERA_HPP_

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include "OpenGL.hpp"

namespace Saturn {

enum class Direction { Forward, Backward, Left, Right, Up, Down };

class FPSCamera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float Speed;
    float Sensitivity;
    float Zoom;

    FPSCamera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
              glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
              float yaw = YAW,
              float pitch = PITCH);

    FPSCamera(float posX,
              float posY,
              float posZ,
              float upX,
              float upY,
              float upZ,
              float yaw,
              float pitch);

    glm::mat4 view_matrix();

    void move(Direction dir, float deltaTime);
    void mouse_move(float xoffset, float yoffset);
    void zoom(float yoffset);

private:
    void calculate_vectors();

    static constexpr float YAW = -90.0f;
    static constexpr float PITCH = 0.0f;
    static constexpr float SPEED = 5.0f;
    static constexpr float SENSITIVITY = 0.1f;
    static constexpr float ZOOM = 45.0f;
};

} // namespace Saturn

#endif
