#include <cassert>
#include <type_traits>

#include <aviutl2_sdk_cpp/generic.hpp>

class ValidGenericPlugin : public aviutl2::generic::GenericPlugin<ValidGenericPlugin> {
  public:
    explicit ValidGenericPlugin(token) {}

    void register_impl(aviutl2::generic::HostApp& host) {}
};

static_assert(aviutl2::generic::GenericPluginType<ValidGenericPlugin>);
static_assert(aviutl2::generic::RegisterablePlugin<ValidGenericPlugin>);

void test_plugin() { ValidGenericPlugin::instance(); }

int main() { test_plugin(); }
