#pragma once

#include <vector>
#include "Objects/SegmentObject.hpp"

class Snake
{
public:
    Snake();
    ~Snake();
    Snake(unsigned int segmentsNo, float totalWeight);

    int GetSegmentsNumber();
    float GetTotalWeight();
    SegmentObject* GetSegment(unsigned int segmentIndex);
    SegmentObject* GetHead();
    SegmentObject* GetTail();
    btVector3 GetPosition(unsigned int segmentIndex);
    btVector3 GetAveragePosition();
    btVector3 GetTorque(unsigned int segmentIndex);

    btConeTwistConstraint* GetConstraint(unsigned int constraintIndex);

    bool TurnSegment(unsigned int segmentIndex, btVector3 torque);

private:
    unsigned int mSegmentsNo;
    float mSegmentWeight;
    std::vector<SegmentObject> mSegments;
    std::vector<btConeTwistConstraint> mConstraints;
};