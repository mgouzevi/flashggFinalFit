max_ind=(3 3 3 3 3 3 3 4 3 4 4 5);
#max_ind=(3 3 3 3 3 3 4 3 3 3 4 5);
ext=new_1D_ttHon_ethbound
#ext=Mjj_new_1D_ttHon0.21
#ext=new_ptmgg_ptmjj_dR_1D_ttHon_ethbound
#ext=new_ptmgg_ptmjj_dR_1D_ttHon
#ext=new_ptmgg_ptmjj_dR_2D_ttHon_mjj80
#ext=new_ptmgg_ptmjj_dR_2D_ttHon_newbound

mkdir -p /eos/user/i/ivovtin/www/HHbbgg/envelope/${ext}
cp /eos/user/i/ivovtin/www/tmp/index.php /eos/user/i/ivovtin/www/HHbbgg/envelope/${ext}
for var in {0..11}
do
root -l -b -q 'envAllN.C+("'${ext}'",'$var','${max_ind[$var]}')'
done


#for var in {3..10}
#do
#root -l -b -q "SlicePicCorr.C+($var)"
#done
