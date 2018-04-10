from collections import deque
import random
import numpy as np
import pickle

class ReplayBuffer(object):

    def __init__(self, buffer_size, path=None, random_seed=123):
        self.buffer_size = buffer_size
        self.count = 0 # incrementing should be faster then len() with big container
        self.buffer = deque()
        self.path = path
        random.seed(random_seed)

    def add(self, s, a, r, t, s2):
        experience = (s, a, r, t, s2)
        if self.count < self.buffer_size: 
            self.buffer.append(experience)
            self.count += 1
        else:
            self.buffer.popleft()
            self.buffer.append(experience)

    def size(self):
        return self.count

    def sample_batch(self, batch_size):
        batch = []

        if self.count < batch_size:
            batch = random.sample(self.buffer, self.count)
        else:
            batch = random.sample(self.buffer, batch_size)

        s_batch = np.array([_[0] for _ in batch])
        a_batch = np.array([_[1] for _ in batch])
        r_batch = np.array([_[2] for _ in batch])
        t_batch = np.array([_[3] for _ in batch])
        s2_batch = np.array([_[4] for _ in batch])

        return s_batch, a_batch, r_batch, t_batch, s2_batch

    def clear(self):
        self.buffer.clear()
        self.count = 0

    def save(self, path):
        print('/tBuffer save size = %d' % self.count)
        with open(path, 'wb') as fout:
            pickle.dump(self.buffer, fout)
        print('/tBuffer save finished!')

    def load(self, path):
        try:
            with open(path, 'rb') as fin:
                self.buffer = pickle.load(fin)
                self.count = len(self.buffer)
                print('/tBuffer load size = %d' % self.count)
        except:
            print('Errors while loading replay buffer!')

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if self.path is not None:
            self.save(self.path)