#pragma once

#include "Object.hpp"


class PyramidObject : public Object
{
public:
    PyramidObject(btVector3 halfSize);
    PyramidObject(float halfSizeX, float halfSizeY, float halfSizeZ);
    ~PyramidObject();

private:
    btVector3 mHalfSize;

    void AddShape() override;
};
