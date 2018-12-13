#ifndef SATURN_GL_HEADERS_HPP_
#define SATURN_GL_HEADERS_HPP_

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include <GLM/glm.hpp>

#define GLFW_DONT_GIVE_A_SHIT GLFW_DONT_CARE

std::ostream& operator<<(std::ostream& out, glm::vec3 const& vec);

#endif
