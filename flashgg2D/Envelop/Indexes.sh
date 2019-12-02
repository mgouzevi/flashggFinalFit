#!/bin/bash
eval `scram runtime -sh`
combine -M MultiDimFit -d ${1}/Node_SM/hhbbgg_13TeV_DataCard_envelope.txt  --rMin -1 --rMax 4 -P r   --saveSpecifiedIndex pdf_index_cat0,pdf_index_cat1,pdf_index_cat2,pdf_index_cat3,pdf_index_cat4,pdf_index_cat5,pdf_index_cat6,pdf_index_cat7,pdf_index_cat8,pdf_index_cat9,pdf_index_cat10,pdf_index_cat11  --cminDefaultMinimizerStrategy 0  --saveNLL --X-rtd REMOVE_CONSTANT_ZERO_POINT=1
root -l -b -q 'Indexes.C("'${1}'")'
cp higgsCombineTest.MultiDimFit.mH120.root ${1}
