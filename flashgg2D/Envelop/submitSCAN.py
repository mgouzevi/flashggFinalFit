#!/usr/bin/python

import sys
import os
from os import listdir,system
import commands
from commands import getstatusoutput
from commands import getoutput
import datetime
import argparse

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

# microcentury
# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

def prepareJob (i_job, basedir, outdir, name, datacard, all, cat, index): 
#    datacard = basedir+'/WS2D9/Node_SM/hhbbgg_13TeV_DataCard_envelope_mini.txt'
    datacardPath = datacard
    datacardPath +='/Node_SM/hhbbgg_13TeV_DataCard_envelope_cat'+str(cat)+'.txt'

    bestfit_ind_all = {
    'new_1D_ttHon_ethbound':[0,0,2,1,0,0,1,3,2,3,3,2],
    #'new_2D_ttHon0.21':[0,6,6,3,0,0,3,17,7,9,12,22],
    #'Mjj_new_1D_ttHon0.21':[0,1,2,2,2,1,2,3,3,2,2,2],
    #'new_ptmgg_ptmjj_dR_1D_ttHon_ethbound':[0,2,2,2,0,2,4,2,1,2,2,4],
    #'new_ptmgg_ptmjj_dR_1D_ttHon':[0,2,2,1,1,2,3,1,1,2,2,2],
    #'new_ptmgg_ptmjj_dR_2D_ttHon_mjj80':[2,7,6,6,1,6,9,9,4,6,3,12],
    #'new_ptmgg_ptmjj_dR_2D_ttHon_newbound' :[0,6,6,0,0,6,6,17,6,3,6,22],
    #'new_ptmgg_ptmjj_dR_2D_ttHon_newbound_mjj80':[0,6,6,0,0,6,6,17,6,3,6,22]
    }
    
    
    #bestfit_ind = [0,2,3,2,3,0,3,4,2,3,3,4]  #FIXME
    bestfit_ind = bestfit_ind_all[name]
    if(i_job==0):
        print("bestfir_ind",bestfit_ind)
    jobname = outdir+'/'+name+'_'+str(i_job)+'.sh'

    FileExtToy='cat'+str(cat)+'BestFit'
    joblineToy = 'combine -d'+datacardPath+' -n '+FileExtToy+' -M GenerateOnly --expectSignal 0 -t -1 --saveToys --toysFrequentist --X-rtd TMCSO_AdaptivePseudoAsimov=0 --X-rtd TMCSO_PseudoAsimov=0 --setParameters '
    joblineToy+='pdf_index_cat'+str(cat)+'='+str(bestfit_ind[cat])
        
    FileExt=''
    ToyName='higgsCombine'+FileExtToy+'.GenerateOnly.mH120.123456.root'
    if(all==0):
        FileExt='cat'+str(cat)+'ind'+str(index)
        jobline = 'combine -M MultiDimFit -d '+datacardPath+' -n '+FileExt+' --toysFile='+ToyName+' --rMin -1 --rMax 4 -P r  --algo grid --points 40 --saveSpecifiedIndex pdf_index_cat'+str(cat)+'  --cminDefaultMinimizerStrategy 0  --saveNLL --setParameterRanges r=-1,4 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1  --setParameters pdf_index_cat'+str(cat)+'='+str(index)+'  --freezeParameters pdf_index_cat'+str(cat)
    else:
        FileExt='cat'+str(cat)+'indAll'
        jobline = 'combine -M MultiDimFit -d '+datacardPath+' -n '+FileExt+' --toysFile='+ToyName+' --rMin -1 --rMax 4 -P r  --algo grid --points 40 --saveSpecifiedIndex pdf_index_cat'+str(cat)+'  --cminDefaultMinimizerStrategy 0  --saveNLL --setParameterRanges r=-1,4 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1 '
        
    f = open (jobname, 'w')
    f.write ('#!/bin/sh' + '\n\n')
    f.write ('cd '+basedir+' \n')
    f.write ('eval `scram runtime -sh` \n')
    f.write ('cd - \n\n')    
    f.write (joblineToy+' \n\n')
    f.write (jobline+' \n\n')
    f.write ('mv higgsCombine'+FileExt+'.MultiDimFit.mH120.root '+outdir+' \n')
    f.write ('rm '+ToyName+' \n')
    f.close ()
    getstatusoutput ('chmod 755 ' + jobname)


# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

if __name__ == '__main__':

    parser = argparse.ArgumentParser (description = 'Submit series of combine jobs, each job with different random seed')
    parser.add_argument('-n', '--name' , default='Test', help='Name of combine jobs')
    parser.add_argument('-o', '--outdir' , default = '$CMSSW_BASE/output', help='Output path')
    parser.add_argument('-q', '--queue' , default = 'espresso', help='Job max runtime, "microcentury"=1h, "tomorrow"=1d, "nextweek"=7d')
    parser.add_argument('-j', '--jobs' , default = '10', type=int, help='Number of jobs, each job should has 100 MC toys')
    parser.add_argument('-i', '--index' , default = '0', type=int, help='Index in RooMultiPdf')    
    parser.add_argument('-c', '--cat' , default = '-1', type=int, help='Which category to fit, cat<0 run fit for all categories')
    parser.add_argument('-d', '--datacard' , default = '', help='datacard postfix')
    parser.add_argument('--dryrun' , action="store_true", default=False, help='Do not submit the jobs, just create them')
    parser.add_argument('--allsyst' , action="store_true", default=False, help='Fit without fixing the nuisance parameters')

    
    args = parser.parse_args ()
    
    n_jobs =  args.jobs # number of jobs in series
    ## expand output
    outdir = os.path.abspath(os.path.expandvars(args.outdir))
    name = args.name
    print("Output will be copied to: "+outdir)
    getstatusoutput('mkdir -p '+outdir)
    queue = args.queue 

    max_ind_all = {
    'new_1D_ttHon_ethbound':[3,3,3,3,3,3,3,4,3,4,4,5],
    #'new_2D_ttHon0.21':[9,9,9,9,9,9,12,20,9,12,15,25],
    #'Mjj_new_1D_ttHon0.21':[3,4,3,3,3,3,4,4,4,4,5,6],
    #'new_ptmgg_ptmjj_dR_1D_ttHon_ethbound':[3,3,3,3,3,3,5,3,3,3,4,5],
    #'new_ptmgg_ptmjj_dR_1D_ttHon':[3,3,3,3,3,3,4,3,3,3,4,5],
    #'new_ptmgg_ptmjj_dR_2D_ttHon_mjj80':[9,9,9,12,9,9,12,12,9,9,12,15],
    #'new_ptmgg_ptmjj_dR_2D_ttHon_newbound' :[9,9,9,9,9,9,12,20,12,9,12,25],
    #'new_ptmgg_ptmjj_dR_2D_ttHon_newbound_mjj80':[9,9,9,9,9,9,12,12,12,9,12,15]
    }
    
    #max_ind = [3,3,4,3,5,4,5,5,5,5,5,5]  # FIXME
    max_ind = max_ind_all[name]
    print("max_ind",max_ind)
    ## create jobs
    #basedir = os.path.expandvars("$CMSSW_BASE")+'/src/HiggsAnalysis/bbggLimits2018'
    basedir = os.path.expandvars("$CMSSW_BASE")+'/src/Envelop'

    i_job=0
    all=0
    for cat in range(0, 12):
        for ind in range(0, max_ind[cat]):
            prepareJob( i_job, basedir, outdir, name, args.datacard, all, cat, ind)
            i_job+=1
    
    all=1            
    for cat in range(0, 12):
        prepareJob( i_job, basedir, outdir, name, args.datacard, all, cat, ind)
        i_job+=1

    #create sub file
    condname = outdir+'/Combine_'+name+'.sub'
    f = open (condname, 'w')
    f.write ('executable            = '+outdir+'/'+name+'_$(ProcId).sh\n')
    f.write ('arguments             = $(ClusterID) $(ProcId) \n')
    f.write ('output                = '+outdir+'/'+name+'.$(ClusterId).$(ProcId).out\n')
    f.write ('error                 = '+outdir+'/'+name+'.$(ClusterId).$(ProcId).err\n')
    f.write ('log                   = '+outdir+'/'+name+'.$(ClusterId).$(ProcId).log\n')
    f.write ('+JobFlavour           = "'+queue+'"  \n')
    f.write ('queue '+str(i_job))
    f.close ()
        
    print("Submitting "+str(i_job)+" jobs to condor...")            
    if not args.dryrun:
        system('condor_submit ' + condname)

