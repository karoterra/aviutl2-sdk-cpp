//----------------------------------------------------------------------------------
// サンプルウィンドウクライアントプラグイン for AviUtl ExEdit2
//
// 公式SDKのサンプルコードを本ラッパー用に書き換えたものです
//----------------------------------------------------------------------------------

#include <aviutl2_sdk_cpp/generic.hpp>

#include <commctrl.h>

#define SampleWindowName L"SampleWindowClient"
#define IDC_BUTTON 1001

// オブジェクトエイリアスデータ定義
constexpr LPCSTR alias = R"(
[Object]
[Object.0]
effect.name=テキスト
サイズ=150.00
文字色=3bcb48
テキスト=さんぷる
[Object.1]
effect.name=標準描画
[Object.2]
effect.name=縁取り
サイズ=10
縁色=ffffff
)";

class WindowClientPlugin : public aviutl2::generic::GenericPlugin<WindowClientPlugin> {
  public:
    aviutl2::edit::EditHandle edit_handle;

    explicit WindowClientPlugin(token) {
        name_ = L"Sample Window Client";
        information_ = L"Sample Window Client version 2.00 By ＫＥＮくん";
    }

    bool initialize_impl(DWORD version) {
        if (version < 2003300) {
            return false;
        }
        return true;
    }

    void register_impl(aviutl2::generic::HostApp& host) {
        // 自身のウィンドウを作成
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.lpszClassName = SampleWindowName;
        wcex.lpfnWndProc = wnd_proc;
        wcex.hInstance = GetModuleHandle(0);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        if (!RegisterClassEx(&wcex)) {
            return;
        }
        auto hwnd = CreateWindowEx(
            0, SampleWindowName, SampleWindowName,
            WS_POPUP, // 親ウィンドウの指定無しでWS_CHILDが作れないので一旦WS_POPUPで作成しています
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, GetModuleHandle(0), nullptr);
        if (!hwnd) {
            return;
        }
        // ボタンの作成
        CreateWindowEx(0, WC_BUTTON, aviutl2::config::translate(L"オブジェクト作成"),
                       WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 10, 200,
                       aviutl2::config::get_layout_size("SettingItemHeight"), hwnd, (HMENU)IDC_BUTTON,
                       GetModuleHandle(0), nullptr);

        // ウィンドウを登録
        host.register_window_client(SampleWindowName, hwnd);

        // 編集ハンドルを作成
        edit_handle = host.create_edit_handle();
    }

    // ウィンドウプロシージャ
    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
        switch (message) {
            case WM_COMMAND:
                switch (LOWORD(wparam)) {
                    case IDC_BUTTON:
                        // 編集セクションで処理を呼び出す
                        WindowClientPlugin::instance().edit_handle.call_edit_section(
                            [](aviutl2::edit::EditSection& edit) {
                                // エイリアスデータからオブジェクトを作成
                                if (edit.create_object_from_alias(alias, edit.info()->layer, edit.info()->frame, 10)) {
                                    aviutl2::logger::log(L"create alias object");
                                } else {
                                    aviutl2::logger::warn(L"create alias failed");
                                }
                            });
                        SetFocus(NULL); // ボタンのフォーカスを外す
                        return 0;
                }
                break;
        }
        return DefWindowProc(hwnd, message, wparam, lparam);
    }
};

static_assert(aviutl2::InitializablePlugin<WindowClientPlugin>);

AVIUTL2_REGISTER_GENERIC_PLUGIN(WindowClientPlugin)
