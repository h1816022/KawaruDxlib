#include <DxLib.h>
#include <cassert>
#include "File.h"

File::~File()
{
    switch (type_)
    {
    case FILE_TYPE::Image:
        DeleteGraph(handle_);
        break;

    case FILE_TYPE::Sound:
        DeleteGraph(handle_);
        break;

    default:
        break;
    }
}

int File::GetHandle() const
{
    assert(handle_ != -1);

    return handle_;
}

File::File(int handle, FILE_TYPE type) :
    handle_(handle), type_(type)
{
}