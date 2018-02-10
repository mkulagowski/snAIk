#pragma once

#include "Primitives/BoxObject.hpp"
#define SNAKE_TAG 13

class SegmentObject : public BoxObject
{
public:
    SegmentObject(btVector3 halfSize);
    ~SegmentObject();
};
