CMS_hhbbgg_13TeV_mgg_bkg_par1_cat8[0.9, -1000, 1000];
CMS_hhbbgg_13TeV_mgg_bkg_par2_cat8[0.5, -1000, 1000];
CMS_hhbbgg_13TeV_mgg_bkg_par3_cat8[0.5, -1000, 1000];

mgg_sig_m0_cat8[125., 122, 127];
mgg_sig_sigma_cat8[1.0, 0.1, 3.0];
mgg_sig_alpha1_cat8[1.0, 0.05, 10.0];
mgg_sig_n1_cat8[2.0, 0.1, 10.0];
mgg_sig_alpha2_cat8[1.0, 0.05, 10.0];
mgg_sig_n2_cat8[2.0, 0.1, 10.0];
mggSig_cat8 = RooDoubleCB(mgg, mgg_sig_m0_cat8, mgg_sig_sigma_cat8, mgg_sig_alpha1_cat8, mgg_sig_n1_cat8, mgg_sig_alpha2_cat8, mgg_sig_n2_cat8);

mgg_hig_m0_ggh_cat8[124.2, 123, 125];
mgg_hig_sigma_ggh_cat8[2.0, 0.1, 3.0];
mgg_hig_alpha1_ggh_cat8[1.0, 0.05, 10.0];
mgg_hig_n1_ggh_cat8[2.0, 0.1, 10.0];
mgg_hig_alpha2_ggh_cat8[1.0, 0.05, 10.0];
mgg_hig_n2_ggh_cat8[2.0, 0.1, 10.0];
mggHig_ggh_cat8 = RooDoubleCB(mgg, mgg_hig_m0_ggh_cat8, mgg_hig_sigma_ggh_cat8, mgg_hig_alpha1_ggh_cat8, mgg_hig_n1_ggh_cat8, mgg_hig_alpha2_ggh_cat8, mgg_hig_n2_ggh_cat8);

mgg_hig_m0_tth_cat8[124.2, 123, 125];
mgg_hig_sigma_tth_cat8[2.0, 0.1, 3.0];
mgg_hig_alpha1_tth_cat8[1.0, 0.05, 10.0];
mgg_hig_n1_tth_cat8[2.0, 0.1, 10.0];
mgg_hig_alpha2_tth_cat8[1.0, 0.05, 10.0];
mgg_hig_n2_tth_cat8[2.0, 0.1, 10.0];
mggHig_tth_cat8 = RooDoubleCB(mgg, mgg_hig_m0_tth_cat8, mgg_hig_sigma_tth_cat8, mgg_hig_alpha1_tth_cat8, mgg_hig_n1_tth_cat8, mgg_hig_alpha2_tth_cat8, mgg_hig_n2_tth_cat8);

mgg_hig_m0_vh_cat8[124.2, 123, 125];
mgg_hig_sigma_vh_cat8[2.0, 0.1, 3.0];
mgg_hig_alpha1_vh_cat8[1.0, 0.05, 10.0];
mgg_hig_n1_vh_cat8[2.0, 0.1, 10.0];
mgg_hig_alpha2_vh_cat8[1.0, 0.05, 10.0];
mgg_hig_n2_vh_cat8[2.0, 0.1, 10.0];
mggHig_vh_cat8 = RooDoubleCB(mgg, mgg_hig_m0_vh_cat8, mgg_hig_sigma_vh_cat8, mgg_hig_alpha1_vh_cat8, mgg_hig_n1_vh_cat8, mgg_hig_alpha2_vh_cat8, mgg_hig_n2_vh_cat8);

mgg_hig_m0_vbf_cat8[124.2, 123, 125];
mgg_hig_sigma_vbf_cat8[2.0, 0.1, 3.0];
mgg_hig_alpha1_vbf_cat8[1.0, 0.05, 10.0];
mgg_hig_n1_vbf_cat8[2.0, 0.1, 10.0];
mgg_hig_alpha2_vbf_cat8[1.0, 0.05, 10.0];
mgg_hig_n2_vbf_cat8[2.0, 0.1, 10.0];
mggHig_vbf_cat8 = RooDoubleCB(mgg, mgg_hig_m0_vbf_cat8, mgg_hig_sigma_vbf_cat8, mgg_hig_alpha1_vbf_cat8, mgg_hig_n1_vbf_cat8, mgg_hig_alpha2_vbf_cat8, mgg_hig_n2_vbf_cat8);

