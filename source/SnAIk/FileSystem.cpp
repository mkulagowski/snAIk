#include <Windows.h>
#include <memory>

#include "FileSystem.hpp"


namespace FileSystem
{

std::string GetParentDir(const std::string& path)
{
    size_t found = path.find_last_of("/\\");
    return path.substr(0, found);
}

std::string GetExecutableDir()
{
    std::unique_ptr<TCHAR[]> execPath;
    DWORD sizeRead = 0;

    // Maximum length of a relative paths, available in Windows
    unsigned int len = MAX_PATH;

    // Maximum length of a path, available in Windows
    const unsigned int maxPathWide = 32768;

    for (; len < maxPathWide; len *= 2)
    {
        execPath.reset(new TCHAR[len]);
        sizeRead = ::GetModuleFileName(nullptr, execPath.get(), len);

        if (sizeRead < len && sizeRead != 0)
            break;
    }

    // Check if the buffer did not overflow
    if (len >= maxPathWide)
    {
        printf("FILESYSTEM: Failed to resolve executable's path.");
        return "";
    }

    return GetParentDir(execPath.get());
}

void ChangeDirectory(const std::string& dir)
{
    if (::SetCurrentDirectory(dir.c_str()) == 0)
    {
        printf("FILESYSTEM: Failed to change directory to '%s'.", dir.c_str());
        return;
    }
}
}