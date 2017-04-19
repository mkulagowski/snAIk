#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Timer.hpp"
#include "Physics.hpp"
#include "Renderer.hpp"
#include "Snake.hpp"
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
    void MainLoop();

    unsigned short GetWidth();
    unsigned short GetHeight();

private:
    unsigned int mWidth, mHeight;
    unsigned int mScore;

    Snake* mSnake;
    GLFWwindow* mGameWindow;
    Timer mGameTimer;
    Renderer mRenderer;
    Physics mPhysics;


    GameManager();
    ~GameManager();

    void GetPlayerInput(double deltaTime);
    void CreateSnake();
};