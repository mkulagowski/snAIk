#!/usr/bin/env python

import SnAIk
import threading
import tensorflow as tf
import tensorflow.contrib.slim as slim
import numpy as np
import os
from datetime import datetime
import csv

##### VARIABLES
# number of hours network should learn for without drawing
total_hours = 8
# whether to calculate number of episodes based on total_hours
use_hours = True
# number of episodes if use_hours is False
total_episodes = 100
# maximum steps in a single episode
max_ep = 30*60 # 1 minute of 30fps
# update network every N steps
update_frequency = 5
gamma = 0.99
# Last directory index
subfolderIdx = 1
# Should restore model from harddrive
load_model = True

##### CONSTANTS
snaik_size = 15
seg = range(0,15)
coord = range(-1,2)
actions = [(x,y,z) for x in seg for y in coord for z in coord]
modelDir = "E:/snaikModel"
modelPath = modelDir + '/' + str(subfolderIdx) + 'snaikModel'
oldModelPath = modelDir + '/' + str(subfolderIdx - 1) + 'snaikModel'
if use_hours is True:
    total_episodes = round(hours * 3600 * 10 / 6.5)

def calcEpisodes(hours):
    return round(hours * 3600 * 10 / 6.5)

def snaikToList(snake):
    segs = snake.segments
    retList = []
    for v in segs:
        retList.append(v.rotation.x)
        retList.append(v.rotation.y)
        retList.append(v.rotation.z)
        retList.append(v.position.x)
        retList.append(v.position.y)
        retList.append(v.position.z)
    return retList

def listToAction(moves):
    move = SnAIk.SnakeMove()
    move.segment = moves[0]
    move.torque = 1
    vect = SnAIk.VectStruct()
    vect.x = moves[1]
    vect.y = 0
    vect.z = moves[2]
    move.direction = vect
    return move

def discount_rewards(r):
    """ take 1D float array of rewards and compute discounted reward """
    discounted_r = np.zeros_like(r)
    running_add = 0
    for t in reversed(range(0, r.size)):
        running_add = running_add * gamma + r[t]
        discounted_r[t] = running_add
    return discounted_r

class agent():
    def __init__(self, lr, state_size, action_size, hidden_size, hidden_no):
        # These lines established the feed-forward part of the network. The agent takes a state and produces an action.
        self.state_in = tf.placeholder(shape=[None, state_size], dtype=tf.float32)
        hidden1 = slim.fully_connected(self.state_in, hidden_size, biases_initializer=None, activation_fn=tf.nn.relu)
        hidden2 = None
        for i in range(hidden_no):
            hidden2 = slim.fully_connected(hidden1, hidden_size, biases_initializer=None, activation_fn=tf.nn.relu)
            hidden1 = hidden2
        self.output = slim.fully_connected(hidden1, action_size, activation_fn=tf.nn.softmax, biases_initializer=None)
        self.chosen_action = tf.argmax(self.output, 1)

        # The next six lines establish the training proceedure. We feed the reward and chosen action into the network
        # to compute the loss, and use it to update the network.
        self.reward_holder = tf.placeholder(shape=[None], dtype=tf.float32)
        self.action_holder = tf.placeholder(shape=[None], dtype=tf.int32)

        self.indexes = tf.range(0, tf.shape(self.output)[0]) * tf.shape(self.output)[1] + self.action_holder
        self.responsible_outputs = tf.gather(tf.reshape(self.output, [-1]), self.indexes)

        self.loss = -tf.reduce_mean(tf.log(self.responsible_outputs) * self.reward_holder)

        tvars = tf.trainable_variables()
        self.gradient_holders = []
        for idx, var in enumerate(tvars):
            placeholder = tf.placeholder(tf.float32, name=str(idx) + '_holder')
            self.gradient_holders.append(placeholder)

        self.gradients = tf.gradients(self.loss, tvars)

        optimizer = tf.train.AdamOptimizer(learning_rate=lr)
        self.update_batch = optimizer.apply_gradients(zip(self.gradient_holders, tvars))

