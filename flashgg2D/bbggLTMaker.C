#define bbggLTMaker_cxx

#include "bbggLTMaker.h"

const bool DEBUG = 0;

// Cuts for 2016 categorization (_which==0):
const Float_t cutJetBtag = 0.8;
const Float_t boundary_MX[3]  = {0,350,35000};
const Float_t boundary_MVA_tagger2016_LM[3] = {0.600, 0.985, 1.0};
const Float_t boundary_MVA_tagger2016_HM[3] = {0.600, 0.970, 1.0};

// Cuts for 2017 tagger, using 2016 categorization (_which==1):
const Float_t boundary_MVA_tagger2017_LM[3] = {0.115, 0.597, 1.0};
const Float_t boundary_MVA_tagger2017_HM[3] = {0.226, 0.6195, 1.0};

// Cuts for 2017 tagger, using 2017 optimized categorization (_which==2):
//const Float_t boundary_MVA_tagger2017[4] = {0.271, 0.543, 0.740, 1.05};
//const Float_t boundary_MX_2017[5]  = {250, 341.4, 426.1, 544, 35000};

//// upating the categrisation number according to this part of 2017 analysis code https://github.com/michelif/flashgg/blob/hh_tag_94X_20180601/Taggers/python/flashggDoubleHTag_cfi.py#L31-L34
const Float_t boundary_MVA_2017[4]   = {0.29, 0.441, 0.724, 1.0}; // category boundaries for MVA
const Float_t boundary_MX_2017[5]   = {250., 354., 478., 560., 35000};// .. and MX

//const Float_t MjjCuts_Low[] = {97.,95.,95.,95.,99.,100.,95.,95.,95.,95.,95.,108.};

//const Float_t MjjCuts_High[] = {146.,153.,150.,155.,150.,147.,154.,151.,155.,155.,155.,148.};

const Float_t MjjCuts_Low[] = {98.0,95.0,97.0,96.0,95.0,95.0,95.0,95.0,95.0,95.0,95.0,95.0};
const Float_t MjjCuts_High[] = {150.0,150.0,143.0,150.0,150.0,150.0,150.0,145.0,155.0,142.0,146.0,152.0};

void bbggLTMaker::Begin(TTree * /*tree*/)
{
  TString option = GetOption();

  cout<<"Options are: \n \t "<<option.Data()<<endl;
  TObjArray *args = (TObjArray*)option.Tokenize(" ");
  _normalization = std::stod((string)((TObjString*)args->At(0))->GetString());
  _outFileName = (string)((TObjString*)args->At(1))->GetString();
  _genDiPhotonFilter = (Bool_t) std::stoi( (string)((TObjString*)args->At(2))->GetString() );
  _whichCategorization = (Int_t) std::stoi( (string)((TObjString*)args->At(3))->GetString() );

  std::cout<<"Input paremeters:\n"
	   <<"_normalization: "<<_normalization<<"\n"
	   <<"_outFileName: "<< _outFileName<<"\n"
	   <<"_genDiPhotonFilter: "<< _genDiPhotonFilter<<"\n"
	   <<std::endl;

  // Could make those come from external options as well:
  _phoVariation = 0;
  _trigVariation = 0;

  _outFile = new TFile(_outFileName, "RECREATE");
  _outTree = new TTree("LT", "A tree for studying new particles");

  _outTree->Branch("run", &o_run, "o_run/i");
  _outTree->Branch("evt", &o_evt, "o_evt/l");

  _outTree->Branch("evWeight", &o_weight, "o_weight/D");
  _outTree->Branch("mgg", &o_mgg, "o_mgg/D");
  _outTree->Branch("mjj", &o_mjj, "o_mjj/D");
  _outTree->Branch("MX",  &o_MX,  "o_MX/D");
  _outTree->Branch("mbbgg", &o_bbggMass, "o_bbggMass/D");
  _outTree->Branch("catID", &o_catID, "o_catID/I");


  TString phoSFID_file = TString(std::getenv("CMSSW_BASE")) + TString("/src/HiggsAnalysis/bbggLimits2018/Weights/MVAID/egammaEffi.txt_EGM2D.root");
  if (DEBUG) cout << "phoSFsID file: " << phoSFID_file << endl;
  TString phoSFeveto_file = TString(std::getenv("CMSSW_BASE")) + TString("/src/HiggsAnalysis/bbggLimits2018/Weights/MVAID/ScalingFactors_80X_Summer16.root");
  if (DEBUG) cout << "phoSFsEV file: " << phoSFeveto_file << endl;
  bbggLTMaker::SetupPhotonSF( phoSFID_file, phoSFeveto_file);

  TString trig_file = TString(std::getenv("CMSSW_BASE")) + TString("/src/HiggsAnalysis/bbggLimits2018/Weights/TriggerSF/TriggerSFs.root");
  if (DEBUG) cout << "TriggSF file: " << trig_file << endl;
  bbggLTMaker::SetupTriggerSF(trig_file);

}

