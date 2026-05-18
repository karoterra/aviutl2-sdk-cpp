//----------------------------------------------------------------------------------
// サンプルAVI(vfw経由)入力プラグイン for AviUtl ExEdit2
//
// 公式SDKのサンプルコードを本ラッパー用に書き換えたものです
//----------------------------------------------------------------------------------
#include <aviutl2_sdk_cpp/input.hpp>

#include <vfw.h>
#pragma comment(lib, "vfw32.lib")

using aviutl2::input::InputFlag;
using aviutl2::input::InputHandle;
using aviutl2::input::InputInfo;
using aviutl2::utils::enum_utils::has_flag;

enum class FileFlag : int {
    VIDEO = 1,
    AUDIO = 2,
};
template <> struct aviutl2::utils::enum_utils::enable_bitmask<FileFlag> : std::true_type {};

// ファイルハンドル構造体
struct FileHandle {
    FileFlag flag;
    PAVIFILE pfile;
    PAVISTREAM pvideo, paudio;
    AVIFILEINFO fileinfo;
    AVISTREAMINFO videoinfo, audioinfo;
    void* videoformat;
    LONG videoformatsize;
    void* audioformat;
    LONG audioformatsize;
};

class AviReaderPlugin : public aviutl2::input::InputPlugin<AviReaderPlugin> {
  public:
    explicit AviReaderPlugin(token) {
        // 入力プラグインの情報を設定
        flag_ = InputFlag::VIDEO | InputFlag::AUDIO;
        name_ = L"AVI File Reader2 (sample)";
        filefilter_ = aviutl2::utils::make_file_filter({{L"AviFile (*.avi)", L"*.avi"}});
        information_ = L"Sample AVI File Reader2 version 2.01 By ＫＥＮくん";
    }

    // ファイルオープン
    InputHandle open_impl(LPCWSTR file) {
        FileHandle* fp = (FileHandle*)GlobalAlloc(GMEM_FIXED, sizeof(FileHandle));
        if (fp == NULL)
            return NULL;
        ZeroMemory(fp, sizeof(FileHandle));

        if (AVIFileOpen(&fp->pfile, file, OF_READ, nullptr) != S_OK) {
            GlobalFree(fp);
            return nullptr;
        }

        if (AVIFileInfo(fp->pfile, &fp->fileinfo, sizeof(fp->fileinfo)) == S_OK) {
            for (DWORD i = 0; i < fp->fileinfo.dwStreams; i++) {
                PAVISTREAM pas;
                if (AVIFileGetStream(fp->pfile, &pas, 0, i) == S_OK) {
                    AVISTREAMINFO asi;
                    AVIStreamInfo(pas, &asi, sizeof(asi));
                    if (asi.fccType == streamtypeVIDEO) {
                        //	ビデオストリームの設定
                        fp->pvideo = pas;
                        fp->videoinfo = asi;
                        AVIStreamFormatSize(fp->pvideo, 0, &fp->videoformatsize);
                        fp->videoformat = (FileHandle*)GlobalAlloc(GMEM_FIXED, fp->videoformatsize);
                        if (fp->videoformat) {
                            AVIStreamReadFormat(fp->pvideo, 0, fp->videoformat, &fp->videoformatsize);
                            fp->flag |= FileFlag::VIDEO;
                        } else {
                            AVIStreamRelease(pas);
                        }
                    } else if (asi.fccType == streamtypeAUDIO) {
                        //	オーディオストリームの設定
                        fp->paudio = pas;
                        fp->audioinfo = asi;
                        AVIStreamFormatSize(fp->paudio, 0, &fp->audioformatsize);
                        fp->audioformat = (FileHandle*)GlobalAlloc(GMEM_FIXED, fp->audioformatsize);
                        if (fp->audioformat) {
                            AVIStreamReadFormat(fp->paudio, 0, fp->audioformat, &fp->audioformatsize);
                            fp->flag |= FileFlag::AUDIO;
                        } else {
                            AVIStreamRelease(pas);
                        }
                    } else {
                        AVIStreamRelease(pas);
                    }
                }
            }
        }

        return fp;
    }

    // ファイルクローズ
    bool close_impl(InputHandle ih) {
        FileHandle* fp = (FileHandle*)ih;

        if (fp) {
            if (has_flag(fp->flag, FileFlag::AUDIO)) {
                AVIStreamRelease(fp->paudio);
                GlobalFree(fp->audioformat);
            }
            if (has_flag(fp->flag, FileFlag::VIDEO)) {
                AVIStreamRelease(fp->pvideo);
                GlobalFree(fp->videoformat);
            }
            AVIFileRelease(fp->pfile);
            GlobalFree(fp);
        }

        return true;
    }

    // ファイルの情報
    std::optional<InputInfo> get_input_info_impl(InputHandle handle) {
        FileHandle* fp = (FileHandle*)handle;
        InputInfo info;

        if (has_flag(fp->flag, FileFlag::VIDEO)) {
            info.flag |= aviutl2::input::InputInfoFlag::VIDEO;
            info.rate = fp->videoinfo.dwRate;
            info.scale = fp->videoinfo.dwScale;
            info.n = fp->videoinfo.dwLength;
            info.format = (BITMAPINFOHEADER*)fp->videoformat;
            info.format_size = fp->videoformatsize;
        }

        if (has_flag(fp->flag, FileFlag::AUDIO)) {
            info.flag |= aviutl2::input::InputInfoFlag::AUDIO;
            info.audio_n = fp->audioinfo.dwLength;
            info.audio_format = (WAVEFORMATEX*)fp->audioformat;
            info.audio_format_size = fp->audioformatsize;
        }

        return info;
    }

    // 画像読み込み
    int read_video_impl(InputHandle handle, int frame, void* buf) {
        FileHandle* fp = (FileHandle*)handle;

        LONG videosize, size;
        if (AVIStreamRead(fp->pvideo, frame, 1, NULL, NULL, &videosize, NULL) != S_OK)
            return 0;
        if (AVIStreamRead(fp->pvideo, frame, 1, buf, videosize, &size, NULL) != S_OK)
            return 0;
        return size;
    }

    // 音声読み込み
    int read_audio_impl(InputHandle handle, int start, int length, void* buf) {
        FileHandle* fp = (FileHandle*)handle;
        LONG size;
        int samplesize;

        samplesize = ((WAVEFORMATEX*)fp->audioformat)->nBlockAlign;
        if (AVIStreamRead(fp->paudio, start, length, buf, samplesize * length, NULL, &size) != S_OK)
            return 0;
        return size;
    }

    // 設定ダイアログ
    bool config_impl(HWND hwnd, HINSTANCE dll_hinst) {
        MessageBox(hwnd, L"サンプルダイアログ", L"入力設定", MB_OK);

        // DLLを開放されても設定が残るように保存しておいてください

        return true;
    }
};

AVIUTL2_REGISTER_INPUT_PLUGIN(AviReaderPlugin)
