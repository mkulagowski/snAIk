import SnAIk
import numpy as np
#from gym.spaces import Box
from gym.utils.seeding import np_random

def snaik_to_list(snake):
    segs = snake.segments
    if segs == 0:
        print('ZERO SEGMENTS!!')
    ret_list = []
    speed_list = []
    for v in segs:
        ret_list.append(v.rotation.x)
        ret_list.append(v.rotation.y)
        ret_list.append(v.rotation.z)
        ret_list.append(v.position.x)
        ret_list.append(v.position.y)
        ret_list.append(v.position.z)
        ret_list.append(v.angularVelocity.x)
        ret_list.append(v.angularVelocity.y)
        ret_list.append(v.angularVelocity.z)
        speed_list.append(v.speed)
    return ret_list, speed_list

def list_to_action(segment, action_list):
    action = SnAIk.SnakeMove()
    action.segment = segment
    vect = SnAIk.VectStruct()
    vect.x = action_list[0]
    vect.y = 0
    vect.z = action_list[1]
    action.torque = vect
    return action

def list_to_action_1axis(segment, action_list):
    action = SnAIk.SnakeMove()
    action.segment = segment
    vect = SnAIk.VectStruct()
    vect.x = action_list
    vect.y = 0
    vect.z = 0
    action.torque = vect
    return action

class SnaikManager(object):
    def __init__(self, should_draw, episode_length, size, maxE=None):
        self.name = 'SnaikManager'
        self.api = SnAIk.API.GetInstance()
        self.draw = should_draw
        self.eplen = episode_length
        self.finishline = 50
        self.currStep = 0
        self.snaik_size = size
        self.reward_range = (-np.inf, np.inf)
        self.spec = None
        self.max_energy = self.snaik_size/4 if maxE is None else maxE
        # self.observation_space = Box(-50, 50, shape=(9 * self.snaik_size,), dtype=np.float32)
        # self.action_space = Box(-1, 1, shape=(1 * self.snaik_size,), dtype=np.float32)
        self.observation_space = 9 * self.snaik_size
        self.action_space = 1 * self.snaik_size
        self.observation_limit = 50
        self.action_limit = 1
        self.head_last = None

    def init(self):
        self.currStep = 0
        self.checkpoints = range(0, self.finishline, 5)
        self.api.Init(self.draw)
        self.api.Step(self.draw)
        s,_ = snaik_to_list(self.api.GetSnake())
        self.head_last = s[3:6]
        return s

    def testGenome(self, genome, render=False):
        s = self.init()
        d = False
        r = 0
        while d is False:
            actionsFlat = genome.activate(s)[0]
            suma = sum(actionsFlat)*2
            actionsNorm = [float(i) / suma for i in actionsFlat]
            #actions = [actionsFlat[i:i+2].tolist() for i in range(0, len(actionsFlat), 2)]
            actions = [actionsNorm[i:i + 2] for i in range(0, len(actionsNorm), 2)]
            moves = [list_to_action(idx, act) for idx, act in enumerate(actions)]
            for move in moves:
                self.api.AddMove(move)
            self.api.Step(render)

            s = snaik_to_list(self.api.GetSnake())

            d = abs(s[3]) >= self.finishline or abs(s[4]) >= self.finishline  # check if head is out of bounds
            r += round(np.sqrt(s[3] ** 2 + s[4] ** 2) / 10) - 5 + (
                    d * 100 * (self.eplen - self.currStep))  # 1/10 of distance from mid
            d = d or self.currStep >= self.eplen
        return r

    def step(self, actions):
        return self._step(actions, self.draw)

    def _step(self, actions, render=False):
        self.currStep += 1
        actionsTotal = sum(abs(actions))
        actions = actions * 100 # rescale the actions
        moves = [list_to_action_1axis(idx, act) for idx, act in enumerate(actions)]
        for move in moves:
            self.api.AddMove(move)
        self.api.Step(render)

        # get next state
        s1, speeds = snaik_to_list(self.api.GetSnake())

        # get "done" flag
        d = abs(s1[3]) >= self.finishline or abs(s1[4]) >= self.finishline  # check if head is out of bounds

        # calculate reward
        energyUsed = actionsTotal
        headPos = s1[3:6]
        headSpeed = speeds[0]
        isMoveForward = 10 if np.linalg.norm(headPos[:-1]) > np.linalg.norm(self.head_last[:-1]) else 0
        r = (isMoveForward * headSpeed) - energyUsed

        # check if checkpoint has been reached
        if self.checkpoints and np.linalg.norm(headPos[:-1]) >= self.checkpoints[0]:
            r += self.checkpoints[0] * 10
            self.checkpoints = self.checkpoints[1:]

        # add final reward if needed
        r += d * (self.eplen - self.currStep)

        # update finish flag if step cap has been reached
        d = d or self.currStep >= self.eplen

        self.head_last = s1[3:6]
        return s1, r, d, self.currStep

    def seed(self, seed=None):
        self.np_random, seed = np_random(seed)
        return [seed]

    def reset(self):
        return self.init()

    def __str__(self):
        return 'SnaikManager (draw={}, eplen={}, snaikSize={}, step={}'.format(self.draw, self.eplen, self.snaik_size, self.currStep)

    def __repr__(self):
        return str(self)