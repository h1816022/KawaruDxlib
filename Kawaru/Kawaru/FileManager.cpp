#include <DxLib.h>
#include <cassert>
#include <algorithm>
#include <cassert>
#include <functional>
#include "FileManager.h"
#include "File.h"

FileManager::~FileManager() = default;

namespace
{
    /// <summary>
    /// 拡張子を返す
    /// </summary>
    /// <param name="filePath">ファイルのパス</param>
    /// <returns>拡張子文字列</returns>
    std::wstring GetExtension(const std::wstring& filePath)
    {
        auto idx = filePath.rfind(L'.');

        assert(idx != std::wstring::npos);

        ++idx;

        return filePath.substr(idx, filePath.length() - idx);
    }

    struct HandleAndType
    {
        int handle;
        File::FILE_TYPE type;
    };

    std::unordered_map<std::wstring, std::function<HandleAndType(const wchar_t*)>> loader_;
}

void FileManager::Update()
{
    if (delaySounds_.size() == 0)
    {
        return;
    }

    for (auto& s : delaySounds_)
    {
        ++s.nowTime;

        if (s.nowTime >= s.delayTime)
        {
            PlaySoundMem(s.soundHandle, DX_PLAYTYPE_BACK);
        }
    }

    delaySounds_.remove_if([](DelaySoundData data) {return data.nowTime >= data.delayTime; });
}

std::shared_ptr<File> FileManager::Load(const wchar_t* filePath)
{
    assert(DxLib_IsInit());

    if (resourceTable_.count(filePath) == 0)
    {
        auto ext = GetExtension(filePath);
        auto file = loader_[ext](filePath);

        resourceTable_[filePath].reset(new File(file.handle, file.type));
    }

    return resourceTable_[filePath];
}

void FileManager::DeleteAllResources()
{
    resourceTable_.clear();
}

void FileManager::DelayPlaySound(int handle, int delayTime)
{
    delaySounds_.emplace_back(DelaySoundData(handle, delayTime));
}

FileManager::FileManager()
{
    loader_[L"png"] = loader_[L"jpg"] = [](const wchar_t* path)->HandleAndType
    {
        return { LoadGraph(path), File::FILE_TYPE::Image };
    };

    loader_[L"wav"] = loader_[L"mp3"] = [](const wchar_t* path)->HandleAndType
    {
        return { LoadSoundMem(path), File::FILE_TYPE::Sound };
    };
}
