#include "gtest/gtest.h"
#include "../SnAIk/FileSystem.hpp"


TEST(FileSystemTest, ExeDirTest)
{
    ASSERT_GT(FileSystem::GetExecutableDir().size(), 0);
}

TEST(FileSystemTest, ParentDirTest)
{
    std::string exeDir = FileSystem::GetExecutableDir();
    std::string parentDir = FileSystem::GetParentDir(exeDir);
    ASSERT_GT(exeDir.size(), 0);
    ASSERT_NE(exeDir, parentDir);
}