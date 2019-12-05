## Limit trees

* Make the small limit trees from large fat trees obtained from flashgg:
the samples are described here
https://docs.google.com/spreadsheets/d/1nAMnNzMiREAaep6NCIyHVvMXSPHTZ2T7pfh6pxmcXXA/edit#gid=1146044152
and the flat trees
/eos/cms/store/group/phys_higgs/HiggsExo/HH_bbgg/Run2_legacy/flat_trees_from_ETH_MVA_10_12_2018_commonTraining/

```
./makeLT.py /eos/cms/store/group/phys_higgs/resonant_HH/RunII/FlatTrees/2016/2018_05_04_HHTaggerETH/ -x nonres -o LT_OutDir [-c Y]
```

The core code that makes the trees is `bbggLTMaker.C`. It is based on
[TSelector](https://root.cern.ch/developing-tselector) and does not depend on CMSSW, just
the ROOT.  
The goal of this code is to categorize events and make a new tree which *catID* variable,
as well as *mgg* and *mjj*. Different type of categorizations can be done chosen by option `-c Y`:  
```
 Y = 0: 2016 tagger with categorization used in 2016 analysis (4 categories)
 Y = 1: 2017 ETH tagger, using 2016 style categorization (4 categories);
 Y = 2: 2017 ETH tagger, with optimized categorization without mjj cut (12 categories);
 Y = 3: 2017 ETH tagger, with optimized categorization and mjj cuts (12 categories);
```


### Notes on limit trees
* The scale factors for b-tagging are not included in this code. This is because a) the
  method we used in 2016 is outdated; and b) it requires compilation together with some
  CMSSW packages which is not trivial in TSelector code. For the reference,
  [this code](https://github.com/ResonantHbbHgg/bbggLimits/blob/edda480b80455c3d14f644629e5faaa5997cc9f0/src/bbggLTMaker.cc#L278-L340)
  was used in 2016 to apply the SF for b-tagging.
* In order to implement another categorization, one has to put it
  [here](https://github.com/ResonantHbbHgg/bbggLimits2018/blob/f031e57c6e938be983b006fc1f81a01ec53ea61a/bbggLTMaker.C#L268)
  in _bbggLTMaker_ code, following the structure for already implemented categorizations.

## Fits and limits
* Run the fits and limits on the produced LTs:

```
./runLimit.py -f conf_default.json --node=SM -o LIMS_OutDir
```  
The process may take a while to complete, especially when running with many categories.  
The config file `conf_default.json` can be edited to provide needed parameters. Some of them are:  
```
 LTDIR: location of the input Limit Trees (expected to be in the local diractory, after running previous step)
 ncat: number of categories. This should much the number of categories produced in limit tries (currently, should be 4 or 12)
 fitStrategy: 2 - for 2D fit of (mgg, mjj); 1 - for 1D fit of mgg, in which case a cut is set to 100<mjj<150 somewhere in runLimit.py script.
```

The results of the limit will be in `LIMS_OutDir/Node_SM/result_1.log`. In case of problems,
the logfile _mainLog_data-time.log[.bbgg2D]_ can be useful


### Notes on datacards and limits

* Systematic uncertainties are not real (especially the ones for b-tagging and JEC) for
  the case of 2017 categorization (the older numbers are used). Once proper systematics
  are obtained they need to be propagated to template datacards for each category (in
  _templates_ directory).
* The same background fit function is used in all categories (3d order Benrnstein). It may
  be necessary in the future to have different functions in each category. This should be
  modifien from _templates/models_2D_higgs_mjj70_cat*.rs_ files and then taken care in
  _src/bbgg2DFitter.cc_
  


## The plots 

* Run the following command to do fit diagnostics of combine and make quick plots:
``` 
combine -M FitDiagnostics LIMS_OutDir/Node_SM/hhbbgg_13TeV_DataCard.txt --plots --out LIMS_OutDir/
```  
Note that you may see many warnings. They are ignored at the moment, but should be fixed in the future.

* non-integer bin entry:  
```
[#0] WARNING:Plotting -- RooHist::addBin(ch4_plot__mgg) WARNING: non-integer bin entry 14.5154 with Poisson errors, interpolating between Poisson errors of adjacent integer
```  
These are probably due to the fact that the observed data are taken from MC with weights and the events are not integers. 
* parameters at boundary:  
```
[WARNING] Found [CMS_hhbbgg_13TeV_mjj_bkg_slope2_cat0] at boundary.
```

### Note on plotting
Previously, the plots were made taking the post-fit results of the Maximum Likelihood fit
from combine,
[see here](https://github.com/ResonantHbbHgg/bbggLimits/blob/72ae0c5d502ac4e8554ed0161c7aa345b84f5ec3/scripts/MakeSMHHFullBkgPlots.sh#L11). However,
this is no longer provided with `FitDiagnostics` option, as also pointed out in
[this post in HN](https://hypernews.cern.ch/HyperNews/CMS/get/higgs-combination/1223.html). In the same HN post, it's suggested to use 
`-M MultiDimFit --saveWorkspace` options, which should provide the post-fit PDFs. This should be explored in the future