void bbggLTMaker::SlaveBegin(TTree * /*tree*/)
{
  TString option = GetOption();
}

Bool_t bbggLTMaker::Process(Long64_t entry)
{
  GetEntry(entry);
  //std::cout<<"Processing event "<<event<<" in run "<<run<<std::endl;

  //if( _genDiPhotonFilter && nPromptInDiPhoton < 2 ) return kTRUE;

  o_run = run;
  o_evt = event;

  o_weight = weight*_normalization*(1./77.4);
  //o_weight = 1;
  o_mgg = CMS_hgg_mass;
  o_mjj = Mjj;
  o_bbggMass = diHiggs_mass;

  o_MX = MX;

  o_catID = -1;

  if (_whichCategorization==0){
    if (o_MX > boundary_MX_2017[0] && o_MX <= boundary_MX_2017[1]){
      if( leadingJet_bDis < cutJetBtag || subleadingJet_bDis < cutJetBtag)
	return kTRUE; // Some clean-up based on b-tag scores

      if (HHbbggMVA > boundary_MVA_2017[0] && HHbbggMVA <= boundary_MVA_2017[1])
	o_catID = 3;
      else if (HHbbggMVA > boundary_MVA_2017[1] && HHbbggMVA <= boundary_MVA_2017[2])
	o_catID = 2;
      else {
	return kTRUE;
      }
    }
    else if (o_MX > boundary_MX[1] && o_MX <= boundary_MX[2]){
      if (HHbbggMVA > boundary_MVA_tagger2016_HM[0] && HHbbggMVA <= boundary_MVA_tagger2016_HM[1])
	o_catID = 1;
      else if (HHbbggMVA > boundary_MVA_tagger2016_HM[1] && HHbbggMVA <= boundary_MVA_tagger2016_HM[2])
	o_catID = 0;
      else {
	//if (DEBUG) std::cout<<"MVA is out of bounds!  MVA="<<HHTagger_HM<<std::endl;
	return kTRUE;
      }
    }
    else  {
      std::cout<<"MX is out of bounds!  MX="<<o_MX<<std::endl;
      return kTRUE;
    }
  }
  else if (_whichCategorization==1){
    if (o_MX > boundary_MX[0] && o_MX <= boundary_MX[1]){
      if( leadingJet_bDis < cutJetBtag || subleadingJet_bDis < cutJetBtag)
	return kTRUE; // Some clean-up based on b-tag scores

      if (HHbbggMVA > boundary_MVA_tagger2017_LM[0] && HHbbggMVA <= boundary_MVA_tagger2017_LM[1])
	o_catID = 3;
      else if (HHbbggMVA > boundary_MVA_tagger2017_LM[1] && HHbbggMVA <= boundary_MVA_tagger2017_LM[2])
	o_catID = 2;
      else {
	//if (DEBUG) std::cout<<"MVA is out of bounds!  MVA="<<HHTagger_LM<<std::endl;
	return kTRUE;
      }
    }
    else if (o_MX > boundary_MX[1] && o_MX <= boundary_MX[2]){
      if (HHbbggMVA > boundary_MVA_tagger2017_HM[0] && HHbbggMVA <= boundary_MVA_tagger2017_HM[1])
	o_catID = 1;
      else if (HHbbggMVA > boundary_MVA_tagger2017_HM[1] && HHbbggMVA <= boundary_MVA_tagger2017_HM[2])
	o_catID = 0;
      else {
	//if (DEBUG) std::cout<<"MVA is out of bounds!  MVA="<<HHTagger_HM<<std::endl;
	return kTRUE;
      }
    }
    else  {
      std::cout<<"MX is out of bounds!  MX="<<o_MX<<std::endl;
      return kTRUE;
    }
  }

//// using categrisation 2 for 2017 ///////

  else if (_whichCategorization==2 || _whichCategorization==3){
    if (o_MX > boundary_MX_2017[0] && o_MX <= boundary_MX_2017[1]){
      if (HHbbggMVA > boundary_MVA_2017[0] && HHbbggMVA <= boundary_MVA_2017[1]){
	o_catID = 11;
	if (_whichCategorization==3 && (o_mjj < MjjCuts_Low[o_catID] || o_mjj > MjjCuts_High[o_catID]) )
	  return kTRUE;
      }
      else if (HHbbggMVA > boundary_MVA_2017[1] && HHbbggMVA <= boundary_MVA_2017[2]){
	o_catID = 7;
	if (_whichCategorization==3 && (o_mjj < MjjCuts_Low[o_catID] || o_mjj > MjjCuts_High[o_catID]) )
	  return kTRUE;
      }
      else if (HHbbggMVA > boundary_MVA_2017[2] && HHbbggMVA <= boundary_MVA_2017[3]){
        o_catID = 3;
        if (_whichCategorization==3 && (o_mjj < MjjCuts_Low[o_catID] || o_mjj > MjjCuts_High[o_catID]) )
          return kTRUE;
      }
      else {
	return kTRUE;
      }
    }

    else if (o_MX > boundary_MX_2017[1] && o_MX <= boundary_MX_2017[2]){
      if (HHbbggMVA > boundary_MVA_2017[0] && HHbbggMVA <= boundary_MVA_2017[1]){
	o_catID = 10;
	if (_whichCategorization==3 && (o_mjj < MjjCuts_Low[o_catID] || o_mjj > MjjCuts_High[o_catID]) )
	  return kTRUE;
      }
      else if (HHbbggMVA > boundary_MVA_2017[1] && HHbbggMVA <= boundary_MVA_2017[2]){
	o_catID = 6;
	if (_whichCategorization==3 && (o_mjj < MjjCuts_Low[o_catID] || o_mjj > MjjCuts_High[o_catID]) )
	  return kTRUE;
      }
      else if (HHbbggMVA > boundary_MVA_2017[2] && HHbbggMVA <= boundary_MVA_2017[3]){
        o_catID = 2;
        if (_whichCategorization==3 && (o_mjj < MjjCuts_Low[o_catID] || o_mjj > MjjCuts_High[o_catID]) )
          return kTRUE;
      }
      else {
	return kTRUE;
      }
    }

    else if (o_MX > boundary_MX_2017[2] && o_MX <= boundary_MX_2017[3]){
      if (HHbbggMVA > boundary_MVA_2017[0] && HHbbggMVA <= boundary_MVA_2017[1]){
	o_catID = 9;
	if (_whichCategorization==3 && (o_mjj < MjjCuts_Low[o_catID] || o_mjj > MjjCuts_High[o_catID]) )
	  return kTRUE;
      }
      else if (HHbbggMVA > boundary_MVA_2017[1] && HHbbggMVA <= boundary_MVA_2017[2]){
	o_catID = 5;
	if (_whichCategorization==3 && (o_mjj < MjjCuts_Low[o_catID] || o_mjj > MjjCuts_High[o_catID]) )
	  return kTRUE;
      }
      else if (HHbbggMVA > boundary_MVA_2017[2] && HHbbggMVA <= boundary_MVA_2017[3]){
        o_catID = 1;
        if (_whichCategorization==3 && (o_mjj < MjjCuts_Low[o_catID] || o_mjj > MjjCuts_High[o_catID]) )
          return kTRUE;
      }
      else {
	return kTRUE;
      }
    }
    
    else if (o_MX > boundary_MX_2017[3] && o_MX <= boundary_MX_2017[4]){
      if (HHbbggMVA > boundary_MVA_2017[0] && HHbbggMVA <= boundary_MVA_2017[1]){
	o_catID = 8;
	if (_whichCategorization==3 && (o_mjj < MjjCuts_Low[o_catID] || o_mjj > MjjCuts_High[o_catID]) )
	  return kTRUE;
      }
      else if (HHbbggMVA > boundary_MVA_2017[1] && HHbbggMVA <= boundary_MVA_2017[2]){
	o_catID = 4;
	if (_whichCategorization==3 && (o_mjj < MjjCuts_Low[o_catID] || o_mjj > MjjCuts_High[o_catID]) )
	  return kTRUE;
      }
      else if (HHbbggMVA > boundary_MVA_2017[2] && HHbbggMVA <= boundary_MVA_2017[3]){
        o_catID = 0;
        if (_whichCategorization==3 && (o_mjj < MjjCuts_Low[o_catID] || o_mjj > MjjCuts_High[o_catID]) )
          return kTRUE;
      }
      else {
        return kTRUE;
      }

     }

    else  {
      std::cout<<"MX is out of bounds!  MX="<<o_MX<<std::endl;
      return kTRUE;
    }
  }
  else {
    std::cout<<"This categorization is not supported: "<<_whichCategorization<<std::endl;
    return kTRUE;
  }

  const Double_t preweight = 1*_normalization;
  if( preweight == 1) o_weight = 1; // When preweight == 1 it's the data, no SF needed.
  /*else {
    const Double_t pho1_sf = bbggLTMaker::PhotonSF(*leadingPhoton, _phoVariation);
    const Double_t pho2_sf = bbggLTMaker::PhotonSF(*subleadingPhoton, _phoVariation);

    const Double_t trig_sf = bbggLTMaker::TriggerSF(*leadingPhoton, leadingPhotonR9full5x5, *subleadingPhoton, subleadingPhotonR9full5x5, _trigVariation);
    if(DEBUG)
      cout << "Trigger sf: " << trig_sf << " lpt " << leadingPhoton->Pt()<< " spt " << subleadingPhoton->Pt()
	   << " leta "<< leadingPhoton->Eta() << " seta "<< subleadingPhoton->Eta()
	   << " lr9 "<< leadingPhotonR9full5x5 << " sr9 "<< subleadingPhotonR9full5x5 << std::endl;

    const Double_t bDiffWeight = 0.96; // Use an average SF for b-tagging

    o_weight = preweight*bDiffWeight*pho1_sf*pho2_sf*trig_sf;
  }*/

  // This is temporary, to check effect on fake data
  //if (_normalization == 1) o_weight = 1;

  _outTree->Fill();

  return kTRUE;
}

