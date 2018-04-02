#include "Snake.hpp"
#include "Defines.hpp"

constexpr float PI = 3.14159265358979323846f;
constexpr float PI2 = PI / 2.0f;
constexpr float PI4 = PI / 4.0f;
constexpr float PI6 = PI / 6.0f;
constexpr float PI12 = PI / 12.0f;
constexpr float PI24 = PI / 24.0f;

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
        btVector3 segmentSize(1, 1, 1); ///< in meters
		segmentSize *= SCALE;
        mSegments.reserve(mSegmentsNo);
        mConstraints.reserve(mSegmentsNo);
        for (unsigned int i = 0; i < mSegmentsNo; i++)
        {
            mSegments.emplace_back(segmentSize);
            SegmentObject& segment = mSegments.back();
            segment.Init(segmentColor, mSegmentWeight);
			segment.Move(i * segmentSize.getX() , btVector3(1, 0, 0));
            segment.Move(segmentSize.getZ(), btVector3(0, 0, 1));
            segment.GetBody()->forceActivationState(DISABLE_DEACTIVATION);
			segment.GetBody()->setAnisotropicFriction(btVector3(2, 2, 1)); // X - lateral, Y - dorsal-ventral
			segment.GetBody()->setAngularFactor(1);
        }

        btTransform localA, localB;
        btRigidBody *bA, *bB;
        for (unsigned int i = 0; i < mSegmentsNo - 1; i++)
        {
			bA = mSegments[i + 1].GetBody();
			bB = mSegments[i].GetBody();

            localA.setIdentity();
            localB.setIdentity();

			localA.setOrigin(btVector3(0.f, -segmentSize.getY(), 0.f));
			localB.setOrigin(btVector3(0.f, segmentSize.getY(), 0.f));

            mConstraints.emplace_back(*bA, *bB, localA, localB, false);
			mConstraints.back().setLinearLowerLimit(btVector3(0, 0, 0));
			mConstraints.back().setLinearUpperLimit(btVector3(0, 0, 0));
			mConstraints.back().setAngularLowerLimit(btVector3(-PI4, -PI24, -PI4));
			mConstraints.back().setAngularUpperLimit(btVector3(PI4, PI24, PI4));
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

const btGeneric6DofConstraint * Snake::GetConstraint(unsigned int constraintIndex) const
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

	float torqueLen = torque.length();
	if (torqueLen > MAX_TORQUE)
		mSegments[segmentIndex].ApplyTorque(torque * (MAX_TORQUE / torqueLen));
	else
		mSegments[segmentIndex].ApplyTorque(torque);

    return true;
}

btVector3 Snake::GetAngularVelocity(unsigned int segmentIndex) const
{
	const SegmentObject* obj = GetSegment(segmentIndex);
	if (obj == nullptr) {
		return btVector3(0, 0, 0);
	}

	return obj->GetAngularVelocity();
}

btVector3 Snake::GetLinearVelocity(unsigned int segmentIndex) const
{
	const SegmentObject* obj = GetSegment(segmentIndex);
	if (obj == nullptr) {
		return btVector3(0, 0, 0);
	}

	return obj->GetLinearVelocity();
}

btVector3 Snake::GetRotation(unsigned int segmentIndex) const
{
	const SegmentObject* obj = GetSegment(segmentIndex);
	if (obj == nullptr)
		return btVector3(0, 0, 0);

	return obj->GetRotation();
}
