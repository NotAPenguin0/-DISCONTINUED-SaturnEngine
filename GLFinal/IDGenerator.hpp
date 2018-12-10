#ifndef MVG_ID_GENERATOR_HPP_
#define MVG_ID_GENERATOR_HPP_

namespace mvg {

class ObjectIDGenerator {
public:
    using IDType = unsigned long long;
    inline IDType next() {
        static IDType cur = 0;
        return ++cur;
    }
};

} // namespace mvg

#endif
