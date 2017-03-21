#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

#include "Physics.hpp"


Physics::Physics()
    : mBroadphase(new btDbvtBroadphase)
    ,mCollisionConfiguration(new btDefaultCollisionConfiguration)
    ,mDispatcher(new btCollisionDispatcher(mCollisionConfiguration))
    ,mSolver(new btSequentialImpulseConstraintSolver)
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
        // Set no gravity
        mDynamicsWorld->setGravity(btVector3(0, 0, -10));
        return true;
    }

    return false;
}

void Physics::AddObject(btRigidBody* object)
{
    mDynamicsWorld->addRigidBody(object);
}

void Physics::RemoveObject(btRigidBody* object)
{
    mDynamicsWorld->removeRigidBody(object);
}

void Physics::Update(double deltaTime)
{
    mDynamicsWorld->stepSimulation(static_cast<btScalar>(deltaTime), 7);
}

btDynamicsWorld* Physics::GetWorld()
{
    return mDynamicsWorld;
}