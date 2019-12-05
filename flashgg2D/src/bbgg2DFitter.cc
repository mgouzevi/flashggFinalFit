#define bbgg2DFitter_cxx
#include "flashggFinalFit/flashgg2D/interface/bbgg2DFitter.h"

std::ofstream newCout;

void bbgg2DFitter::Initialize(RooWorkspace* workspace, Int_t SigMass, float Lumi,std::string folder_name,
			      std::string energy, Bool_t doBlinding, Int_t nCat, bool AddHiggs,
			      float minMggMassFit,float maxMggMassFit,float minMjjMassFit,float maxMjjMassFit,
			      float minSigFitMgg,float maxSigFitMgg,float minSigFitMjj,float maxSigFitMjj,
			      float minHigMggFit,float maxHigMggFit,float minHigMjjFit,float maxHigMjjFit,
			      Int_t doNRW, std::string logFileName, bool doARW)
{
  //std::cout<<"DBG.  We Initialize..."<<std::endl;

  _w = new RooWorkspace(*workspace);
  _doblinding = doBlinding;
  _NCAT = nCat;
  _sigMass = SigMass;
  _addHiggs = AddHiggs;
  _lumi = Lumi;
  _cut = "1";
  _folder_name=folder_name;
  _energy=energy;
  _minMggMassFit=minMggMassFit;
  _maxMggMassFit=maxMggMassFit;
  _minMjjMassFit=minMjjMassFit;
  _maxMjjMassFit=maxMjjMassFit;
  _minSigFitMgg=minSigFitMgg;
  _maxSigFitMgg=maxSigFitMgg;
  _minSigFitMjj=minSigFitMjj;
  _maxSigFitMjj=maxSigFitMjj;
  _minHigMggFit=minHigMggFit;
  _maxHigMggFit=maxHigMggFit;
  _minHigMjjFit=minHigMjjFit;
  _maxHigMjjFit=maxHigMjjFit;
  
  _doARW = doARW;

  _nonResWeightIndex = doNRW;

  // Some defaults here are:
  // -2: do Resonant limits
  // -1: Non-resonant limits from Nodes
  // 0-1506 && 1507-1518: Non-resonant limits with re-weighting
  if (_nonResWeightIndex>=0)
    _wName = Form("evWeight_NRW_%d",doNRW);
  else
    _wName = "evWeight";

  if (doARW) {
    _wName = "new_evWeight";
    _nonResWeightIndex = -10;
  }

  _NR_MassRegion=0;

  if (logFileName!=""){
    // If the file for logging specified, redirect all std::out to it:
    newCout.open(logFileName, std::ofstream::out);
    std::cout.rdbuf(newCout.rdbuf());
  }

  std::cout<<"\t Initialized the fitter"<<std::endl;
  std::cout<<"SigMass: "<<SigMass
	   <<"\n NR_MassRegion: "<<_NR_MassRegion
	   <<"\n doNRW: "<< doNRW
	   <<"\n wName:"<<_wName
	   <<"\n "<<std::endl;
}

RooAbsPdf* bbgg2DFitter::getPdf(PdfModelBuilder pdfsModel, string type, int order, const char* ext=""){
  
  if (type=="Bernstein") return pdfsModel.getBernstein(Form("%s_bern%d",ext,order),order); 
  else if (type=="Chebychev") return pdfsModel.getChebychev(Form("%s_cheb%d",ext,order),order); 
  else if (type=="Exponential") return pdfsModel.getExponentialSingle(Form("%s_exp%d",ext,order),order); 
  else if (type=="PowerLaw") return pdfsModel.getPowerLawSingle(Form("%s_pow%d",ext,order),order); 
  else if (type=="Laurent") return pdfsModel.getLaurentSeries(Form("%s_lau%d",ext,order),order); 
  else {
    cerr << "[ERROR] -- getPdf() -- type " << type << " not recognised." << endl;
    return NULL;
  }
}

RooArgSet* bbgg2DFitter::defineVariables(bool swithToSimpleWeight=false)
{
  RooRealVar* mgg  = new RooRealVar("mgg","M(#gamma#gamma)",_minMggMassFit,_maxMggMassFit,"GeV");
  RooRealVar* mtot = new RooRealVar("mtot","M(#gamma#gammajj)",200,1600,"GeV");
  RooRealVar* mjj  = new RooRealVar("mjj","M(jj)",_minMjjMassFit,_maxMjjMassFit,"GeV");
  RooRealVar* ttHTagger  = new RooRealVar("ttHTagger","BDT",-1,1,"");
  RooCategory* catID = new RooCategory("catID","Event category ID") ;
  RooRealVar* evWeight = 0;
  RooRealVar* new_evWeight = 0;

  TString tmp_wName(_wName.c_str());
  // This is to address a specific issue when adding single Higgs samples, while running with --NRW option.
  // In that case, for a signal sample we should take evWeight_NRW_%d, while for single higgs sample, we should use evWeight
  if (swithToSimpleWeight)
    tmp_wName="evWeight";
  // --- //

  if (!_doARW)
    evWeight = new RooRealVar(tmp_wName,"HqT x PUwei",-100000.,100000, "");
  else
  {
    evWeight = new RooRealVar("evWeight","HqT x PUwei",-100000, 100000,"");
    new_evWeight = new RooRealVar("new_evWeight","HqT x PUwei x ARW",-100000,100000,"");
  }


  for ( int i=0; i<_NCAT; ++i)
    catID->defineType(Form("cat_%i", i), i);
  
  //
  RooArgSet* ntplVars = 0;
  if (_doARW)
    ntplVars = new RooArgSet(*mgg, *mjj, *catID, *evWeight, *new_evWeight);
  else if (_nonResWeightIndex>=-1) {
    ntplVars = new RooArgSet(*mgg, *mjj, *catID, *evWeight);
    ntplVars->add(*mtot);
  }
  else {
    ntplVars = new RooArgSet(*mgg, *mjj, *catID, *evWeight);
  }
  
  if (_cut.Contains("ttHTagger"))
    ntplVars->add(*ttHTagger);
  
  return ntplVars;
}

