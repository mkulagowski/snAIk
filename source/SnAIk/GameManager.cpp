#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GameManager.hpp"
#include "Defines.hpp"

const SegmentObject* mHead = nullptr;

GameManager::GameManager()
    : mWidth(1024)
    , mHeight(768)
    , mHasInitialized(false)
    , mPhysicsSteps(1)
	, mAPI(&API::GetInstance())
	, mInitRenderer(true)
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

static void _tickCallback(btDynamicsWorld* world, btScalar timeStep)
{
	GameManager::GetInstance().tickCallback(world, timeStep);
}

bool GameManager::Init(bool initRenderer)
{
    if (!mHasInitialized)
    {
		mInitRenderer = initRenderer;
		if (mInitRenderer)
		{
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
		}
        if (!mPhysics.Init())
        {
            fprintf(stderr, "GAMEMANAGER: Failed to initialize Physics module.\n");
            glfwTerminate();
			return false;
        }

		// Create floor
        PlaneObject* floor = new PlaneObject(btVector3(0, 0, 1), 100.0f * SCALE, 100.0f * SCALE);
		floor->Init(btVector3(1, 1, 1), 0.0f);
		mRenderer.AddObject(floor);
		floor->GetBody()->setFriction(1);
		mPhysics.AddObject(floor->GetBody());

		// Create floor limit
		PlaneObject* floor2 = new PlaneObject(btVector3(0, 0, 1), 101.0f * SCALE, 101.0f * SCALE);
		floor2->Init(btVector3(1, 0, 0), 0.0f);
		mRenderer.AddObject(floor2);
		floor2->GetBody()->setFriction(1);
		floor2->Move(.1f * SCALE, btVector3(0, 0, -1));

		mPhysics.GetWorld()->setInternalTickCallback(_tickCallback);

        mHasInitialized = true;
    }
    return true;
}

void GameManager::tickCallback(btDynamicsWorld *world, btScalar timeStep)
{
	if (mSnake)
	{
		int limit = mSnake->GetSegmentsNumber();
		for (int i = 0; i < limit; ++i)
		{
			const SegmentObject* segment = mSnake->GetSegment(i);
			btVector3 velocity = segment->GetAngularVelocity();
			btScalar speed = velocity.length();
			if (speed > MAX_ANGULAR_SPEED) {
				velocity *= MAX_ANGULAR_SPEED / speed;
				segment->GetBody()->setAngularVelocity(velocity);
			}
		}
	}
}

void GameManager::MainLoop(int loopsNumber, bool render)
{
	CreateSnake();
	if (mInitRenderer) {
		glfwSetWindowTitle(mGameWindow, "SnAIk");
	}
    double deltaTime;
    mGameTimer.Start();

	// Loop management variables
	loopsNumber = loopsNumber > 0 ? loopsNumber: 0;
	const int changeVal = loopsNumber > 0 ? 1 : 0;
	int counter = 0;

	const auto loopLambda = [&]() -> bool {
		if (mInitRenderer)
			return glfwGetKey(mGameWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(mGameWindow) == 0;
		return true;
	};

	do
	{
		// Get frame interval
		deltaTime = mGameTimer.Stop();
		mGameTimer.Start();

		// Get players input
		if (render)
			GetPlayerInput(deltaTime);
		//printf("%f \t || %f\n", mHead->GetAngularVelocity().length(), mHead->GetBody()->getLinearVelocity().length());
		// Update physics world and check for collisions
		mPhysics.Update(mPhysicsSteps * deltaTime);

		if (render)
		{
			// Draw all objects in renderer
			mRenderer.Draw();

			// Swap buffers and check events
			glfwSwapBuffers(mGameWindow);
			glfwPollEvents();
		}

		mAPI->SetSnake(mSnake.get());
		if (mAPI->IsMoveAvailable())
		{
			for (const auto& move : mAPI->GetMove()) {
				btVector3 torque(move.mTorque.x, move.mTorque.y, move.mTorque.z);
				mSnake->TurnSegment(move.mSegment, torque);
			}
		}

		counter += changeVal;
		if (counter > loopsNumber)
			break;

	} while (loopLambda());
}

void GameManager::InitLoop()
{
	CreateSnake();
	if (mInitRenderer) {
		glfwSetWindowTitle(mGameWindow, "SnAIk");
	}
	mGameTimer.Start();
}

void GameManager::Step(bool render)
{
	// Get frame interval
	mDeltaTime = mGameTimer.Stop();
	mGameTimer.Start();

	// Update physics world and check for collisions
	mPhysics.Update(mPhysicsSteps * mDeltaTime);

	if (render)
	{
		// Draw all objects in renderer
		mRenderer.Draw();

		// Swap buffers and check events
		glfwSwapBuffers(mGameWindow);
		glfwPollEvents();
	}

	mAPI->SetSnake(mSnake.get());
	if (mAPI->IsMoveAvailable())
	{
		for (const auto& move : mAPI->GetMove()) {
			btVector3 torque(move.mTorque.x, move.mTorque.y, move.mTorque.z);
			mSnake->TurnSegment(move.mSegment, torque);
		}
	}
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
		if (mInitRenderer)
			mRenderer.RemoveAllByTag(SNAKE_TAG);
		mHead = nullptr;
	}

    mSnake = std::make_unique<Snake>(SEGMENT_LIMIT, TOTAL_WEIGHT);
    for (int i = 0; i < SEGMENT_LIMIT; i++)
    {
        const SegmentObject* segment = mSnake->GetSegment(i);
		if (mInitRenderer)
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


	if (glfwGetKey(mGameWindow, GLFW_KEY_Z) == GLFW_PRESS)
	{
		mRenderer.MoveCamera(btVector3(0, 0, 1 * SCALE));
	}
	if (glfwGetKey(mGameWindow, GLFW_KEY_X) == GLFW_PRESS)
	{
		mRenderer.MoveCamera(btVector3(0, 0, -1 * SCALE));
	}

	if (glfwGetKey(mGameWindow, GLFW_KEY_C) == GLFW_PRESS)
	{
		mRenderer.ChangeLightPower(10);
	}
	if (glfwGetKey(mGameWindow, GLFW_KEY_V) == GLFW_PRESS)
	{
		mRenderer.ChangeLightPower(-10);
	}

    btVector3 torque(upward, forward, side);
    mSnake->TurnSegment(--segmentIdx, torque);
}

void GameManager::SetPhysicsSteps(unsigned int steps)
{
	if (steps > 0)
		mPhysicsSteps = steps;
}

const bool GameManager::IsRendering() const
{
	return mInitRenderer;
}