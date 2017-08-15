#pragma once

#include "Snake.hpp"
#include "Objects/SegmentObject.hpp"
#include <mutex>
#include <forward_list>
#include <btBulletDynamicsCommon.h>

class API 
{
public:
    using SnakeMoveStruct = struct
    {
        int mSegment;
        float mTorque;
        btVector3 mDirection;
    };

    using SegmentSnapshotStruct = struct SegmentSnapshotStruct
    {
        btVector3 mRotation;
        btVector3 mPosition;
        SegmentSnapshotStruct(btVector3 rot, btVector3 pos)
            : mRotation(rot)
            , mPosition(pos)
        {};
    };

    using SnakeSnapshotStruct = struct
    {
        int mSegmentsNo;
        std::forward_list<SegmentSnapshotStruct> mSegments;
        btVector3 mAveragePosition;
    };

    static API& getInstance();

    SnakeMoveStruct getMove();
    void setMove(const SnakeMoveStruct& move);

    SnakeSnapshotStruct getSnake();
    void setSnake(const Snake& snake);

    const bool isMoveAvailable() const;
    const bool isSnakeAvailable() const;

private:
    bool mIsMoveAvailable;
    SnakeMoveStruct mMove;
    std::mutex mMoveMutex;
    bool mIsSnakeAvailable;
    SnakeSnapshotStruct mSnake;
    std::mutex mSnakeMutex;

    API();
    ~API();
};