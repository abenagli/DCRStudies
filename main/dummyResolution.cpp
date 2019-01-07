#include "interface/FitUtils.h"
#include "interface/SetTDRStyle.h"

#include <iostream>
#include <string>
#include <map>

#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TApplication.h"

// double Scintillation(double* x, double* par)
// {
//   double xx = x[0];
//   double tau_r = par[0];
//   double tau_d = par[1];
//   if( xx < 0. ) return 0.;
//   return( ( exp(-1.*xx/tau_d) - exp(-1.*xx/tau_r) ) / (tau_d-tau_r) );
// }


// double SiPM(double* x, double* par)
// {
//   double xx = x[0];
//   double tau_r = par[0];
//   double tau_d = par[1];
//   double t0 = par[2];
//   double norm = (tau_d - tau_r) / ( pow(tau_d/tau_r,tau_r/(tau_r-tau_d)) - pow(tau_d/tau_r,tau_d/(tau_r-tau_d)) );
//   if( (xx-t0) > 0. ) return( norm * ( exp(-1.*(xx-t0)/tau_d) - exp(-1.*(xx-t0)/tau_r) ) / (tau_d-tau_r) );
//   else return 0.;
// }



// double Convolution(double* x, double* par)
// {
//   double xx = x[0];
//   int nPhE = par[0];
  
//   double val = 0.;
//   for(int ii = 0; ii < nPhE; ++ii)
//   {
//     double* par_SiPM = new double[3];
//     par_SiPM[0] = par[1+ii*3+0];
//     par_SiPM[1] = par[1+ii*3+1];
//     par_SiPM[2] = par[1+ii*3+2];
//     val += SiPM(x,par_SiPM);
//   }
  
//   return val;
// }



// void generateEvents()
// {
//   int nToys = 1000;
//   double timeBin = 0.010;
  
//   double tau_r = 0.080;
//   double tau_d = 40.;
  
//   double SiPM_tau_r = 1.;
//   double SiPM_tau_d = 5.;
  
  
//   //----
//   TF1* f_scintillation = new TF1("f_scintillation",Scintillation,-50.,200.,2);
//   f_scintillation -> SetParameters(tau_r,tau_d);
//   f_scintillation -> SetNpx(10000);
  
  
  
//   //----
//   TF1* f_SiPM = new TF1("f_SiPM",SiPM,-50.,200.,2);
//   f_SiPM -> SetParameters(SiPM_tau_r,SiPM_tau_d);
//   f_SiPM -> SetNpx(10000);

//   TCanvas* c = new TCanvas("c_pulses","c_pulses",1200,500);
//   c -> Divide(2,1);
//   c -> cd(1);
//   f_scintillation -> Draw();
//   c -> cd(2);
//   f_SiPM -> Draw();
  
  
  
//   //-----
//   std::vector<int> nPhEs;
//   nPhEs.push_back(10);
//   nPhEs.push_back(20);
//   nPhEs.push_back(30);
//   nPhEs.push_back(50);
//   nPhEs.push_back(100);
//   nPhEs.push_back(200);
//   nPhEs.push_back(300);
//   nPhEs.push_back(500);
//   nPhEs.push_back(1000);
//   nPhEs.push_back(2000);
//   nPhEs.push_back(3000);
//   nPhEs.push_back(5000);
//   nPhEs.push_back(10000);
//   nPhEs.push_back(20000);
//   nPhEs.push_back(30000);
//   nPhEs.push_back(50000);
  
//   std::vector<int> thrs;
//   thrs.push_back(1);
//   thrs.push_back(2);
//   thrs.push_back(3);
//   thrs.push_back(5);
//   thrs.push_back(10);
//   thrs.push_back(20);
//   thrs.push_back(30);
//   thrs.push_back(50);
//   thrs.push_back(100);
  