int bbgg2DFitter::AddSigData(float mass, TString signalfile)
{
  if (_verbLvl>1) std::cout << "================= Add Signal========================== " << _wName.c_str() << " " << _doARW << " " << _nonResWeightIndex << std::endl;
  if (_verbLvl>1) std::cout << " File to open:"<<signalfile  << std::endl;
  TFile *sigFile = TFile::Open(signalfile);
  bool opened=sigFile->IsOpen();
  if(opened==false) return -1;
  if (_verbLvl>1) std::cout << " TFile opened:"<<signalfile  << std::endl;

  TTree* sigTree = (TTree*)sigFile->Get("LT");

  //Luminosity
  RooRealVar lumi("lumi","lumi", _lumi);
  _w->import(lumi);
  //Define variables
  RooArgSet* ntplVars = bbgg2DFitter::defineVariables();
  if(sigTree==nullptr)
    {
      if (_verbLvl>1) std::cout<<"LT tree for AddSigData not found."<<std::endl;
      std::exit(1);
    }

  if (_verbLvl>0) {
    std::cout<<"[DBG]  Prining ntplVars from sig"<<std::endl;
    ntplVars->Print();
  }

  RooDataSet sigScaled("sigScaled", "dataset", sigTree, *ntplVars, _cut, _wName.c_str());
  //  if(_doARW) sigScaled = RooDataSet("sigScaled","dataset",sigTree,*ntplVars,_cut, "new_evWeight");
  //  else sigScaled = RooDataSet("sigScaled","dataset",sigTree,*ntplVars,_cut, _wName.c_str());

  RooDataSet* sigToFit[_NCAT];
  TString cut0 = " && 1>0";

  RooArgList myArgList(*_w->var("mgg"));
  myArgList.add(*_w->var("mjj"));

  if (_nonResWeightIndex>=-1)
    myArgList.add(*_w->var("mtot"));
  

  myArgList.Print();

  for ( int i=0; i<_NCAT; ++i)
    {

      if (_verbLvl>0) {
	std::cout << "-- Reducing category " << i << std::endl;
	std::cout << "Including the _cut: " << _cut << std::endl;
      }
      
      sigToFit[i] = (RooDataSet*) sigScaled.reduce(myArgList,_cut+TString::Format(" && catID==%d ",i)+cut0);

      this->SetSigExpectedCats(i, sigToFit[i]->sumEntries());

      if (_verbLvl>0) {
	std::cout << "======================================================================" <<std::endl;
	std::cout<<"[DBG]  Cat="<<i<< "\t Sig sumEntries="<<sigToFit[i]->sumEntries()<<std::endl;
	std::cout<<"mGG:  Mean = "<<sigToFit[i]->mean(*_w->var("mgg"))<<"  sigma = "<<sigToFit[i]->sigma(*_w->var("mgg"))<<std::endl;
	if (_fitStrategy != 1)
	  std::cout<<"mJJ:  Mean = "<<sigToFit[i]->mean(*_w->var("mjj"))<<"  sigma = "<<sigToFit[i]->sigma(*_w->var("mjj"))<<std::endl;

	if (_nonResWeightIndex>=-1)
	  std::cout<<"mTot: Mean = "<<sigToFit[i]->mean(*_w->var("mtot"))<<"  sigma = "<<sigToFit[i]->sigma(*_w->var("mtot"))<<std::endl;
      }

      /*This defines each category*/
      std::cout << "-- Importing cat " << i << std::endl;
      _w->import(*sigToFit[i],Rename(TString::Format("Sig_cat%d",i)));
    }
  
  // Create full signal data set without categorization
  std::cout << "-- Reducing all signal, no cat" << std::endl;
  RooDataSet* sigToFitAll = (RooDataSet*) sigScaled.reduce(myArgList,_cut+cut0);

  _w->import(*sigToFitAll,Rename("Sig"));

  // here we print the number of entries on the different categories
  if (_verbLvl>1) {
    std::cout << "======================================================================" <<std::endl;
    std::cout << "========= the number of entries on the different categories ==========" <<std::endl;
    std::cout << "---- one channel: " << sigScaled.sumEntries() <<std::endl;
    for (int c = 0; c < _NCAT; ++c)
      {
	Float_t nExpEvt = sigToFit[c]->sumEntries();
	std::cout<<TString::Format("nEvt exp. cat%d : ",c)<<nExpEvt<<TString::Format(" eff x Acc cat%d : ",c)<<std::endl;
    } // close ncat
    std::cout << "======================================================================" <<std::endl;
    sigScaled.Print("v");
    std::cout << "----- DONE With Adding Signal! \n\n"<< std::endl;
  }
  return 0;
}

std::vector<float> bbgg2DFitter::AddHigData(float mass, TString signalfile, int higgschannel, TString higName)
{
  if (_verbLvl>1) {
    std::cout << "================= Adding Single Higgs ==========================" <<std::endl;
    std::cout<<" \t mass: "<<mass<<" signalfile="<<signalfile<<" higgschannel="<<higgschannel<<" higName="<<higName<<std::endl;
  }

  RooArgSet* ntplVars = defineVariables(1);

  TFile higFile(signalfile);
  TTree* higTree = (TTree*) higFile.Get("LT");
  if(higTree==nullptr)
    {
      if (_verbLvl>1) std::cout<<"LT for AddHigData not found "<<std::endl;
      std::exit(1);
    }
  RooDataSet higScaled("higScaled1","dataset",higTree, /* all variables of RooArgList*/*ntplVars,_cut,"evWeight");
  //
  RooDataSet* higToFit[_NCAT];
  TString cut0 = "&& 1>0";

  for ( int i=0; i<_NCAT; ++i)
    {
      higToFit[i] = (RooDataSet*) higScaled.reduce(RooArgList(*_w->var("mgg"),*_w->var("mjj")),_cut+TString::Format(" && catID==%d ",i)+cut0);
      //if(_fitStrategy == 1) higToFit[i] = (RooDataSet*) higScaled.reduce(RooArgList(*_w->var("mgg")),_cut+TString::Format(" && catID==%d ",i)+cut0);
      _w->import(*higToFit[i],Rename(TString::Format("Hig_%s_cat%d",higName.Data(),i)));
    }
  // Create full data set without categorization:
  RooDataSet* higToFitAll = (RooDataSet*) higScaled.reduce(RooArgList(*_w->var("mgg"),*_w->var("mjj")),_cut);
  _w->import(*higToFitAll,Rename("Hig"));
  // here we print the number of entries on the different categories
  if (_verbLvl>1) {
    std::cout << "========= the number of entries on the different categories (Higgs data) ==========" <<std::endl;
    std::cout << "---- one channel: " << higScaled.sumEntries() <<std::endl;
    for (int c = 0; c < _NCAT; ++c)
      {
	Float_t nExpEvt = higToFit[c]->sumEntries();
        std::cout<<TString::Format("nEvt exp. cat%d : ",c)<<nExpEvt<<TString::Format(" eff x Acc cat%d : ",c)<<"%"<<std::endl;
    }
    std::cout << "======================================================================" <<std::endl;
    higScaled.Print("v");
    std::cout << "===  DONE With Hig Data =="<<std::endl;
  }

  std::vector<float> thisExpHig;
  for (int c = 0; c < _NCAT; ++c)
  {
    Float_t nExpEvt = higToFit[c]->sumEntries();
    thisExpHig.push_back(nExpEvt);
  }
  return thisExpHig;

}

