#! /usr/bin/env python

import os,sys
import numpy as np

from ROOT import *
gROOT.SetBatch()

# Read the workspaces:
rooWs = TFile('2016_2017_withPU_BerFast_2D_FORBIAS_BER3All/Node_SM/ws_hhbbgg.HH.sig.mH125_13TeV.root')

myWs = rooWs.Get('w_all')
myWs.Print()

mgg = myWs.var('mgg')
mjj = myWs.var('mjj')
for i in range(0,12):
  pdf_mgg  = myWs.pdf("mggSig_cat"+str(i)+"_CMS_sig_cat"+str(i))
  pdf_mjj  = myWs.pdf("mjjSig_cat"+str(i)+"_CMS_sig_cat"+str(i))
  pdf_prod = myWs.pdf("CMS_sig_cat"+str(i)) # This is a product of the two PDFs above
  dataSet  = myWs.data("Sig_cat"+str(i))

  print "\n Printout the parameters of the sig PDF"
  l0 = RooArgSet(mgg,mjj)
  pars = pdf_prod.getParameters(l0)
  pars.Print()
  parsiter = pars.createIterator()
  var=parsiter.Next()
  while var!=None:
    print '%s: %.3f  +/ %.3f'%(var.GetName(), var.getVal(), var.getError())
    var=parsiter.Next()


  # Make some plots:
  print 
  c = TCanvas("c","c",0,0,900,600)
  c.cd()

  mgg.setRange('signal',118,135)
  mjj.setRange('signal', 70,190)

  mggFrame = mgg.frame(RooFit.Range("signal"))
  dataSet.plotOn(mggFrame, RooFit.Binning(80))
  pdf_mgg.plotOn(mggFrame, RooFit.LineColor(kRed+1), RooFit.LineWidth(2))
  mggFrame.Draw()
  c.SaveAs('tmpfig_mgg_cat'+str(i)+'.png')

 
  mjjFrame = mjj.frame(RooFit.Range("signal"))
  dataSet.plotOn(mjjFrame, RooFit.Binning(30))
  pdf_mjj.plotOn(mjjFrame, RooFit.LineColor(kGreen+1), RooFit.LineWidth(2))
  mjjFrame.Draw()
  c.SaveAs('tmpfig_mjj_cat'+str(i)+'.png')

