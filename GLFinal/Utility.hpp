#ifndef SATURN_UTILITY_HPP_
#define SATURN_UTILITY_HPP_

#include <vector>

namespace Saturn {

template<typename T, typename U>
void merge(std::vector<T> const& src, std::vector<U>& dest) {
    // Reserve memory, because we know we will need this exact amount of
    // elements
    dest.reserve(dest.size() + src.size());
    // Loop over src, push_back elements in dest
    for (auto const& elem : src) { dest.push_back(elem); }
}

//Creates a vector of pointers from a vector of objects
template<typename T>
std::vector<T*> to_pointers(std::vector<T>& vec) {
    std::vector<T*> pointers(vec.size());
    for (auto& elem : vec) { pointers.push_back(&elem); }
    return pointers;
}

} // namespace Saturn

#endif