void bbggLTMaker::SlaveTerminate()
{
}

void bbggLTMaker::Terminate()
{
  _outFile->Write();
  _outFile->Close();
}


void bbggLTMaker::SetupPhotonSF(TString idfile, TString evfile)
{
  photonidFile = new TFile(idfile, "READ");
  photonIDhist = (TH2F*) photonidFile->Get("EGamma_SF2D;1");
  csevFile = new TFile(evfile, "READ");
  csevhist = (TH2F*) csevFile->Get("Scaling_Factors_CSEV_R9 Inclusive");
}


float bbggLTMaker::PhotonSF(bbggLTMaker::LorentzVector pho, int phovar)
{
  float sf_id = -99, sf_ev = -99, sf_id_err = -99, sf_ev_err = -99;
  sf_id_err = photonIDhist->GetBinError( photonIDhist->FindBin(pho.eta(), pho.pt()) );
  sf_id = photonIDhist->GetBinContent( photonIDhist->FindBin(pho.eta(), pho.pt()) ) + phovar*sf_id_err;
  sf_ev_err = csevhist->GetBinContent( csevhist->FindBin(pho.eta(), pho.pt()) );
  if(sf_ev_err==1) sf_ev_err=0;
  sf_ev = csevhist->GetBinContent( csevhist->FindBin(fabs(pho.eta()), pho.pt()) ) + phovar*sf_ev_err;
  float totSF = sf_id*sf_ev;
  if (totSF < 1E-1) return 1;
  else return totSF;
}

