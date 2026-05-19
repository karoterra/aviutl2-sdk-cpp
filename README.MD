# aviutl2-sdk-cpp

AviUtl2 SDK の C++ ラッパー

## 状態

開発中

API はまだ安定していません。

### 対応状況

- [x] フィルタプラグイン `.auf2`
- [x] 入力プラグイン `.aui2`
- [x] 出力プラグイン `.auo2`
- [x] スクリプトモジュール `.mod2`
- [ ] 汎用プラグイン `.aux2`

## 特徴
- AviUtl2 SDK の薄い C++ ラッパー
- C++20
- ヘッダーオンリー

## サンプル

使用例は [examples](examples/) を参照してください。

## ビルド

```bash
# Ninja
cmake --preset ninja
cmake --build --preset ninja
ctest --preset ninja

# MSVC
cmake --preset vs
cmake --build --preset vs
ctest --preset vs
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
