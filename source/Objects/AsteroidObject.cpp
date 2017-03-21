#include "AsteroidObject.hpp"


RockObject::RockObject(btVector3 halfSize)
    : BoxObject(halfSize)
{
    mType = ObjectType::Asteroid;
}

RockObject::~RockObject()
{
}

CollisionResult RockObject::Collide(ObjectType otherObj)
{
    switch (otherObj)
    {
        case ObjectType::Player:
            return CollisionResult::Reinit;
        case ObjectType::Projectile:
            mToDelete = true;
            return CollisionResult::Score;
        case ObjectType::Wall:
            mToDelete = true;
        default:
            return CollisionResult::None;
    }
}