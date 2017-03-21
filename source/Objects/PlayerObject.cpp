#include "PlayerObject.hpp"


PlayerObject::PlayerObject()
    : BoxObject(btVector3(1, 1, 3))
{
    mType = ObjectType::Player;
    mVelocity = 100.0f;
}

PlayerObject::~PlayerObject()
{
}

CollisionResult PlayerObject::Collide(ObjectType otherObj)
{
    switch (otherObj)
    {
        case ObjectType::Asteroid:
            return CollisionResult::Reinit;
        default:
            return CollisionResult::None;
    }
}