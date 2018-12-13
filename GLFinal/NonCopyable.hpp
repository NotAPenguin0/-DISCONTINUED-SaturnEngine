#ifndef SATURN_NON_COPYABLE_HPP_
#define SATURN_NON_COPYABLE_HPP_

namespace Saturn {

class NonCopyable {
public:
    NonCopyable() = default;
    ~NonCopyable() = default;
private:
    NonCopyable(NonCopyable const&) = delete;
    NonCopyable& operator=(NonCopyable const&) = delete;
};

} // namespace Saturn

#endif