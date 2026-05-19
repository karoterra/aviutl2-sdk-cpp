#include <cassert>
#include <type_traits>

#include <aviutl2_sdk_cpp/output.hpp>

using aviutl2::output::OutputFlag;
using aviutl2::output::OutputInfo;

class ValidOutputPlugin : public aviutl2::output::OutputPlugin<ValidOutputPlugin> {
  public:
    explicit ValidOutputPlugin(token) {}

    bool output_impl(OutputInfo& info) { return false; }
};

static_assert(aviutl2::output::OutputablePlugin<ValidOutputPlugin>);

void test_plugin() { ValidOutputPlugin::instance(); }

int main() { test_plugin(); }
