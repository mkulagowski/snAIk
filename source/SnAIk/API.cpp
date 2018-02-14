#include "API.hpp"
#include "FileSystem.hpp"
#include "GameManager.hpp"
#include <exception>
#include <fstream>
#define BOOST_PYTHON_STATIC_LIB
#include "boost\python\errors.hpp"

API& API::GetInstance()
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

API::SnakeMoveStruct API::GetMove()
{
    std::lock_guard<std::mutex> lock(mMoveMutex);
    if (mIsMoveAvailable)
    {
        mIsMoveAvailable = false;
        return mMove;
    }

    return SnakeMoveStruct();
}

void API::SetMove(const SnakeMoveStruct& move)
{
    std::lock_guard<std::mutex> lock(mMoveMutex);
    mMove = move;
    mIsMoveAvailable = true;
}

API::SnakeSnapshotStruct API::GetSnake()
{
    std::lock_guard<std::mutex> lock(mSnakeMutex);
    if (mIsSnakeAvailable)
    {
        mIsSnakeAvailable = false;
        return mSnake;
    }
	return SnakeSnapshotStruct();
}

void API::SetSnake(const Snake* snake)
{
    std::lock_guard<std::mutex> lock(mSnakeMutex);
    mSnake.mSegmentsNo = snake->GetSegmentsNumber();
    mSnake.mSegments.clear();
    for (int i = 0; i < mSnake.mSegmentsNo; ++i)
    {
        btVector3 rot = snake->GetRotation(i);
        btVector3 pos = snake->GetPosition(i);
		btVector3 trq = snake->GetTorque(i);
        mSnake.mSegments.push_back(SegmentSnapshotStruct(rot, pos, trq));
    }
    mIsSnakeAvailable = true;
}

const bool API::IsMoveAvailable() const
{
    return mIsMoveAvailable;
}

const bool API::IsSnakeAvailable() const
{
    return mIsSnakeAvailable;
}

void API::RunSimulation(int loopsNumber, bool render) const
{
	// Start main loop
	if (GameManager::GetInstance().Init(render)) {
		GameManager::GetInstance().MainLoop(loopsNumber, render);
	}
}

void API::Init(bool render) const
{
	if (GameManager::GetInstance().Init(render))
		GameManager::GetInstance().InitLoop();
}

void API::Step(bool render)
{
	GameManager::GetInstance().Step(render);
}