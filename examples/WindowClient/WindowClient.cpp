//----------------------------------------------------------------------------------
// サンプルウィンドウクライアントプラグイン for AviUtl ExEdit2
//
// 公式SDKのサンプルコードを本ラッパー用に書き換えたものです
//----------------------------------------------------------------------------------

#include <aviutl2_sdk_cpp/raw/config.hpp> // 設定関連
#include <aviutl2_sdk_cpp/raw/logger.hpp> // ログ出力
#include <aviutl2_sdk_cpp/raw/plugin.hpp>

#include <commctrl.h>

#define SampleWindowName L"SampleWindowClient"
#define IDC_BUTTON 1001
aviutl2::raw::EDIT_HANDLE* edit_handle;
aviutl2::raw::LOG_HANDLE* logger;
aviutl2::raw::CONFIG_HANDLE* config;

//---------------------------------------------------------------------
// 汎用プラグイン構造体定義
//---------------------------------------------------------------------
aviutl2::raw::COMMON_PLUGIN_TABLE common_plugin_table = {
    L"Sample Window Client",                            // プラグインの名前
    L"Sample Window Client version 2.00 By ＫＥＮくん", // プラグインの情報
};

//---------------------------------------------------------------------
// 必要とする本体バージョン番号取得関数 (未定義なら呼ばれません)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) DWORD RequiredVersion() { return 2003300; }

//---------------------------------------------------------------------
// ログ出力機能初期化関数 (未定義なら呼ばれません)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) void InitializeLogger(aviutl2::raw::LOG_HANDLE* handle) { logger = handle; }

//---------------------------------------------------------------------
// 設定関連初期化関数 (未定義なら呼ばれません)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) void InitializeConfig(aviutl2::raw::CONFIG_HANDLE* handle) { config = handle; }

//---------------------------------------------------------------------
// プラグインDLL初期化関数 (未定義なら呼ばれません)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) bool InitializePlugin(DWORD version) { return true; }

//---------------------------------------------------------------------
// プラグインDLL解放関数 (未定義なら呼ばれません)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) void UninitializePlugin() {}

//---------------------------------------------------------------------
// 汎用プラグイン構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) aviutl2::raw::COMMON_PLUGIN_TABLE* GetCommonPluginTable(void) {
    return &common_plugin_table;
}

//---------------------------------------------------------------------
// オブジェクトエイリアスデータ定義
//---------------------------------------------------------------------
LPCSTR alias = R"(
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

//---------------------------------------------------------------------
// ウィンドウプロシージャ
//---------------------------------------------------------------------
LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case IDC_BUTTON:
                    // 編集セクションで処理を呼び出す
                    edit_handle->call_edit_section_param(&message, [](void* message, EDIT_SECTION* edit) {
                        // エイリアスデータからオブジェクトを作成
                        if (edit->create_object_from_alias(alias, edit->info->layer, edit->info->frame, 10)) {
                            logger->log(logger, L"create alias object");
                        } else {
                            logger->warn(logger, L"create alias failed");
                        }
                    });
                    SetFocus(NULL); // ボタンのフォーカスを外す
                    return 0;
            }
            break;
    }
    return DefWindowProc(hwnd, message, wparam, lparam);
}

//---------------------------------------------------------------------
// プラグイン登録関数
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) void RegisterPlugin(aviutl2::raw::HOST_APP_TABLE* host) {
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
    auto hwnd = CreateWindowEx(0, SampleWindowName, SampleWindowName,
                               WS_POPUP, // 親ウィンドウの指定無しでWS_CHILDが作れないので一旦WS_POPUPで作成しています
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr,
                               GetModuleHandle(0), nullptr);
    if (!hwnd) {
        return;
    }
    // ボタンの作成
    CreateWindowEx(0, WC_BUTTON, config->translate(config, L"オブジェクト作成"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                   10, 10, 200, config->get_layout_size(config, "SettingItemHeight"), hwnd, (HMENU)IDC_BUTTON,
                   GetModuleHandle(0), nullptr);

    // ウィンドウを登録
    host->register_window_client(SampleWindowName, hwnd);

    // 編集ハンドルを作成
    edit_handle = host->create_edit_handle();
}
