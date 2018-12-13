#include "Object.hpp"

#include "Components.hpp"

namespace Saturn {

Object::~Object() { delete_components_for_index(ID); }

} // namespace Saturn
