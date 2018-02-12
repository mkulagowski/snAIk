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


#define MOVEMENT_SPEED 10.0f
#define SEGMENT_LIMIT 15
#define TOTAL_WEIGHT 5

class GameManager
{
public:
    static GameManager& GetInstance();
	bool Init();
    void MainLoop(int loopsNumber = 0, bool draw = true);
	void InitLoop();
	void Step(bool draw);

    const unsigned short GetWidth() const;
    const unsigned short GetHeight() const;
	void SetPhysicsSteps(unsigned int steps);

private:
    unsigned int mWidth, mHeight;
    unsigned int mScore;
    unsigned int mPhysicsSteps;
	double mDeltaTime;
    bool mHasInitialized;

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