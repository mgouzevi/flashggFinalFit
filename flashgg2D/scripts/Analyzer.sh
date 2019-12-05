#!/bin/bash

echo '=== Preparing datacards in root format ==='

DIR=$1/Node_SM
echo $DIR

text2workspace.py $DIR/hhbbgg_13TeV_DataCard.txt $DIR/hhbbgg_13TeV_DataCard.root -m 125

cd $DIR

printf '\n=== Start fitting for background plots ==\n\n'

combine -M FitDiagnostics -t -1 -d hhbbgg_13TeV_DataCard.root -S 0  --saveWorkspace --saveShapes --saveNormalization  --saveToys --savePredictionsPerToy

printf '\n=====\n\n'

combine -M AsymptoticLimits -d hhbbgg_13TeV_DataCard.root --run blind -m 125 -n SM_13TeV_3ab -S 0 &> Limit_stat.txt

echo '== 3.1) Finished stat only limits'

tail  Limit_stat.txt

#printf '\n=====\n\n'

#combine -M Asymptotic -d hhbbgg_13TeV_DataCard.root -t -1  --run blind --expectSignal 1 -m 125 -n SM_13TeV_3ab &> Limit.txt

#echo '== 3.2) Finished full limits'

#head  Limit.txt

#printf '\n=====\n\n'

cd -

printf '\n== END ===\n\n'