void bbgg2DFitter::AddBkgData(TString datafile)
{
  //Define variables
  RooArgSet* ntplVars = bbgg2DFitter::defineVariables();

  TFile dataFile(datafile);
  TTree* dataTree = (TTree*) dataFile.Get("LT");
  if(dataTree==nullptr){
    if (_verbLvl>1) std::cout<<"LT for AddBkgData not found "<<std::endl;
    std::exit(1);
  }
  RooDataSet Data("Data","dataset",dataTree,*ntplVars,"","evWeight");
  RooDataSet* dataToFit[_NCAT];
  RooDataSet* dataToPlot[_NCAT];
  TString cut0 = "&& 1>0"; // Cut for blinding (not used now)
  
  if (_verbLvl>1) std::cout<<"\n ================= Add Bkg ==============================="<<std::endl;
  if (_verbLvl>1) {
    std::cout<<"\t Total events in root file: "<<Data.sumEntries()<<std::endl;
    for( int i=0; i<_NCAT; ++i){
      std::cout<<"\t Reduced to category "<<i<<": "<<Data.reduce(Form("catID==%i",i))->sumEntries()
	       <<";  from the original Tree: "<<dataTree->GetEntries(Form("catID==%i",i))<<std::endl;    
    } 
  }
    
  for( int i=0; i<_NCAT; ++i){
    
    dataToFit[i] = (RooDataSet*) Data.reduce(RooArgList(*_w->var("mgg"),*_w->var("mjj")),_cut+TString::Format(" && catID==%d",i));
    
    if(_doblinding)
      dataToPlot[i] = (RooDataSet*) Data.reduce(RooArgList(*_w->var("mgg"),*_w->var("mjj")),_cut+TString::Format(" && catID==%d",i)+cut0);
    else
      dataToPlot[i] = (RooDataSet*) Data.reduce(RooArgList(*_w->var("mgg"),*_w->var("mjj")),_cut+TString::Format(" && catID==%d",i) );
    
    this->SetObservedCats(i, dataToFit[i]->sumEntries());
    
    if (_verbLvl>1) std::cout<<"\t categ="<<i<<"  events="<<dataToFit[i]->sumEntries()<<std::endl;
    
    
    _w->import(*dataToFit[i],Rename(TString::Format("Data_cat%d",i)));
    _w->import(*dataToPlot[i],Rename(TString::Format("Dataplot_cat%d",i)));
  }
  // Create full data set without categorization
  RooDataSet* data = (RooDataSet*) Data.reduce(RooArgList(*_w->var("mgg"),*_w->var("mjj")),_cut);
  //if (_fitStrategy == 1)
  //data = (RooDataSet*) Data.reduce(RooArgList(*_w->var("mgg")),_cut + TString(" && mjj < 140 "));
  _w->import(*data, Rename("Data"));
  if (_verbLvl>1) data->Print("v");
}
    
void bbgg2DFitter::SigModelFit(float mass)
{
  //******************************************//
  // Fit signal with model pdfs
  //******************************************//
  if (_verbLvl>1) std::cout << "Doing signal model fit for M = " <<mass<<std::endl;
  
  // four categories to fit
  RooDataSet* sigToFit[_NCAT];
  RooAbsPdf* mggSig[_NCAT];
  RooAbsPdf* mjjSig[_NCAT];
  RooProdPdf* SigPdf[_NCAT];
  RooAbsPdf* SigPdf1[_NCAT];

  RooRealVar* mgg = _w->var("mgg");
  RooRealVar* mjj = _w->var("mjj");
  mgg->setRange("SigFitRange",_minSigFitMgg,_maxSigFitMgg);
  mjj->setRange("SigFitRange",_minSigFitMjj,_maxSigFitMjj);
  for (int c = 0; c < _NCAT; ++c)
    {

      if (_verbLvl>1) std::cout << " Started with cat="<<c<<std::endl;
      
      sigToFit[c] = (RooDataSet*) _w->data(TString::Format("Sig_cat%d",c));
      mggSig[c] = (RooAbsPdf*) _w->pdf(TString::Format("mggSig_cat%d",c));
      mjjSig[c] = (RooAbsPdf*) _w->pdf(TString::Format("mjjSig_cat%d",c));

      if (_verbLvl>1) std::cout << " DBG point 1 cat="<<c<<std::endl;
      mggSig[c]->Print();
      mjjSig[c]->Print();
      
      if(_fitStrategy == 2) SigPdf[c] = new RooProdPdf(TString::Format("SigPdf_cat%d",c),"",RooArgSet(*mggSig[c], *mjjSig[c]));
      if (_verbLvl>1) std::cout << " DBG point 2 cat="<<c<<std::endl;
      if(_fitStrategy == 1) SigPdf1[c] = (RooAbsPdf*) mggSig[c]->Clone(TString::Format("SigPdf_cat%d",c));
      if (_verbLvl>1) std::cout << " DBG point 3 cat="<<c<<std::endl;
      
      ((RooRealVar*) _w->var(TString::Format("mgg_sig_m0_cat%d",c)))->setVal(mass);

      if (_verbLvl>1) std::cout << "OK up to now... Mass point: " <<mass<<"  cat="<<c<<std::endl;
      if(_fitStrategy == 2) SigPdf[c]->fitTo(*sigToFit[c],Range("SigFitRange"),SumW2Error(kTRUE),PrintLevel(-1));
      if(_fitStrategy == 1) SigPdf1[c]->fitTo(*sigToFit[c],Range("SigFitRange"),SumW2Error(kTRUE),PrintLevel(-1));
      if (_verbLvl>1) std::cout << "How is the Fit? Mass point: " <<mass<<"  cat="<<c<<std::endl;

      RooArgSet *sigParams = 0;
      if (_fitStrategy==2)
	sigParams = (RooArgSet*) SigPdf[c]->getParameters(RooArgSet(*mgg, *mjj));
      if (_fitStrategy==1)
	sigParams = (RooArgSet*) SigPdf1[c]->getParameters(RooArgSet(*mgg));
      
      _w->defineSet(TString::Format("SigPdfParam_cat%d",c), *sigParams);
      _w->set(TString::Format("SigPdfParam_cat%d",c))->Print("v");
      SetConstantParams(_w->set(TString::Format("SigPdfParam_cat%d",c)));
      
      if (_verbLvl>1){
	std::cout << "Print out the parameters of the fit" << std::endl;
	TIterator* paramIter = (TIterator*) sigParams->createIterator();
	TObject* tempObj = nullptr;
	while( (tempObj = paramIter->Next()) ) {
	  if ( (TString(tempObj->GetName()).EqualTo("mjj")) || (TString(tempObj->GetName()).EqualTo("mgg"))) continue;
	  std::cout << "Signal variables: " << tempObj->GetName() << std::endl;
	}
       	sigParams->Print("v");
      }
      
      if(_fitStrategy == 1) {
	_w->import(*SigPdf1[c]);
	//_w->import(*mggSig[c]);
      }
      if(_fitStrategy == 2)
	_w->import(*SigPdf[c]);

      if (_verbLvl>1) std::cout<<std::endl;
    }

  if (_verbLvl>1) std::cout << "Signal fit is done and imported to WS. M = " <<mass<<std::endl;

}

