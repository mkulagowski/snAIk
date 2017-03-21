#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GameManager.hpp"


GameManager::GameManager()
    : mWidth(1024)
    , mHeight(768)
    /*, mConstraints{
        WallObject(btVector3( 0, 0, 1)),
        WallObject(btVector3(-1, 0, 0)),
        WallObject(btVector3( 0, 0, 1)),
        WallObject(btVector3( 1, 0, 0))}*/
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
    mGameWindow = glfwCreateWindow(mWidth, mHeight, "11Bit Homework", NULL, NULL);
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
    if(!mRenderer.Init())
    {
        fprintf(stderr, "GAMEMANAGER: Failed to initialize Renderer module.\n");
        return false;
    }

    if(!mPhysics.Init())
    {
        fprintf(stderr, "GAMEMANAGER: Failed to initialize Physics module.\n");
        glfwTerminate();
        return false;
    }

    // Initialize and set up player
    mPlayer.Init(btVector3(0, 1, 0), 10.0f);
    mPlayer.GetBody()->setAngularFactor(0);
    mPlayer.Move(0.25f, btVector3(0, 0, 1));
    mPlayer.GetBody()->forceActivationState(DISABLE_DEACTIVATION);

    BoxObject* bo = new RockObject(btVector3(1,1,1));
    bo->Init(btVector3(1, 0, 0), 5.0f);
    mRenderer.AddObject(bo);
    bo->Move(1.0f, btVector3(0, 0, 1));
    mPhysics.AddObject(bo->GetBody());
    bo = new RockObject(btVector3(1, 1, 1));
    bo->Init(btVector3(1, 1, 0), 5.0f);
    mRenderer.AddObject(bo);
    bo->Move(0.5f, btVector3(0, 1, 1));
    mPhysics.AddObject(bo->GetBody());
    bo = new RockObject(btVector3(1, 1, 1));
    bo->Init(btVector3(1, 0, 1), 5.0f);
    mRenderer.AddObject(bo);
    bo->Move(0.5f, btVector3(1, 0, 1));

    PlaneObject* po = new PlaneObject(btVector3(0, 0, 1), 20.0f, 20.0f);
    po->Init(btVector3(1, 1, 1), 0.0f);
    mRenderer.AddObject(po);
    mPhysics.AddObject(po->GetBody());
    mPhysics.AddObject(bo->GetBody());

    return true;
}

void GameManager::MainLoop()
{
    std::string titleStr = "Arkanoid - Score ";
    double deltaTime;
    mGameTimer.Start();

    do
    {
        // Get frame interval
        deltaTime = mGameTimer.Stop();
        mGameTimer.Start();

        // Show score in windows title
        glfwSetWindowTitle(mGameWindow, (titleStr + std::to_string(mScore)).c_str());

        // Get players input
        //GetPlayerInput(deltaTime);

        // Update physics world and check for collisions
        mPhysics.Update(deltaTime);
        //CheckCollisions();

        // Draw all objects in renderer
        mRenderer.Draw();

        // Swap buffers and check events
        glfwSwapBuffers(mGameWindow);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(mGameWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(mGameWindow) == 0);
}

void GameManager::Reinit()
{
    // Make all objects ready to be removed from physics and renderer
    // by the UpdateObjectsArrays() method
    for (auto& i : mAsteroids)
        i.Collide(ObjectType::Wall);
    for (auto& i : mBullets)
        i.Collide(ObjectType::Wall);

    // Reset players position
    mPlayer.SetPosition(btVector3(0, 0, 0));
    mPlayer.Move(0.25f, btVector3(0, 0, 1));

    // Reset score and shooting delay
    mScore = 0;
    mShootInterval = 0;
}


unsigned short GameManager::GetWidth()
{
    return mWidth;
}

unsigned short GameManager::GetHeight()
{
    return mHeight;
}

void GameManager::GetPlayerInput(double deltaTime)
{
    // Reset players movement
    mPlayer.GetBody()->setLinearVelocity(btVector3(0, 0, 0));

    // Strafe right
    if (glfwGetKey(mGameWindow, GLFW_KEY_RIGHT) == GLFW_PRESS
        && mPlayer.GetPosition().getX() < 50.0f)
    {
        mPlayer.GetBody()->setLinearVelocity(btVector3(SHIP_MOVEMENT_SPEED, 0, 0));
    }

    // Strafe left
    if (glfwGetKey(mGameWindow, GLFW_KEY_LEFT) == GLFW_PRESS
        && mPlayer.GetPosition().getX() > -50.0f)
    {
        mPlayer.GetBody()->setLinearVelocity(btVector3(-SHIP_MOVEMENT_SPEED, 0, 0));
    }

    // Shoot a projectile
    mShootInterval -= deltaTime;
    if (glfwGetKey(mGameWindow, GLFW_KEY_SPACE) == GLFW_PRESS && mShootInterval <= 0)
    {
        // Create and init a projectile
        mBullets.push_front(ProjectileObject());
        ProjectileObject* bullet = &mBullets.front();
        bullet->Init(btVector3(1, 1, 0), 10.0f);

        // Move it just above the ship
        bullet->SetPosition(mPlayer.GetPosition());
        bullet->Move(0.5f, btVector3(0, 0, -1));

        // Set up movement and rotation and rotation axis
        bullet->GetBody()->setLinearVelocity(btVector3(0, 0, -20));
        bullet->GetBody()->setAngularVelocity(btVector3(0, 0, 10));
        bullet->GetBody()->setAngularFactor(btVector3(0, 0, 1));

        // Add projectile to renderer and physics world
        mRenderer.AddObject(bullet);
        mPhysics.AddObject(bullet->GetBody());

        // Reset shooting interval timer
        mShootInterval = SHOOTING_INTERVAL_SEC;
    }
}

void GameManager::CheckCollisions()
{
    // Get number of manifolds from the physics world
    btDynamicsWorld* world = mPhysics.GetWorld();
    int numManifolds = world->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; i++)
    {
        // If manifold has any contact with other body - it's a collision
        btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);

        int numContacts = contactManifold->getNumContacts();
        if (numContacts > 0)
        {
            // Get colliding bodies and cast them to Object pointers
            const btCollisionObject* bodyA = contactManifold->getBody0();
            const btCollisionObject* bodyB = contactManifold->getBody1();

            Object* objectA = reinterpret_cast<Object*>(bodyA->getUserPointer());
            Object* objectB = reinterpret_cast<Object*>(bodyB->getUserPointer());

            // Check collisions for each object colliding
            CollisionResult resultA = objectA->Collide(objectB->mType);
            CollisionResult resultB = objectB->Collide(objectA->mType);

            // Resolve collision effects
            if (resultA == CollisionResult::Score
                || resultB == CollisionResult::Score)
            {
                mScore++;
            }
            else if (resultA == CollisionResult::Reinit
                     || resultB == CollisionResult::Reinit)
            {
                Reinit();
                return;
            }
        }
    }
}

