#include "ProjectileObject.hpp"


ProjectileObject::ProjectileObject()
    : PyramidObject(btVector3(0.5f, 0.5f, 1.5f))
{
    mType = ObjectType::Projectile;
}

ProjectileObject::~ProjectileObject()
{
}

CollisionResult ProjectileObject::Collide(ObjectType otherObj)
{
    switch (otherObj)
    {
        case ObjectType::Asteroid:
            mToDelete = true;
            return CollisionResult::Score;
        case ObjectType::Wall:
            mToDelete = true;
        default:
            return CollisionResult::None;
    }
}