#ifndef YSC_MATRIX_TEST_INCLUDE_UTILS_HPP
#define YSC_MATRIX_TEST_INCLUDE_UTILS_HPP

#include <type_traits>
#include <cstddef>
#include <memory>

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

static auto non_zero_memory(std::size_t size)
{
    constexpr std::byte non_zero = static_cast<std::byte>(0xC5);

    auto memory = std::make_unique<std::byte[]>(size);
    std::fill(memory.get(), memory.get()+size, non_zero);
    return memory;
}

template <class T, class... Args>
static auto on_non_zero_memory(Args&& ...args)
{
    auto memory = non_zero_memory(sizeof(T));
    T* const naked_object = ([&]() {
        if constexpr (0 == sizeof...(Args)) {
            return new (memory.get()) T;
        } else {
            return new (memory.get()) T {std::forward<Args>(args)...};
        }
    })();
    memory.release();

    return std::shared_ptr<T>(naked_object, [](T* ptr) {
        ptr->~T();
        ::operator delete[] (reinterpret_cast<void*>(ptr));
    });
}

} // namespace ysc::test

#endif // YSC_MATRIX_TEST_INCLUDE_UTILS_HPP
