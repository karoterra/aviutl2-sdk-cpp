#include <cassert>
#include <type_traits>

#include <aviutl2_sdk_cpp/filter.hpp>

class ValidFilterPlugin : public aviutl2::filter::FilterPlugin<ValidFilterPlugin> {
  public:
    explicit ValidFilterPlugin(token) {}
};

static_assert(aviutl2::filter::FilterPluginType<ValidFilterPlugin>);

void test_plugin() { ValidFilterPlugin::instance(); }

int main() { test_plugin(); }
