#!/usr/bin/env python

import os,sys,json,time,re,string
import logging
from shutil import copy
from pprint import pformat
import getpass
username = getpass.getuser()


def createDir(myDir, log=None, over=True):
  if log!=None:
    log.info('Creating a new directory: %s', myDir)
  if os.path.exists(myDir):
    if log!=None:
      log.warning("\t This directory already exists: %s", myDir)
    if over:
      # Overwrite it...
      if log!=None:
        log.warning("But we will continue anyway (I will --overwrite it!)")
    else:
      if log!=None:
        log.error(' And so I exit this place...')
      print 'The directory exist and we exit. Dir = ', myDir
      sys.exit(1)
  else:
    try: os.makedirs(myDir)
    except OSError:
      if os.path.isdir(myDir): pass
      else: raise


def runCombine(inDir, doBlind, log, combineOpt = 1, Label = None, scaleSingleHiggs=False):
  log.info('Running combine tool.  Dir: %s Blinded: %r', inDir, doBlind)
  log.debug('inDir should be the immediate directory where the card is located')

  if doBlind and combineOpt!=3:
    # In HybridNew this option does not work
    blinded = "--run blind"
  else:
    blinded = ''

  if combineOpt==1:
    combineMethod = 'AsymptoticLimits'
  elif combineOpt==2:
    combineMethod = 'AsymptoticLimits --X-rtd TMCSO_AdaptivePseudoAsimov=50'
  elif combineOpt==3:
    combineMethod = 'HybridNew --testStat=LHC --frequentist'
  elif combineOpt==4:
    combineMethod = 'AsymptoticLimits -S 0' 

  else:
    log.error('This option is not supported: %r', combineOpt)
    return __BAD__

  if scaleSingleHiggs:
    cardName = inDir+"/kt_scaled_hhbbgg_13TeV_DataCard.txt"
    resFile  = inDir+"/kt_scaled_result_"+str(combineOpt)+".log"
  else:
    cardName = inDir+"/hhbbgg_13TeV_DataCard.txt"
    resFile  = inDir+"/result_"+str(combineOpt)+".log"


  command1 = ' '.join(['combine -M', combineMethod,'-m 125 -n',Label,blinded,cardName,">",resFile,"2>&1"])
  log.info('Combine command we run:\n%s', command1)

  combExitCode = os.system(command1)

  if combineOpt in [1,2]:
    fName = 'higgsCombine'+Label+'.Asymptotic.mH125.root'
  elif combineOpt in [3]:
    fName = 'higgsCombine'+Label+'.HybridNew.mH125.root'

  outDir = inDir
  os.rename(fName, outDir+'/'+fName.replace('.root', '_%i.root'%combineOpt))

  log.info("Combine is done. %r should be produced", fName)
  return combExitCode


def DataCardMaker_wHiggs(Folder, nCats, signalExp, observed, higgsExp, log):
  # Need to loop over categories here

  for n in range(nCats):
    # print "Making card for cat", n
    inputDatacardName = os.getenv("CMSSW_BASE")+'/src/HiggsAnalysis/bbggLimits2018/templates/NonResDatacardTemplate_wHiggs_2016_cat'+str(n)+'.txt'

    outToWrite=''
    with open(inputDatacardName, 'r') as cardTemp:
      outToWrite = cardTemp.read()

      outToWrite = outToWrite.replace("INPUTBKGLOC", str(Folder + '/ws_hhbbgg.data_bkg.root'))
      outToWrite = outToWrite.replace("INPUTSIGLOC", str(Folder + '/ws_hhbbgg.HH.sig.mH125_13TeV.root'))

      ##observed
      outToWrite = outToWrite.replace("OBS_CAT"+str(n), '%.1f' % observed[n])
      #print outToWrite
      ##expected signal
      outToWrite = outToWrite.replace("SIG_CAT"+str(n), '%.5f' % signalExp[n])
      ## higgs
      # print higgsExp
      for hty in higgsExp:
        upper_hty = hty.upper()
        #location
        outToWrite = outToWrite.replace("INPUT"+upper_hty+"LOC", Folder + '/ws_hhbbgg.'+hty+'.root')
        #exp
        outToWrite = outToWrite.replace(upper_hty+"_CAT"+str(n), '%.5f'%float(higgsExp[hty][n]))

      with open(Folder+'/hhbbgg_13TeV_DataCard_cat'+str(n)+'.txt', 'w') as outputDatacard:
        outputDatacard.write(outToWrite)
        
  # print "Combining the cards"
  combCard = Folder+'/hhbbgg_13TeV_DataCard.txt'

  combo="combineCards.py "

  for n in range(nCats):
    combo = combo + "cat"+str(n)+"="+Folder+"/hhbbgg_13TeV_DataCard_cat"+str(n)+".txt "

  combo = combo + " > " + combCard

  log.info("========= ", combo)
  
  os.system(combo)

  # Now we actually need to fix the combined card
  #strReplace = Folder+'/'
  strReplace = Folder+'/'+os.getenv("CMSSW_BASE")+'/src/HiggsAnalysis/bbggLimits2018/'
  os.system("sed -i 's|"+strReplace+"||g' "+combCard)
  # print strReplace

if __name__ == "__main__":
  print "This is the __main__ part"
