#pragma once

#include "Snake.hpp"
#include "Objects/SegmentObject.hpp"
#include <mutex>
#include <vector>
#include <btBulletDynamicsCommon.h>

class API 
{
public:
	using VectStruct = struct VectStruct
	{
		float x, y, z;
		VectStruct(float xx, float yy, float zz)
			: x(xx)
			, y(yy)
			, z(zz)
		{};
		VectStruct(btVector3 vect)
			: x(vect.x())
			, y(vect.y())
			, z(vect.z())
		{};
		VectStruct() {};
		bool operator==(const VectStruct& rhs)
		{
			return x == rhs.x && y == rhs.y && z == rhs.z;
		}
	};

    using SnakeMoveStruct = struct
    {
	public:
		int mSegment;
		VectStruct mTorque;
    };

    using SegmentSnapshotStruct = struct SegmentSnapshotStruct
    {
		VectStruct mRotation;
		VectStruct mPosition;
		VectStruct mAngularVelocity;
		float mLinearSpeed;
		SegmentSnapshotStruct(VectStruct rot, VectStruct pos, VectStruct av, float speed)
			: mRotation(rot)
			, mPosition(pos)
			, mAngularVelocity(av)
			, mLinearSpeed(speed)
		{};
		SegmentSnapshotStruct(VectStruct rot, VectStruct pos)
			: mRotation(rot)
			, mPosition(pos)
		{};
		SegmentSnapshotStruct(){};
		bool operator==(const SegmentSnapshotStruct& rhs)
		{
			return mRotation == rhs.mRotation && mPosition == rhs.mPosition && mAngularVelocity == rhs.mAngularVelocity && mLinearSpeed == rhs.mLinearSpeed;
		}
    };

	using SegmentList = std::vector<SegmentSnapshotStruct>;
    using SnakeSnapshotStruct = struct
    {
        int mSegmentsNo;
		SegmentList mSegments;
    };

    static API& GetInstance();

	std::vector<SnakeMoveStruct> GetMove();
	void AddMove(const SnakeMoveStruct& move);
    void SetMove(const std::vector<SnakeMoveStruct>& move);
	//void SetMove( move);
	SnakeSnapshotStruct GetSnake();
    void SetSnake(const Snake* snake);

    const bool IsMoveAvailable() const;
    const bool IsSnakeAvailable() const;

	void RunSimulation(int loopsNumber = 0, bool draw = true) const;
	void Init(bool render) const;
	void Step(bool render);

private:
    bool mIsMoveAvailable;
	std::vector<SnakeMoveStruct> mMove;
    std::mutex mMoveMutex;
    bool mIsSnakeAvailable;
    SnakeSnapshotStruct mSnake;
    std::mutex mSnakeMutex;

    API();
    ~API();
};