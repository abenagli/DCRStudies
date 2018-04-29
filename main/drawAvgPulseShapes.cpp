#include "interface/graphFunc.h"
#include "interface/Discriminator.h"
#include "interface/SetTDRStyle.h"
#include "CfgManager/interface/CfgManager.h"
#include "CfgManager/interface/CfgManagerT.h"

#include <iostream>

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
    std::cerr << ">>>>> drawAvgPulseShapes.exe::usage:   " << argv[0] << " configFileName   [default=0/debug=1]" << std::endl;
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
  
  float DCR = opts.GetOpt<float>("Input.DCR");
  int nDCR = int(500.*DCR);
  std::cout << ">>> number of DCR ph.e. for DCR = " << DCR << " GHz (within a 500 ns window): " << nDCR << std::endl;

  int trackBaseline = opts.GetOpt<int>("Input.trackBaseline");
  float baselineMin = opts.GetOpt<float>("Input.baselineMin");
  float baselineMax = opts.GetOpt<float>("Input.baselineMax");
  
  int nToys = opts.GetOpt<int>("Input.nToys");
  
  float xMin = opts.GetOpt<float>("Input.xMin");
  float xMax = opts.GetOpt<float>("Input.xMax");
  float xBinWidth = opts.GetOpt<float>("Input.xBinWidth");
  int overSampling = opts.GetOpt<int>("Input.overSampling");
  int nPoints = int( (xMax-xMin) / xBinWidth );
  int globalShift = int( ( (0.-xMin) / xBinWidth ) + 0.1*xBinWidth );
  
  TF1* f_scintillation = new TF1("f_scintillation",Scintillation,0.,200.,2);
  f_scintillation -> SetParameters(tau_r,tau_d);
  
  
  //--- other global variables
  TRandom3 r(seed);
  double x0,y0;
  double x,y;
  
  
  
  //--------------------
  //---interactive plots
  setTDRStyle();
  TApplication* theApp = new TApplication("App", &argc, argv);
  
  
  
  //------------
  // draw shapes
  std::string inFileName_1pe = opts.GetOpt<std::string>("Input.inFileName1pe");
  TGraph* g_ps_1pe = new TGraph(inFileName_1pe.c_str(),"%le,%lf");
  TGraph* g_ps_1pe_baseSub = new TGraph();
  g_ps_1pe -> GetPoint(0,x0,y0);
  for(int point = 0; point < g_ps_1pe->GetN(); ++point)
  {
    g_ps_1pe -> GetPoint(point,x,y);
    g_ps_1pe_baseSub -> SetPoint(point,1.E09*x,y-y0);
  }
  g_ps_1pe_baseSub -> SetMarkerSize(0.02);
  g_ps_1pe_baseSub -> SetMarkerColor(kBlack);
  g_ps_1pe_baseSub -> SetLineColor(kBlack);

