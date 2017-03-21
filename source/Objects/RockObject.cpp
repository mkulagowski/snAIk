#include "RockObject.hpp"


RockObject::RockObject(btVector3 halfSize)
    : BoxObject(halfSize)
{
    mType = ObjectType::Obstacle;
}

RockObject::~RockObject()
{
}

CollisionResult RockObject::Collide(ObjectType otherObj)
{
    // Rocks do nothing on collisions
    return CollisionResult::None;
}