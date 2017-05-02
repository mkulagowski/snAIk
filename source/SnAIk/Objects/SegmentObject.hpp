#pragma once

#include "Primitives/BoxObject.hpp"


class SegmentObject : public BoxObject
{
public:
    SegmentObject(btVector3 halfSize);
    ~SegmentObject();
};