void bbggLTMaker::SetupTriggerSF(TString sfFile)
{
  triggerFile = new TFile(sfFile, "READ");
  ltriggerHist = (TH3F*) triggerFile->Get("leadingPhotonTSF");
  striggerHist = (TH3F*) triggerFile->Get("subleadingPhotonTSF");
}

float bbggLTMaker::TriggerSF(LorentzVector lpho, float lr9, LorentzVector spho, float sr9, int var)
{
   int leadingBin = ltriggerHist->FindBin(lr9, fabs(lpho.Eta()), lpho.Pt());
   int subleadingBin = striggerHist->FindBin(sr9, fabs(spho.Eta()), spho.Pt());
   if(DEBUG) cout << "[bbggLTMaker::TriggerSF] leading Bin: " << leadingBin << " subleadingBin " << subleadingBin << std::endl;

   float leadingSF = ltriggerHist->GetBinContent(leadingBin);
   float sleadingSF =striggerHist->GetBinContent(subleadingBin);
   if(DEBUG) cout << "[bbggLTMaker::TriggerSF] leading SF: " << leadingSF << " subleadingSF " << sleadingSF << std::endl;

   float leadingErr = ltriggerHist->GetBinError(leadingBin);
   float subleadingErr = striggerHist->GetBinError(subleadingBin);
   if(DEBUG) cout << "[bbggLTMaker::TriggerSF] leading SFerr: " << leadingErr << " subleadingSFerr " << subleadingErr << std::endl;

   return (leadingSF + var*leadingErr)*(sleadingSF + var*subleadingErr);
}
