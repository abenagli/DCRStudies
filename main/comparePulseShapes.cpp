#include "interface/graphFunc.h"
#include "interface/Discriminator.h"
#include "interface/SetTDRStyle.h"
#include "CfgManager/interface/CfgManager.h"
#include "CfgManager/interface/CfgManagerT.h"

#include <iostream>
#include <numeric>
#include <ctime>

#include "TFile.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TRandom3.h"
#include "TLine.h"
#include "TApplication.h"



int main(int argc, char** argv)
{
  if( argc < 2 )
  {
    std::cerr << ">>>>> comparePulseShapes.exe::usage:   " << argv[0] << " configFileName   [default=0/debug=1]" << std::endl;
    return -1;
  }
  
  
  //----------------------
  // parse the config file
  CfgManager opts;
  opts.ParseConfigFile(argv[1]);
  
  int debugMode = 0;
  if( argc > 2 ) debugMode = atoi(argv[2]);
  
  
  //--- get parameters
  int seed = opts.GetOpt<int>("Input.seed");
  
  float nPhE = opts.GetOpt<float>("Input.nPhE");
  std::cout << ">>> number of signal ph.e.: " << nPhE << std::endl;
  
  float SPTR = opts.GetOpt<float>("Input.SPTR");
  
  float DCR = opts.GetOpt<float>("Input.DCR");
  int nDCR = int(500.*DCR);
  std::cout << ">>> number of DCR ph.e. for DCR = " << DCR << " GHz (within a 500 ns window): " << nDCR << std::endl;
  
  int trackBaseline = opts.GetOpt<int>("Input.trackBaseline");
  float baselineXmin = opts.GetOpt<float>("Input.baselineXmin");
  float baselineXmax = opts.GetOpt<float>("Input.baselineXmax");
  
  int CFD = opts.GetOpt<int>("Input.CFD");
  float CFDDelay = opts.GetOpt<float>("Input.CFDDelay");
  
  int nToys = opts.GetOpt<int>("Input.nToys");
  
  float xMin = opts.GetOpt<float>("Input.xMin");
  float xMax = opts.GetOpt<float>("Input.xMax");
  float xBinWidth = opts.GetOpt<float>("Input.xBinWidth");
  int overSampling = opts.GetOpt<int>("Input.overSampling");
  int nPoints = (xMax-xMin) / xBinWidth;
  int globalShift = int( ( (0.-xMin) / xBinWidth ) + 0.1*xBinWidth );
  float* xAxis = new float[nPoints];
  for(int point = 0; point < nPoints; ++point)
  {
    xAxis[point] = xMin + point*xBinWidth;
  }

  std::vector<std::string> lightPDE = opts.GetOpt<std::vector<std::string> >("Input.lightPDE");
  TF1* f_scintillation = new TF1("f_scintillation",lightPDE.at(0).c_str(),xMin,xMax);
  for(unsigned int iPar = 0; iPar < lightPDE.size()-1; ++iPar)
    f_scintillation -> SetParameter(iPar,atof(lightPDE.at(iPar+1).c_str()));
  f_scintillation -> SetNpx((xMax-xMin)/(xBinWidth/5.));
  
  
  
  //--- other global variables
  gRandom = new TRandom3(seed);
  TRandom3 r(seed);
  double x0,y0;
  double x,y;
  
  

  //--------------------
  //---interactive plots
  setTDRStyle();
  TApplication* theApp;
  if( debugMode ) theApp = new TApplication("App", &argc, argv);
  
  
  
  //--------------
  // get 1pe shape
  std::string inFileName_1pe = opts.GetOpt<std::string>("Input.inFileName1pe");
  TGraph* g_ps_1pe = NULL;
  TGraph* g_ps_1pe_baseSub = NULL;
  graphFunc* hf_ps_1pe_baseSub = NULL;
  float* hf_psFine_1pe_baseSub = NULL;
  int nFinePoints = -1;

  std::vector<std::string> formula_1pe;
  TF1* f_ps_1pe = NULL;
  float f_ps_t0 = 0.;
  if( inFileName_1pe != "NULL" )
  {
    g_ps_1pe = new TGraph(("data/"+inFileName_1pe+".csv").c_str(),"%le,%lf");
    g_ps_1pe_baseSub = new TGraph();
    g_ps_1pe -> GetPoint(0,x0,y0);
    for(int point = 0; point < g_ps_1pe->GetN(); ++point)
    {
      g_ps_1pe -> GetPoint(point,x,y);
      g_ps_1pe_baseSub -> SetPoint(point,1.E09*x,y-y0);
    }
    
    hf_ps_1pe_baseSub = new graphFunc(g_ps_1pe_baseSub,1.*xBinWidth/overSampling);
    hf_psFine_1pe_baseSub = hf_ps_1pe_baseSub->GetFineYaxis();
    nFinePoints = hf_ps_1pe_baseSub -> GetNFinePoints();
  }

  else
  {
    formula_1pe = opts.GetOpt<std::vector<std::string> >("Input.formula1pe");
    f_ps_1pe = new TF1("f_ps_1pe",formula_1pe.at(0).c_str(),xMin,xMax);

    for(unsigned int iPar = 0; iPar < formula_1pe.size()-1; ++iPar)
      f_ps_1pe -> SetParameter(iPar,atof(formula_1pe.at(iPar+1).c_str()));
    f_ps_1pe -> SetParameter(0.,1./f_ps_1pe->GetMaximum());
    f_ps_t0 = f_ps_1pe -> GetParameter(formula_1pe.size()-2);
  }
  
  

  //----------------
  // build 1pe shape
  float* yAxis_1PhE     = new float[nPoints];
  float* yAxis_1PhE_CFD = new float[nPoints];
  for(int point = 0; point < nPoints; ++point)
  {
    yAxis_1PhE[point] = 0.;
    yAxis_1PhE_CFD[point] = 0.;
  }
  
  float max_1pe = -999.;
  float max_1pe_CFD = -999.;
  if( inFileName_1pe != "NULL" )
  {
    int shift = 1.*overSampling*globalShift;
    for(int jj = 0; jj < nPoints; ++jj)
    {
      if( (overSampling*jj-shift) >= 0 && (overSampling*jj-shift) < nFinePoints )
      {
        yAxis_1PhE[jj]     = ( hf_psFine_1pe_baseSub[overSampling*jj-shift] );
        yAxis_1PhE_CFD[jj] = ( hf_psFine_1pe_baseSub[overSampling*jj-shift] );

        if( yAxis_1PhE[jj] > max_1pe ) max_1pe = yAxis_1PhE[jj];
      }
    }
  }
  else
  {
    for(int jj = 0; jj < nPoints; ++jj)
    {
      float yVal = f_ps_1pe->Eval(xAxis[jj]);
      yAxis_1PhE[jj]     = ( yVal );
      yAxis_1PhE_CFD[jj] = ( yVal );
      
      if( yAxis_1PhE[jj] > max_1pe ) max_1pe = yAxis_1PhE[jj];
    }
  }
  if( CFD )
  {
    ImplementCFD(int(CFDDelay/(1.*xBinWidth/overSampling)),nPoints,yAxis_1PhE_CFD,yAxis_1PhE);

    for(int jj = 0; jj < nPoints; ++jj)
    {
      int shift = 1.*overSampling*globalShift;
      if( (overSampling*jj-shift) >= 0 && (overSampling*jj-shift) < nFinePoints )
      {
        if( yAxis_1PhE_CFD[jj] > max_1pe_CFD ) max_1pe_CFD = yAxis_1PhE_CFD[jj];      
      }
    }
  }
  std::cout << "max_1pe: " << max_1pe << "   max_1pe_CFD: " << max_1pe_CFD << std::endl;
  
  
  
  //---------
  // run toys
  float* yAxis_sumNPhE = new float[nPoints];
  float* yAxis_sumNPhE_baseSub = new float[nPoints];
  float* yAxis_sumNPhE_CFD = new float[nPoints];

  for(int point = 0; point < nPoints; ++point)
  {
    yAxis_sumNPhE[point] = 0.;
    yAxis_sumNPhE_baseSub[point] = 0.;
    yAxis_sumNPhE_CFD[point] = 0.;
  }
  
  for(int iToy = 0; iToy < nToys; ++iToy)
  {
    std::cout << ">>> processing toy " << iToy << " / " << nToys << "\r" << std::flush;
    
    
    //--- signal ph.e.
    std::vector<double> times;
    for(int ii = 0; ii < int(nPhE); ++ii)
    {
      float time = f_scintillation->GetRandom() + r.Gaus(0.,SPTR);
      times.push_back(time);

      if( inFileName_1pe != "NULL" )
      {
        int shift = 1.*overSampling*globalShift + (time / (1.*xBinWidth/overSampling) );
        for(int jj = 0; jj < nPoints; ++jj)
        {
          if( (overSampling*jj-shift) >= 0 && (overSampling*jj-shift) < nFinePoints )
          {
            yAxis_sumNPhE[jj]         += ( hf_psFine_1pe_baseSub[overSampling*jj-shift] );
            yAxis_sumNPhE_baseSub[jj] += ( hf_psFine_1pe_baseSub[overSampling*jj-shift] );
            yAxis_sumNPhE_CFD[jj]     += ( hf_psFine_1pe_baseSub[overSampling*jj-shift] );
          }
        }
      }
      else
      {
        f_ps_1pe -> SetParameter(formula_1pe.size()-2,time+f_ps_t0);
        for(int jj = 0; jj < nPoints; ++jj)
        {
          float yVal = f_ps_1pe->Eval(xAxis[jj]);
          yAxis_sumNPhE[jj]         += ( yVal );
          yAxis_sumNPhE_baseSub[jj] += ( yVal );
          yAxis_sumNPhE_CFD[jj]     += ( yVal );
        }
      }
    }
    std::sort(times.begin(),times.end());
    
    
    //--- dark ph.e.
    int nDCR_pois = r.Poisson(nDCR);
    for(int ii = 0; ii < nDCR_pois; ++ii)
    {
      float time = r.Uniform(-300.,200.);
      
      if( inFileName_1pe != "NULL" )
      {
        int shift = 1.*overSampling*globalShift + (time / (1.*xBinWidth/overSampling) );
        for(int jj = 0; jj < nPoints; ++jj)
        {
          if( (overSampling*jj-shift) >= 0 && (overSampling*jj-shift) < nFinePoints )
          {
            yAxis_sumNPhE[jj]         += ( hf_psFine_1pe_baseSub[overSampling*jj-shift] );
            yAxis_sumNPhE_baseSub[jj] += ( hf_psFine_1pe_baseSub[overSampling*jj-shift] );
            yAxis_sumNPhE_CFD[jj]     += ( hf_psFine_1pe_baseSub[overSampling*jj-shift] );
          }
        }
      }
      else
      {
        f_ps_1pe -> SetParameter(formula_1pe.size()-2,time+f_ps_t0);
        for(int jj = 0; jj < nPoints; ++jj)
        {
          float yVal = f_ps_1pe->Eval(xAxis[jj]);
          yAxis_sumNPhE[jj]         += ( yVal );
          yAxis_sumNPhE_baseSub[jj] += ( yVal );
          yAxis_sumNPhE_CFD[jj]     += ( yVal );
        }
      }      
    }
  }

  for(int point = 0; point < nPoints; ++point)
  {
    yAxis_sumNPhE[point] /= nToys;
    yAxis_sumNPhE_baseSub[point] /= nToys;
    yAxis_sumNPhE_CFD[point] /= nToys;
  }

  
  //--- implement baseline tracking
  std::pair<float,float> baseline;
  if( trackBaseline )
    baseline = SubtractBaseline(baselineXmin,baselineXmax,nPoints,xAxis,yAxis_sumNPhE_baseSub);
  else
    baseline = SubtractBaseline(xMin,xMin+50.,nPoints,xAxis,yAxis_sumNPhE_baseSub);
  
  
  //--- implement CFD
  if( CFD )
    ImplementCFD(int(CFDDelay/(1.*xBinWidth/overSampling)),nPoints,yAxis_sumNPhE_CFD,yAxis_sumNPhE);
  

  
  //--- draw the pulseshape
  {
    TGraph* g_ps_SumnPhE = new TGraph(nPoints,xAxis,yAxis_sumNPhE);
    g_ps_SumnPhE -> SetMarkerColor(kBlack);
    g_ps_SumnPhE -> SetLineColor(kBlack);
    
    TGraph* g_ps_SumnPhE_baseSub = new TGraph(nPoints,xAxis,yAxis_sumNPhE_baseSub);
    g_ps_SumnPhE_baseSub -> SetMarkerColor(kRed);
    g_ps_SumnPhE_baseSub -> SetLineColor(kRed);
    
    TGraph* g_ps_SumnPhE_CFD = new TGraph(nPoints,xAxis,yAxis_sumNPhE_CFD);
    g_ps_SumnPhE_CFD -> SetMarkerColor(kRed);
    g_ps_SumnPhE_CFD -> SetLineColor(kRed);
    
    float yMin = 999999.;
    float yMax = -999999.;
    for(int point = 0; point < nPoints; ++point)
    {
      if( yAxis_sumNPhE_baseSub[point] < yMin ) yMin = yAxis_sumNPhE_baseSub[point];
      if( yAxis_sumNPhE_baseSub[point] > yMax ) yMax = yAxis_sumNPhE_baseSub[point];
    }
    
    float yMin_CFD = 999999.;
    float yMax_CFD = -999999.;
    for(int point = 0; point < nPoints; ++point)
    {
      if( yAxis_sumNPhE_CFD[point] < yMin_CFD ) yMin_CFD = yAxis_sumNPhE_CFD[point];
      if( yAxis_sumNPhE_CFD[point] > yMax_CFD ) yMax_CFD = yAxis_sumNPhE_CFD[point];
    }
    TCanvas* c = new TCanvas(Form("c_%dtoys",nToys),Form("c_%dtoys",nToys),1800,600);
    c -> Divide(3,1);
    
    c -> cd(1);
    f_scintillation -> Draw();
    
    c -> cd(2);
    TH1F* hPad = (TH1F*)( gPad->DrawFrame(xMin-0.05*(xMax-xMin),yMin-0.05*(yMax-yMin),xMax+0.05*(xMax-xMin),yMax+0.05*(yMax-yMin)) );
    hPad -> SetTitle(";time [ns];amplitude [a.u.]");
    hPad -> Draw();
    g_ps_SumnPhE         -> Draw("PL,same");
    g_ps_SumnPhE_baseSub -> Draw("PL,same");
    
    std::vector<std::string> additional1 = opts.GetOpt<std::vector<std::string> >("Input.additional1");
    for(auto add1 : additional1)
    {
      TGraph* g_ps_add = new TGraph(("data/"+add1+".csv").c_str(),"%le,%lf");
      
      g_ps_add -> GetPoint(0,x0,y0);
      for(int point = 0; point < g_ps_add->GetN(); ++point)
      {
        g_ps_add -> GetPoint(point,x,y);
        g_ps_add -> SetPoint(point,1.E09*x,y-y0);
      }
      
      g_ps_add -> SetMarkerSize(0.5);
      g_ps_add -> SetMarkerColor(kBlue);
      g_ps_add -> SetLineColor(kBlue);
      g_ps_add -> Draw("PL,same");
    }
    
    gPad -> Update();
    
    c -> cd(3);
    hPad = (TH1F*)( gPad->DrawFrame(xMin-0.05*(xMax-xMin),yMin_CFD-0.05*(yMax_CFD-yMin_CFD),xMax+0.05*(xMax-xMin),yMax_CFD+0.05*(yMax_CFD-yMin_CFD)) );
    hPad -> SetTitle(";time [ns];amplitude [a.u.]");
    hPad -> Draw();
    g_ps_SumnPhE_CFD -> Draw("PL,same");
    gPad -> Update();
    
    std::vector<std::string> additional2 = opts.GetOpt<std::vector<std::string> >("Input.additional2");
    for(auto add2 : additional2)
    {
      TGraph* g_ps_add = new TGraph(("data/"+add2+".csv").c_str(),"%le,%lf");
      
      g_ps_add -> GetPoint(0,x0,y0);
      for(int point = 0; point < g_ps_add->GetN(); ++point)
      {
        g_ps_add -> GetPoint(point,x,y);
        g_ps_add -> SetPoint(point,1.E09*x,y-y0);
      }
      
      g_ps_add -> SetMarkerSize(0.5);
      g_ps_add -> SetMarkerColor(kBlue);
      g_ps_add -> SetLineColor(kBlue);
      g_ps_add -> Draw("PL,same");
    }
    
    gPad -> Update();
  }
  
  std::cout << std::endl;
  
  if( debugMode ) theApp -> Run();
}
