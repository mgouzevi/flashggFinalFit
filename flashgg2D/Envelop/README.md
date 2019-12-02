# FTest and functions for Envelop

For get Expected Limits we used https://github.com/ivovtin/bbggLimits2018/tree/ForPAS package <br />

## Getting the code
Fork to your directory the code here: https://github.com/ivovtin/Envelop<br />
Clone it locally:<br />
git clone https://github.com/[YOURNAME]/Envelop <br />

## FTest

CMSSW_8_1_0 release <br />
For FTest you must get next code: https://github.com/cms-analysis/flashggFinalFit/tree/topic-migrate-to-cmssw_747 <br />
In this code need fTest.cpp file replace in directory flashggFinalFit/Background/test/ on https://github.com/ivovtin/Envelop/blob/master/FTest/fTest.cpp<br /> 
Then done make in flashggFinalFit/Background <br />

1) For run FTest you can use runFTest.sh . <br />
2) Need see plots from FTest for check fits. Results from FTest in the end fTestRes_${ext}.dat file. <br />
3) The resulting orders are write to the https://github.com/ivovtin/Envelop/blob/master/Env_json_new_1D_ttHon.dat file. Env_jso.dat file we use in https://github.com/ivovtin/bbggLimits2018/tree/ForPAS package for get Expected Limits with Envelop method. <br />
4) In bbggLimits2018 you must create new workspace with received orders from FTEST. <br />

## Reducing the number of functions in Envelop -- important for 2D method

CMSSW_10_2_13 release <br />
You must use branch https://github.com/ivovtin/HiggsAnalysis-CombinedLimit/tree/nckw_option_to_remove_const_offset from CombinedLimit <br />
1) Run Indexes.sh and find a combination of functions that gives a minimum of NLL, array of maximum index values, array of indices with a combination of functions for Asimov, etc. <br />
-- ./Indexes.sh /afs/cern.ch/user/i/ivovtin/HHggbb/CMSSW_8_1_0/src/HiggsAnalysis/bbggLimits2018/ws_2016-17_new_ptmgg_ptmjj_dR_1D_ttHon <br />
2) Then yoy must rewrite all the results in submitSCAN.py and ScanStudy.C <br />
3) Run the submitSCAN.py with help runScan.sh script <br />
4) Then we launch ScanStudy.C to search for functions that give a minimum <br />
-- root -l -b -q 'ScanStudy.C+("new_ptmgg_ptmjj_dR_1D_ttHon")' <br /> 
5) To check draw pictures using envAllN.C or ./job.sh <br />
6) Write functions to Env_jso.dat a file  <br />