mgg_hig_m0_bbh_cat8[124.2, 123, 125];
mgg_hig_sigma_bbh_cat8[2.0, 0.1, 3.0];
mgg_hig_alpha1_bbh_cat8[1.0, 0.05, 10.0];
mgg_hig_n1_bbh_cat8[2.0, 0.1, 10.0];
mgg_hig_alpha2_bbh_cat8[1.0, 0.05, 10.0];
mgg_hig_n2_bbh_cat8[2.0, 0.1, 10.0];
mggHig_bbh_cat8 = RooDoubleCB(mgg, mgg_hig_m0_bbh_cat8, mgg_hig_sigma_bbh_cat8, mgg_hig_alpha1_bbh_cat8, mgg_hig_n1_bbh_cat8, mgg_hig_alpha2_bbh_cat8, mgg_hig_n2_bbh_cat8);


mjj_sig_m0_cat8[110.0, 99, 140];
mjj_sig_sigma_cat8[10.0, 1.0, 60.0];
mjj_sig_alpha1_cat8[1.0, 0.05, 10.0];
mjj_sig_n1_cat8[2.0, 0.1, 10.0];
mjj_sig_alpha2_cat8[1.0, 0.05, 10.0];
mjj_sig_n2_cat8[2.0, 0.1, 10.0];
mjjSig_cat8 = RooDoubleCB(mjj, mjj_sig_m0_cat8, mjj_sig_sigma_cat8, mjj_sig_alpha1_cat8, mjj_sig_n1_cat8, mjj_sig_alpha2_cat8, mjj_sig_n2_cat8);

CMS_hhbbgg_13TeV_mjj_bkg_par1_cat8[0.9, -1000, 1000];
CMS_hhbbgg_13TeV_mjj_bkg_par2_cat8[0.5, -1000, 1000];
CMS_hhbbgg_13TeV_mjj_bkg_par3_cat8[0.5, -1000, 1000];

mjj_hig_par1_ggh_cat8[0.1, 0, 10];
mjj_hig_par2_ggh_cat8[0.1, 0, 10];
mjj_hig_par3_ggh_cat8[0.1, 0, 10];

mjj_hig_par1_vbf_cat8[0.1, 0, 10];
mjj_hig_par2_vbf_cat8[0.1, 0, 10];
mjj_hig_par3_vbf_cat8[0.1, 0, 10];

mjj_hig_m0_tth_cat8[100, 70, 190];
mjj_hig_sigma_tth_cat8[50, 10, 100];
mjj_hig_alpha1_tth_cat8[1.0, 0.01, 10];
mjj_hig_n1_tth_cat8[1, 0.01, 10];
mjj_hig_alpha2_tth_cat8[1.0, 0.01, 10];
mjj_hig_n2_tth_cat8[1, 0.01, 10];
mjjHig_tth_cat8 = RooDoubleCB(mjj, mjj_hig_m0_tth_cat8, mjj_hig_sigma_tth_cat8, mjj_hig_alpha1_tth_cat8, mjj_hig_n1_tth_cat8, mjj_hig_alpha2_tth_cat8, mjj_hig_n2_tth_cat8);

mjj_hig_m0_vh_cat8[100, 70, 190];
mjj_hig_sigma_vh_cat8[50, 10, 100];
mjj_hig_alpha1_vh_cat8[1.0, 0.01, 10];
mjj_hig_n1_vh_cat8[1, 0.01, 10];
mjj_hig_alpha2_vh_cat8[1.0, 0.01, 10];
mjj_hig_n2_vh_cat8[1, 0.01, 10];
mjjHig_vh_cat8 = RooDoubleCB(mjj, mjj_hig_m0_vh_cat8, mjj_hig_sigma_vh_cat8, mjj_hig_alpha1_vh_cat8, mjj_hig_n1_vh_cat8, mjj_hig_alpha2_vh_cat8, mjj_hig_n2_vh_cat8);

mjj_hig_m0_bbh_cat8[100, 10, 180];
mjj_hig_sigma_bbh_cat8[50, 1.0, 100];
mjj_hig_alpha1_bbh_cat8[1.0, 0.01, 10];
mjj_hig_n1_bbh_cat8[1, 0.01, 10];
mjj_hig_alpha2_bbh_cat8[1.0, 0.01, 10];
mjj_hig_n2_bbh_cat8[1, 0.01, 10];
mjjHig_bbh_cat8 = RooDoubleCB(mjj, mjj_hig_m0_bbh_cat8, mjj_hig_sigma_bbh_cat8, mjj_hig_alpha1_bbh_cat8, mjj_hig_n1_bbh_cat8, mjj_hig_alpha2_bbh_cat8, mjj_hig_n2_bbh_cat8);