void bbgg2DFitter::HigModelFit(float mass, int higgschannel, TString higName)
{
  // four categories to fit
  RooDataSet* higToFit[_NCAT];
  RooAbsPdf* mggHig[_NCAT];
  RooAbsPdf* mjjHig[_NCAT];
  RooProdPdf* HigPdf[_NCAT];
  RooAbsPdf* HigPdf1[_NCAT];
  RooRealVar* mgg = _w->var("mgg");
  RooRealVar* mjj = _w->var("mjj");
  mgg->setRange("HigFitRange",_minHigMggFit,_maxHigMggFit);
  mjj->setRange("HigFitRange",_minHigMjjFit,_maxHigMjjFit);
  for (int c = 0; c < _NCAT; ++c)
    {
      // import sig and data from workspace
      higToFit[c] = (RooDataSet*) _w->data(TString::Format("Hig_%s_cat%d",higName.Data(),c));
      mggHig[c] = (RooAbsPdf*) _w->pdf(TString::Format("mggHig_%s_cat%d",higName.Data(),c));


      if (_fitStrategy==2){
	
	if(higName.Contains("ggh") == 1 || higName.Contains("vbf") == 1) {
	  mjjHig[c] = new RooBernstein(TString::Format("mjjHig_%s_cat%d",higName.Data(),c),"",*mjj,
				       RooArgList( *_w->var( TString::Format("mjj_hig_par1_%s_cat%d", higName.Data(),c) ),
						   *_w->var( TString::Format("mjj_hig_par2_%s_cat%d", higName.Data(),c) ),
						   *_w->var( TString::Format("mjj_hig_par3_%s_cat%d", higName.Data(),c) ) ));
	}
	else 
	  mjjHig[c] = (RooAbsPdf*) _w->pdf(TString::Format("mjjHig_%s_cat%d",higName.Data(),c));
	
	HigPdf[c] = new RooProdPdf(TString::Format("HigPdf_%s_cat%d",higName.Data(),c),"",RooArgSet(*mggHig[c], *mjjHig[c]));
      }
      if(_fitStrategy == 1) HigPdf1[c] = (RooAbsPdf*) mggHig[c]->Clone(TString::Format("HigPdf_%s_cat%d",higName.Data(),c));
      
      if (_verbLvl>1) {
	std::cout << TString::Format("mggHig_%s_cat%d",higName.Data(),c) << std::endl;
	mggHig[c]->Print();
	if (_fitStrategy==2){
	  std::cout << TString::Format("mjjHig_%s_cat%d",higName.Data(),c) << std::endl;
	  mjjHig[c]->Print();
	  std::cout << TString::Format("HigPdf_%s_cat%d",higName.Data(),c) << std::endl;
	  HigPdf[c]->Print();
	}
	std::cout << TString::Format("Dataset: Hig_%s_cat%d",higName.Data(),c) << std::endl;
	higToFit[c]->Print();
      }

      if (_verbLvl>1) std::cout << "OK up to now... Mass point: " <<mass<<std::endl;

      if(_fitStrategy == 2) HigPdf[c]->fitTo(*higToFit[c],Range("HigFitRange"),SumW2Error(kTRUE),PrintLevel(-1));
      if(_fitStrategy == 1) HigPdf1[c]->fitTo(*higToFit[c],Range("HigFitRange"),SumW2Error(kTRUE),PrintLevel(-1));
      if (_verbLvl>1) std::cout << "How is the Fit? Mass point: " <<mass<<"  cat="<<c<<std::endl;
      
      // IMPORTANT: fix all pdf parameters to constant

      RooArgSet *higParams = 0;
      if (_fitStrategy==2)
	higParams = (RooArgSet*) HigPdf[c]->getParameters(RooArgSet(*mgg, *mjj));
      if (_fitStrategy==1)
	higParams = (RooArgSet*) HigPdf1[c]->getParameters(RooArgSet(*mgg));
      
      _w->defineSet(TString::Format("HigPdfParam_%s_cat%d",higName.Data(),c), *higParams);
      SetConstantParams(_w->set(TString::Format("HigPdfParam_%s_cat%d",higName.Data(),c)));
            
      if (_verbLvl>1){
	std::cout << "Print out the parameters of the fit" << std::endl;
	TIterator* paramIter = (TIterator*) higParams->createIterator();
	TObject* tempObj = nullptr;
	while( (tempObj = paramIter->Next()) ) {
	  if ( (TString(tempObj->GetName()).EqualTo("mjj")) || (TString(tempObj->GetName()).EqualTo("mgg"))) continue;
	  std::cout << " variables: " << tempObj->GetName() << std::endl;
	}
       	higParams->Print("v");
      }
      
      if (_verbLvl>1) std::cout<<std::endl;
      
      if(_fitStrategy == 1) {
	_w->import(*HigPdf1[c]);
	//_w->import(*mggHig[c]);
      }
      if(_fitStrategy == 2)
	_w->import(*HigPdf[c]);

      
    } // close for ncat
} // close higgs model fit


void bbgg2DFitter::MakeSigWS(std::string fileBaseName)
{
  //**********************************************************************//
  // Write pdfs and datasets into the workspace for running limits
  //**********************************************************************//
  
  TString wsDir = TString::Format("%s/",_folder_name.data());
  std::vector<RooAbsPdf*> SigPdf(_NCAT,nullptr);
  RooWorkspace *wAll = new RooWorkspace("w_all","w_all");

  for (int c = 0; c < _NCAT; ++c)
    {

      _w->factory(TString::Format("CMS_hgg_sig_m0_absShift[1,1,1]"));
      _w->factory(TString::Format("CMS_hgg_sig_sigmaScale[1,1,1]"));

      if (_fitStrategy==2){
	_w->factory(TString::Format("CMS_hbb_sig_sigmaScale[1,1,1]"));
	_w->factory(TString::Format("CMS_hbb_sig_m0_absShift[1,1,1]"));
      }
      

      SigPdf[c] = (RooAbsPdf*) _w->pdf(TString::Format("SigPdf_cat%d",c));
      
      RooArgSet *sigParams = (RooArgSet*) SigPdf[c]->getParameters(RooArgSet(*_w->var("mgg"), *_w->var("mjj")));
                 
      TIterator* paramIter = (TIterator*) sigParams->createIterator();
      TObject* tempObj = nullptr;
      std::vector<std::pair<TString,TString>> varsToChange;
      while( (tempObj = paramIter->Next()) ) {
	if ( (TString(tempObj->GetName()).EqualTo("mjj")) || (TString(tempObj->GetName()).EqualTo("mgg"))) continue;
        TString thisVarName(tempObj->GetName());
        TString newVarName = TString(thisVarName);
        if ( !newVarName.Contains("m0") && !newVarName.Contains("sigma") ) {
          if ( newVarName.Contains("mgg") ) newVarName.ReplaceAll("mgg_", "CMS_hgg_");
          if ( newVarName.Contains("mjj") ) newVarName.ReplaceAll("mjj_", "CMS_hbb_");
          varsToChange.push_back(std::make_pair(thisVarName, newVarName));
        }
        std::cout << "Importing variable with new name: old - " << thisVarName << " new - " << newVarName << std::endl;
        _w->import( *_w->var( tempObj->GetName() ), RenameVariable( thisVarName, newVarName));
        wAll->import( *_w->var( tempObj->GetName() ), RenameVariable( thisVarName, newVarName));
      }
      //Shifts and smearings
      _w->factory(TString::Format("prod::CMS_hgg_sig_m0_cat%d(mgg_sig_m0_cat%d, CMS_hgg_sig_m0_absShift)", c, c));
      _w->factory(TString::Format("prod::CMS_hgg_sig_sigma_cat%d(mgg_sig_sigma_cat%d, CMS_hgg_sig_sigmaScale)", c, c));
      if (_fitStrategy==2){
	_w->factory(TString::Format("prod::CMS_hbb_sig_m0_cat%d(mjj_sig_m0_cat%d, CMS_hbb_sig_m0_absShift)", c, c));
	_w->factory(TString::Format("prod::CMS_hbb_sig_sigma_cat%d(mjj_sig_sigma_cat%d, CMS_hbb_sig_sigmaScale)", c, c));
      }
      if(!_useDSCB) {
        _w->factory(TString::Format("prod::CMS_hgg_gsigma_cat%d(mgg_sig_gsigma_cat%d, CMS_hgg_sig_sigmaScale)", c, c));
	if (_fitStrategy==2)
	  _w->factory(TString::Format("prod::CMS_hbb_gsigma_cat%d(mjj_sig_gsigma_cat%d, CMS_hbb_sig_sigmaScale)", c, c));
      }

      TString EditPDF = TString::Format("EDIT::CMS_sig_cat%d(SigPdf_cat%d,", c, c);
      for (unsigned int iv = 0; iv < varsToChange.size(); iv++)
        EditPDF += TString::Format("%s=%s,", varsToChange[iv].first.Data(), varsToChange[iv].second.Data());
      //Shifted and smeared vars
      if(!_useDSCB) {
        EditPDF += TString::Format("mgg_sig_gsigma_cat%d=CMS_hgg_sig_gsigma_cat%d", c, c);
	if (_fitStrategy==2)
	  EditPDF += TString::Format(",mjj_sig_gsigma_cat%d=CMS_hbb_sig_gsigma_cat%d)", c, c);
	else
	  EditPDF += (")");

      }
      EditPDF += TString::Format("mgg_sig_m0_cat%d=CMS_hgg_sig_m0_cat%d,", c, c);
      EditPDF += TString::Format("mgg_sig_sigma_cat%d=CMS_hgg_sig_sigma_cat%d", c, c);
      if (_fitStrategy==2){
	EditPDF += TString::Format(",mjj_sig_m0_cat%d=CMS_hbb_sig_m0_cat%d,", c, c);
	EditPDF += TString::Format("mjj_sig_sigma_cat%d=CMS_hbb_sig_sigma_cat%d)", c, c);
      }
      else
	EditPDF += (")");
      
      std::cout << "STRINGTOCHANGE   ---  " << EditPDF << std::endl;
      _w->factory(EditPDF);

      wAll->import(*_w->pdf(TString::Format("CMS_sig_cat%d",c)));
      wAll->import(*_w->data(TString::Format("Sig_cat%d",c)), Rename(TString::Format("Sig_cat%d", c)));
      //if (_fitStrategy==1)
      //wAll->import(*_w->pdf(TString::Format("mggSig_cat%d",c)), Rename(TString::Format("mggSig_cat%d", c)));
    }
  wAll->Print("v");
  //TString filename(wsDir+TString(fileBaseName)+".root");
  TString filename(wsDir+TString(fileBaseName)+".root");
  wAll->writeToFile(filename);
  if (_verbLvl>1) std::cout << "Write signal workspace in: " << filename << " file" << std::endl;
  return;
} // close make signal WP