//   TFile* outFile = TFile::Open("dummyResolution.root","RECREATE");
//   outFile -> cd();
//   std::map<std::string,TH1F*> h_time_nthPhE;
//   std::map<std::string,TH1F*> h_time_avgNPhE;
//   for(unsigned int nPhEIt = 0; nPhEIt < nPhEs.size(); ++nPhEIt)
//     for(unsigned int thrIt = 0; thrIt < thrs.size(); ++thrIt)
//     {
//       int nPhE = nPhEs.at(nPhEIt);
//       int thr = thrs.at(thrIt);
//       std::string label = Form("nPhE_%d__thr_%d",nPhE,thr);
//       h_time_nthPhE[label]  = new TH1F(Form("h_time_nthPhE__%s",label.c_str()), "",1000000,0.,100.);
//       h_time_avgNPhE[label] = new TH1F(Form("h_time_avgNPhE__%s",label.c_str()),"",1000000,0.,100.);
//     }
  
  
  
//   //----
//   for(int iToy = 0; iToy < nToys; ++iToy)
//   {
//     if( iToy%100 == 0 )
//       std::cout << "processing iToy " << iToy << " / " << nToys << "\r" << std::flush;
    
//     for(unsigned int nPhEIt = 0; nPhEIt < nPhEs.size(); ++nPhEIt)
//     {
//       int nPhE = nPhEs.at(nPhEIt);
      
//       std::vector<double> times;
//       for(int ii = 0; ii < nPhE; ++ii)
//       {
//         double time = f_scintillation->GetRandom();      
//         times.push_back(time);
//       }
//       std::sort(times.begin(),times.end());
      
//       for(int thrIt = 0; thrIt < thrs.size(); ++thrIt)
//       {
//         int thr = thrs.at(thrIt);
//         std::string label = Form("nPhE_%d__thr_%d",nPhE,thr);
        
//         if( thr < times.size() )
//         {
//           h_time_nthPhE[label]  -> Fill(times.at(thr-1));
//           h_time_avgNPhE[label] -> Fill(std::accumulate(times.begin(),times.begin()+thr,0.)/(thr));
//         }
//       }
//     }
    
//     // TF1* f_convolution = new TF1("f_convolution",Convolution,-50.,200.,1+3*times.size());
//     // f_convolution -> SetParameter(0,nPhE);
//     // for(int ii = 0; ii < nPhE; ++ii)
//     // {
//     //   f_convolution -> SetParameter(1+ii*3+0,SiPM_tau_r);
//     //   f_convolution -> SetParameter(1+ii*3+1,SiPM_tau_d);
//     //   f_convolution -> SetParameter(1+ii*3+2,times.at(ii));
//     // }
//     // f_convolution -> SetNpx(10000);

//     // std::map<double,double> timesLE;
//     // int step = 0;
//     // while(1)
//     // {
//     //   double timeTemp = 0. + step*timeBin;
//     //   if(timeTemp > 200. ) break;
      
//     //   for(int thrIt = 0; thrIt < thrs.size(); ++thrIt)
//     //   {
//     //     double thr = thrs.at(thrIt);
//     //     if( f_convolution->Eval(timeTemp) > thr && timesLE[thr] == 0. )
//     //     {
//     //       std::cout << "thr: " << thr << " time: " << timeTemp << "   ";
//     //       timesLE[thr] = timeTemp;
//     //       if( thrIt == thrs.size()-1 ) break;
//     //     }
//     //   }
      
//     //   ++step;
//     // }
    
//     // delete f_convolution;
//   }
//   std::cout << std::endl;



//   int bytes = outFile -> Write();
//   std::cout << "============================================"  << std::endl;
//   std::cout << "nr of  B written:  " << int(bytes)             << std::endl;
//   std::cout << "nr of KB written:  " << int(bytes/1024.)       << std::endl;
//   std::cout << "nr of MB written:  " << int(bytes/1024./1024.) << std::endl;
//   std::cout << "============================================"  << std::endl;
// }



