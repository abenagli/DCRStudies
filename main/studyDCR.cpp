#include "interface/graphFunc.h"
#include "interface/Discriminator.h"
#include "interface/SetTDRStyle.h"
#include "CfgManager/interface/CfgManager.h"
#include "CfgManager/interface/CfgManagerT.h"

#include <iostream>
#include <numeric>

#include "TFile.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TRandom3.h"
#include "TLine.h"
#include "TApplication.h"



double Scintillation(double* x, double* par)
{
  double xx = x[0];
  double tau_r = par[0];
  double tau_d = par[1];
  return( (exp(-1.*xx/tau_d) - exp(-1.*xx/tau_r) ) / (tau_d-tau_r) );
}



int main(int argc, char** argv)
{
  if( argc < 2 )
  {
    std::cerr << ">>>>> studyDCR.exe::usage:   " << argv[0] << " configFileName   [default=0/debug=1]" << std::endl;
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
  float tau_r = opts.GetOpt<float>("Input.tau_r");
  float tau_d = opts.GetOpt<float>("Input.tau_d");
  float thickness = opts.GetOpt<float>("Input.thickness");
  float LY = opts.GetOpt<float>("Input.LY");
  float LCE = opts.GetOpt<float>("Input.LCE");
  float PDE = opts.GetOpt<float>("Input.PDE");
  float nPhE = opts.GetOpt<float>("Input.nPhE");
  if( nPhE <= 0 ) nPhE = thickness * LY * LCE * PDE;
  std::cout << ">>> number of signal ph.e.: " << nPhE << std::endl;

  float SPTR = opts.GetOpt<float>("Input.SPTR");
  
  float DCR = opts.GetOpt<float>("Input.DCR");
  int nDCR = int(500.*DCR);
  std::cout << ">>> number of DCR ph.e. for DCR = " << DCR << " GHz (within a 500 ns window): " << nDCR << std::endl;

  int trackBaseline = opts.GetOpt<int>("Input.trackBaseline");
  float baselineXmin = opts.GetOpt<float>("Input.baselineXmin");
  float baselineXmax = opts.GetOpt<float>("Input.baselineXmax");
  float signalXmin = opts.GetOpt<float>("Input.signalXmin");
  
  int nToys = opts.GetOpt<int>("Input.nToys");
  
  float xMin = opts.GetOpt<float>("Input.xMin");
  float xMax = opts.GetOpt<float>("Input.xMax");
  float xBinWidth = opts.GetOpt<float>("Input.xBinWidth");
  int overSampling = opts.GetOpt<int>("Input.overSampling");
  int nPoints = (xMax-xMin) / xBinWidth;
  int globalShift = int( ( (0.-xMin) / xBinWidth ) + 0.1*xBinWidth );
  
  TF1* f_scintillation = new TF1("f_scintillation",Scintillation,0.,200.,2);
  f_scintillation -> SetParameters(tau_r,tau_d);
  f_scintillation -> SetNpx(10000);
  
  
  //--- discrimination thresholds
  float thr_1pe = 0.00555;
  std::vector<int> thrs_nPhE;
  std::vector<float> thrs;
  thrs_nPhE.push_back(1);    thrs.push_back(1.*thr_1pe);
  thrs_nPhE.push_back(2);    thrs.push_back(2.*thr_1pe);
  thrs_nPhE.push_back(3);    thrs.push_back(3.*thr_1pe);
  thrs_nPhE.push_back(5);    thrs.push_back(5.*thr_1pe);
  thrs_nPhE.push_back(10);   thrs.push_back(10.*thr_1pe);
  thrs_nPhE.push_back(20);   thrs.push_back(20.*thr_1pe);
  thrs_nPhE.push_back(30);   thrs.push_back(30.*thr_1pe);
  thrs_nPhE.push_back(50);   thrs.push_back(50.*thr_1pe);
  thrs_nPhE.push_back(100);  thrs.push_back(100.*thr_1pe);
  thrs_nPhE.push_back(200);  thrs.push_back(200.*thr_1pe);
  thrs_nPhE.push_back(300);  thrs.push_back(300.*thr_1pe);
  thrs_nPhE.push_back(500);  thrs.push_back(500.*thr_1pe);
  thrs_nPhE.push_back(1000); thrs.push_back(1000.*thr_1pe);
  thrs_nPhE.push_back(2000); thrs.push_back(2000.*thr_1pe);
  thrs_nPhE.push_back(3000); thrs.push_back(3000.*thr_1pe);


  //--- deltaTs
  std::vector<int> deltaTs;
  deltaTs.push_back(1);
  deltaTs.push_back(2);
  deltaTs.push_back(3);
  deltaTs.push_back(5);
  deltaTs.push_back(10);
  deltaTs.push_back(20);
  deltaTs.push_back(30);
  deltaTs.push_back(50);
  deltaTs.push_back(100);
  deltaTs.push_back(200);
  deltaTs.push_back(300);
  deltaTs.push_back(500);

  
  //--- other global variables
  TRandom3 r(seed);
  double x0,y0;
  double x,y;
  
  

  //--------------------
  //---interactive plots
  setTDRStyle();
  TApplication* theApp;
  if( debugMode ) theApp = new TApplication("App", &argc, argv);
  
  
  
  //----------------
  // build 1pe shape
  std::string inFileName_1pe = opts.GetOpt<std::string>("Input.inFileName1pe");
  TGraph* g_ps_1pe = new TGraph(("data/"+inFileName_1pe+".csv").c_str(),"%le,%lf");
  TGraph* g_ps_1pe_baseSub = new TGraph();
  g_ps_1pe -> GetPoint(0,x0,y0);
  for(int point = 0; point < g_ps_1pe->GetN(); ++point)
  {
    g_ps_1pe -> GetPoint(point,x,y);
    g_ps_1pe_baseSub -> SetPoint(point,1.E09*x,y-y0);
  }
  
  graphFunc* hf_ps_1pe_baseSub = new graphFunc(g_ps_1pe_baseSub,1.*xBinWidth/overSampling);
  float* hf_psFine_1pe_baseSub = hf_ps_1pe_baseSub->GetFineYaxis();
  int nFinePoints = hf_ps_1pe_baseSub -> GetNFinePoints();
  
  

  //------------
  // output file
  std::string outputFileName = opts.GetOpt<std::string>("Output.outputFileName");
  outputFileName += std::string(Form("_nPhE%05d_DCR%07.3fGHz_SPTR%.03fns_%s",int(nPhE),DCR,SPTR,inFileName_1pe.c_str()));
  if( trackBaseline )
    outputFileName += std::string(Form("_baselineTracking%06.3f-%06.3fns",baselineXmin,baselineXmax));
  outputFileName += std::string(Form("_nToys%d.root",nToys));
  TFile* outFile = TFile::Open(outputFileName.c_str(),"RECREATE");
  outFile -> cd();
  
  TH1F* h_baseline = new TH1F("h_baseline","",2000,-1.,1.);
  
  std::map<int,TH1F*> h1_timeNthPhE;
  std::map<int,TH1F*> h1_timeAvgNPhE;
  std::map<int,TH1F*> h1_timeLE;
  std::map<int,TH1F*> h1_timeLESig;
  for(unsigned int ii = 0; ii < thrs_nPhE.size(); ++ii)
  {
    int thr_nPhE = thrs_nPhE.at(ii);
    h1_timeNthPhE[thr_nPhE]  = new TH1F(Form( "h1_timeNthPhE_thr%04dPhE",thr_nPhE),"",int(overSampling*(xMax-xMin)/xBinWidth),xMin,xMax);
    h1_timeAvgNPhE[thr_nPhE] = new TH1F(Form("h1_timeAvgNPhE_thr%04dPhE",thr_nPhE),"",int(overSampling*(xMax-xMin)/xBinWidth),xMin,xMax);
    h1_timeLE[thr_nPhE]      = new TH1F(Form(     "h1_timeLE_thr%04dPhE",thr_nPhE),"",int(overSampling*(xMax-xMin)/xBinWidth),xMin,xMax);
    h1_timeLESig[thr_nPhE]   = new TH1F(Form(  "h1_timeLESig_thr%04dPhE",thr_nPhE),"",int(overSampling*(xMax-xMin)/xBinWidth),xMin,xMax);
  }
  
  std::map<int,TH1F*> h1_deltaY;
  for(unsigned int ii = 0; ii < deltaTs.size(); ++ii)
  {
    int deltaT = deltaTs.at(ii);
    h1_deltaY[deltaT]  = new TH1F(Form( "h1_deltaY_deltaT%06.3fns",deltaT*xBinWidth),"",100000,-100.,100.);
  }
  
  
  //---------
  // run toys
  for(int iToy = 0; iToy < nToys; ++iToy)
  {
    if( !debugMode && iToy%10 == 0 ) std::cout << ">>> processing toy " << iToy << " / " << nToys << std::endl;
    if(  debugMode && iToy%1  == 0 ) std::cout << ">>> processing toy " << iToy << " / " << nToys << "\r" << std::flush;
    
    
    float* xAxis = new float[nPoints];
    float* yAxis_sumNPhE_baseSub = new float[nPoints];
    for(int point = 0; point < nPoints; ++point)
    {
      xAxis[point] = xMin + point*xBinWidth;
      yAxis_sumNPhE_baseSub[point] = 0.;
    }

    
    //--- signal ph.e.
    std::vector<double> times;
    for(int ii = 0; ii < int(nPhE); ++ii)
    {
      float time = f_scintillation->GetRandom() + r.Gaus(0.,SPTR);
      times.push_back(time);
      
      int shift = 1.*overSampling*globalShift + (time / (1.*xBinWidth/overSampling) );
      for(int jj = 0; jj < nPoints; ++jj)
      {
        if( (overSampling*jj-shift) >= 0 && (overSampling*jj-shift) < nFinePoints )
          yAxis_sumNPhE_baseSub[jj] += ( hf_psFine_1pe_baseSub[overSampling*jj-shift] );
      }
    }
    std::sort(times.begin(),times.end());
    
    
    //--- dark ph.e.
    int nDCR_pois = r.Poisson(nDCR);
    for(int ii = 0; ii < nDCR_pois; ++ii)
    {
      float time = r.Uniform(-300.,200.);
      int shift = 1.*overSampling*globalShift + (time / (1.*xBinWidth/overSampling) );
      for(int jj = 0; jj < nPoints; ++jj)
      {
        if( (overSampling*jj-shift) >= 0 && (overSampling*jj-shift) < nFinePoints )
          yAxis_sumNPhE_baseSub[jj] += ( hf_psFine_1pe_baseSub[overSampling*jj-shift] );
      }
    }
    
    
    //--- implement baseline tracking
    float baseline = 0.;
    if( trackBaseline )
      baseline = SubtractBaseline(baselineXmin,baselineXmax,nPoints,xAxis,yAxis_sumNPhE_baseSub);
    h_baseline -> Fill( baseline );
    
    
    //--- discriminate the pulseshape
    if( debugMode ) std::cout << ">>>>>> discriminating pulseshape: " << std::endl;
    std::vector<float> timesLE = GetTimeLE(thrs,nPoints,xAxis,yAxis_sumNPhE_baseSub,baselineXmax);
    std::vector<float> timesLESig = GetTimeLE(thrs,nPoints,xAxis,yAxis_sumNPhE_baseSub,signalXmin);
    for(unsigned int ii = 0; ii < timesLE.size(); ++ii)
    {
      int thr_nPhE = thrs_nPhE.at(ii);

      if( thr_nPhE < nPhE )
      {
        h1_timeNthPhE[thr_nPhE] -> Fill( times.at(thr_nPhE-1) );
        h1_timeAvgNPhE[thr_nPhE] -> Fill( std::accumulate(times.begin(),times.begin()+thr_nPhE,0.)/thr_nPhE );
      }
      
      h1_timeLE[thr_nPhE] -> Fill( timesLE.at(ii) );
      h1_timeLESig[thr_nPhE] -> Fill( timesLESig.at(ii) );
      if( debugMode) std::cout << ">>>>>>>>> thr: " << thr_nPhE << " ph.e.   time: " << timesLE.at(ii) << std::endl;
    }
    
    
    //---- study baseline
    for(unsigned int ii = 0; ii < deltaTs.size(); ++ii)
    {
      int deltaT = deltaTs.at(ii);

      h1_deltaY[deltaT] -> Fill((yAxis_sumNPhE_baseSub[globalShift+deltaT]-yAxis_sumNPhE_baseSub[globalShift])/thr_1pe);
    }
    
    
    //--- draw the pulseshape
    if( debugMode && iToy < 10 )
    {
      TGraph* g_ps_SumnPhE_baseSub = new TGraph(nPoints,xAxis,yAxis_sumNPhE_baseSub);
      g_ps_SumnPhE_baseSub -> SetMarkerSize(0.2);
      g_ps_SumnPhE_baseSub -> SetMarkerColor(kGreen);
      g_ps_SumnPhE_baseSub -> SetLineColor(kGreen);

      float yMin = 999999.;
      float yMax = -999999.;
      for(int point = 0; point < nPoints; ++point)
      {
        if( yAxis_sumNPhE_baseSub[point] < yMin ) yMin = yAxis_sumNPhE_baseSub[point];
        if( yAxis_sumNPhE_baseSub[point] > yMax ) yMax = yAxis_sumNPhE_baseSub[point];
      }
      
      TCanvas* c = new TCanvas(Form("c_toy%d",iToy),Form("c_toy%d",iToy));
      c -> cd();
      TH1F* hPad = (TH1F*)( gPad->DrawFrame(xMin-0.05*(xMax-xMin),yMin-0.05*(yMax-yMin),xMax+0.05*(xMax-xMin),yMax+0.05*(yMax-yMin)) );
      hPad -> SetTitle(";time [ns];amplitude [V]");
      hPad -> Draw();
      // g_ps_1pe_baseSub -> Draw("PL,same");
      g_ps_SumnPhE_baseSub -> Draw("PL,same");
      gPad -> Update();
      
      for(unsigned int thrIt = 0; thrIt < thrs.size(); ++thrIt)
      {
        TLine* line = new TLine(timesLE[thrIt],yMin,timesLE[thrIt],yMax);
        line -> Draw("same");
      }
      gPad -> Update();
    }

    
    delete[] xAxis;
    delete[] yAxis_sumNPhE_baseSub;
  }
  std::cout << std::endl;
  
  
  
  int bytes = outFile -> Write();
  std::cout << "============================================"  << std::endl;
  std::cout << "nr of  B written:  " << int(bytes)             << std::endl;
  std::cout << "nr of KB written:  " << int(bytes/1024.)       << std::endl;
  std::cout << "nr of MB written:  " << int(bytes/1024./1024.) << std::endl;
  std::cout << "============================================"  << std::endl;
  
  
  
  if( debugMode ) theApp -> Run();
}
