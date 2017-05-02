#include "gtest/gtest.h"
#include "../SnAIk/FileSystem.hpp"


int main(int argc, char* argv[])
{
    // Navigate to project root directory
    FileSystem::ChangeDirectory(FileSystem::GetExecutableDir() + "/../../..");

    // Start all tests
    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    return result;
}