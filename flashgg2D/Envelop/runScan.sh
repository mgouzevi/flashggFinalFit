#!/bin/bash
#


#for var in new_ptmgg_ptmjj_dR_2D_ttHon_mjj80 new_ptmgg_ptmjj_dR_2D_ttHon_newbound new_ptmgg_ptmjj_dR_2D_ttHon_newbound_mjj80
for var in new_1D_ttHon_ethbound
do
#python submitSCAN.py -n ${var} -o SCAN_${var} -d ${PWD}/WS_FROM_IVAN/ws_2016-17_${var} 
python submitSCAN.py -n ${var} -o SCAN_${var} -d /afs/cern.ch/user/i/ivovtin/HHggbb/CMSSW_8_1_0/src/HiggsAnalysis/bbggLimits2018/ws_2016-17_${var} 
done
