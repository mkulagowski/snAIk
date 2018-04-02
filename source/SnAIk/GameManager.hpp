#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "Timer.hpp"
#include "Physics.hpp"
#include "Renderer.hpp"
#include "Snake.hpp"
#include "API.hpp"
#include "Objects/WallObject.hpp"
#include "Objects/SegmentObject.hpp"

class GameManager
{
public:
    static GameManager& GetInstance();
	bool Init(bool initRenderer = true);
    void MainLoop(int loopsNumber = 0, bool render = true);
	void InitLoop();
	void Step(bool render);

    const unsigned short GetWidth() const;
    const unsigned short GetHeight() const;
	void SetPhysicsSteps(unsigned int steps);
	const bool IsRendering() const;
	void tickCallback(btDynamicsWorld *world, btScalar timeStep);
private:
    unsigned int mWidth, mHeight;
    unsigned int mScore;
    unsigned int mPhysicsSteps;
	double mDeltaTime;
    bool mHasInitialized, mInitRenderer;

    std::unique_ptr<Snake> mSnake;
	API* mAPI;
	GLFWwindow* mGameWindow;
    Timer mGameTimer;
    Renderer mRenderer;
    Physics mPhysics;


    GameManager();
    ~GameManager();

    void GetPlayerInput(double deltaTime);
    void CreateSnake();
};