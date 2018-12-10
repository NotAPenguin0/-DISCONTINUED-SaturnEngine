#include "Object.hpp"

#include "Components.hpp"

namespace mvg {

Object::~Object() { delete_components_for_index(ID); }

} // namespace mvg
