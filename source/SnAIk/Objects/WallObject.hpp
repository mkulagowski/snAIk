#pragma once

#include "Primitives/PlaneObject.hpp"


class WallObject : public PlaneObject
{
public:
    WallObject(btVector3 mPlaneNormal);
    ~WallObject();
};
