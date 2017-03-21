#include "GameManager.hpp"
#include "FileSystem.hpp"


int main()
{
    // Navigate to project root directory
    FileSystem::ChangeDirectory(FileSystem::GetExecutableDir() + "/../../..");

    // Start main loop
    if(GameManager::GetInstance().Init())
        GameManager::GetInstance().MainLoop();

    return 0;
}