#include "SegmentObject.hpp"


SegmentObject::SegmentObject(btVector3 halfSize)
    : BoxObject(halfSize)
{
	mTag = SNAKE_TAG;
}

SegmentObject::~SegmentObject()
{
}