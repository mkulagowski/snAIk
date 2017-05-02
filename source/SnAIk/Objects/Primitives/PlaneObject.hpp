#pragma once

#include "Object.hpp"


class PlaneObject : public Object
{
public:
    PlaneObject(btVector3 planeNormal);
    PlaneObject(btVector3 planeNormal, float halfSizeX, float halfSizeY);
    ~PlaneObject();

private:
    btVector3 mPlaneNormal;

    void AddShape() override;
};
