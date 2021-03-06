#pragma once

#include <vector>
#include "Objects/SegmentObject.hpp"

class Snake
{
public:
    Snake();
    ~Snake();
    Snake(unsigned int segmentsNo, float totalWeight);

    int GetSegmentsNumber() const;
    float GetTotalWeight() const;
    const SegmentObject* GetSegment(unsigned int segmentIndex) const;
    const SegmentObject* GetHead() const;
    const SegmentObject* GetTail() const;
    btVector3 GetPosition(unsigned int segmentIndex) const;
    btVector3 GetAveragePosition() const;
	btVector3 GetAngularVelocity(unsigned int segmentIndex) const;
	btVector3 GetLinearVelocity(unsigned int segmentIndex) const;
	btVector3 GetRotation(unsigned int segmentIndex) const;

    const btGeneric6DofConstraint* GetConstraint(unsigned int constraintIndex) const;
    void AddConstraint(unsigned int constraintIndex, btDynamicsWorld* world);
	void RemoveAllConstraints(btDynamicsWorld* world);

    bool TurnSegment(unsigned int segmentIndex, btVector3 torque);

private:
    unsigned int mSegmentsNo;
    float mSegmentWeight;
    std::vector<SegmentObject> mSegments;
    std::vector<btGeneric6DofConstraint> mConstraints;
};