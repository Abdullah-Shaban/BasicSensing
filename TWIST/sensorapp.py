#!/usr/bin/env python

# copied from http://mail.millennium.berkeley.edu/pipermail/tinyos-help/2007-September/028165.html
# This is a quick and dirty example of how to use the MoteIF interface in Python

from CBSweepDataMsg import *
from CBRepoQueryMsg import *
from tinyos.message import MoteIF
import sys
import signal
import time

def signal_handler(signal, frame):
        # print 'You pressed Ctrl+C!'
        print ''
        sys.exit(0)

class MyClass:

    def __init__(self, sfsource):
        # Create a MoteIF
        self.mif = MoteIF.MoteIF()
        # Attach a source to it
        self.source = self.mif.addSource(sfsource)

        # SomeMessageClass.py would be generated by MIG
        self.mif.addListener(self, CBRepoQueryMsg)
        self.mif.addListener(self, CBSweepDataMsg)

    # Called by the MoteIF's receive thread when a new message
    # is received
    def receive(self, src, msg):
      if msg.get_amType() == CBSweepDataMsg.get_amType():
        print "%f: RF noise: %s" % (time.time(), str(msg.get_rssi()))
      else:
        print "%f: Received message: " % (time.time(), str(msg))

if __name__ == "__main__":
    print "Running"
    signal.signal(signal.SIGINT, signal_handler)
    if (len(sys.argv) == 2):
        m = MyClass(sys.argv[1])
    else:
        print "Usage:   " + sys.argv[0] + " <sf-source>"
        print "Example: " + sys.argv[0] + " localhost:9002"
    print 'Press Ctrl+C'
    signal.pause()
