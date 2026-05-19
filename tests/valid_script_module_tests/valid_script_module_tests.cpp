#include <cassert>
#include <type_traits>

#include <aviutl2_sdk_cpp/module.hpp>

class ValidScriptModule : public aviutl2::module::ScriptModule<ValidScriptModule> {
  public:
    explicit ValidScriptModule(token) {}
};

void test_plugin() { ValidScriptModule::instance(); }

int main() { test_plugin(); }
