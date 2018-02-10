#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GameManager.hpp"

const SegmentObject* mHead = nullptr;

GameManager::GameManager()
    : mWidth(1024)
    , mHeight(768)
    , mHasInitialized(false)
    , mPhysicsSteps(10)
	, mAPI(&API::getInstance())
{
}

GameManager::~GameManager()
{
    // Only glfw needs termination, lists will clear itself
    glfwTerminate();
}

GameManager& GameManager::GetInstance()
{
    static GameManager instance;
    return instance;
}

bool GameManager::Init()
{
    if (!mHasInitialized)
    {
        // Initialize rand() function for asteroids randomization
        srand(static_cast<unsigned int>(time(NULL)));

        // Initialize glfw
        if (!glfwInit())
        {
            fprintf(stderr, "GAMEMANAGER: Failed to initialize GLFW\n");
            return false;
        }

        // Set up window
        glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

        // Create window
        mGameWindow = glfwCreateWindow(mWidth, mHeight, "SnAIk", NULL, NULL);
        if (mGameWindow == NULL)
        {
            fprintf(stderr, "GAMEMANAGER: Failed to open GLFW window. If you have an Intel GPU, they are not OGL3.3 compatible.\n");
            return false;
        }

        // Create context for ogl
        glfwMakeContextCurrent(mGameWindow);

        // Enable players input
        glfwSetInputMode(mGameWindow, GLFW_STICKY_KEYS, GL_TRUE);

        // Initialize renderer and physics world
        if (!mRenderer.Init())
        {
            fprintf(stderr, "GAMEMANAGER: Failed to initialize Renderer module.\n");
			return false;
        }

        if (!mPhysics.Init())
        {
            fprintf(stderr, "GAMEMANAGER: Failed to initialize Physics module.\n");
            glfwTerminate();
			return false;
        }

        // Create floor
        PlaneObject* floor = new PlaneObject(btVector3(0, 0, 1), 100.0f, 100.0f);
        floor->Init(btVector3(1, 1, 1), 0.0f);
        mRenderer.AddObject(floor);
        mPhysics.AddObject(floor->GetBody());

        // Create snake
        //CreateSnake();
        mHasInitialized = true;
    }
    return true;
}

