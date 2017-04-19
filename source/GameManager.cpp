#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GameManager.hpp"

constexpr float SEGMENT_WEIGHT = SEGMENT_LIMIT != 0 ? TOTAL_WEIGHT / static_cast<float>(SEGMENT_LIMIT) : 0;
constexpr float PI = 3.14159265358979323846f;
constexpr float PI2 = PI / 2.0f;
constexpr float PI4 = PI / 4.0f;
constexpr float PI6 = PI / 6.0f;
constexpr float PI12 = PI / 12.0f;

SegmentObject* mHead = nullptr;

GameManager::GameManager()
    : mWidth(1024)
    , mHeight(768)
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
    CreateSnake();

    return true;
}

void GameManager::MainLoop()
{
    std::string titleStr = "SnAIk head velocity = ";
    glfwSetWindowTitle(mGameWindow, titleStr.c_str());
    double deltaTime;
    mGameTimer.Start();

    do
    {
        // Get frame interval
        deltaTime = mGameTimer.Stop();
        mGameTimer.Start();

        // Show score in windows title
        glfwSetWindowTitle(mGameWindow, (titleStr + std::to_string(mHead->GetBody()->getLinearVelocity().length())).c_str());

        // Get players input
        GetPlayerInput(deltaTime);

        // Update physics world and check for collisions
        mPhysics.Update(deltaTime);

        // Draw all objects in renderer
        mRenderer.Draw();

        // Swap buffers and check events
        glfwSwapBuffers(mGameWindow);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(mGameWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(mGameWindow) == 0);
}


unsigned short GameManager::GetWidth()
{
    return mWidth;
}

unsigned short GameManager::GetHeight()
{
    return mHeight;
}

void GameManager::CreateSnake()
{
    if (SEGMENT_LIMIT <= 0)
        return;

    SegmentObject* bo;
    for (int i = 0; i < SEGMENT_LIMIT; i++)
    {
        bo = new SegmentObject(btVector3(1, 1, 1));
        bo->Init(btVector3(0, 1, 0), SEGMENT_WEIGHT);
        mRenderer.AddObject(bo);
        bo->Move(i * 1.0f, btVector3(1, 0, 0));
        mPhysics.AddObject(bo->GetBody());
        bo->GetBody()->forceActivationState(DISABLE_DEACTIVATION);
        mSegments.push_back(bo);
    }

    btConeTwistConstraint* coneC;
    btTransform localA, localB;
    btRigidBody *b1, *b2;
    for (int i = 0; i < SEGMENT_LIMIT - 1; i++)
    {
        mSegments[i]->Move(2.0f, btVector3(0, 0, 1));
        mSegments[i + 1]->Move(2.0f, btVector3(0, 0, 1));
        b1 = mSegments[i]->GetBody();
        b2 = mSegments[i + 1]->GetBody();

        localA.setIdentity();
        localB.setIdentity();

        localA.getBasis().setEulerZYX(0, 0, PI2); localA.setOrigin(btVector3(0.f, 1.f, 0.f));
        localB.getBasis().setEulerZYX(0, 0, PI2); localB.setOrigin(btVector3(0.f, -1.f, 0.f));

        coneC = new btConeTwistConstraint(*b1, *b2, localA, localB);
        coneC->setLimit(PI6, PI6, PI12,
                        1.f, .15f, .5f);

        mPhysics.GetWorld()->addConstraint(coneC, true);
    }

    mHead = mSegments[0];
    mHead->SetColor(btVector3(1, 1, 0));
}

void GameManager::GetPlayerInput(double deltaTime)
{
    btVector3 moveVect(0,0,0);
    float side = 0, forward = 0, upward = 0;
    int segmentIdx = 1;

    // Strafe right
    if (glfwGetKey(mGameWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        moveVect.setX(-MOVEMENT_SPEED);
    }

    // Strafe left
    if (glfwGetKey(mGameWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        moveVect.setX(+MOVEMENT_SPEED);
    }

    // Strafe left
    if (glfwGetKey(mGameWindow, GLFW_KEY_UP) == GLFW_PRESS)
    {
        moveVect.setZ(-MOVEMENT_SPEED);
    }
    // Strafe left
    if (glfwGetKey(mGameWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        moveVect.setZ(MOVEMENT_SPEED);
    }

    // Strafe left
    if (glfwGetKey(mGameWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        moveVect.setY(MOVEMENT_SPEED);
    }

    
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

    btRigidBody* body = mSegments[--segmentIdx]->GetBody();
    body->applyTorqueImpulse(body->getInvInertiaTensorWorld().inverse() * btVector3(upward, forward, side));
    //mHead->GetBody()->applyCentralForce(mHead->GetBody()->getWorldTransform().getBasis()*moveVect*100);
}