int main(int argc, char** argv)
{
  setTDRStyle();
  
  //---interactive plots
  TApplication* theApp = new TApplication("App", &argc, argv);


  
  //-----
  std::vector<int> nPhEs;
  nPhEs.push_back(100);
  nPhEs.push_back(200);
  nPhEs.push_back(300);
  nPhEs.push_back(500);
  nPhEs.push_back(1000);
  nPhEs.push_back(2000);
  nPhEs.push_back(3000);
  nPhEs.push_back(5000);
  nPhEs.push_back(10000);
  nPhEs.push_back(20000);
  nPhEs.push_back(30000);
  nPhEs.push_back(50000);
  
  std::vector<int> thrs;
  thrs.push_back(1);
  thrs.push_back(2);
  thrs.push_back(3);
  thrs.push_back(5);
  thrs.push_back(10);
  thrs.push_back(20);
  thrs.push_back(30);
  thrs.push_back(50);
  thrs.push_back(100);

  std::vector<std::string> methods;
  std::vector<std::string> methodLabels;
  // methods.push_back("step_1pe"); methodLabels.push_back("step function");
  methods.push_back("bipolar_1pe"); methodLabels.push_back("bipolar");
  methods.push_back("dummySiPM_0.20nsRT_7.70nsnsDT_1pe"); methodLabels.push_back("dummy SiPM response");
  
  TF1* f_30ps = new TF1("f_30ps","30.",0.01,100000.);
  f_30ps -> SetLineColor(kBlack);
  f_30ps -> SetLineStyle(2);
  f_30ps -> SetLineWidth(2);
  f_30ps -> SetNpx(10000);
  
  

  //--- plots vs. nPhE
  for(unsigned int thrIt = 0; thrIt < thrs.size(); ++thrIt)
  {
    int thr = thrs.at(thrIt);

    TCanvas* c = new TCanvas(Form("c_resolution_thr%d",thr),Form("c_resolution_thr%d",thr));
    c -> cd();
    
    TH1F* hPad = (TH1F*)( gPad->DrawFrame(10.,1.,100000.,10000.) );
    gPad -> SetLogx();
    gPad -> SetLogy();
    hPad -> SetTitle(";N_{pe};#sigma_{t} (ps)");
    hPad -> Draw();
    
    TGraphErrors* g_timeNthPhE = new TGraphErrors();
    TGraphErrors* g_timeAvgNPhE = new TGraphErrors();
    std::map<std::string,TGraphErrors*> g_timeLE;
    
    for(unsigned int nPhEIt = 0; nPhEIt < nPhEs.size(); ++nPhEIt)
    {
      int nPhE = nPhEs.at(nPhEIt);

      TFile* inFile = TFile::Open(Form("plots/studyDCR_nPhE%05d_DCR000.000GHz_SPTR0.000ns_step_1pe_nToys10000.root",nPhE),"READ");
      
      TH1F* h1_timeNthPhE = (TH1F*)( inFile->Get(Form("h1_timeNthPhE_thr%04dPhE",thr)) );
      TH1F* h1_timeAvgNPhE = (TH1F*)( inFile->Get(Form("h1_timeAvgNPhE_thr%04dPhE",thr)) );
      
      if( h1_timeNthPhE->Integral() < 2000 ) continue;

      float* vals = new float[6];
      FindSmallestInterval(vals,h1_timeNthPhE,0.68);
      float mean = vals[0];
      float min = vals[4];
      float max = vals[5];
      float delta = max-min;
      float sigma = 0.5*delta;
      float effSigma = sigma;
      
      g_timeNthPhE -> SetPoint(g_timeNthPhE->GetN(),nPhE,1000.*effSigma);
      g_timeNthPhE -> SetPointError(g_timeNthPhE->GetN()-1,0.,1000.*h1_timeNthPhE->GetRMSError());
      
      FindSmallestInterval(vals,h1_timeAvgNPhE,0.68);
      mean = vals[0];
      min = vals[4];
      max = vals[5];
      delta = max-min;
      sigma = 0.5*delta;
      effSigma = sigma;
      
      g_timeAvgNPhE -> SetPoint(g_timeAvgNPhE->GetN(),nPhE,1000.*effSigma);
      g_timeAvgNPhE -> SetPointError(g_timeAvgNPhE->GetN()-1,0.,1000.*h1_timeAvgNPhE->GetRMSError());
      
      // TF1* f_gaus = new TF1("f_gaus","[0]*exp(-1.*(x-[1])*(x-[1])/(2.*[2]*[2]))",0.,1000);
      // f_gaus -> SetParameters(histo->GetMaximum(),histo->GetMean(),histo->GetRMS());
      // f_gaus -> SetNpx(10000);
      
      // histo -> Fit(f_gaus,"QNRS+");
      
      // g -> SetPoint(g->GetN(),nPhE,1000.*f_gaus->GetParameter(2));
      // g -> SetPointError(g->GetN()-1,0.,1000.*f_gaus->GetParError(2));

      inFile -> Close();
    }

    for(auto method: methods)
    {
      g_timeLE[method] = new TGraphErrors();
      
      for(unsigned int nPhEIt = 0; nPhEIt < nPhEs.size(); ++nPhEIt)
      {
        int nPhE = nPhEs.at(nPhEIt);
        
        TFile* inFile = TFile::Open(Form("plots/studyDCR_nPhE%05d_DCR000.000GHz_SPTR0.000ns_%s_nToys10000.root",nPhE,method.c_str()),"READ");
        
        TH1F* h1_timeLE = (TH1F*)( inFile->Get(Form("h1_timeLE_thr%04dPhE",thr)) );
        
        if( h1_timeLE->Integral() < 2000 ) continue;
        
        float* vals = new float[6];
        FindSmallestInterval(vals,h1_timeLE,0.68);
        float mean = vals[0];
        float min = vals[4];
        float max = vals[5];
        float delta = max-min;
        float sigma = 0.5*delta;
        float effSigma = sigma;
        
        g_timeLE[method] -> SetPoint(g_timeLE[method]->GetN(),nPhE,1000.*effSigma);
        g_timeLE[method] -> SetPointError(g_timeLE[method]->GetN()-1,0.,1000.*h1_timeLE->GetRMSError());
        
        inFile -> Close();
      }
    }


    
    g_timeNthPhE -> SetMarkerColor(1);
    g_timeNthPhE -> Draw("P,same");

    TF1* fitFunc_nthPhE = new TF1(Form("fitFunc_nthPhE_thr_%d",thr),"[0]/(x^[1])+[2]",1.,100000.);
    fitFunc_nthPhE -> SetParameters(1000000.,0.5,0.);
    // fitFunc_nthPhE -> FixParameter(2,0);
    fitFunc_nthPhE -> SetNpx(10000);
    fitFunc_nthPhE -> SetLineColor(1);
    
    g_timeNthPhE -> Fit(fitFunc_nthPhE,"QNRS+");
    fitFunc_nthPhE -> Draw("same");
    
    TLatex* latex_nthPhE = new TLatex(0.20,0.15,Form("N^{th} p.e.: f(N_{pe}) = %.1e / N_{pe}^{%.2f} + %.1f",
                                                     fitFunc_nthPhE->GetParameter(0),fitFunc_nthPhE->GetParameter(1),fitFunc_nthPhE->GetParameter(2)));
    latex_nthPhE -> SetNDC();
    latex_nthPhE -> SetTextFont(42);
    latex_nthPhE -> SetTextSize(0.03);
    latex_nthPhE ->SetTextAlign(11);
    latex_nthPhE -> Draw("same");


    
    g_timeAvgNPhE -> SetMarkerColor(2);
    g_timeAvgNPhE -> Draw("P,same");

    TF1* fitFunc_avgNPhE = new TF1(Form("fitFunc_avgNPhE_thr_%d",thr),"[0]/(x^[1])+[2]",1.,100000.);
    fitFunc_avgNPhE -> SetParameters(1000000.,0.5,0.);
    // fitFunc_avgNPhE -> FixParameter(1.,0.5);
    // fitFunc_avgNPhE -> FixParameter(2.,0.);
    fitFunc_avgNPhE -> SetNpx(10000);
    fitFunc_avgNPhE -> SetLineColor(2);
    
    g_timeAvgNPhE -> Fit(fitFunc_avgNPhE,"QNRS+");
    fitFunc_avgNPhE -> Draw("same");
    
    TLatex* latex_avgNPhE = new TLatex(0.20,0.20,Form("#LT N p.e. #GT: f(N_{pe}) = %.1e / N_{pe}^{%.2f} + %.1f",
                                                      fitFunc_avgNPhE->GetParameter(0),fitFunc_avgNPhE->GetParameter(1),fitFunc_avgNPhE->GetParameter(2)));
    latex_avgNPhE -> SetNDC();
    latex_avgNPhE -> SetTextFont(42);
    latex_avgNPhE -> SetTextSize(0.03);
    latex_avgNPhE -> SetTextColor(2);
    latex_avgNPhE ->SetTextAlign(11);
    latex_avgNPhE -> Draw("same");
    


    int methodIt = 0;
    for(auto method: methods)
    {
      g_timeLE[method] -> SetMarkerColor(3+methodIt);
      g_timeLE[method] -> Draw("P,same");
      
      TF1* fitFunc_LE = new TF1(Form("fitFunc_LE_thr_%d_%s",thr,method.c_str()),"[0]/(x^[1])+[2]",1.,100000.);
      fitFunc_LE -> SetParameters(1000000.,0.5,0.);
      // fitFunc_LE -> FixParameter(1.,0.5);
      // fitFunc_LE -> FixParameter(2.,0.);
      fitFunc_LE -> SetNpx(10000);
      fitFunc_LE -> SetLineColor(3+methodIt);
      
      g_timeLE[method] -> Fit(fitFunc_LE,"QNRS+");
      fitFunc_LE -> Draw("same");
      
      TLatex* latex_LE = new TLatex(0.20,0.25+0.05*methodIt,Form("%s: f(N_{pe}) = %.1e / N_{pe}^{%.2f} + %.1f",
                                                                 methodLabels.at(methodIt).c_str(),fitFunc_LE->GetParameter(0),fitFunc_LE->GetParameter(1),fitFunc_LE->GetParameter(2)));
      latex_LE -> SetNDC();
      latex_LE -> SetTextFont(42);
      latex_LE -> SetTextSize(0.03);
      latex_LE -> SetTextColor(3+methodIt);
      latex_LE ->SetTextAlign(11);
      latex_LE -> Draw("same");
      
      ++methodIt;
    }
    
    
    
    f_30ps -> Draw("same");
    
    c -> Print(Form("c_dummyResolution_thr_%03dpe.png",thr));
    c -> Print(Form("c_dummyResolution_thr_%03dpe.pdf",thr));
  }


  
  //--- plots vs. thr
  for(unsigned int nPhEIt = 0; nPhEIt < nPhEs.size(); ++nPhEIt)
  {
    int nPhE = nPhEs.at(nPhEIt);

    TCanvas* c = new TCanvas(Form("c_resolution_nPhE%d",nPhE),Form("c_resolution_nPhE%d",nPhE));
    c -> cd();
    
    TH1F* hPad = (TH1F*)( gPad->DrawFrame(0.1,1.,1000.,10000.) );
    gPad -> SetLogx();
    gPad -> SetLogy();
    hPad -> SetTitle(";threshold [p.e.];#sigma_{t} (ps)");
    hPad -> Draw();
    
    TGraphErrors* g_timeNthPhE = new TGraphErrors();
    TGraphErrors* g_timeAvgNPhE = new TGraphErrors();
    std::map<std::string,TGraphErrors*> g_timeLE;
    
    for(unsigned int thrIt = 0; thrIt < thrs.size(); ++thrIt)
    {
      int thr = thrs.at(thrIt);

      TFile* inFile = TFile::Open(Form("plots/studyDCR_nPhE%05d_DCR000.000GHz_SPTR0.000ns_step_1pe_nToys10000.root",nPhE),"READ");
      
      TH1F* h1_timeNthPhE = (TH1F*)( inFile->Get(Form("h1_timeNthPhE_thr%04dPhE",thr)) );
      TH1F* h1_timeAvgNPhE = (TH1F*)( inFile->Get(Form("h1_timeAvgNPhE_thr%04dPhE",thr)) );
      
      if( h1_timeNthPhE->Integral() < 2000 ) continue;

      float* vals = new float[6];
      FindSmallestInterval(vals,h1_timeNthPhE,0.68);
      float mean = vals[0];
      float min = vals[4];
      float max = vals[5];
      float delta = max-min;
      float sigma = 0.5*delta;
      float effSigma = sigma;
      
      g_timeNthPhE -> SetPoint(g_timeNthPhE->GetN(),thr,1000.*effSigma);
      g_timeNthPhE -> SetPointError(g_timeNthPhE->GetN()-1,0.,1000.*h1_timeNthPhE->GetRMSError());
      
      FindSmallestInterval(vals,h1_timeAvgNPhE,0.68);
      mean = vals[0];
      min = vals[4];
      max = vals[5];
      delta = max-min;
      sigma = 0.5*delta;
      effSigma = sigma;
      
      g_timeAvgNPhE -> SetPoint(g_timeAvgNPhE->GetN(),thr,1000.*effSigma);
      g_timeAvgNPhE -> SetPointError(g_timeAvgNPhE->GetN()-1,0.,1000.*h1_timeAvgNPhE->GetRMSError());
      
      inFile -> Close();
    }

    for(auto method: methods)
    {
      g_timeLE[method] = new TGraphErrors();
      
      for(unsigned int thrIt = 0; thrIt < thrs.size(); ++thrIt)
      {
        int thr = thrs.at(thrIt);
        
        TFile* inFile = TFile::Open(Form("plots/studyDCR_nPhE%05d_DCR000.000GHz_SPTR0.000ns_%s_nToys10000.root",nPhE,method.c_str()),"READ");
        
        TH1F* h1_timeLE = (TH1F*)( inFile->Get(Form("h1_timeLE_thr%04dPhE",thr)) );
        
        if( h1_timeLE->Integral() < 2000 ) continue;
        
        float* vals = new float[6];
        FindSmallestInterval(vals,h1_timeLE,0.68);
        float mean = vals[0];
        float min = vals[4];
        float max = vals[5];
        float delta = max-min;
        float sigma = 0.5*delta;
        float effSigma = sigma;
        
        g_timeLE[method] -> SetPoint(g_timeLE[method]->GetN(),thr,1000.*effSigma);
        g_timeLE[method] -> SetPointError(g_timeLE[method]->GetN()-1,0.,1000.*h1_timeLE->GetRMSError());
        
        inFile -> Close();
      }
    }


    
    g_timeNthPhE -> SetMarkerColor(1);
    g_timeNthPhE -> Draw("P,same");

    TF1* fitFunc_nthPhE = new TF1(Form("fitFunc_nthPhE_nPhE_%d",nPhE),"[0]*(x^[1])+[2]",1.,100000.);
    fitFunc_nthPhE -> SetParameters(1000.,0.5,0.);
    // fitFunc_nthPhE -> FixParameter(2,0);
    fitFunc_nthPhE -> SetNpx(10000);
    fitFunc_nthPhE -> SetLineColor(1);
    
    g_timeNthPhE -> Fit(fitFunc_nthPhE,"QNRS+");
    fitFunc_nthPhE -> Draw("same");
    
    TLatex* latex_nthPhE = new TLatex(0.20,0.15,Form("N^{th} p.e.: f(thr) = %.1e #times thr^{%.2f} + %.1f",
                                                     fitFunc_nthPhE->GetParameter(0),fitFunc_nthPhE->GetParameter(1),fitFunc_nthPhE->GetParameter(2)));
    latex_nthPhE -> SetNDC();
    latex_nthPhE -> SetTextFont(42);
    latex_nthPhE -> SetTextSize(0.03);
    latex_nthPhE ->SetTextAlign(11);
    latex_nthPhE -> Draw("same");


    
    g_timeAvgNPhE -> SetMarkerColor(2);
    g_timeAvgNPhE -> Draw("P,same");

    TF1* fitFunc_avgNPhE = new TF1(Form("fitFunc_avgNPhE_nPhE_%d",nPhE),"[0]*(x^[1])+[2]",1.,100000.);
    fitFunc_avgNPhE -> SetParameters(1000.,0.5,0.);
    // fitFunc_avgNPhE -> FixParameter(2.,0.);
    fitFunc_avgNPhE -> SetNpx(10000);
    fitFunc_avgNPhE -> SetLineColor(2);
    
    g_timeAvgNPhE -> Fit(fitFunc_avgNPhE,"QNRS+");
    fitFunc_avgNPhE -> Draw("same");
    
    TLatex* latex_avgNPhE = new TLatex(0.20,0.20,Form("#LT N p.e. #GT: f(thr) = %.1e #times lthr^{%.2f} + %.1f",
                                                      fitFunc_avgNPhE->GetParameter(0),fitFunc_avgNPhE->GetParameter(1),fitFunc_avgNPhE->GetParameter(2)));
    latex_avgNPhE -> SetNDC();
    latex_avgNPhE -> SetTextFont(42);
    latex_avgNPhE -> SetTextSize(0.03);
    latex_avgNPhE -> SetTextColor(2);
    latex_avgNPhE ->SetTextAlign(11);
    latex_avgNPhE -> Draw("same");
    


    int methodIt = 0;
    for(auto method: methods)
    {
      g_timeLE[method] -> SetMarkerColor(3+methodIt);
      g_timeLE[method] -> Draw("P,same");
      
      TF1* fitFunc_LE = new TF1(Form("fitFunc_LE_nPhE_%d_%s",nPhE,method.c_str()),"[0]*(x^[1])+[2]",1.,100000.);
      fitFunc_LE -> SetParameters(1000.,0.5,0.);
      // fitFunc_LE -> FixParameter(2.,0.);
      fitFunc_LE -> SetNpx(10000);
      fitFunc_LE -> SetLineColor(3+methodIt);
      
      g_timeLE[method] -> Fit(fitFunc_LE,"QNRS+");
      fitFunc_LE -> Draw("same");
      
      TLatex* latex_LE = new TLatex(0.20,0.25+0.05*methodIt,Form("%s: f(thr) = %.1e #times thr^{%.2f} + %.1f",
                                                                 methodLabels.at(methodIt).c_str(),fitFunc_LE->GetParameter(0),fitFunc_LE->GetParameter(1),fitFunc_LE->GetParameter(2)));
      latex_LE -> SetNDC();
      latex_LE -> SetTextFont(42);
      latex_LE -> SetTextSize(0.03);
      latex_LE -> SetTextColor(3+methodIt);
      latex_LE ->SetTextAlign(11);
      latex_LE -> Draw("same");
      
      ++methodIt;
    }
    
    
    
    f_30ps -> Draw("same");
    
    c -> Print(Form("c_dummyResolution_nPhE_%05d.png",nPhE));
    c -> Print(Form("c_dummyResolution_nPhE_%05d.pdf",nPhE));
  }


  
  std::cout << ">>> popping up the plots" << std::endl; theApp -> Run();
}
