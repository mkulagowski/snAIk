#include "Snake.hpp"

constexpr float PI = 3.14159265358979323846f;
constexpr float PI2 = PI / 2.0f;
constexpr float PI4 = PI / 4.0f;
constexpr float PI6 = PI / 6.0f;
constexpr float PI12 = PI / 12.0f;

Snake::Snake()
{
}

Snake::~Snake()
{
}

Snake::Snake(unsigned int segmentsNo, float totalWeight)
    : mSegmentsNo(segmentsNo)
    , mSegmentWeight(totalWeight / segmentsNo)
{

    if (mSegmentsNo != 0)
    {
        btVector3 segmentColor(0, 1, 0);
        btVector3 segmentSize(1, 1, 1);
        mSegments.reserve(mSegmentsNo);
        mConstraints.reserve(mSegmentsNo);
        for (unsigned int i = 0; i < mSegmentsNo; i++)
        {
            mSegments.emplace_back(segmentSize);
            SegmentObject& segment = mSegments.back();
            segment.Init(segmentColor, mSegmentWeight);
            segment.Move(i * 1.f, btVector3(1, 0, 0));
            segment.Move(1.f, btVector3(0, 0, 1));
            segment.GetBody()->forceActivationState(DISABLE_DEACTIVATION);
			segment.GetBody()->setAnisotropicFriction(btVector3(3, 4, 0));
        }

        btTransform localA, localB;
        btRigidBody *b1, *b2;
        for (unsigned int i = 0; i < mSegmentsNo - 1; i++)
        {
            b1 = mSegments[i].GetBody();
            b2 = mSegments[i + 1].GetBody();

            localA.setIdentity();
            localB.setIdentity();

            localA.getBasis().setEulerZYX(0, 0, PI2);
			localA.setOrigin(btVector3(0.f, 1.f, 0.f));

            localB.getBasis().setEulerZYX(0, 0, PI2);
			localB.setOrigin(btVector3(0.f, -1.f, 0.f));

            mConstraints.emplace_back(*b1, *b2, localA, localB);
            mConstraints.back().setLimit(PI6, PI6, PI12,
                                         1.f, .15f, .5f);
        }

        mSegments.front().SetColor(btVector3(1, 1, 0));
    }
}

int Snake::GetSegmentsNumber() const
{
    return mSegmentsNo;
}

float Snake::GetTotalWeight() const
{
    return mSegmentWeight * mSegmentsNo;
}

const SegmentObject * Snake::GetSegment(unsigned int segmentIndex) const
{
    if (segmentIndex >= mSegments.size())
        return nullptr;
    
    return &mSegments[segmentIndex];
}

const SegmentObject * Snake::GetHead() const
{
    return GetSegment(0);
}

const SegmentObject * Snake::GetTail() const
{
    return GetSegment(mSegmentsNo - 1);
}

btVector3 Snake::GetPosition(unsigned int segmentIndex) const
{
    const SegmentObject* obj = GetSegment(segmentIndex);
    if (obj == nullptr)
        return btVector3(0, 0, 0);

    return obj->GetPosition();
}

btVector3 Snake::GetAveragePosition() const
{
    btVector3 result(0, 0, 0);
    for (const auto& i : mSegments)
        result += i.GetPosition();
    
    return result / static_cast<btScalar>(mSegmentsNo);
}

const btConeTwistConstraint * Snake::GetConstraint(unsigned int constraintIndex) const
{
    if (constraintIndex >= mConstraints.size())
        return nullptr;

    return &mConstraints[constraintIndex];
}

void Snake::AddConstraint(unsigned int constraintIndex, btDynamicsWorld* world)
{
    if (constraintIndex >= mConstraints.size())
        return;

    world->addConstraint(&mConstraints[constraintIndex], true);
}

void Snake::RemoveAllConstraints(btDynamicsWorld* world)
{
	for (auto& i : mConstraints)
		world->removeConstraint(&i);
}

bool Snake::TurnSegment(unsigned int segmentIndex, btVector3 torque)
{
    if (segmentIndex >= mSegments.size())
        return false;

    mSegments[segmentIndex].ApplyTorque(torque * 100);

    return true;
}

btVector3 Snake::GetTorque(unsigned int segmentIndex) const
{
    const SegmentObject* obj = GetSegment(segmentIndex);
    if (obj == nullptr)
        return btVector3(0, 0, 0);

    return obj->GetRotation();
}
