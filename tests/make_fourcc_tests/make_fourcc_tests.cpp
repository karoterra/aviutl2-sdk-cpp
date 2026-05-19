#include <cassert>

#include <aviutl2_sdk_cpp/output.hpp>

using aviutl2::output::OutputVideoFormat;
using aviutl2::output::detail::make_fourcc;

void test_make_fourcc_test() {
    // t: 0x74
    // e: 0x65
    // s: 0x73
    // t: 0x74
    assert(make_fourcc('t', 'e', 's', 't') == 0x74736574u);
}

void test_make_fourcc_YUY2() {
    // Y: 0x59
    // U: 0x55
    // Y: 0x59
    // 2: 0x32
    assert(make_fourcc('Y', 'U', 'Y', '2') == static_cast<DWORD>(OutputVideoFormat::YUY2));
    assert(make_fourcc('Y', 'U', 'Y', '2') == 0x32595559u);
}

int main() {
    test_make_fourcc_test();
    test_make_fourcc_YUY2();
    return 0;
}
