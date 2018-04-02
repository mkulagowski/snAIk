#include "GameManager.hpp"
#include "FileSystem.hpp"


int main()
{
    // Navigate to project root directory
    FileSystem::ChangeDirectory(FileSystem::GetExecutableDir() + "/../../..");

	//API::GetInstance().Init(false);
	//API::GetInstance().Step(false);
	//return 0;
	
    // Start main loop
    if(GameManager::GetInstance().Init())
        GameManager::GetInstance().MainLoop();

    return 0;
}