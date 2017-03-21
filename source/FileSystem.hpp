#pragma once

#include <string>


namespace FileSystem
{
std::string GetParentDir(const std::string& path);
std::string GetExecutableDir();
void ChangeDirectory(const std::string& dir);
}