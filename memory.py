import sys
from optparse import OptionParser
import random
import math

def hfunc(index):
    if index == -1:
        return 'MISS'
    else:
        return 'HIT '

def vfunc(victim):
    if victim == -1:
        return '-'
    else:
        return str(victim)

#
# main program
#
parser = OptionParser()
parser.add_option('-a', '--addresses', default='-1',   help='a set of comma-separated pages to access; -1 means randomly generate',  action='store', type='string', dest='addresses')
parser.add_option('-t', '--t', default='2',    help='size of the physical page frame, in pages',                                  action='store', type='string', dest='pageframesize')
parser.add_option('-N', '--notrace', default=False,    help='do not print out a detailed trace',                                     action='store_true', dest='notrace')

(options, args) = parser.parse_args()

addresses   = str(options.addresses)
pageframesize   = int(options.pageframesize)

addrList = []
addrList = addresses.split(',')

for i in range(0,len(addrList)):
    addrList[i] = int(addrList[i])

notrace = False

if notrace == False:
    print 'Solving...\n'

# init memory structure
count = 0
memory = []
hits = 0
miss = 0
TR = pageframesize
lastMissT = -1
nowT = 0

# track reference bits for clock
ref   = {}

cdebug = False

# need to generate addresses
addrIndex = 0
for n in addrList:
    # first, lookup
    #n = int(nStr)
    try:
        idx = memory.index(n)
        hits = hits + 1
    except:
        idx = -1
        miss = miss + 1
    
    
    if idx == -1:
        
        if nowT - lastMissT <= TR:
            memory.append(n)
            print 'Access: %d  %s -> %12s and nothing replaced [Hits:%d Misses:%d]' % (n, hfunc(idx), memory, hits, miss)
        else:
            victim = []
            for m in memory:
                if not (m in addrList[lastMissT + 1:nowT]):
                    victim.append(m)
            for toremove in victim:
                memory.remove(toremove)
            memory.append(n)
            print 'Access: %d  %s -> %12s replaced %12s [Hits:%d Misses:%d]' % (n, hfunc(idx), memory, victim, hits, miss)
        lastMissT = nowT
    else:
        print 'Access: %d hits %12s' % (n,memory)
    nowT = nowT + 1
    
print ''
print 'FINALSTATS hits %d   misses %d   hitrate %.2f' % (hits, miss, (100.0*float(hits))/(float(hits)+float(miss)))
print ''
