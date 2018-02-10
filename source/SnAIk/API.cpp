#include "API.hpp"
#include "FileSystem.hpp"
#include "GameManager.hpp"

API& API::getInstance()
{
    static API instance;
    return instance;
}

API::API()
    : mIsMoveAvailable(false)
    , mIsSnakeAvailable(false)
{
	// Navigate to project root directory
	FileSystem::ChangeDirectory(FileSystem::GetExecutableDir() + "/../../..");
}

API::~API()
{
}

API::SnakeMoveStruct API::getMove()
{
    std::lock_guard<std::mutex> lock(mMoveMutex);
    if (mIsMoveAvailable)
    {
        mIsMoveAvailable = false;
        return mMove;
    }

    return SnakeMoveStruct();
}

void API::setMove(const SnakeMoveStruct& move)
{
    std::lock_guard<std::mutex> lock(mMoveMutex);
    mMove = move;
    mIsMoveAvailable = true;
}

API::SnakeSnapshotStruct API::getSnake()
{
    std::lock_guard<std::mutex> lock(mSnakeMutex);
    if (mIsSnakeAvailable)
    {
        mIsSnakeAvailable = false;
        return mSnake;
    }
	return SnakeSnapshotStruct();
}

void API::setSnake(const Snake* snake)
{
    std::lock_guard<std::mutex> lock(mSnakeMutex);
    mSnake.mSegmentsNo = snake->GetSegmentsNumber();
    mSnake.mSegments.clear();
    for (int i = 0; i < mSnake.mSegmentsNo; ++i)
    {
        btVector3 rot = snake->GetTorque(i);
        btVector3 pos = snake->GetPosition(i);
        mSnake.mSegments.push_back(SegmentSnapshotStruct(rot, pos));
    }
    mIsSnakeAvailable = true;
}

const bool API::isMoveAvailable() const
{
    return mIsMoveAvailable;
}

const bool API::isSnakeAvailable() const
{
    return mIsSnakeAvailable;
}

void API::runSimulation(int loopsNumber, bool draw) const
{
	// Start main loop
	if (GameManager::GetInstance().Init()) {
		GameManager::GetInstance().MainLoop(loopsNumber, draw);
	}
}