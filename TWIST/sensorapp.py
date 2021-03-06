#!/usr/bin/env python

# copied from http://mail.millennium.berkeley.edu/pipermail/tinyos-help/2007-September/028165.html
# This is a quick and dirty example of how to use the MoteIF interface in Python

from CBSweepDataMsg import *
from tinyos.message import MoteIF
import sys
import signal
import time
from getpass import getpass

def signal_handler(signal, frame):
        # print 'You pressed Ctrl+C!'
        print ''
        sys.exit(0)

class MyClass:

    def __init__(self, sfsource):
        # Create a MoteIF
        self.mif = MoteIF.MoteIF()
        # Attach a source to it
        if ':' in sfsource:
            # self.log.debug("Assuming Serial Forwarder interface")
            self.source = self.mif.addSource("sf@" + sfsource)
        elif "dev" in sfsource:
            # self.log.debug("Assuming Serial interface")
            self.source = self.mif.addSource("serial@" + sfsource + ":115200")
        else:
            try:
                import twist
                from threading import Thread
                sfsource = int(sfsource)
                print "Provide ssh password: "
                passwd = getpass()
                thread1 = Thread(target = twist.ssh_tunnel, args = ([sfsource],passwd))
                thread1.daemon = True
                thread1.start()
                time.sleep(3)
                self.source = self.mif.addSource("sf@localhost:9{0:03d}".format(sfsource))
            except Exception, e:
                raise

        # SomeMessageClass.py would be generated by MIG
        self.mif.addListener(self, CBSweepDataMsg)

    # Called by the MoteIF's receive thread when a new message
    # is received
    def receive(self, src, msg):
      if msg.get_amType() == CBSweepDataMsg.get_amType():
        print "%f: %s" % (time.time(), str(msg.get_rssi()))

if __name__ == "__main__":
    print "# Running"
    signal.signal(signal.SIGINT, signal_handler)
    if (len(sys.argv) == 2):
        m = MyClass(sys.argv[1])
    else:
        print "Usage:   " + sys.argv[0] + " <sf-source>"
        print "Example: " + sys.argv[0] + " localhost:9002"
    print '# Press Ctrl+C to stop'
    signal.pause()