void GameManager::UpdateObjectsArrays()
{
    bool updateNeeded = false;

    // Iterate bullets and asteroids object arrays
    // if mToDelete is set -> remove object from physics world
    for (auto& i : mBullets)
    {
        if (i.ToDelete())
        {
            mPhysics.RemoveObject(i.GetBody());
            updateNeeded = true;
        }
    }

    for (auto& i : mAsteroids)
    {
        if (i.ToDelete())
        {
            mPhysics.RemoveObject(i.GetBody());
            updateNeeded = true;
        }
    }

    // If any object was deleted, Renderer and local arrays need update
    if (updateNeeded)
    {
        mRenderer.Update();
        mBullets.remove_if([](const ProjectileObject& i){ return i.ToDelete(); });
        mAsteroids.remove_if([](const RockObject& i){ return i.ToDelete(); });
    }
}

void GameManager::CheckAsteroids()
{
    // Calculate number of asteroids in the game
    unsigned int asteroidsNo;
    asteroidsNo = static_cast<unsigned int>(std::distance(mAsteroids.begin(),
                                                          mAsteroids.end()));

    // Number of asteroids is relative to the player's score
    if (mScore >= asteroidsNo)
    {
        // Randomize size and rotation
        float sizeX, sizeY, sizeZ, rotX, rotY, rotZ;
        sizeX = (std::rand() % 10) + 1.0f;
        sizeY = (std::rand() % 10) + 1.0f;
        sizeZ = (std::rand() % 10) + 1.0f;
        rotX = ((std::rand() % 24) - 12) / 4.0f;
        rotY = ((std::rand() % 24) - 12) / 4.0f;
        rotZ = ((std::rand() % 24) - 12) / 4.0f;

        // Randomize position on the upper end of the screen
        float position = ((std::rand() % 96) - 48.0f) / 8.0f;

        // Randomize falling deviation
        float deviation = ((std::rand() % 40) - 20.0f) / 10.0f;

        // Create asteroid and move it onto position
        mAsteroids.push_front(RockObject(btVector3(sizeX, sizeY, sizeZ)));
        RockObject* meteor = &mAsteroids.front();
        meteor->Init(btVector3(0, 0, 1), 10.0f);

        // Move it onto position
        meteor->Move(position, btVector3(1, 0, 0));
        meteor->Move(6.0f, btVector3(0, 0, -1));

        // Set movement and rotation
        meteor->GetBody()->setLinearVelocity(btVector3(deviation, 0, 10.0f));
        meteor->GetBody()->setAngularVelocity(btVector3(rotX, rotY, rotZ));

        // Add to renderer and physics world
        mRenderer.AddObject(meteor);
        mPhysics.AddObject(meteor->GetBody());
    }
}
