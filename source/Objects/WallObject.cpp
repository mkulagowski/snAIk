#include "WallObject.hpp"


WallObject::WallObject(btVector3 planeNormal)
    : PlaneObject(planeNormal)
{
    mType = ObjectType::Wall;
}

WallObject::~WallObject()
{
}
