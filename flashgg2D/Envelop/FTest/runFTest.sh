#
for var in ws_2016-17_new_1D_ttHon_ethbound
#for var in new_ptmgg_ptmjj_dR_2D_ttHon_ethbound
do
ext=${var}_mgg
dir=${var} 
/afs/cern.ch/user/i/ivovtin/HHggbb/CMSSW_8_1_0/src/flashggFinalFit/Background/bin/fTest -i /afs/cern.ch/user/i/ivovtin/HHggbb/CMSSW_8_1_0/src/HiggsAnalysis/bbggLimits2018/${dir}/Node_SM/ws_hhbbgg.data_bkg.root --saveMultiPdf CMS-HGG_multipdf_${ext}.root  -D IVAN_${ext} -d IVAN_${ext}/res.dat -f cat0,cat1,cat2,cat3,cat4,cat5,cat6,cat7,cat8,cat9,cat10,cat11 --isData 1 -c 12 --ismgg 1 &> fTestRes_${ext}.dat
eosdir=/eos/user/i/ivovtin/www/HHbbgg/fTest/${ext}
mkdir -p ${eosdir}
cp IVAN_${ext}/*png ${eosdir}
cp fTestRes_${ext}.dat ${eosdir}
cp /eos/user/i/ivovtin/www/tmp/index.php ${eosdir}

#ext=${var}_mjj
#/afs/cern.ch/user/i/ivovtin/HHggbb/CMSSW_8_1_0/src/flashggFinalFit/Background/bin/fTest -i /afs/cern.ch/user/i/ivovtin/HHggbb/CMSSW_8_1_0/src/HiggsAnalysis/bbggLimits2018/${dir}/Node_SM/ws_hhbbgg.data_bkg.root --saveMultiPdf CMS-HGG_multipdf_${ext}.root  -D IVAN_${ext} -d IVAN_${ext}/res.dat -f cat0,cat1,cat2,cat3,cat4,cat5,cat6,cat7,cat8,cat9,cat10,cat11 --isData 1 -c 12 --ismgg 0 &> fTestRes_${ext}.dat
#eosdir=/eos/user/i/ivovtin/www/HHbbgg/fTest/${ext}
#mkdir -p ${eosdir}
#cp IVAN_${ext}/*png ${eosdir}
#cp fTestRes_${ext}.dat ${eosdir}
#cp /eos/user/i/ivovtin/www/tmp/index.php ${eosdir}
done


#for var in std_2D_ttHoff std_2D_ttHon
#do
#ext0=2D_Y2_BS
#ext=${ext0}_${var}_mgg
#dir=${ext0}/ws_2016-17_${var} 
#./bin/fTest -i /afs/cern.ch/work/i/ivovtin/public/ForLeonid/${dir}/Node_SM/ws_hhbbgg.data_bkg.root --saveMultiPdf CMS-HGG_multipdf_${ext}.root  -D IVAN_${ext} -d IVAN_${ext}/res.dat -f cat0,cat1,cat2,cat3,cat4,cat5,cat6,cat7,cat8,cat9,cat10,cat11 --isData 1 -c 12 --ismgg 1 &> fTestRes_${ext}.dat
#mkdir ${eosdir}
#cp IVAN_${ext}/*png ${eosdir}
#cp fTestRes_${ext}.dat ${eosdir}
#cp /eos/user/l/lkardapo/www/tmp/index.php ${eosdir}
#
#ext=${ext0}_${var}_mjj
#./bin/fTest -i /afs/cern.ch/work/i/ivovtin/public/ForLeonid/${dir}/Node_SM/ws_hhbbgg.data_bkg.root --saveMultiPdf CMS-HGG_multipdf_${ext}.root  -D IVAN_${ext} -d IVAN_${ext}/res.dat -f cat0,cat1,cat2,cat3,cat4,cat5,cat6,cat7,cat8,cat9,cat10,cat11 --isData 1 -c 12 --ismgg 0 &> fTestRes_${ext}.dat
#mkdir ${eosdir}
#cp IVAN_${ext}/*png ${eosdir}
#cp fTestRes_${ext}.dat ${eosdir}
#cp /eos/user/l/lkardapo/www/tmp/index.php ${eosdir}
#done






#for var in new_1D_ttHoff new_1D_ttHon std_1D_ttHoff std_1D_ttHon
#do
#ext0=1D_Y3
#ext=${ext0}_${var}
#dir=${ext0}/ws_2016-17_${var} 
#./bin/fTest -i /afs/cern.ch/work/i/ivovtin/public/ForLeonid/${dir}/Node_SM/ws_hhbbgg.data_bkg.root --saveMultiPdf CMS-HGG_multipdf_${ext}.root  -D IVAN_${ext} -d IVAN_${ext}/res.dat -f cat0,cat1,cat2,cat3,cat4,cat5,cat6,cat7,cat8,cat9,cat10,cat11 --isData 1 -c 12 --ismgg 1 &> fTestRes_${ext}.dat
#eosdir=/eos/user/l/lkardapo/www/HHbbgg/fTest/${ext}
#mkdir ${eosdir}
#cp IVAN_${ext}/*png ${eosdir}
#cp fTestRes_${ext}.dat ${eosdir}
#cp /eos/user/l/lkardapo/www/tmp/index.php ${eosdir}
#done
#
#
#
#
#
#
