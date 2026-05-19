//----------------------------------------------------------------------------------
// サンプルスクリプトモジュールプラグイン for AviUtl ExEdit2
//
// 公式SDKのサンプルコードを本ラッパー用に書き換えたものです
//----------------------------------------------------------------------------------
#include <aviutl2_sdk_cpp/module.hpp>

#include <algorithm>

// 合計を計算するサンプル関数
void sum(aviutl2::module::ScriptModuleParam* param) {
    // 引数の合計を計算
    double total = 0.0;
    auto num = param->get_param_num();
    for (int i = 0; i < num; i++) {
        total += param->get_param_double(i);
    }
    param->push_result_double(total);
}

// 明るさを調整するサンプル関数
void luminance(aviutl2::module::ScriptModuleParam* param) {
    // 引数を取得
    auto n = param->get_param_num();
    if (n != 4) {
        param->set_error("引数の数が正しくありません");
        return;
    }
    auto p = (aviutl2::PixelRgba*)param->get_param_data(0);
    auto w = param->get_param_int(1);
    auto h = param->get_param_int(2);
    auto v = param->get_param_double(3);
    if (!p || w <= 0 || h <= 0) {
        param->set_error("引数の値が正しくありません");
        return;
    }

    // 明るさを調整
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            p->r = (unsigned char)std::clamp(p->r * v, 0.0, 255.0);
            p->g = (unsigned char)std::clamp(p->g * v, 0.0, 255.0);
            p->b = (unsigned char)std::clamp(p->b * v, 0.0, 255.0);
            p++;
        }
    }
}

class SampleScriptModule : public aviutl2::module::ScriptModule<SampleScriptModule> {
  public:
    explicit SampleScriptModule(token) {
        // モジュールの情報を設定
        information_ = L"Sample ScriptModule version 2.00 By ＫＥＮくん";
        add_functions({
            {L"sum", sum},
            {L"luminance", luminance},
        });
    }
};

AVIUTL2_REGISTER_SCRIPT_MODULE(SampleScriptModule)
