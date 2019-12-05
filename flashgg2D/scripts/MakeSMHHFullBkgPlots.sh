#!/bin/bash

LIMFOLDER=$1/Node_SM

INFILE=${LIMFOLDER}/higgsCombineTest.FitDiagnostics.mH120.123456.root
DATAFILE=${LIMFOLDER}/ws_hhbbgg.data_bkg.root

BKG=${LIMFOLDER}/Background
mkdir ${BKG}

echo $INFILE
echo $BKG

FACT=1.


for icat in {0..11}
do
    python scripts/MakeFullBackgroundFit_Data.py -i ${INFILE} -d ${DATAFILE} -o ${BKG} -c ${icat} --signalFactor ${FACT} ${FACT} --unblind
done
