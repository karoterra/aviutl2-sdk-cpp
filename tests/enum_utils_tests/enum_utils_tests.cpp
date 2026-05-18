#include <cassert>

#include <aviutl2_sdk_cpp/utils.hpp>

enum class Enum : int {
    A = 1 << 0,
    B = 1 << 1,
    C = 1 << 2,
};
template <> struct aviutl2::utils::enum_utils::enable_bitmask<Enum> : std::true_type {};

void test_or_operator() {
    Enum e1 = Enum::A;
    Enum e2 = Enum::B;
    Enum e3 = e1 | e2;
    assert(static_cast<int>(e3) == (static_cast<int>(Enum::A) | static_cast<int>(Enum::B)));
}

void test_and_operator() {
    Enum e1 = Enum::A | Enum::B;
    Enum e2 = Enum::B | Enum::C;
    Enum e3 = e1 & e2;
    assert(static_cast<int>(e3) == (static_cast<int>(Enum::B)));
}

void test_has_flag() {
    Enum e = Enum::A | Enum::C;
    assert(aviutl2::utils::enum_utils::has_flag(e, Enum::A));
    assert(!aviutl2::utils::enum_utils::has_flag(e, Enum::B));
    assert(aviutl2::utils::enum_utils::has_flag(e, Enum::C));
}

int main() {
    test_or_operator();
    test_and_operator();
    test_has_flag();
    return 0;
}