void bbgg2DFitter::MakeHigWS(std::string fileHiggsName,int higgschannel, TString higName)
{
  TString wsDir = TString::Format("%s/",_folder_name.data());
  //**********************************************************************//
  // Write pdfs and datasets into the workspace before to save to a file
  // for statistical tests.
  //**********************************************************************//
  if (_verbLvl>1) std::cout << " \n ===== Creating WS for single Higgs background ====" << std::endl;

  std::vector<RooAbsPdf*> HigPdf(_NCAT,nullptr);
  RooWorkspace *wAll = new RooWorkspace("w_all","w_all");

  for (int c = 0; c < _NCAT; ++c)
    {
      HigPdf[c] = (RooAbsPdf*) _w->pdf(TString::Format("HigPdf_%s_cat%d",higName.Data(),c));


      RooArgSet *singleHigParams = (RooArgSet*) HigPdf[c]->getParameters(RooArgSet(*_w->var("mgg"), *_w->var("mjj")));

      TIterator* paramIter = (TIterator*) singleHigParams->createIterator();
      TObject* tempObj = nullptr;
      std::vector<std::pair<TString,TString>> varsToChange;
      while( (tempObj = paramIter->Next()) ) {
        if ( (TString(tempObj->GetName()).EqualTo("mjj")) || (TString(tempObj->GetName()).EqualTo("mgg"))) continue;
        TString thisVarName(tempObj->GetName());
        TString newVarName = TString(thisVarName).ReplaceAll(TString::Format("cat%d", c), TString::Format("cat%d", c));
        if ( !newVarName.Contains("m0") && !newVarName.Contains("sigma") ) {
          if ( newVarName.Contains("mgg") ) newVarName.ReplaceAll("mgg_", "CMS_hgg_");
          if ( newVarName.Contains("mjj") ) newVarName.ReplaceAll("mjj_", "CMS_hbb_");
          varsToChange.push_back(std::make_pair(thisVarName, newVarName));
        }
        std::cout << "Importing variable with new name: old - " << thisVarName << " new - " << newVarName << std::endl;
        _w->import( *_w->var( tempObj->GetName() ), RenameVariable( thisVarName, newVarName));
        wAll->import( *_w->var( tempObj->GetName() ), RenameVariable( thisVarName, newVarName));
      }

      //Shifts and smearings
      _w->factory(TString::Format("prod::CMS_hgg_hig_m0_%s_cat%d(mgg_hig_m0_%s_cat%d, CMS_hgg_sig_m0_absShift)", higName.Data(), c, higName.Data(), c));
      _w->factory(TString::Format("prod::CMS_hgg_hig_sigma_%s_cat%d(mgg_hig_sigma_%s_cat%d, CMS_hgg_sig_sigmaScale)", higName.Data(), c, higName.Data(), c));

      if(!_useDSCB) _w->factory(TString::Format("prod::CMS_hgg_gsigma_%s_cat%d(mgg_hig_gsigma_%s_cat%d, CMS_hgg_sig_sigmaScale)",
						higName.Data(), c, higName.Data(), c));
      if (higName.Contains("ggh") == 0 && higName.Contains("vbf") == 0 && _fitStrategy==2) {
        _w->factory(TString::Format("prod::CMS_hbb_hig_m0_%s_cat%d(mjj_hig_m0_%s_cat%d, CMS_hbb_sig_m0_absShift)", higName.Data(), c, higName.Data(), c));
        _w->factory(TString::Format("prod::CMS_hbb_hig_sigma_%s_cat%d(mjj_hig_sigma_%s_cat%d, CMS_hbb_sig_sigmaScale)", higName.Data(), c, higName.Data(), c));
        if(!_useDSCB) _w->factory(TString::Format("prod::CMS_hbb_hig_gsigma_%s_cat%d(mjj_hig_gsigma_%s_cat%d, CMS_hbb_sig_sigmaScale)", higName.Data(), c, higName.Data(), c));
      }

      TString EditPDF = TString::Format("EDIT::CMS_hig_%s_cat%d(HigPdf_%s_cat%d,", higName.Data(), c, higName.Data(), c);
      for (unsigned int iv = 0; iv < varsToChange.size(); iv++)
        EditPDF += TString::Format("%s=%s,", varsToChange[iv].first.Data(), varsToChange[iv].second.Data());
      //Shifted and smeared vars
      if(higName.Contains("ggh") == 0 && higName.Contains("vbf") == 0 && _fitStrategy==2) {
        if(!_useDSCB) EditPDF += TString::Format("mjj_hig_gsigma_%s_cat%d=CMS_hbb_hig_gsigma_%s_cat%d,", higName.Data(), c, higName.Data(), c);
        EditPDF += TString::Format("mjj_hig_m0_%s_cat%d=CMS_hbb_hig_m0_%s_cat%d,", higName.Data(), c, higName.Data(), c);
        EditPDF += TString::Format("mjj_hig_sigma_%s_cat%d=CMS_hbb_hig_sigma_%s_cat%d,", higName.Data(), c, higName.Data(), c);
      }
      if(!_useDSCB) EditPDF += TString::Format("mgg_hig_gsigma_%s_cat%d=CMS_hgg_hig_gsigma_%s_cat%d,", higName.Data(), c, higName.Data(), c);
      EditPDF += TString::Format("mgg_hig_m0_%s_cat%d=CMS_hgg_hig_m0_%s_cat%d,", higName.Data(), c, higName.Data(), c);
      EditPDF += TString::Format("mgg_hig_sigma_%s_cat%d=CMS_hgg_hig_sigma_%s_cat%d)", higName.Data(), c, higName.Data(), c);
      std::cout << "STRINGTOCHANGE   ---  " << EditPDF << std::endl;
      _w->factory(EditPDF);

      wAll->import(*_w->pdf(TString::Format("CMS_hig_%s_cat%d",higName.Data(),c)));
      wAll->import(*_w->data(TString::Format("Hig_%s_cat%d",higName.Data(), c)), Rename(TString::Format("Hig_%s_cat%d", higName.Data(), c)));
      //if (_fitStrategy==1)
      //wAll->import(*_w->pdf(TString::Format("mggHig_%s_cat%d",higName.Data(),c)), Rename(TString::Format("Hig_%s_cat%d", higName.Data(), c)));
      
    }
  TString filename(wsDir+fileHiggsName+".root");
  wAll->Print("v");
  wAll->writeToFile(filename);
  if (_verbLvl>1) std::cout << "Write single Higgs workspace in: " << filename << " file" << std::endl;

  return;
} // close make higgs WP

