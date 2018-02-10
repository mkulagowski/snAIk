#!/usr/bin/env python

import SnAIk

def snaikToList(snake):
    segs = snake.segments
    retList = []
    for v in sk.segments:
        retList.append(v.rotation.x)
        retList.append(v.rotation.y)
        retList.append(v.rotation.z)
        retList.append(v.position.x)
        retList.append(v.position.y)
        retList.append(v.position.z)
    return retList

def listToMove(moveList):
    move = SnAIk.SnakeMove()
    for moves in moveList:
        move.segment = moves[0]
        move.torque = moves[1]
        vect = SnAIk.VectStruct()
        vect.x = moves[2]
        vect.y = moves[3]
        vect.z = moves[4]
        move.direction = vect
    return move

def emptyToNone(move):
    if not isinstance(move, SnAIk.SnakeMove):
        print("wrong type of arg!")
        return None;
    if move.segment == 0 and move.torque == 0 and move.direction.x == 0 and move.direction.y == 0 and move.direction.z == 0:
        move = None
    return move

sn = SnAIk.API.getInstance()
print(sn)
#sn.runSimulation()

snap = SnAIk.SnakeMove()
print(snap)
print(snap.segment)
print(snap.torque)
print(snap.direction)
print(snap.direction.x)
print(snap.direction.y)
print(snap.direction.z)

sk = sn.getSnake();
print(sk)
print(sk.segmentsNo)
print(sk.averagePosition)
print(sk.segments)
for i,v in enumerate(sk.segments):
    print('seg[{}] = {}'.format(i, v))

print("now check conversion:")
print(snaikToList(sk))

movess = [[1, 4.231, 2, 3, 5],[3, 2.123, 3, 0, 9]]
snap = listToMove(movess)
print(movess)
print(snap)
print(snap.segment)
print(snap.torque)
print(snap.direction)
print(snap.direction.x)
print(snap.direction.y)
print(snap.direction.z)

print("get move:")
snap = emptyToNone(sn.getMove())
if snap is not None:
    print(snap)
    print(snap.segment)
    print(snap.torque)
    print(snap.direction)
    print(snap.direction.x)
    print(snap.direction.y)
    print(snap.direction.z)

print(sn.isMoveAvailable())
sn.setMove(listToMove(movess))
print(sn.isMoveAvailable())

snap = emptyToNone(sn.getMove())
if snap is not None:
    print(snap)
    print(snap.segment)
    print(snap.torque)
    print(snap.direction)
    print(snap.direction.x)
    print(snap.direction.y)
    print(snap.direction.z)