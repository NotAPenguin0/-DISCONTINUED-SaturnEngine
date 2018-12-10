#ifndef MVG_NON_COPYABLE_HPP_
#define MVG_NON_COPYABLE_HPP_

namespace mvg {

class NonCopyable {
public:
    NonCopyable() = default;
    ~NonCopyable() = default;
private:
    NonCopyable(NonCopyable const&) = delete;
    NonCopyable& operator=(NonCopyable const&) = delete;
};

} // namespace mvg

#endif