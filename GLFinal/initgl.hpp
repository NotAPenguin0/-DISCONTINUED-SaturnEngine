#ifndef MVG_INIT_GL_HPP_
#define MVG_INIT_GL_HPP_

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "FPSCamera.hpp"
#include "Window.hpp"

extern Saturn::FPSCamera camera;
extern bool firstMouse;
extern float lastX;
extern float lastY;

void init();

void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window,
                               int w,
                               int h);

void mouse_callback([[maybe_unused]] GLFWwindow* window,
                    double xpos,
                    double ypos);

void scroll_callback([[maybe_unused]] GLFWwindow* window,
                     [[maybe_unused]] double xoffset,
                     double yoffset);

Saturn::Window init_all();

#endif