void GameManager::MainLoop(int loopsNumber, bool draw)
{
	CreateSnake();
    std::string titleStr = "SnAIk head velocity = ";
    glfwSetWindowTitle(mGameWindow, titleStr.c_str());
    double deltaTime;
    mGameTimer.Start();
    btVector3 torque;
    std::string text;

	// Loop management variables
	loopsNumber = loopsNumber > 0 ? loopsNumber: 0;
	const int changeVal = loopsNumber > 0 ? 1 : 0;
	int counter = 0;

	do
	{
		// Get frame interval
		deltaTime = mGameTimer.Stop();
		mGameTimer.Start();

		// Show score in windows title
		//glfwSetWindowTitle(mGameWindow, (titleStr + std::to_string(mHead->GetBody()->getLinearVelocity().length())).c_str());
		torque = mSnake->GetTorque(0);
		text = titleStr + "[ " + std::to_string(torque.x()) + ", " + std::to_string(torque.y()) + ", " + std::to_string(torque.z()) + " ]";
		glfwSetWindowTitle(mGameWindow, text.c_str());

		// Get players input
		if (draw)
			GetPlayerInput(deltaTime);

		// Update physics world and check for collisions
		mPhysics.Update(mPhysicsSteps * deltaTime);

		if (draw)
		{
			// Draw all objects in renderer
			mRenderer.Draw();

			// Swap buffers and check events
			glfwSwapBuffers(mGameWindow);
			glfwPollEvents();
		}

		mAPI->setSnake(mSnake.get());
		if (mAPI->isMoveAvailable())
		{
			API::SnakeMoveStruct move = mAPI->getMove();
			btVector3 direction(move.mDirection.x, move.mDirection.y, move.mDirection.z);
			mSnake->TurnSegment(move.mSegment, direction * move.mTorque);
		}

		counter += changeVal;
		if (counter > loopsNumber)
			break;

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(mGameWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(mGameWindow) == 0);
}


const unsigned short GameManager::GetWidth() const
{
    return mWidth;
}

const unsigned short GameManager::GetHeight() const
{
    return mHeight;
}

void GameManager::CreateSnake()
{
	if (SEGMENT_LIMIT <= 0)
		return;

	if (mSnake)
	{
		int limit = mSnake->GetSegmentsNumber();
		for (int i = 0; i < limit; ++i)
		{
			const SegmentObject* segment = mSnake->GetSegment(i);
			mPhysics.RemoveObject(segment->GetBody());
		}
		mSnake->RemoveAllConstraints(mPhysics.GetWorld());
		mRenderer.RemoveAllByTag(SNAKE_TAG);
		mHead = nullptr;
	}

    mSnake = std::make_unique<Snake>(SEGMENT_LIMIT, TOTAL_WEIGHT);
    for (int i = 0; i < SEGMENT_LIMIT; i++)
    {
        const SegmentObject* segment = mSnake->GetSegment(i);
        mRenderer.AddObject(segment);
        mPhysics.AddObject(segment->GetBody());

        if (i != (SEGMENT_LIMIT - 1))
            mSnake->AddConstraint(i, mPhysics.GetWorld());
    }

    mHead = mSnake->GetHead();
}

void GameManager::GetPlayerInput(double deltaTime)
{
    float side = 0, forward = 0, upward = 0;
    int segmentIdx = 1;
    
    if (glfwGetKey(mGameWindow, GLFW_KEY_Q) == GLFW_PRESS)
    {
        side = MOVEMENT_SPEED;
        segmentIdx = 1;
    }
    if (glfwGetKey(mGameWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        side = -MOVEMENT_SPEED;
        segmentIdx = 1;
    }


    if (glfwGetKey(mGameWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        side = MOVEMENT_SPEED;
        segmentIdx = (SEGMENT_LIMIT * 2) / 5;
    }
    if (glfwGetKey(mGameWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        side = -MOVEMENT_SPEED;
        segmentIdx = (SEGMENT_LIMIT * 2) / 5;
    }


    if (glfwGetKey(mGameWindow, GLFW_KEY_E) == GLFW_PRESS)
    {
        side = MOVEMENT_SPEED;
        segmentIdx = (SEGMENT_LIMIT * 3) / 5;
    }
    if (glfwGetKey(mGameWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        side = -MOVEMENT_SPEED;
        segmentIdx = (SEGMENT_LIMIT * 3) / 5;
    }


    if (glfwGetKey(mGameWindow, GLFW_KEY_R) == GLFW_PRESS)
    {
        side = MOVEMENT_SPEED;
        segmentIdx = (SEGMENT_LIMIT * 4) / 5;
    }
    if (glfwGetKey(mGameWindow, GLFW_KEY_F) == GLFW_PRESS)
    {
        side = -MOVEMENT_SPEED;
        segmentIdx = (SEGMENT_LIMIT * 4) / 5;
    }


    if (glfwGetKey(mGameWindow, GLFW_KEY_T) == GLFW_PRESS)
    {
        side = MOVEMENT_SPEED;
        segmentIdx = (SEGMENT_LIMIT * 5) / 5;
    }
    if (glfwGetKey(mGameWindow, GLFW_KEY_G) == GLFW_PRESS)
    {
        side = -MOVEMENT_SPEED;
        segmentIdx = (SEGMENT_LIMIT * 5) / 5;
    }

    btVector3 torque(upward, forward, side);
    mSnake->TurnSegment(--segmentIdx, torque);
}

void GameManager::SetPhysicsSteps(unsigned int steps)
{
	if (steps > 0)
		mPhysicsSteps = steps;
}