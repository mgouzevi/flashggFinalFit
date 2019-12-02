#! /usr/bin/env python
import sys,argparse, collections, pprint
from ROOT import *
gROOT.SetBatch()

parser =  argparse.ArgumentParser(description='Ploting my plots', usage="./compare.py --f1 fName1 --f2 fName2")
parser.add_argument("--f1",  dest="f1", type=str, required=True, help="Filename with flat tree")
parser.add_argument("--f2",  dest="f2", type=str, required=True, help="Filename with flat tree")
parser.add_argument("--cat",  dest="cat", type=int, default=0, help="Category ID")

opt = parser.parse_args()


Event = collections.namedtuple('Event',
                               ['run','evt','mgg', 'mjj'])


def getInfo(inFile):
    if inFile.Get("TCVARS"):
        tr = inFile.Get('TCVARS')
        tType=1
    elif inFile.Get("LT"):
        tr = inFile.Get('LT')
        tType=2
    else:
        print 'File', inFile.GetName(), 'does not contain expected trees with data to compare.'
        sys.exit(0)

    # tr.Print()
    
    ev = set()
    evData = []
                               
    for e in tr:
        if tType==1:
            if e.cut_based_ct!=opt.cat: continue
        if tType==2:
            if e.catID!=opt.cat: continue

        ev.add((int(e.run), int(e.evt)))
        p = Event(run = e.run, evt=e.evt, mgg=e.mgg, mjj=e.mjj )
        evData.append(p)
        
    return ev, evData


f1 = TFile(opt.f1,'OPEN')
f2 = TFile(opt.f2,'OPEN')


ev1,ev1More = getInfo(f1)
ev2,ev2More = getInfo(f2)

print 'Total events in file %s: %i' %(opt.f1, len(ev1))
print 'Total events in file %s: %i' %(opt.f2, len(ev2))
print 'Intersection of the two: %i' %(len(ev1.intersection(ev2)))

print 'Events in %s but not in %s: %i' %(opt.f1, opt.f2, len(ev1.difference(ev2)))
print '\t And here is a first few of those:'
for i,a in enumerate(ev1.difference(ev2)):
    if i>5:
        break
    print i, a
    for e in ev1More:
        if getattr(e, 'run')==a[0] and getattr(e, 'evt')==a[1]: print e

print 'Events in %s but not in %s: %i' %(opt.f2, opt.f1, len(ev2.difference(ev1)))
print '\t And here is a first few of those:'
for i,a in enumerate(ev2.difference(ev1)):
    if i>5:
        break
    print i, a
    for e in ev2More:
        if getattr(e, 'run')==a[0] and getattr(e, 'evt')==a[1]: print e



print 'Events in %s AND in %s: %i' %(opt.f1, opt.f2, len(ev1.intersection(ev2)))
print '\t And here is a first few of those:'
for i,a in enumerate(ev1.intersection(ev2)):
    if i>2:
        break
    print i, a
    for e in ev1More:
        if getattr(e, 'run')==a[0] and getattr(e, 'evt')==a[1]: print e
    for e in ev2More:
        if getattr(e, 'run')==a[0] and getattr(e, 'evt')==a[1]: pprint.pprint(e)