class SimulatorHandler():
    def __init__(self, should_draw, episode_length):
        self.api = SnAIk.API.getInstance()
        self.draw = should_draw
        self.eplen = episode_length
        self.curr = 0

    def init(self):
        self.api.initSim()
        self.api.step(False)
        s =  snaikToList(self.api.getSnake())
        return s

    def step(self, action):
        self.curr += 1
        self.api.setMove(listToAction(action))
        self.api.step(self.draw)
        # get next state
        s1 = snaikToList(self.api.getSnake())
        # get "done" flag
        d = s1[3] >= 50 or s1[4] >= 50  # check if head is out of bounds
        #get reward
        r = round(np.sqrt(s1[3] ** 2 + s1[4] ** 2) / 10) - 5 + (d * 100 * (max_ep - j)) # 1/10 of distance from mid
        d = d or self.curr >= self.eplen
        return s1, r, d, self.curr
        # s1, r, d, _ = env.step(a)


tf.reset_default_graph() #Clear the Tensorflow graph.
myAgent = agent(lr = 1e-2,
                state_size = 6 * snaik_size,
                action_size = len(actions),
                hidden_size = 8,
                hidden_no = 3
) #Load the agent.

init = tf.global_variables_initializer()

if not os.path.exists(modelPath):
    os.makedirs(modelPath)

with open(modelPath + '/log.csv', 'w') as myfile:
    wr = csv.writer(myfile, quoting = csv.QUOTE_ALL)
    wr.writerow(['Episode', 'Reward', 'Length', 'Avg Last 100 Rewards'])

    saver = tf.train.Saver()
    # Launch the tensorflow graph
    with tf.Session() as sess:
        if load_model == True:
            print('Loading Model...')
            ckpt = tf.train.get_checkpoint_state(oldModelPath)
            saver.restore(sess, ckpt.model_checkpoint_path)
            print('Restored from %s' % ckpt.model_checkpoint_path)
        sess.run(init)
        i = 0
        total_reward = []
        total_lenght = []

        gradBuffer = sess.run(tf.trainable_variables())
        for ix, grad in enumerate(gradBuffer):
            gradBuffer[ix] = grad * 0

        a = None
        while i < total_episodes:
            env = SimulatorHandler(False, max_ep)
            s = env.init()
            running_reward = 0
            j = 0
            ep_history = []
            d = False
            while d is False:
                # Probabilistically pick an action given our network outputs.
                action_dist = sess.run(myAgent.output, feed_dict={myAgent.state_in: [s]})
                a = np.random.choice(action_dist[0], p=action_dist[0])
                a = np.argmax(action_dist == a)

                s1, r, d, j = env.step(actions[a])

                ep_history.append([s, a, r, s1])
                s = s1
                
                running_reward += r

            total_lenght.append(j)
            total_reward.append(running_reward)

            # Update the network.
            ep_history = np.array(ep_history)
            ep_history[:, 2] = discount_rewards(ep_history[:, 2])
            feed_dict = {myAgent.reward_holder: ep_history[:, 2],
                         myAgent.action_holder: ep_history[:, 1],
                         myAgent.state_in: np.vstack(ep_history[:, 0])}

            grads = sess.run(myAgent.gradients, feed_dict=feed_dict)
            for idx, grad in enumerate(grads):
                gradBuffer[idx] += grad

            if i % update_frequency == 0 and i != 0:
                feed_dict = dictionary = dict(zip(myAgent.gradient_holders, gradBuffer))
                _ = sess.run(myAgent.update_batch, feed_dict=feed_dict)
                for ix, grad in enumerate(gradBuffer):
                    gradBuffer[ix] = grad * 0

            wr.writerow([i, total_reward[-1], total_lenght[-1], np.mean(total_reward[-100:])])
            i += 1

            if (i % 100):
                saver.save(sess, modelPath + '/model-' + str(i) + '.cptk')

        #end of training
        path = saver.save(sess, modelPath + '/model-' + str(i) + '.cptk')
        print("Last model saved in path: %s" % path)
