from ROOT import *
from HiggsAnalysis.bbggLimits2018.MyCMSStyle import *
import argparse, sys
import numpy as n


gSystem.Load("libHiggsAnalysisCombinedLimit.so")
gROOT.SetBatch(kTRUE)

parser =  argparse.ArgumentParser(description='Background fit plot maker')
parser.add_argument('-d', '--dataFile', dest="dataname", type=str, default=None, required=True)
parser.add_argument('-o', '--outName', dest="outName", type=str, default=None, required=True)
parser.add_argument('-L', '--lumi', dest='lumi', type=str, default='77.4')
parser.add_argument('-u', '--unblind', dest='unblind',  default=False, action="store_true")
parser.add_argument('-p', '--purity', dest='ipurity', type=int, default=0)
parser.add_argument('-m', '--mggmax', dest='mggmax', type=float, default=180)

opt = parser.parse_args()


mggmin = 100
mggmax = opt.mggmax
binning = int((mggmax-mggmin)/2)

xtitle = 'm_{#gamma#gamma} [GeV]'
ytitle = 'Events/(2 GeV)'
Cats = ["HP", "MP", "LP"]

slices=[80,100,120,140,160,190] #regions to fit
purities=[0,4,8,12] # Combine categories to have more stats. Look correlations over all MX region


#slices=[80,95,110,125,140,155,170,190]
#slices=[80,90,100,110,120,130,140,150,160,170,180,190]

print "============================== Datafile name ", opt.dataname
datafile = TFile(opt.dataname, "READ")


mgg = w_all.var('mgg')
mjj = w_all.var('mjj')
myArgSet = RooArgSet(mgg, mjj)

w_data = datafile.Get("w_all")
data = w_data.data("data_obs_cat0")



for jcat in range(purities[opt.ipurity],purities[opt.ipurity+1]-1):

  jdata="data_obs_cat"+str(jcat)
  data.append(w_data.data(jdata))
  

lslices = len(slices)
gr = TGraphErrors()
gr.SetName("slope study")

w_data.factory("norm[2000.0,0.0,10000]");
w_data.factory("p0[-0.2, -1, 1]")
w_data.factory("RooExponential:Exp_mgg(mgg, p0)")

myModel = w_data.pdf('Exp_mgg')

myModelExt = RooExtendPdf("myModelExt","", myModel,w_data.var('norm'))

# --------- Fitting and producing exponential slopes

for slc in range(0, lslices-1):

  data2d =  data.reduce(myArgSet, '( mjj > ' +str(slices[slc])+' && mjj < '+ str(slices[slc+1]) + ')  && mgg < ' + str(mggmax))
  if opt.unblind:
    data2d =  data.reduce(myArgSet, '( mjj > ' +str(slices[slc])+' && mjj < '+ str(slices[slc+1]) + ') && mgg < ' + str(mggmax))

  print "============================== totData = ",   data2d.sumEntries()

  myModelExt.fitTo(data2d, RooFit.Strategy(2),RooFit.Minos(kTRUE));
  
  norm = w_data.var('norm');
  p0 =  w_data.var('p0')
  
  print "in ", slices[slc], " < mjj < ", slices[slc+1], " expo = ",  p0.getVal(), " +- ",  p0.getError()

  avg = (slices[slc+1] + slices[slc])/2
  diff = avg-slices[slc]
  gr.SetPoint(slc, avg, p0.getVal())
  gr.SetPointError(slc, diff, p0.getError())

#-------------------------------------------#


data2d =  data.reduce(myArgSet, 'mgg < ' + str(mggmax))

print "============================== totData = ",   data2d.sumEntries()

myModelExt.fitTo(data2d, RooFit.Strategy(2),RooFit.Minos(kTRUE));
  
norm = w_data.var('norm');
p0 =  w_data.var('p0')
  
print "in 70 < mjj < 180, expo = ",  p0.getVal(), " +- ",  p0.getError()

# --------------- Plot the total data in given purity category with exponenatial fit -------------------

cNiceBlueDark = TColor.GetColor('#2175E0')
cNiceGreenDark = TColor.GetColor('#008040')
cNiceRed = TColor.GetColor('#FA4912')

ymax =  norm.getVal()/binning*5

frame = mgg.frame(RooFit.Title(" "),RooFit.Bins(binning),RooFit.Range(mggmin,mggmax))

mgg.setRange("unblindReg_1",mggmin,120);
mgg.setRange("unblindReg_2",130,mggmax);
if not opt.unblind: 
  data2d.plotOn(frame,RooFit.DataError(RooAbsData.SumW2),RooFit.XErrorSize(0),RooFit.CutRange("unblindReg_1"));
  data2d.plotOn(frame,RooFit.DataError(RooAbsData.SumW2),RooFit.XErrorSize(0),RooFit.CutRange("unblindReg_2"));
else:
  data2d.plotOn(frame,RooFit.DataError(RooAbsData.SumW2),RooFit.XErrorSize(0));


myModelExt.plotOn(frame,RooFit.LineColor(cNiceGreenDark), RooFit.LineStyle(kDashed), RooFit.Precision(1E-5), RooFit.Normalization(norm.getVal(), RooAbsReal.NumEvent))

c = TCanvas("c", "c", 800, 600)
frame.Draw()
frame.GetXaxis().SetTitle(xtitle)
frame.GetYaxis().SetTitle(ytitle)
frame.SetMaximum(ymax)
frame.SetMinimum(0.001)
SetPadStyle(c)
SetAxisTextSizes(frame)
DrawCMSLabels(c, opt.lumi)
DrawCatLabels(c, "Purity " + Cats[opt.ipurity])
c.Update()


poiss_pdfout = opt.outName+"/Purity_"+Cats[opt.ipurity] + "_mgg_data_expfit"+"_mgg_"+str(int(mggmax))+".pdf"
poiss_pngout = opt.outName+"/Purity_"+Cats[opt.ipurity] + "_mgg_data_expfit"+"_mgg_"+str(int(mggmax))+".png"

c.SaveAs(poiss_pdfout)
c.SaveAs(poiss_pngout)

# --------------- Plot the slopes variation with linear fit -------------------

func = TF1("func", "[1]")

gr.Fit(func)

gr.Draw("APLE*")
gr.GetYaxis().SetRangeUser(-0.1,0.1)
gr.GetYaxis().SetTitle("exponential slope [1/GeV]")
gr.GetXaxis().SetTitle("mjj range")

DrawCMSLabels(c, opt.lumi)
#label = "Purity " + Cats[opt.ipurity] + "for mgg < " + str(mggmax) + ",  #chi^{2}/NDF = " + str('%.1f' % func.GetChisquare()) + "/4,    pval = " + str('%.2f' % func.GetProb())
label = "Purity " + Cats[opt.ipurity]

DrawCatLabels(c, label)

nameout=opt.outName+"/Correlation_Purity_"+Cats[opt.ipurity]+"_mgg_"+str(int(mggmax))+"_nopval.png"
c.SaveAs(nameout)
