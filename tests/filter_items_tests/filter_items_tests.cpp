#include <cassert>
#include <type_traits>

#include <aviutl2_sdk_cpp/filter.hpp>

using aviutl2::filter::Check;
using aviutl2::filter::FilterPlugin;
using aviutl2::filter::group;
using aviutl2::filter::Select;
using aviutl2::filter::separator;
using aviutl2::filter::Track;

enum class Component : int {
    R = 1,
    G = 2,
    B = 4,
    All = R | G | B,
};

class TestPlugin : public FilterPlugin<TestPlugin> {
  public:
    Track<double> luminance{L"明るさ", 1.0, 0.0, 2.0, 0.01};
    Select<Component> component{
        L"対象",
        Component::All,
        {
            {L"R", Component::R},
            {L"G", Component::G},
            {L"B", Component::B},
            {L"RGB", Component::All},
        },
    };
    Track<double> volume{L"音量", 1.0, 0.0, 2.0, 0.01};
    Check mono{L"モノラル化", false};
    Check dummy{L"ダミー", false};

    explicit TestPlugin(token) {}
};

void test_plain_items() {
    auto& plugin = TestPlugin::instance();

    plugin.add_items(plugin.luminance, plugin.component, plugin.volume, plugin.mono);

    const auto& items = plugin.items();

    assert(items.size() == 5);
    assert(items[0] == plugin.luminance.item());
    assert(items[1] == plugin.component.item());
    assert(items[2] == plugin.volume.item());
    assert(items[3] == plugin.mono.item());
    assert(items[4] == nullptr);
}

void test_separator() {
    auto& plugin = TestPlugin::instance();

    plugin.add_items(plugin.luminance, plugin.component, separator(L"セパレート"), plugin.volume, plugin.mono);

    const auto& items = plugin.items();

    assert(items.size() == 6);
    assert(items[0] == plugin.luminance.item());
    assert(items[1] == plugin.component.item());
    assert(items[2] != nullptr); // separator
    assert(items[3] == plugin.volume.item());
    assert(items[4] == plugin.mono.item());
    assert(items[5] == nullptr);
}

void test_group_with_separator() {
    auto& plugin = TestPlugin::instance();

    plugin.add_items(plugin.luminance,
                     group(L"グループ1", true, plugin.component, separator(L"セパレート2"), plugin.volume, plugin.mono),
                     plugin.dummy);

    const auto& items = plugin.items();

    assert(items.size() == 9);
    assert(items[0] == plugin.luminance.item());
    assert(items[1] != nullptr); // group begin
    assert(items[2] == plugin.component.item());
    assert(items[3] != nullptr); // separator
    assert(items[4] == plugin.volume.item());
    assert(items[5] == plugin.mono.item());
    assert(items[6] != nullptr); // group end
    assert(items[7] == plugin.dummy.item());
    assert(items[8] == nullptr); // items terminator
}

int main() {
    test_plain_items();
    test_separator();
    test_group_with_separator();
}
