#pragma once
#include <btBulletDynamicsCommon.h>


class Physics
{
public:
    Physics();
    ~Physics();

    bool Init();
    void Update(double deltaTime);

    btDynamicsWorld* GetWorld();
    void AddObject(btRigidBody* object);
    void RemoveObject(btRigidBody* object);

private:
    btBroadphaseInterface* mBroadphase;
    btDefaultCollisionConfiguration* mCollisionConfiguration;
    btCollisionDispatcher* mDispatcher;
    btConstraintSolver* mSolver;
    btDiscreteDynamicsWorld* mDynamicsWorld;
};