void bbgg2DFitter::MakeBkgWS(std::string fileBaseName)
{
  TString wsDir = TString::Format("%s/",_folder_name.data());
  //**********************************************************************//
  // Write pdfs and datasets into the workspace before to save to a file
  // for statistical tests.
  //**********************************************************************//
  std::vector<RooDataSet*> data(_NCAT,nullptr);
  std::vector<RooAbsPdf*> BkgPdf(_NCAT,nullptr);
  RooWorkspace *wAll = new RooWorkspace("w_all","w_all");
  for (int c = 0; c < _NCAT; ++c)
    {
      BkgPdf[c] = (RooAbsPdf*) _w->pdf(TString::Format("BkgPdf_cat%d",c));

      RooArgSet* bkgParams = (RooArgSet*) BkgPdf[c]->getParameters(RooArgSet(*_w->var("mgg"), *_w->var("mjj")));
      TIterator* paramIter = (TIterator*) bkgParams->createIterator();
      TObject* tempObj = nullptr;
      std::vector<std::pair<TString,TString>> varsToChange;

      while( (tempObj = paramIter->Next()) ) {

        if ( (TString(tempObj->GetName()).EqualTo("mjj")) || (TString(tempObj->GetName()).EqualTo("mgg"))) continue;
        TString thisVarName(tempObj->GetName());
        TString newVarName = TString(thisVarName).ReplaceAll(TString::Format("cat%d", c), TString::Format("cat%d", c));
        varsToChange.push_back(std::make_pair(thisVarName, newVarName));
        std::cout << "Importing variable with new name: old - " << thisVarName << " new - " << newVarName << std::endl;
        _w->import( *_w->var( tempObj->GetName() ), RenameVariable( thisVarName, newVarName));
        wAll->import( *_w->var( tempObj->GetName() ), RenameVariable( thisVarName, newVarName));

      }

      TString EditPDF = TString::Format("EDIT::CMS_Bkg_cat%d(BkgPdf_cat%d", c,  c);
      for (unsigned int iv = 0; iv < varsToChange.size(); iv++)
        EditPDF += TString::Format(",%s=%s", varsToChange[iv].first.Data(), varsToChange[iv].second.Data());
      EditPDF += ")";
      std::cout << "EDITSTRING: " << EditPDF << std::endl;
      _w->factory(EditPDF);

      wAll->import(*_w->pdf(TString::Format("CMS_Bkg_cat%d", c)));
      wAll->import(*_w->var(TString::Format("BkgPdf_cat%d_norm", c)), RenameVariable(TString::Format("BkgPdf_cat%d_norm", c) , TString::Format("CMS_Bkg_cat%d_norm",c)));
      wAll->import(*_w->data(TString::Format("Data_cat%d", c)), Rename(TString::Format("data_obs_cat%d", c) ));
      //if (_fitStrategy==1)
      //wAll->import(*_w->pdf(TString::Format("mggBkgTmpBer1_cat%d",c)), Rename(TString::Format("mggBkgTmpBer1_cat%d",c)));

    } // close ncat

  TString filename(wsDir+fileBaseName+".root");
  wAll->writeToFile(filename);
  if (_verbLvl>1) std::cout << "Write background workspace in: " << filename << " file" <<std::endl;
  if (_verbLvl>1) std::cout << "observation ";
  for (int c = 0; c < _NCAT; ++c)
    {
      if (_verbLvl>1) std::cout << " " << wAll->data(TString::Format("data_obs_cat%d",c))->sumEntries();
    }
  if (_verbLvl>1) std::cout << std::endl;
  return;
} // close make BKG workspace

void bbgg2DFitter::SetConstantParams(const RooArgSet* params)
{
  // set constant parameters for signal fit, ... NO IDEA !!!!
  TIterator* iter(params->createIterator());
  for (TObject *a = iter->Next(); a != 0; a = iter->Next())
    {
      RooRealVar *rrv = dynamic_cast<RooRealVar *>(a);
      if (rrv) rrv->setConstant(true);
      if (_verbLvl>1) std::cout << " Setting this parameter to constant: " << rrv->GetName() << std::endl;
    }
} // close set const parameters

