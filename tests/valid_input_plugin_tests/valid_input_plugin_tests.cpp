#include <Windows.h>
#include <cassert>
#include <type_traits>

#include <aviutl2_sdk_cpp/input.hpp>

using aviutl2::input::InputFlag;
using aviutl2::input::InputHandle;
using aviutl2::input::InputInfo;

class ValidInputPlugin : public aviutl2::input::InputPlugin<ValidInputPlugin> {
  public:
    explicit ValidInputPlugin(token) {}

    InputHandle open_impl(LPCWSTR file) { return nullptr; }

    bool close_impl(InputHandle handle) { return true; }

    std::optional<aviutl2::input::InputInfo> get_input_info_impl(aviutl2::input::InputHandle handle) {
        return std::nullopt;
    }
};

static_assert(aviutl2::input::OpenablePlugin<ValidInputPlugin>);
static_assert(aviutl2::input::CloseablePlugin<ValidInputPlugin>);
static_assert(aviutl2::input::InputInfoGettablePlugin<ValidInputPlugin>);

void test_plugin() { ValidInputPlugin::instance(); }

int main() { test_plugin(); }
