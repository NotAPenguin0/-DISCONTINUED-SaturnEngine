#ifndef SATURN_ID_GENERATOR_HPP_
#define SATURN_ID_GENERATOR_HPP_

namespace Saturn {

class ObjectIDGenerator {
public:
    using IDType = unsigned long long;
    inline IDType next() {
        static IDType cur = 0;
        return ++cur;
    }
};

} // namespace Saturn

#endif
