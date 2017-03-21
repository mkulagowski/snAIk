#pragma once

#include "Object.hpp"


class BoxObject : public Object
{
public:
    BoxObject(float x, float y, float z);
    BoxObject(btVector3 halfSize);
    ~BoxObject();

private:
    btVector3 mHalfSize;

    void AddShape() override;

};