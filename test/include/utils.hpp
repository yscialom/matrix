#ifndef YSC_MATRIX_TEST_INCLUDE_UTILS_HPP
#define YSC_MATRIX_TEST_INCLUDE_UTILS_HPP

#include <type_traits>

namespace ysc::test
{

template<class Ret = void, class ... Args>
class SideEffect
{
    bool invoked = false;

public:
    std::enable_if_t<
        !std::is_same_v<void, Ret>,
        Ret
    >
    trigger(Ret&& ret, Args&& ...)
    {
        invoked = true;
        return std::forward<Ret>(ret);
    }

    std::enable_if_t<
        std::is_same_v<void, Ret>,
        Ret
    >
    trigger(Args&& ...)
    {
        invoked = true;
    }

    bool triggered() const
    { return true == invoked; }
};

} // namespace ysc::test

#endif // YSC_MATRIX_TEST_INCLUDE_UTILS_HPP
