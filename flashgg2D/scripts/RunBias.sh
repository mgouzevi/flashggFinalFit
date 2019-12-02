LIMFOLDER=$1/Node_SM


cd $LIMFOLDER
mkdir BIAS
mkdir BIAS/LOGS
arr=(Ber Exp Pow)
asig=(1 1 1 10 10 10 10 20 20 20 20 20)
func=0
NGEN=2000

echo " ========== Func used " ${arr[func]} " with " ${NGEN} " events " 
echo " "

for icat in {0..11}
do
    for truth in {0..2}
    do
	SIG=${asig[${icat}]}
	seed=$(($icat*100+$func+$truth))
	rMin=-10
	rMax=$((2*$SIG+6))

	CAT=cat${icat}
	DATACARD=hhbbgg_13TeV_DataCard_bias_${CAT}.txt
	PDFIDX=pdf_index_$CAT
	SETUP=${CAT}.t${arr[${truth}]}.f${arr[${func}]}.musig$SIG
	GENERATE=higgsCombineTest.GenerateOnly.mH${icat}.${seed}.root


	TOYS=BIAS/higgsCombineTest.GenerateOnly.${SETUP}.root
	MLFIT=BIAS/mlfit.${SETUP}.root

	echo $CAT " with truth " ${arr[truth]} " fit by function " ${arr[func]} " with a random seed " ${seed}
	GENERATING="combine $DATACARD -M GenerateOnly --setPhysicsModelParameters $PDFIDX=${truth} --toysFrequentist -t ${NGEN} --expectSignal $SIG --saveToys -m ${icat} --freezeNuisances $PDFIDX -s ${seed} -v -1 &>BIAS/LOGS/Gen_${SETUP}.txt"
	echo $GENERATING
	eval $GENERATING

	mv $GENERATE $TOYS
	echo "generated " $TOYS

	MLFITTING="combine $DATACARD -M MaxLikelihoodFit  --setPhysicsModelParameters $PDFIDX=${func}  --toysFile $TOYS  -t ${NGEN} --rMin $rMin --rMax $rMax  --freezeNuisances $PDFIDX -v -1 &>BIAS/LOGS/MLFits_${SETUP}.txt"
	echo ${MLFITTING}
	eval ${MLFITTING}
	mv mlfit.root $MLFIT
	rm $TOYS
	echo "fitted " $MLFIT
	
    done
done

