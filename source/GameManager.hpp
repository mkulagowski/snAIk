#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Timer.hpp"
#include "Physics.hpp"
#include "Renderer.hpp"
#include "Objects/GameObjects.hpp"

#define SHOOTING_INTERVAL_SEC 0.25f
#define SHIP_MOVEMENT_SPEED 100.0f


class GameManager
{
public:
    static GameManager& GetInstance();
    bool Init();
    void MainLoop();

    unsigned short GetWidth();
    unsigned short GetHeight();

private:
    double mShootInterval;
    unsigned int mWidth, mHeight;
    unsigned int mScore;

    PlayerObject mPlayer;
    //WallObject mConstraints[4];
    std::forward_list<RockObject> mAsteroids;
    std::forward_list<ProjectileObject> mBullets;

    GLFWwindow* mGameWindow;
    Timer mGameTimer;
    Renderer mRenderer;
    Physics mPhysics;


    GameManager();
    ~GameManager();

    void Reinit();
    void CheckCollisions();
    void CheckAsteroids();
    void GetPlayerInput(double deltaTime);
    void UpdateObjectsArrays();
};