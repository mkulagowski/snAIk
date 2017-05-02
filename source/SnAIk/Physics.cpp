#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

#include "Physics.hpp"
#include "snakeSolver.hpp"


Physics::Physics()
    : mBroadphase(new btDbvtBroadphase)
    ,mCollisionConfiguration(new btDefaultCollisionConfiguration)
    ,mDispatcher(new btCollisionDispatcher(mCollisionConfiguration))
    ,mSolver(new snakeSolver)
    ,mDynamicsWorld(new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver,
                                                mCollisionConfiguration))
{
}

Physics::~Physics()
{
    // Delete all pointers
    delete mDynamicsWorld;
    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfiguration;
    delete mBroadphase;
}

bool Physics::Init()
{
    if (mDynamicsWorld)
    {
        // Change gravity to Z axis (standard is -10 on Y)
        mDynamicsWorld->setGravity(btVector3(0, 0, -10));
        return true;
    }

    return false;
}

void Physics::AddObject(btRigidBody* object)
{
    if (mDynamicsWorld)
        mDynamicsWorld->addRigidBody(object);
}

void Physics::RemoveObject(btRigidBody* object)
{
    if (mDynamicsWorld)
        mDynamicsWorld->removeRigidBody(object);
}

// This function should be as fast as possible, so there is no pointer check!
void Physics::Update(double deltaTime)
{
    mDynamicsWorld->stepSimulation(static_cast<btScalar>(deltaTime));// , 7);
}

btDynamicsWorld* Physics::GetWorld()
{
    return mDynamicsWorld;
}