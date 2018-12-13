#include "ErrorHandler.hpp"

#include "log.hpp"
#include <string>

namespace Saturn {

void glfw_error_callback(int errcode, const char* desc) {
    using namespace std::literals::string_literals;

    Saturn::error("GLFW Error: ["s + std::to_string(errcode) + "] " + desc);
}

} // namespace Saturn