//   std::string inFileName_100pe = opts.GetOpt<std::string>("Input.inFileName100pe");
//   TGraph* g_ps_LYSO_100pe = new TGraph(inFileName_100pe.c_str(),"%le,%lf");
//   TGraph* g_ps_LYSO_100pe_baseSub = new TGraph();
//   g_ps_LYSO_100pe -> GetPoint(0,x0,y0);
//   for(int point = 0; point < g_ps_LYSO_100pe->GetN(); ++point)
//   {
//     g_ps_LYSO_100pe -> GetPoint(point,x,y);
//     g_ps_LYSO_100pe_baseSub -> SetPoint(point,1.E09*x,y-y0);
//   }
//   g_ps_LYSO_100pe_baseSub -> SetMarkerSize(0.02);
//   g_ps_LYSO_100pe_baseSub -> SetMarkerColor(kRed);
//   g_ps_LYSO_100pe_baseSub -> SetLineColor(kRed);
  

  
  //------------------------------------
  // build a nPhE shape from the 1pe one
  float* xAxis = new float[nPoints];
  float* yAxis_sumNPhE_baseSub = new float[nPoints];
  for(int point = 0; point < nPoints; ++point)
  {
    xAxis[point] = xMin + point*xBinWidth;
    yAxis_sumNPhE_baseSub[point] = 0.;
  }
  
  graphFunc* hf_ps_1pe_baseSub = new graphFunc(g_ps_1pe_baseSub,1.*xBinWidth/overSampling);
  float* hf_psFine_1pe_baseSub = hf_ps_1pe_baseSub->GetFineYaxis();
  int nFinePoints = hf_ps_1pe_baseSub -> GetNFinePoints();
  
  for(int iToy = 0; iToy < nToys; ++iToy)
  {
    if( iToy%10 == 0 )
      std::cout << ">>> processing toy " << iToy << " / " << nToys << "\r" << std::flush;
    
    for(int ii = 0; ii < int(nPhE); ++ii)
    {
      float time = f_scintillation->GetRandom();
      int shift = 1.*overSampling*globalShift + (time / (1.*xBinWidth/overSampling) );
      for(int jj = 0; jj < nPoints; ++jj)
      {
        if( (overSampling*jj-shift) >= 0 && (overSampling*jj-shift) < nFinePoints )
          yAxis_sumNPhE_baseSub[jj] += ( hf_psFine_1pe_baseSub[overSampling*jj-shift] / nToys );
      }
    }

    int nDCR_pois = r.Poisson(nDCR);
    for(int ii = 0; ii < nDCR_pois; ++ii)
    {
      float time = r.Uniform(-300.,200.);
      int shift = 1.*overSampling*globalShift + (time / (1.*xBinWidth/overSampling) );
      for(int jj = 0; jj < nPoints; ++jj)
      {
        if( (overSampling*jj-shift) >= 0 && (overSampling*jj-shift) < nFinePoints )
          yAxis_sumNPhE_baseSub[jj] += ( hf_psFine_1pe_baseSub[overSampling*jj-shift] / nToys );
      }
    }
  }
  std::cout << std::endl;

  //--- implement baseline tracking
  if( trackBaseline )
    SubtractBaseline(baselineMin,baselineMax,nPoints,xAxis,yAxis_sumNPhE_baseSub);
  
  TGraph* g_ps_sumnPhE_baseSub = new TGraph(nPoints,xAxis,yAxis_sumNPhE_baseSub);
  g_ps_sumnPhE_baseSub -> SetMarkerSize(0.2);
  g_ps_sumnPhE_baseSub -> SetMarkerColor(kGreen);
  g_ps_sumnPhE_baseSub -> SetLineColor(kGreen);

  float yMin = 999999.;
  float yMax = -999999.;
  for(int point = 0; point < nPoints; ++point)
  {
    if( yAxis_sumNPhE_baseSub[point] < yMin ) yMin = yAxis_sumNPhE_baseSub[point];
    if( yAxis_sumNPhE_baseSub[point] > yMax ) yMax = yAxis_sumNPhE_baseSub[point];
  }
  
  
  
  //----------------------------
  // discriminate the pulseshape
  float thr_1pe = 0.003;
  std::vector<float> thrs;
  thrs.push_back(1.*thr_1pe);
  thrs.push_back(2.*thr_1pe);
  thrs.push_back(3.*thr_1pe);
  thrs.push_back(5.*thr_1pe);
  thrs.push_back(10.*thr_1pe);
  thrs.push_back(20.*thr_1pe);
  thrs.push_back(30.*thr_1pe);
  thrs.push_back(50.*thr_1pe);
  thrs.push_back(100.*thr_1pe);
  thrs.push_back(200.*thr_1pe);
  thrs.push_back(300.*thr_1pe);
  thrs.push_back(500.*thr_1pe);
  std::vector<float> timesLE = GetTimeLE(thrs,nPoints,xAxis,yAxis_sumNPhE_baseSub);
  
  

  //--------------------
  // draw the pulseshape
  TCanvas* c = new TCanvas();
  c -> cd();
  TH1F* hPad = (TH1F*)( gPad->DrawFrame(xMin-0.05*(xMax-xMin),yMin-0.05*(yMax-yMin),xMax+0.05*(xMax-xMin),yMax+0.05*(yMax-yMin)) );
  hPad -> SetTitle(";time [ns];amplitude [V]");
  hPad -> Draw();
  g_ps_1pe_baseSub -> Draw("P,same");
  //g_ps_LYSO_100pe_baseSub -> Draw("P,same");
  g_ps_sumnPhE_baseSub -> Draw("PL,same");
  gPad -> Update();

  //for(unsigned int thrIt = 0; thrIt < thrs.size(); ++thrIt)
  //{
  //  TLine* line = new TLine(timesLE[thrIt],yMin,timesLE[thrIt],yMax);
  //  line -> Draw("same");
  //}
  {
    TLine* line1 = new TLine(baselineMin,yMin,baselineMin,yMax);
    line1 -> Draw("same");
    line1 -> SetLineStyle(2);
    TLine* line2 = new TLine(baselineMax,yMin,baselineMax,yMax);
    line2 -> Draw("same");
    line2 -> SetLineStyle(2);
  }
  gPad -> Update();
  
  c -> Print(Form("pulseShape_nPhE%05.0f_DCR%07.3fGHz_nToys%d.png",nPhE,DCR,nToys));
  c -> Print(Form("pulseShape_nPhE%05.0f_DCR%07.3fGHz_nToys%d.pdf",nPhE,DCR,nToys));
  
  
  theApp -> Run();
}
