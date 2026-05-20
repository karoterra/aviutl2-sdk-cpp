# aviutl2-sdk-cpp

[![CI](https://github.com/karoterra/aviutl2-sdk-cpp/actions/workflows/ci.yaml/badge.svg)](https://github.com/karoterra/aviutl2-sdk-cpp/actions/workflows/ci.yaml)

AviUtl2 SDK の C++ ラッパー

## 状態

開発中

API はまだ安定していません。

### 対応状況

- [x] フィルタプラグイン `.auf2`
- [x] 入力プラグイン `.aui2`
- [x] 出力プラグイン `.auo2`
- [x] スクリプトモジュール `.mod2`
- [x] 汎用プラグイン `.aux2`

## 特徴

- AviUtl2 SDK の薄い C++ ラッパー
- 元SDKの構造や概念は大きく変えない
- ヘッダーオンリー
- C++20 (`enum class`, `std::span`, etc..)
- 必須フックは concept / static_assert により検査
- 必要に応じて raw API にもアクセス可能

## 動作確認環境

- MSVC
- clang-cl
- CMake 3.31+
- C++20

## サンプル

使用例は [examples](examples/) を参照してください。
- フィルタプラグイン
  - [**MediaFilter**](examples/MediaFilter/): 公式サンプルをラッパーを使って書き換えたもの
  - [**MediaObject**](examples/MediaObject/): 公式サンプルをラッパーを使って書き換えたもの
- 入力プラグイン
  - [**skeleton_input_plugin**](examples/skeleton_input_plugin/): 最小構成テンプレート
  - [**AviReader**](examples/AviReader/): 公式サンプルをラッパーを使って書き換えたもの
- 出力プラグイン
  - [**skeleton_output_plugin**](examples/skeleton_output_plugin/): 最小構成テンプレート
  - [**AviSaver**](examples/AviSaver/): 公式サンプルをラッパーを使って書き換えたもの
- スクリプトモジュール
  - [**skeleton_script_module**](examples/skeleton_script_module/): 最小構成テンプレート
  - [**ScriptModule**](examples/ScriptModule/): 公式サンプルをラッパーを使って書き換えたもの
- 汎用プラグイン
  - [**skeleton_generic_plugin**](examples/skeleton_generic_plugin/): 最小構成テンプレート
  - [**WindowClient**](examples/WindowClient/): 公式サンプルをラッパーを使って書き換えたもの

### 最小例

```cpp
#include <aviutl2_sdk_cpp/filter.hpp>

class MyFilterPlugin
    : public aviutl2::filter::FilterPlugin<MyFilterPlugin> {
  public:
    explicit MyFilterPlugin(token) {
        flag_ = aviutl2::filter::FilterFlag::VIDEO;
        name_ = L"My Filter";
        label_ = L"sample";
        information_ = L"My Filter Plugin (sample)";
    }

    bool proc_video_impl(aviutl2::filter::FilterProcVideo& video) {
        return true;
    }
};

AVIUTL2_REGISTER_FILTER_PLUGIN(MyFilterPlugin)
```

## 利用方法

### CMakeプロジェクトの設定例

本リポジトリを FetchContent で取得すると `aviutl2::sdk_cpp` を INTERFACE ライブラリとして利用できます。

```cmake
FetchContent_Declare(
    aviutl2_sdk_cpp
    GIT_REPOSITORY https://github.com/karoterra/aviutl2-sdk-cpp.git
    GIT_TAG main
)

FetchContent_MakeAvailable(aviutl2_sdk_cpp)

target_link_libraries(my_plugin PRIVATE aviutl2::sdk_cpp)
```

### raw SDK API

元SDKのAPIは `aviutl2::raw` 名前空間から利用できます。

```cpp
aviutl2::raw::EDIT_HANDLE
aviutl2::raw::FILTER_PLUGIN_TABLE
```

## 開発

フォーマットには clang-format を使用します。

ビルド・テスト

```bash
# MSVC
cmake --preset vs
cmake --build --preset vs
ctest --preset vs

# Ninja + msvc
cmake --preset ninja-msvc
cmake --build --preset ninja-msvc
ctest --preset ninja-msvc

# Ninja + clang-cl
cmake --preset ninja-clang-cl
cmake --build --preset ninja-clang-cl
ctest --preset ninja-clang-cl
```

## AviUtl2 SDK

cmake の config 時に自動でミラーから取得します。

`AVIUTL2_SDK_DIR` を指定した場合はそちらを参照します。

## ライセンス

このソフトウェアは MIT ライセンスのもとで公開されます。
詳細は以下のファイルを参照してください。

- [LICENSE](LICENSE)

## 謝辞

このプロジェクトは AviUtl2 SDK を利用して開発されています。

- [AviUtlのお部屋](https://spring-fragrance.mints.ne.jp/aviutl/)
- [AviUtl2 SDK Mirror](https://github.com/aviutl2/aviutl2_sdk_mirror)
