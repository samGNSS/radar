#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Created on Sat Jan 21 09:24:51 2017

@author: sam
"""
import time
import numpy as np
import matplotlib.pyplot as plt

def chirp(numSamps,fs,bw,cf):
    f_start = cf-bw/2.    
    n = np.arange(0,numSamps)
    instFreq = ((f_start+(numSamps/fs)*n)*n)/float(fs)
    sig = np.exp(1j*2*np.pi*instFreq)
    return sig

def main(test):    
    num = 0
    count = 0
    for i in test:
        if i == 1:
            num += 1
        else:
            if num > count:
                count = num
            num = 0
    if num > count:
        count = num
    print count

if __name__=="__main__":
    test = [1,0,1,0,1,0,1,1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,0]
    aveTime = 0
    numIter = 100
    for x in xrange(numIter):
        start = time.time()
        main(test)
        aveTime += time.time() - start
        
    print aveTime/numIter