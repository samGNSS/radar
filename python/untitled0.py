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
    pass

if __name__=="__main__":
    main()