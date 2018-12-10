#include "FPSCamera.hpp"

#include <cmath>

#include <iostream>

namespace mvg {

FPSCamera::FPSCamera(glm::vec3 pos /*= glm::vec3(0.0f, 0.0f, 0.0f)*/,
                     glm::vec3 up /*= glm::vec3(0.0f, 1.0f, 0.0f)*/,
                     float yaw /*= YAW*/,
                     float pitch /*= PITCH*/) {

    Position = pos;
    Front = glm::vec3(0.0, 0.0, -1.0);
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    Speed = SPEED;
    Sensitivity = SENSITIVITY;
    Zoom = ZOOM;

    calculate_vectors();
}

FPSCamera::FPSCamera(float posX,
                     float posY,
                     float posZ,
                     float upX,
                     float upY,
                     float upZ,
                     float yaw,
                     float pitch) :
    FPSCamera( // chain constructors for simplicity
        glm::vec3(posX, posY, posZ),
        glm::vec3(upX, upY, upZ),
        yaw,
        pitch) {}

glm::mat4 FPSCamera::view_matrix()  {
    return glm::lookAt(Position, Position + Front, Up);
}

void FPSCamera::move(Direction dir, float deltaTime) {
    float vel = Speed * deltaTime;
    if (dir == Direction::Forward) {
        Position += vel * Front;
    } else if (dir == Direction::Backward) {
        Position -= vel * Front;
    } else if (dir == Direction::Right) {
        Position += vel * Right;
    } else if (dir == Direction::Left) {
        Position -= vel * Right;
    } else if (dir == Direction::Up) {
        Position += vel * WorldUp;
    } else if (dir == Direction::Down) {
        Position -= vel * WorldUp;
    }
}

void FPSCamera::mouse_move(float xoffset, float yoffset) {
    xoffset *= Sensitivity;
    yoffset *= Sensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped

    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    // Update Front, Right and Up Vectors using the updated Euler angles
    calculate_vectors();
}

void FPSCamera::zoom(float yoffset) {
    if (Zoom >= 1.0f && Zoom <= 90.0f) Zoom -= yoffset;
    if (Zoom <= 1.0f) Zoom = 1.0f;
    if (Zoom >= 90.0f) Zoom = 90.0f;
}

void FPSCamera::calculate_vectors() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(
        Front, WorldUp)); // Normalize the vectors, because their length gets
                          // closer to 0 the more you look up or down which
                          // results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

} // namespace mvg