RooFitResult* bbgg2DFitter::BkgModelFit(Bool_t dobands, bool addhiggs)
{
  //******************************************//
  // Fit background with model pdfs
  //******************************************//
  // retrieve pdfs and datasets from workspace to fit with pdf models
  std::vector<RooDataSet*> data(_NCAT,nullptr);
  std::vector<RooDataSet*> dataplot(_NCAT,nullptr); // the data
  std::vector<RooBernstein*> mggBkg(_NCAT,nullptr);// the polinomial of 4* order
  std::vector<RooBernstein*> mjjBkg(_NCAT,nullptr);// the polinomial of 4* order
  std::vector<RooPlot*> plotmggBkg(_NCAT,nullptr);
  std::vector<RooPlot*> plotmjjBkg(_NCAT,nullptr);;
  std::vector<RooDataSet*>vecset(_NCAT,nullptr);
  std::vector<RooAbsPdf*>vecpdf(_NCAT,nullptr);
  std::vector<std::vector<RooDataSet*>>sigToFitvec(5,vecset);
  std::vector<std::vector<RooAbsPdf*>>mggSigvec(5,vecpdf);
  std::vector<std::vector<RooAbsPdf*>>mjjSigvec(5,vecpdf);
  std::vector<RooAbsPdf*> mggSig(_NCAT,nullptr);
  std::vector<RooAbsPdf*> mjjSig(_NCAT,nullptr);
  RooProdPdf* BkgPdfBer = nullptr;
  RooProdPdf* BkgPdfExp = nullptr;
  RooProdPdf* BkgPdfPL	= nullptr;
  PdfModelBuilder pdfsModel;
  PdfModelBuilder pdfsModel_1;
  RooAbsPdf* mggBkgTmpBer;
  RooAbsPdf* mjjBkgTmpBer;
  RooAbsPdf* mggBkgTmpExp;
  RooAbsPdf* mjjBkgTmpExp;
  RooAbsPdf* mggBkgTmpPL;
  RooAbsPdf* mjjBkgTmpPL;
  /*RooExponential* mjjBkgTmpExp1 = nullptr;
  RooExponential* mggBkgTmpExp1 = nullptr;
  RooBernstein* mjjBkgTmpBer1 = nullptr;
  RooBernstein* mggBkgTmpBer1 = nullptr;*/

  RooRealVar* mgg = _w->var("mgg");
  RooRealVar* mjj = _w->var("mjj");
  pdfsModel.setObsVar(mgg);
  pdfsModel_1.setObsVar(mjj);
  //RooRealVar* mtot = _w->var("mtot");
  mgg->setUnit("GeV");
  mjj->setUnit("GeV");
  mgg->setRange("BkgFitRange",_minMggMassFit,_maxMggMassFit);
  mjj->setRange("BkgFitRange",_minMjjMassFit,_maxMjjMassFit);
  RooFitResult* fitresults = new RooFitResult();

  if (_verbLvl>1) std::cout << "[BkgModelFit] Starting cat loop " << std::endl;
  for (int c = 0; c < _NCAT; ++c) { // to each category
    data[c] = (RooDataSet*) _w->data(TString::Format("Data_cat%d",c));

    TH2* data_h2D = 0;
    TH1* data_h1D = 0;
    if(_fitStrategy==2)  data_h2D = (TH2*) data[c]->createHistogram("mgg,mjj", 60, 40);
    if(_fitStrategy==1)  data_h1D = (TH1*) data[c]->createHistogram("mgg", 60);

    if (_verbLvl>1) {
      std::cout<<"\t categ="<<c<<std::endl;
      if(_doblinding==0 && _fitStrategy==2) std::cout << "####### NUMBER OF OBSERVED EVENTS: " << data_h2D->Integral() << std::endl;
      if(_doblinding==0 && _fitStrategy==1) std::cout << "####### NUMBER OF OBSERVED EVENTS: " << data_h1D->Integral() << std::endl;
      std::cout<<"\t sumEntries()="<<data[c]->sumEntries()<<std::endl;
    }

    int nEvtsObs = -1;
    if(_fitStrategy == 2) nEvtsObs = data_h2D->Integral();
    if(_fitStrategy == 1) nEvtsObs = data_h1D->Integral();

    //data_h1D->Delete();

    if (_verbLvl>1) std::cout << "[BkgModelFit] Cat loop point 1 - cat " << c << std::endl;

    ////////////////////////////////////
    // these are the parameters for the bkg polinomial
    // one par by category - float from -10 > 10
    // we first wrap the normalization of mggBkgTmp0, mjjBkgTmp0
    // CMS_hhbbgg_13TeV_mgg_bkg_par1
    _w->factory(TString::Format("BkgPdf_cat%d_norm[1.0,0.0,100000]",c));
    if (_verbLvl>1) std::cout << "[BkgModelFit] Cat loop point 2 - cat " << c << std::endl;

    /*RooFormulaVar *mgg_p0amp = new RooFormulaVar(TString::Format("mgg_p0amp_cat%d",c),"","@0*@0",
						            *_w->var(TString::Format("CMS_hhbbgg_13TeV_mgg_bkg_par1_cat%d",c)));
    RooFormulaVar *mgg_p1amp = new RooFormulaVar(TString::Format("mgg_p1amp_cat%d",c),"","@0*@0",
						 RooArgList(*_w->var(TString::Format("CMS_hhbbgg_13TeV_mgg_bkg_par2_cat%d",c)) ));
    RooFormulaVar *mgg_p2amp = new RooFormulaVar(TString::Format("mgg_p2amp_cat%d",c),"","@0*@0",
						 RooArgList(*_w->var(TString::Format("CMS_hhbbgg_13TeV_mgg_bkg_par3_cat%d",c)) ));

    RooFormulaVar *mjj_p0amp = new RooFormulaVar(TString::Format("mjj_p0amp_cat%d",c),"","@0*@0",
						            *_w->var(TString::Format("CMS_hhbbgg_13TeV_mjj_bkg_par1_cat%d",c)));
    RooFormulaVar *mjj_p1amp = new RooFormulaVar(TString::Format("mjj_p1amp_cat%d",c),"","@0*@0",
						 RooArgList(*_w->var(TString::Format("CMS_hhbbgg_13TeV_mjj_bkg_par2_cat%d",c)) ));
    RooFormulaVar *mjj_p2amp = new RooFormulaVar(TString::Format("mjj_p2amp_cat%d",c),"","@0*@0",
						 RooArgList(*_w->var(TString::Format("CMS_hhbbgg_13TeV_mjj_bkg_par3_cat%d",c)) ));
    */

    if (_verbLvl>1) std::cout << "[BkgModelFit] Cat loop point 3 - cat " << c << std::endl;
    
    //mggBkgTmpExp1 = new RooExponential(TString::Format("mggBkgTmpExp1_cat%d",c),"",*mgg, *mgg_p0amp);
    //mjjBkgTmpExp1 = new RooExponential(TString::Format("mjjBkgTmpExp1_cat%d",c),"",*mjj, *mjj_p0amp);
    
    //mggBkgTmpBer1 = new RooBernstein(TString::Format("mggBkgTmpBer1_cat%d",c),"",*mgg,RooArgList(*mgg_p0amp));
    //mjjBkgTmpBer1 = new RooBernstein(TString::Format("mjjBkgTmpBer1_cat%d",c),"",*mjj,RooArgList(*mjj_p0amp));

    mggBkgTmpBer = getPdf(pdfsModel,"Bernstein",2,TString::Format("mggBkgTmpBer%d_cat%d",2,c));
    mjjBkgTmpBer = getPdf(pdfsModel,"Bernstein",2,TString::Format("mjjBkgTmpBer%d_cat%d",2,c));

    mggBkgTmpExp = getPdf(pdfsModel,"Exponential",1,TString::Format("mggBkgTmpExp%d_cat%d",1,c));
    mjjBkgTmpExp = getPdf(pdfsModel_1,"Exponential",1,TString::Format("mjjBkgTmpExp%d_cat%d",1,c));

    mggBkgTmpPL = getPdf(pdfsModel,"PowerLaw",1,TString::Format("mggBkgTmpPL%d_cat%d",1,c));
    mjjBkgTmpPL = getPdf(pdfsModel_1,"PowerLaw",1,TString::Format("mjjBkgTmpPL%d_cat%d",1,c));
    

    if(nEvtsObs > 4 && nEvtsObs < 100) {
      mggBkgTmpBer = getPdf(pdfsModel,"Bernstein",2,TString::Format("mggBkgTmpBer%d_cat%d",3,c));
      mjjBkgTmpBer = getPdf(pdfsModel,"Bernstein",2,TString::Format("mjjBkgTmpBer%d_cat%d",3,c));

      mggBkgTmpExp = getPdf(pdfsModel,"Exponential",1,TString::Format("mggBkgTmpExp%d_cat%d",2,c));
      mjjBkgTmpExp = getPdf(pdfsModel_1,"Exponential",1,TString::Format("mjjBkgTmpExp%d_cat%d",2,c));

      mggBkgTmpPL = getPdf(pdfsModel,"PowerLaw",1,TString::Format("mggBkgTmpPL%d_cat%d",2,c));
      mjjBkgTmpPL = getPdf(pdfsModel_1,"PowerLaw",1,TString::Format("mjjBkgTmpPL%d_cat%d",2,c)); 
    }


    if(nEvtsObs > 99) {
      mggBkgTmpBer = getPdf(pdfsModel,"Bernstein",2,TString::Format("mggBkgTmpBer%d_cat%d",4,c));
      mjjBkgTmpBer = getPdf(pdfsModel,"Bernstein",2,TString::Format("mjjBkgTmpBer%d_cat%d",4,c));

      mggBkgTmpExp = getPdf(pdfsModel,"Exponential",1,TString::Format("mggBkgTmpExp%d_cat%d",3,c));
      mjjBkgTmpExp = getPdf(pdfsModel_1,"Exponential",1,TString::Format("mjjBkgTmpExp%d_cat%d",3,c));

      mggBkgTmpPL = getPdf(pdfsModel,"PowerLaw",1,TString::Format("mggBkgTmpPL%d_cat%d",3,c));
      mjjBkgTmpPL = getPdf(pdfsModel_1,"PowerLaw",1,TString::Format("mjjBkgTmpPL%d_cat%d",3,c));     
    }

    if (_verbLvl>1) std::cout << "[BkgModelFit] Cat loop point 4 - cat" << c << std::endl;


      if(_fitStrategy == 2) {                                                                                                                                                                                                                                  
      BkgPdfBer = new RooProdPdf(TString::Format("BkgPdfBer_cat%d",c), "",  RooArgList(*mggBkgTmpBer,  *mjjBkgTmpBer));
      RooExtendPdf* BkgPdfExtBer;                                                                                                                                                                                                                             
      BkgPdfExtBer = new RooExtendPdf(TString::Format("BkgPdfExtBer_cat%d",c),"", *BkgPdfBer,*_w->var(TString::Format("BkgPdfBer_cat%d_norm",c)));                                                                                                        
      fitresults = BkgPdfExtBer->fitTo(*data[c], Strategy(1),Minos(kFALSE), Range("BkgFitRange"),SumW2Error(kTRUE), Save(kTRUE),PrintLevel(-1));                                                                                                              
      _w->import(*BkgPdfExtBer);
      
      BkgPdfExp = new RooProdPdf(TString::Format("BkgPdfExp_cat%d",c), "",  RooArgList(*mggBkgTmpExp,  *mjjBkgTmpExp));
      RooExtendPdf* BkgPdfExtExp;
      BkgPdfExtExp = new RooExtendPdf(TString::Format("BkgPdfExtExp_cat%d",c),"", *BkgPdfExp,*_w->var(TString::Format("BkgPdfExp_cat%d_norm",c)));
      fitresults = BkgPdfExtExp->fitTo(*data[c], Strategy(1),Minos(kFALSE), Range("BkgFitRange"),SumW2Error(kTRUE), Save(kTRUE),PrintLevel(-1));
      _w->import(*BkgPdfExtExp);
      
      BkgPdfPL = new RooProdPdf(TString::Format("BkgPdfPL_cat%d",c), "",  RooArgList(*mggBkgTmpPL,  *mjjBkgTmpPL));
      RooExtendPdf* BkgPdfExtPL;
      BkgPdfExtPL = new RooExtendPdf(TString::Format("BkgPdfExtPL_cat%d",c),"", *BkgPdfPL,*_w->var(TString::Format("BkgPdfPL_cat%d_norm",c)));
      fitresults = BkgPdfExtPL->fitTo(*data[c], Strategy(1),Minos(kFALSE), Range("BkgFitRange"),SumW2Error(kTRUE), Save(kTRUE),PrintLevel(-1));
      _w->import(*BkgPdfExtPL);
      
      }
    /*if(_fitStrategy == 2) {
      BkgPdfBer1 = new RooProdPdf(TString::Format("BkgPdfBer1_cat%d",c), "", RooArgList(*mggBkgTmpBer1, *mjjBkgTmpBer1));
      RooExtendPdf* BkgPdfExtBer1;
      BkgPdfExtBer1 = new RooExtendPdf(TString::Format("BkgPdfExtBer1_cat%d",c),"", *BkgPdfBer1,*_w->var(TString::Format("BkgPdfBer1_cat%d_norm",c)));
      fitresults = BkgPdfExtBer1->fitTo(*data[c], Strategy(1),Minos(kFALSE), Range("BkgFitRange"),SumW2Error(kTRUE), Save(kTRUE),PrintLevel(-1));
      _w->import(*BkgPdfExtBer1);

      BkgPdfExp1 = new RooProdPdf(TString::Format("BkgPdfExp1_cat%d",c), "", RooArgList(*mggBkgTmpExp1, *mjjBkgTmpExp1));
      RooExtendPdf* BkgPdfExtExp1;
      BkgPdfExtExp1 = new RooExtendPdf(TString::Format("BkgPdfExtExp1_cat%d",c),"", *BkgPdfExp1,*_w->var(TString::Format("BkgPdfExp1_cat%d_norm",c)));
      fitresults = BkgPdfExtExp1->fitTo(*data[c], Strategy(1),Minos(kFALSE), Range("BkgFitRange"),SumW2Error(kTRUE), Save(kTRUE),PrintLevel(-1));
      _w->import(*BkgPdfExtExp1);
     
    }
    if(_fitStrategy == 1) {
      fitresults = mggBkgTmpBer1->fitTo(*data[c], Strategy(1),Minos(kFALSE), Range("BkgFitRange"),SumW2Error(kTRUE), Save(kTRUE),PrintLevel(-1));
      RooAbsPdf* BkgPdf1 = (RooAbsPdf*) mggBkgTmpBer1->Clone(TString::Format("BkgPdf_cat%d",c));
      _w->import(*BkgPdf1);
      //_w->import(*mggBkgTmpBer1);
      }*/

    if (_verbLvl>1) std::cout << "[BkgModelFit] Cat loop end - cat " << c << std::endl;

    if (data_h2D) data_h2D->Delete();
    if (data_h1D) data_h1D->Delete();
  }

  return fitresults;
}
