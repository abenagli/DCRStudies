#include "interface/FitUtils.h"
#include "interface/SetTDRStyle.h"
#include "CfgManager/interface/CfgManager.h"
#include "CfgManager/interface/CfgManagerT.h"

#include <iostream>
#include <numeric>

#include "TFile.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TRandom3.h"
#include "TLine.h"
#include "TApplication.h"
#include "TLatex.h"
#include "TLegend.h"



int main(int argc, char** argv)
{
  if( argc < 2 )
  {
    std::cerr << ">>>>> studyDCR.exe::usage:   " << argv[0] << " configFileName   [default=0/debug=1]" << std::endl;
    return -1;
  }

  setTDRStyle();
  
  
  //----------------------
  // parse the config file
  CfgManager opts;
  opts.ParseConfigFile(argv[1]);
  
  int debugMode = 0;
  if( argc > 2 ) debugMode = atoi(argv[2]);
  
  float SPTR = opts.GetOpt<float>("Input.SPTR");
  int nToys = opts.GetOpt<int>("Input.nToys");
  std::string inFileName_1pe = opts.GetOpt<std::string>("Input.inFileName1pe");
  
  
  TH1F* histo;
  TFile* inFile;
  TLegend* legend;
  
  TH1F* hPad1;
  TH1F* hPad2;
  std::string title1 = "";
  std::string title2 = "";
  float xMin1;
  float xMax1;
  float yMin1;
  float yMax1;
  float xMin2;
  float xMax2;
  float yMin2;
  float yMax2;
  
  float kFactor = 1.;
  float tau_LE = 1.6; // in ns
  float tau_S = 40.;  // in ns



  //std::string label_DCRSub = "_baseSub";
  std::string label_DCRSub = "_CFD";
  
  std::vector<float> thrs;
  thrs.push_back(1);
  thrs.push_back(2);
  thrs.push_back(3);
  thrs.push_back(5);
  thrs.push_back(10);
  thrs.push_back(20);
  thrs.push_back(30);
  thrs.push_back(50);
  thrs.push_back(100);
  
  std::vector<int> nPhEs;
  nPhEs.push_back(3);
  nPhEs.push_back(5);
  nPhEs.push_back(10);
  nPhEs.push_back(20);
  nPhEs.push_back(30);
  nPhEs.push_back(50);
  nPhEs.push_back(100);
  nPhEs.push_back(200);
  nPhEs.push_back(300);
  
  std::vector<float> DCRs;
  DCRs.push_back(0.);
  DCRs.push_back(0.1);
  DCRs.push_back(0.2);
  DCRs.push_back(0.3);
  DCRs.push_back(0.5);
  DCRs.push_back(1.00);
  DCRs.push_back(2.00);
  DCRs.push_back(3.00);
  DCRs.push_back(5.00);
  DCRs.push_back(10.00);
  DCRs.push_back(20.00);
  DCRs.push_back(30.00);
  
  //--- define inFile names
  TH1F* h_baselineRMS;
  std::map<std::pair<int,float>,std::string> inFileNames;
  for(auto nPhE : nPhEs)
  {
    for(auto DCR : DCRs)
    {
      std::pair<int,float> dummy(nPhE,DCR);
      inFileNames[dummy] = Form("plots/laser_2/studyDCR_nPhE%05d_DCR%07.3fGHz_SPTR%.3fns_%s_CFD00.400ns_nToys%d.root",nPhE,DCR,SPTR,inFileName_1pe.c_str(),nToys);
    }
  }
  
  
  //--- define plot dir
  std::string plotDir = Form("plots/laser_2/DCRPlots_SPTR%.3fns_%s",SPTR,inFileName_1pe.c_str());
  system(Form("mkdir %s",plotDir.c_str()));
  
  /*
  //--- plots vs. nPhE
  std::cout << "\n>>> plots vs. nPhE" << std::endl;
  xMin1 = 1;
  xMax1 = 1000.;
  yMin1 = 0.1;
  yMax1 = 1000.;
  xMin2 = 1;
  xMax2 = 1000.;
  yMin2 = 0.1;
  yMax2 = 1000.;
  title1 = ";N_{p.e.};#sigma_{t} [ps]";
  title2 = ";N_{p.e.};#sigma_{DCR} [ps]";

  nPhEs.clear();
  nPhEs.push_back(3);
  nPhEs.push_back(5);
  nPhEs.push_back(10);
  nPhEs.push_back(20);
  nPhEs.push_back(30);
  nPhEs.push_back(50);
  nPhEs.push_back(100);
  nPhEs.push_back(200);
  nPhEs.push_back(300);
  
  DCRs.clear();
  DCRs.push_back(0.);
  DCRs.push_back(0.5);
  DCRs.push_back(1.00);
  DCRs.push_back(5.00);
  DCRs.push_back(10.00);
  DCRs.push_back(20.00);
  DCRs.push_back(30.00);
  
  for(auto thr : thrs)
  {
    TCanvas* c = new TCanvas(Form("c1_tRes_vs_nPhE_thr%06.1f_allDCR",thr),Form("c1_tRes_vs_nPhE_thr%06.1f_allDCR",thr),2400,1200);
    c -> Divide(2,1);
    c -> cd(1);
    gPad -> SetLogx();
    gPad -> SetLogy();
    hPad1 = (TH1F*)( gPad->DrawFrame(xMin1,yMin1,xMax1,yMax1) );
    hPad1 -> SetTitle(title1.c_str());
    hPad1 -> Draw();
    gPad -> SetGridx();
    gPad -> SetGridy();
    c -> cd(2);
    gPad -> SetLogx();
    gPad -> SetLogy();
    hPad2 = (TH1F*)( gPad->DrawFrame(xMin2,yMin2,xMax2,yMax2) );
    hPad2 -> SetTitle(title2.c_str());
    hPad2 -> Draw();
    gPad -> SetGridx();
    gPad -> SetGridy();
    
    legend = new TLegend(0.19,0.35-0.03*DCRs.size(),0.49,0.35);
    legend -> SetFillColor(0);
    legend -> SetFillStyle(1000);  
    legend -> SetTextFont(42);
    legend -> SetTextSize(0.03);
    
    std::map<int,float> sigma_tot_0DCR;
    std::map<int,float> sigma_tot_err_0DCR;
    int DCRIt = 0;
    for(auto DCR : DCRs)
    {
      TGraphErrors* g_tRes_LE = new TGraphErrors();
      TGraphErrors* g_tResDCR_LE = new TGraphErrors();
      
      TGraphErrors* g_tRes_NthPhE = new TGraphErrors();
      TGraphErrors* g_tResDCR_NthPhE = new TGraphErrors();
      
      TGraphErrors* g_tRes_avgNPhE = new TGraphErrors();
      TGraphErrors* g_tResDCR_avgNPhE = new TGraphErrors();
      
      for(auto nPhE : nPhEs)
      {
        if( thr >= nPhE ) continue;

        std::pair<int,float> dummy(nPhE,DCR);
        inFile = TFile::Open(inFileNames[dummy].c_str(),"READ");
        
        h_baselineRMS = (TH1F*)( inFile->Get("h_baselineRMS"));
        if( thr < 2.*h_baselineRMS->GetMean() ) continue;
        
        histo = (TH1F*)( inFile->Get(Form("h1_timeLE%s_thr%06.1fPhE",label_DCRSub.c_str(),thr)) );
        // histo -> GetXaxis() -> SetRangeUser(21.01,100.);

        float sigma_tot = -999.;
        float sigma_DCR = -999.;
        float sigma_tot_err = -999.;
        
        if( histo->Integral() > 100 )
        {
          TF1* fitFunc = new TF1("fitFunc","gaus(0)",22.1,histo->GetMean()+2.*histo->GetRMS());
          fitFunc -> SetParameters(histo->GetMaximum(),histo->GetMean(),histo->GetRMS());
          histo -> Fit("fitFunc","QNRLS+");
          sigma_tot = 1000. * fabs(fitFunc -> GetParameter(2));
          sigma_tot_err = 1000. * fabs(fitFunc -> GetParError(2));
          delete fitFunc;
          
          // float* vals = new float[6];
          // FindSmallestInterval(vals,histo,0.68,21.01);
          // float mean = vals[0];
          // float min = vals[4];
          // float max = vals[5];
          // float delta = max-min;
          // float effSigma = 0.5*delta;
          // sigma_tot = 1000. * effSigma;
          // sigma_tot_err = 1000. * histo->GetRMSError();
          
          // sigma_tot = 1000. * histo->GetRMSError();
          // sigma_tot_err = 1000. * histo->GetRMSError();
          
          g_tRes_LE -> SetPoint(g_tRes_LE->GetN(),nPhE,sigma_tot);
          g_tRes_LE -> SetPointError(g_tRes_LE->GetN()-1,0.,sigma_tot_err);
          
          if( DCR == 0 )
          {
            sigma_tot_0DCR[nPhE] = sigma_tot;
            sigma_tot_err_0DCR[nPhE] = sigma_tot_err;
          }
          else
          {
            if( sigma_tot > sigma_tot_0DCR[nPhE] )
            {
              float sigma_DCR = sqrt( pow(sigma_tot,2) - pow(sigma_tot_0DCR[nPhE],2) );
              g_tResDCR_LE -> SetPoint(g_tResDCR_LE->GetN(),nPhE,sigma_DCR);
              g_tResDCR_LE -> SetPointError(g_tResDCR_LE->GetN()-1,0.,sqrt(pow(sigma_tot/sigma_DCR*sigma_tot_err,2)+pow(sigma_tot_0DCR[nPhE]/sigma_DCR*sigma_tot_err_0DCR[nPhE],2)));
            }
          }
        }
        
        
        histo = (TH1F*)( inFile->Get(Form("h1_timeNthPhE_thr%06.1fPhE",thr)) );
        if( histo->Integral() > 100 )
        {
          g_tRes_NthPhE -> SetPoint(g_tRes_NthPhE->GetN(),nPhE,1000.*histo->GetRMS());
          g_tRes_NthPhE -> SetPointError(g_tRes_NthPhE->GetN()-1,0.,1000.*histo->GetRMSError());
        }

        
        histo = (TH1F*)( inFile->Get(Form("h1_timeAvgNPhE_thr%06.1fPhE",thr)) );
        if( histo->Integral() > 100 )
        {
          g_tRes_avgNPhE -> SetPoint(g_tRes_avgNPhE->GetN(),nPhE,1000.*histo->GetRMS());
          g_tRes_avgNPhE -> SetPointError(g_tRes_avgNPhE->GetN()-1,0.,1000.*histo->GetRMSError());
        }
        
        inFile -> Close();
      }
      
      
      c -> cd(1);
      
      g_tRes_LE -> SetMarkerSize(1.);
      g_tRes_LE -> SetMarkerColor(51+int(50/DCRs.size())*DCRIt);
      g_tRes_LE -> SetLineColor(51+int(50/DCRs.size())*DCRIt);
      if( DCR == 0 ) g_tRes_LE -> Draw("P,same");
      else           g_tRes_LE -> Draw("PL,same");
      legend -> AddEntry(g_tRes_LE,Form("DCR: %.2f GHz",DCR),"PL");
      
      TLatex* latexL;
      TLatex* latexR;
      
      if( DCR == 0 )
      {
        g_tRes_NthPhE -> SetLineColor(51+int(50/DCRs.size())*DCRIt);
        g_tRes_NthPhE -> SetLineStyle(7);
        g_tRes_NthPhE -> SetLineWidth(2);
        // g_tRes_NthPhE -> Draw("E0,L,same");
        
        g_tRes_avgNPhE -> SetLineColor(51+int(50/DCRs.size())*DCRIt);
        g_tRes_avgNPhE -> SetLineStyle(3);
        g_tRes_avgNPhE -> SetLineWidth(2);
        // g_tRes_avgNPhE -> Draw("E0,L,same");
        
        latexL = new TLatex(0.17,0.90,Form("thr: %.1f p.e.",thr));
        latexL -> SetNDC();
        latexL -> SetTextFont(42);
        latexL -> SetTextSize(0.04);
        latexL ->SetTextAlign(11);
        latexL -> Draw("same");
        
        latexR = new TLatex(0.17,0.95,Form("1 p.e. pulse shape: %s",inFileName_1pe.c_str()));
        latexR -> SetNDC();
        latexR -> SetTextFont(42);
        latexR -> SetTextSize(0.03);
        latexR ->SetTextAlign(11);
        latexR -> Draw("same");

        if( g_tRes_LE->GetN() > 0 )
        {
          TF1* f_model = new TF1(Form("f_model%07.3GHz",DCR),"[0]/(x)^[1]",0.,100000);
          f_model -> SetParameters(100.,1.);
          f_model -> SetLineStyle(2);
          f_model -> SetLineWidth(1);
          f_model -> SetLineColor(51+int(50/DCRs.size())*DCRIt);
          g_tRes_LE -> Fit(f_model,"QNRS+");
          f_model -> Draw("same");
          
          TLatex* latex_model = new TLatex(30.,1.,Form("f(N_{pe}) = %.0f ps / x^{%.2f}",f_model->GetParameter(0),f_model->GetParameter(1)));
          latex_model -> SetTextFont(42);
          latex_model -> SetTextSize(0.04);
          latex_model -> SetTextColor(51+int(50/DCRs.size())*DCRIt);
          latex_model -> Draw("same");
        }
      }
      
      gPad -> Update();
      
      c -> cd(2);
      
      g_tResDCR_LE -> SetMarkerSize(1.);
      g_tResDCR_LE -> SetMarkerColor(51+int(50/DCRs.size())*DCRIt);
      g_tResDCR_LE -> SetLineColor(51+int(50/DCRs.size())*DCRIt);
      g_tResDCR_LE -> Draw("PL");

      if( DCR == 0 )
      {
        latexL -> Draw("same");
        latexR -> Draw("same");
      }

      if( DCR > 0 )
      {
        if( g_tResDCR_LE->GetN() > 0 )
        {
          TF1* f_model = new TF1(Form("f_DCR_model%07.3GHz",DCR),"[0]/(x)^[1]",0.,100000);
          f_model -> SetParameters(100.,1.);
          f_model -> SetLineStyle(2);
          f_model -> SetLineWidth(1);
          f_model -> SetLineColor(51+int(50/DCRs.size())*DCRIt);
          g_tResDCR_LE -> Fit(f_model,"QNRS+","",0.9,100000.);
          f_model -> Draw("same");
          
          TLatex* latex_model = new TLatex(1.5,pow(10,0.7+(DCRIt-1)/(DCRs.size()-1.)),Form("f(N_{pe}) = %.0f ps / x^{%.2f}",f_model->GetParameter(0),f_model->GetParameter(1)));
          latex_model -> SetTextFont(42);
          latex_model -> SetTextSize(0.03);
          latex_model -> SetTextColor(51+int(50/DCRs.size())*DCRIt);
          latex_model -> Draw("same");
        }
      }
      
      gPad -> Update();
      
      ++DCRIt;
    }

    c -> cd(1);
    legend -> Draw("same");
    c -> cd(2);
    legend -> Draw("same");
    
    c -> Print(Form("%s/c1_tRes_vs_nPhE_thr%06.1f_%s.png",plotDir.c_str(),thr,label_DCRSub.c_str()));
    
  } //--- plots vs. nPhE
  
  
  
  //--- plots vs. DCR
  nPhEs.clear();
  nPhEs.push_back(5);
  nPhEs.push_back(10);
  nPhEs.push_back(30);
  nPhEs.push_back(50);
  nPhEs.push_back(100);
  nPhEs.push_back(200);
  
  DCRs.clear();
  DCRs.push_back(0.);
  DCRs.push_back(0.1);
  DCRs.push_back(0.2);
  DCRs.push_back(0.3);
  DCRs.push_back(0.5);
  DCRs.push_back(1.00);
  DCRs.push_back(2.00);
  DCRs.push_back(3.00);
  DCRs.push_back(5.00);
  DCRs.push_back(10.00);
  DCRs.push_back(20.00);
  DCRs.push_back(30.00);
  
  std::cout << "\n>>> plots vs. DCR" << std::endl;
  xMin1 = 0.001;
  xMax1 = 100.;
  yMin1 = 0.1;
  yMax1 = 1000.;
  xMin2 = 0.01;
  xMax2 = 100.;
  yMin2 = 0.1;
  yMax2 = 1000.;
  title1 = ";DCR [GHz];#sigma_{t} [ps]";
  title2 = ";DCR [GHz];#sigma_{DCR} [ps]";
  
  for(auto thr : thrs)
  {
    TCanvas* c = new TCanvas(Form("c1_tRes_vs_DCR_thr%06.1f_allDCR",thr),Form("c1_tRes_vs_DCR_thr%06.1f_allDCR",thr),2400,1200);
    c -> Divide(2,1);
    c -> cd(1);
    gPad -> SetLogx();
    gPad -> SetLogy();
    hPad1 = (TH1F*)( gPad->DrawFrame(xMin1,yMin1,xMax1,yMax1) );
    hPad1 -> SetTitle(title1.c_str());
    hPad1 -> Draw();
    gPad -> SetGridx();
    gPad -> SetGridy();
    c -> cd(2);
    gPad -> SetLogx();
    gPad -> SetLogy();
    hPad2 = (TH1F*)( gPad->DrawFrame(xMin2,yMin2,xMax2,yMax2) );
    hPad2 -> SetTitle(title2.c_str());
    hPad2 -> Draw();
    gPad -> SetGridx();
    gPad -> SetGridy();

    legend = new TLegend(0.19,0.35-0.03*nPhEs.size(),0.49,0.35);
    legend -> SetFillColor(0);
    legend -> SetFillStyle(1000);  
    legend -> SetTextFont(42);
    legend -> SetTextSize(0.03);
    
    std::map<int,float> sigma_tot_0DCR;
    std::map<int,float> sigma_tot_err_0DCR;
    int nPhEIt = 0;
    for(auto nPhE : nPhEs)
    {
      TGraphErrors* g_tRes_LE = new TGraphErrors();
      TGraphErrors* g_tResDCR_LE = new TGraphErrors();
      
      TGraphErrors* g_tRes_NthPhE = new TGraphErrors();
      TGraphErrors* g_tResDCR_NthPhE = new TGraphErrors();
      
      TGraphErrors* g_tRes_avgNPhE = new TGraphErrors();
      TGraphErrors* g_tResDCR_avgNPhE = new TGraphErrors();
      
      for(auto DCR : DCRs)
      {
        if( thr >= nPhE ) continue;
        
        std::pair<int,float> dummy(nPhE,DCR);
        inFile = TFile::Open(inFileNames[dummy].c_str(),"READ");
        
        h_baselineRMS = (TH1F*)( inFile->Get("h_baselineRMS"));
        if( thr < 2.*h_baselineRMS->GetMean() ) continue;
        
        histo = (TH1F*)( inFile->Get(Form("h1_timeLE%s_thr%06.1fPhE",label_DCRSub.c_str(),thr)) );
        // histo -> GetXaxis() -> SetRangeUser(21.01,100.);

        float sigma_tot = -999.;
        float sigma_DCR = -999.;
        float sigma_tot_err = -999.;
        
        if( histo->Integral() > 100 )
        {
          TF1* fitFunc = new TF1("fitFunc","gaus(0)",22.1,histo->GetMean()+2.*histo->GetRMS());
          fitFunc -> SetParameters(histo->GetMaximum(),histo->GetMean(),histo->GetRMS());
          histo -> Fit("fitFunc","QNRLS+");
          sigma_tot = 1000. * fabs(fitFunc -> GetParameter(2));
          sigma_tot_err = 1000. * fabs(fitFunc -> GetParError(2));
          delete fitFunc;
          
          // float* vals = new float[6];
          // FindSmallestInterval(vals,histo,0.68,21.01);
          // float mean = vals[0];
          // float min = vals[4];
          // float max = vals[5];
          // float delta = max-min;
          // float effSigma = 0.5*delta;
          // sigma_tot = 1000. * effSigma;
          // sigma_tot_err = 1000. * histo->GetRMSError();
          
          // sigma_tot = 1000. * histo->GetRMSError();
          // sigma_tot_err = 1000. * histo->GetRMSError();
          
          g_tRes_LE -> SetPoint(g_tRes_LE->GetN(),DCR,sigma_tot);
          g_tRes_LE -> SetPointError(g_tRes_LE->GetN()-1,0.,sigma_tot_err);
          
          if( DCR == 0 )
          {
            sigma_tot_0DCR[nPhE] = sigma_tot;
            sigma_tot_err_0DCR[nPhE] = sigma_tot_err;
          }
          else
          {
            if( sigma_tot > sigma_tot_0DCR[nPhE] )
            {
              float sigma_DCR = sqrt( pow(sigma_tot,2) - pow(sigma_tot_0DCR[nPhE],2) );
              g_tResDCR_LE -> SetPoint(g_tResDCR_LE->GetN(),DCR,sigma_DCR);
              g_tResDCR_LE -> SetPointError(g_tResDCR_LE->GetN()-1,0.,sqrt(pow(sigma_tot/sigma_DCR*sigma_tot_err,2)+pow(sigma_tot_0DCR[nPhE]/sigma_DCR*sigma_tot_err_0DCR[nPhE],2)));
            }
          }
        }

        
        histo = (TH1F*)( inFile->Get(Form("h1_timeNthPhE_thr%06.1fPhE",thr)) );
        if( histo->Integral() > 100 )
        {
          g_tRes_NthPhE -> SetPoint(g_tRes_NthPhE->GetN(),DCR,1000.*histo->GetRMS());
          g_tRes_NthPhE -> SetPointError(g_tRes_NthPhE->GetN()-1,0.,1000.*histo->GetRMSError());
        }

        
        histo = (TH1F*)( inFile->Get(Form("h1_timeAvgNPhE_thr%06.1fPhE",thr)) );
        if( histo->Integral() > 100 )
        {
          g_tRes_avgNPhE -> SetPoint(g_tRes_avgNPhE->GetN(),DCR,1000.*histo->GetRMS());
          g_tRes_avgNPhE -> SetPointError(g_tRes_avgNPhE->GetN()-1,0.,1000.*histo->GetRMSError());
        }
        
        inFile -> Close();
      }

      
      c -> cd(1);
      
      g_tRes_LE -> SetMarkerSize(1.);
      g_tRes_LE -> SetMarkerColor(51+int(50/nPhEs.size())*nPhEIt);
      g_tRes_LE -> SetLineColor(51+int(50/nPhEs.size())*nPhEIt);
      g_tRes_LE -> Draw("PL,same");
      legend -> AddEntry(g_tRes_LE,Form("N_{p.e.}: %d",nPhE),"PL");
      
      TLatex* latexL;
      TLatex* latexR;
      
      if( nPhEIt == 0 )
      {
        latexL = new TLatex(0.17,0.90,Form("thr: %.1f p.e.",thr));
        latexL -> SetNDC();
        latexL -> SetTextFont(42);
        latexL -> SetTextSize(0.04);
        latexL ->SetTextAlign(11);
        latexL -> Draw("same");
        
        latexR = new TLatex(0.17,0.95,Form("1 p.e. pulse shape: %s",inFileName_1pe.c_str()));
        latexR -> SetNDC();
        latexR -> SetTextFont(42);
        latexR -> SetTextSize(0.03);
        latexR ->SetTextAlign(11);
        latexR -> Draw("same");
      }
      
      gPad -> Update();
      
      c -> cd(2);
      
      g_tResDCR_LE -> SetMarkerSize(1.);
      g_tResDCR_LE -> SetMarkerColor(51+int(50/nPhEs.size())*nPhEIt);
      g_tResDCR_LE -> SetLineColor(51+int(50/nPhEs.size())*nPhEIt);
      g_tResDCR_LE -> Draw("PL");
      
      if( nPhEIt == 0 )
      {
        latexL -> Draw("same");
        latexR -> Draw("same");
      }

      if( g_tResDCR_LE->GetN() > 0 )
      {
        TF1* f_model = new TF1(Form("f_DCR_model%d",nPhE),"[0]*(x)^[1]",0.,100000);
        f_model -> SetParameters(100.,0.5);
        f_model -> SetLineStyle(2);
        f_model -> SetLineWidth(1);
        f_model -> SetLineColor(51+int(50/nPhEs.size())*nPhEIt);
        g_tResDCR_LE -> Fit(f_model,"QNRS+","");
        f_model -> Draw("same");
        
        TLatex* latex_model = new TLatex(1.,pow(10,0.-0.75*(nPhEIt)/(nPhEs.size()-1.)),Form("f(DCR) = %.0f ps #times x^{%.2f}",f_model->GetParameter(0),f_model->GetParameter(1)));
        latex_model -> SetTextFont(42);
        latex_model -> SetTextSize(0.03);
        latex_model -> SetTextColor(51+int(50/nPhEs.size())*nPhEIt);
        latex_model -> Draw("same");
      }
      
      gPad -> Update();
      
      ++nPhEIt;
    }

    c -> cd(1);
    legend -> Draw("same");
    c -> cd(2);
    legend -> Draw("same");
    
    c -> Print(Form("%s/c1_tRes_vs_DCR_thr%06.1f_%s.png",plotDir.c_str(),thr,label_DCRSub.c_str()));
    
  } //--- plots vs. DCR
  
  */



  
  //--- plots vs. nPhE
  std::cout << "\n>>> plots vs. nPhE" << std::endl;
  xMin1 = 1;
  xMax1 = 1000.;
  yMin1 = 0.1;
  yMax1 = 1000.;
  xMin2 = 1;
  xMax2 = 1000.;
  yMin2 = 0.1;
  yMax2 = 1000.;
  title1 = ";N_{p.e.};#sigma_{t} [ps]";
  title2 = ";N_{p.e.};#sigma_{DCR} [ps]";

  nPhEs.clear();
  nPhEs.push_back(3);
  nPhEs.push_back(5);
  nPhEs.push_back(10);
  nPhEs.push_back(20);
  nPhEs.push_back(30);
  nPhEs.push_back(50);
  nPhEs.push_back(100);
  nPhEs.push_back(200);
  nPhEs.push_back(300);
  
  DCRs.clear();
  DCRs.push_back(0.);
  DCRs.push_back(0.5);
  DCRs.push_back(1.00);
  DCRs.push_back(5.00);
  DCRs.push_back(10.00);
  DCRs.push_back(20.00);
  DCRs.push_back(30.00);
  
  TCanvas* c = new TCanvas(Form("c1_tRes_vs_nPhE_allDCR"),Form("c1_tRes_vs_nPhE_allDCR"),2400,1200);
  c -> Divide(2,1);
  c -> cd(1);
  gPad -> SetLogx();
  gPad -> SetLogy();
  hPad1 = (TH1F*)( gPad->DrawFrame(xMin1,yMin1,xMax1,yMax1) );
  hPad1 -> SetTitle(title1.c_str());
  hPad1 -> Draw();
  gPad -> SetGridx();
  gPad -> SetGridy();
  c -> cd(2);
  gPad -> SetLogx();
  gPad -> SetLogy();
  hPad2 = (TH1F*)( gPad->DrawFrame(xMin2,yMin2,xMax2,yMax2) );
  hPad2 -> SetTitle(title2.c_str());
  hPad2 -> Draw();
  gPad -> SetGridx();
  gPad -> SetGridy();
  
  legend = new TLegend(0.19,0.35-0.03*DCRs.size(),0.49,0.35);
  legend -> SetFillColor(0);
  legend -> SetFillStyle(1000);  
  legend -> SetTextFont(42);
  legend -> SetTextSize(0.03);
  
  std::map<int,float> sigma_tot_0DCR;
  std::map<int,float> sigma_tot_err_0DCR;
  int DCRIt = 0;
  for(auto DCR : DCRs)
  {
    TGraphErrors* g_tRes_LE = new TGraphErrors();
    TGraphErrors* g_tResDCR_LE = new TGraphErrors();
    
    TGraphErrors* g_tRes_NthPhE = new TGraphErrors();
    TGraphErrors* g_tResDCR_NthPhE = new TGraphErrors();
    
    TGraphErrors* g_tRes_avgNPhE = new TGraphErrors();
    TGraphErrors* g_tResDCR_avgNPhE = new TGraphErrors();
    
    for(auto nPhE : nPhEs)
    {
      std::pair<int,float> dummy(nPhE,DCR);
      inFile = TFile::Open(inFileNames[dummy].c_str(),"READ");
      
      h_baselineRMS = (TH1F*)( inFile->Get("h_baselineRMS"));
      // if( thr < 2.*h_baselineRMS->GetMean() ) continue;
      
      histo = (TH1F*)( inFile->Get(Form("h1_timeLEFit%s",label_DCRSub.c_str())) );
      // histo -> GetXaxis() -> SetRangeUser(21.01,100.);
      
      float sigma_tot = -999.;
      float sigma_DCR = -999.;
      float sigma_tot_err = -999.;
      
      if( histo->Integral() > 100 )
      {
        // TF1* fitFunc = new TF1("fitFunc","gaus(0)",22.1,histo->GetMean()+2.*histo->GetRMS());
        // fitFunc -> SetParameters(histo->GetMaximum(),histo->GetMean(),histo->GetRMS());
        // histo -> Fit("fitFunc","QNRLS+");
        // sigma_tot = 1000. * fabs(fitFunc -> GetParameter(2));
        // sigma_tot_err = 1000. * fabs(fitFunc -> GetParError(2));
        // delete fitFunc;
        
        float* vals = new float[6];
        FindSmallestInterval(vals,histo,0.68,21.01);
        float mean = vals[0];
        float min = vals[4];
        float max = vals[5];
        float delta = max-min;
        float effSigma = 0.5*delta;
        sigma_tot = 1000. * effSigma;
        sigma_tot_err = 1000. * histo->GetRMSError();
        
        // sigma_tot = 1000. * histo->GetRMS();
        // sigma_tot_err = 1000. * histo->GetRMSError();

        if( sigma_tot > 500. ) continue;
        
        g_tRes_LE -> SetPoint(g_tRes_LE->GetN(),nPhE,sigma_tot);
        g_tRes_LE -> SetPointError(g_tRes_LE->GetN()-1,0.,sigma_tot_err);
        
        if( DCR == 0 )
        {
          sigma_tot_0DCR[nPhE] = sigma_tot;
          sigma_tot_err_0DCR[nPhE] = sigma_tot_err;
        }
        else
        {
          if( sigma_tot > sigma_tot_0DCR[nPhE] )
          {
            float sigma_DCR = sqrt( pow(sigma_tot,2) - pow(sigma_tot_0DCR[nPhE],2) );
            g_tResDCR_LE -> SetPoint(g_tResDCR_LE->GetN(),nPhE,sigma_DCR);
            g_tResDCR_LE -> SetPointError(g_tResDCR_LE->GetN()-1,0.,sqrt(pow(sigma_tot/sigma_DCR*sigma_tot_err,2)+pow(sigma_tot_0DCR[nPhE]/sigma_DCR*sigma_tot_err_0DCR[nPhE],2)));
          }
        }
      }
      
      inFile -> Close();
    }
    
    
    c -> cd(1);
    
    g_tRes_LE -> SetMarkerSize(1.);
    g_tRes_LE -> SetMarkerColor(51+int(50/DCRs.size())*DCRIt);
    g_tRes_LE -> SetLineColor(51+int(50/DCRs.size())*DCRIt);
    if( DCR == 0 ) g_tRes_LE -> Draw("P,same");
    else           g_tRes_LE -> Draw("PL,same");
    legend -> AddEntry(g_tRes_LE,Form("DCR: %.2f GHz",DCR),"PL");
    
    TLatex* latexL;
    TLatex* latexR;
    
    if( DCR == 0 )
    {
      g_tRes_NthPhE -> SetLineColor(51+int(50/DCRs.size())*DCRIt);
      g_tRes_NthPhE -> SetLineStyle(7);
      g_tRes_NthPhE -> SetLineWidth(2);
      // g_tRes_NthPhE -> Draw("E0,L,same");
      
      g_tRes_avgNPhE -> SetLineColor(51+int(50/DCRs.size())*DCRIt);
      g_tRes_avgNPhE -> SetLineStyle(3);
      g_tRes_avgNPhE -> SetLineWidth(2);
      // g_tRes_avgNPhE -> Draw("E0,L,same");
      
      latexR = new TLatex(0.17,0.95,Form("1 p.e. pulse shape: %s",inFileName_1pe.c_str()));
      latexR -> SetNDC();
      latexR -> SetTextFont(42);
      latexR -> SetTextSize(0.03);
      latexR ->SetTextAlign(11);
      latexR -> Draw("same");
      
      if( g_tRes_LE->GetN() > 0 )
      {
        TF1* f_model = new TF1(Form("f_model%07.3GHz",DCR),"[0]/(x)^[1]",0.,100000);
        f_model -> SetParameters(100.,1.);
        f_model -> SetLineStyle(2);
        f_model -> SetLineWidth(1);
        f_model -> SetLineColor(51+int(50/DCRs.size())*DCRIt);
        g_tRes_LE -> Fit(f_model,"QNRS+");
        f_model -> Draw("same");
        
        TLatex* latex_model = new TLatex(30.,1.,Form("f(N_{pe}) = %.0f ps / x^{%.2f}",f_model->GetParameter(0),f_model->GetParameter(1)));
        latex_model -> SetTextFont(42);
        latex_model -> SetTextSize(0.04);
        latex_model -> SetTextColor(51+int(50/DCRs.size())*DCRIt);
        latex_model -> Draw("same");
      }
    }
    
    gPad -> Update();
    
    c -> cd(2);
    
    g_tResDCR_LE -> SetMarkerSize(1.);
    g_tResDCR_LE -> SetMarkerColor(51+int(50/DCRs.size())*DCRIt);
    g_tResDCR_LE -> SetLineColor(51+int(50/DCRs.size())*DCRIt);
    g_tResDCR_LE -> Draw("PL");
    
    if( DCR == 0 )
    {
      latexR -> Draw("same");
    }
    
    if( DCR > 0 )
    {
      if( g_tResDCR_LE->GetN() > 0 )
      {
        TF1* f_model = new TF1(Form("f_DCR_model%07.3GHz",DCR),"[0]/(x)^[1]",0.,100000);
        f_model -> SetParameters(100.,1.);
        f_model -> SetLineStyle(2);
        f_model -> SetLineWidth(1);
        f_model -> SetLineColor(51+int(50/DCRs.size())*DCRIt);
        g_tResDCR_LE -> Fit(f_model,"QNRS+","",0.9,100000.);
        f_model -> Draw("same");
        
        TLatex* latex_model = new TLatex(1.5,pow(10,0.7+(DCRIt-1)/(DCRs.size()-1.)),Form("f(N_{pe}) = %.0f ps / x^{%.2f}",f_model->GetParameter(0),f_model->GetParameter(1)));
        latex_model -> SetTextFont(42);
        latex_model -> SetTextSize(0.03);
        latex_model -> SetTextColor(51+int(50/DCRs.size())*DCRIt);
        latex_model -> Draw("same");
      }
    }
    
    gPad -> Update();
    
    ++DCRIt;
  }
  
  c -> cd(1);
  legend -> Draw("same");
  c -> cd(2);
  legend -> Draw("same");
  
  c -> Print(Form("%s/c1_tRes_vs_nPhEFit%s.png",plotDir.c_str(),label_DCRSub.c_str()));
  
  //--- plots vs. nPhE


  /*
  //--- plots vs. nPhE
  xMin1 = 2;
  xMax1 = 2000.;
  yMin1 = 1.;
  yMax1 = 1000.;
  xMin2 = 2;
  xMax2 = 2000.;
  yMin2 = 1.;
  yMax2 = 1000.;
  title1 = ";LY#timesLCE#timesPDE [# p.e.];#sigma_{t} [ps]";
  title2 = ";LY#timesLCE#timesPDE [# p.e.];#sigma_{DCR} [ps]";
  
  for(auto thr : thrs)
  {
    TCanvas* c = new TCanvas(Form("c1_tRes_vs_nPhE_CFD_thr%06.1f_allDCR",thr),Form("c1_tRes_vs_nPhE_CFD_thr%06.1f_allDCR",thr),2400,1200);
    c -> Divide(2,1);
    c -> cd(1);
    gPad -> SetLogx();
    gPad -> SetLogy();
    hPad1 = (TH1F*)( gPad->DrawFrame(xMin1,yMin1,xMax1,yMax1) );
    hPad1 -> SetTitle(title1.c_str());
    hPad1 -> Draw();
    gPad -> SetGridx();
    gPad -> SetGridy();
    c -> cd(2);
    gPad -> SetLogx();
    gPad -> SetLogy();
    hPad2 = (TH1F*)( gPad->DrawFrame(xMin2,yMin2,xMax2,yMax2) );
    hPad2 -> SetTitle(title2.c_str());
    hPad2 -> Draw();
    gPad -> SetGridx();
    gPad -> SetGridy();
    
    legend = new TLegend(0.75,0.90-0.04*DCRs.size(),0.99,0.90);
    legend -> SetFillColor(0);
    legend -> SetFillStyle(1000);  
    legend -> SetTextFont(42);
    legend -> SetTextSize(0.03);
    
    std::map<int,float> RMS_0DCR;
    int DCRIt = 0;
    for(auto DCR : DCRs)
    {
      TGraphErrors* g_tRes_LE = new TGraphErrors();
      TGraphErrors* g_tResDCR_LE = new TGraphErrors();

      TGraphErrors* g_tRes_NthPhE = new TGraphErrors();
      TGraphErrors* g_tResDCR_NthPhE = new TGraphErrors();
      
      TGraphErrors* g_tRes_avgNPhE = new TGraphErrors();
      TGraphErrors* g_tResDCR_avgNPhE = new TGraphErrors();
      
      for(auto nPhE : nPhEs)
      {
        if( thr >= nPhE ) continue;
        
        std::pair<int,float> dummy(nPhE,DCR);
        inFile = TFile::Open(inFileNames[dummy].c_str(),"READ");

        histo = (TH1F*)( inFile->Get(Form("h1_timeLE_CFD_thr%06.1fPhE",thr)) );
        histo -> GetXaxis() -> SetRangeUser(21.01,100.);
        if( histo->Integral() > 100 )
        {
          g_tRes_LE -> SetPoint(g_tRes_LE->GetN(),nPhE,1000.*histo->GetRMS());
          g_tRes_LE -> SetPointError(g_tRes_LE->GetN()-1,0.,1000.*histo->GetRMSError());
        }
        
        if( DCR == 0 ) RMS_0DCR[nPhE] = 1000.*histo->GetRMS();
        else if( histo->Integral() > 100 )
        {
          g_tResDCR_LE -> SetPoint(g_tResDCR_LE->GetN(),nPhE,sqrt(pow(1000.*histo->GetRMS(),2)-pow(RMS_0DCR[nPhE],2)));
          g_tResDCR_LE -> SetPointError(g_tResDCR_LE->GetN()-1,0.,sqrt(1000.*histo->GetRMSError()));
        }
        
        histo = (TH1F*)( inFile->Get(Form("h1_timeNthPhE_thr%06.1fPhE",thr)) );
        if( histo->Integral() > 100 )
        {
          g_tRes_NthPhE -> SetPoint(g_tRes_NthPhE->GetN(),nPhE,1000.*histo->GetRMS());
          g_tRes_NthPhE -> SetPointError(g_tRes_NthPhE->GetN()-1,0.,1000.*histo->GetRMSError());
        }
        
        histo = (TH1F*)( inFile->Get(Form("h1_timeAvgNPhE_thr%06.1fPhE",thr)) );
        if( histo->Integral() > 100 )
        {
          g_tRes_avgNPhE -> SetPoint(g_tRes_avgNPhE->GetN(),nPhE,1000.*histo->GetRMS());
          g_tRes_avgNPhE -> SetPointError(g_tRes_avgNPhE->GetN()-1,0.,1000.*histo->GetRMSError());
        }
        
        inFile -> Close();
      }

      c -> cd(1);
      g_tRes_LE -> SetMarkerSize(1.);
      g_tRes_LE -> SetMarkerColor(51+5*DCRIt);
      g_tRes_LE -> SetLineColor(51+5*DCRIt);
      if( DCR == 0 ) g_tRes_LE -> Draw("P,same");
      else           g_tRes_LE -> Draw("PL,same");
      legend -> AddEntry(g_tRes_LE,Form("DCR: %.2f GHz",DCR),"PL");

      TLatex* latexL;
      TLatex* latexR;
      
      if( DCR == 0 )
      {
        g_tRes_NthPhE -> SetLineColor(51+5*DCRIt);
        g_tRes_NthPhE -> SetLineStyle(7);
        g_tRes_NthPhE -> SetLineWidth(2);
        // g_tRes_NthPhE -> Draw("E0,L,same");
        
        g_tRes_avgNPhE -> SetLineColor(51+5*DCRIt);
        g_tRes_avgNPhE -> SetLineStyle(3);
        g_tRes_avgNPhE -> SetLineWidth(2);
        // g_tRes_avgNPhE -> Draw("E0,L,same");
        
        latexL = new TLatex(0.14,0.90,Form("thr: %.1f p.e.",thr));
        latexL -> SetNDC();
        latexL -> SetTextFont(42);
        latexL -> SetTextSize(0.04);
        latexL ->SetTextAlign(11);
        latexL -> Draw("same");
        
        latexR = new TLatex(0.80,0.96,Form("1 p.e. pulse shape: %s",inFileName_1pe.c_str()));
        latexR -> SetNDC();
        latexR -> SetTextFont(42);
        latexR -> SetTextSize(0.03);
        latexR ->SetTextAlign(31);
        latexR -> Draw("same");
        
        // f_30ps -> Draw("same");
        // TLatex* latex_30ps = new TLatex(xMin1,30.,"30 ps");
        // latex_30ps -> SetTextFont(42);
        // latex_30ps -> SetTextSize(0.02);
        // latex_30ps -> Draw("same");
        
        // f_50ps -> Draw("same");
        // TLatex* latex_50ps = new TLatex(xMin1,50.,"50 ps");
        // latex_50ps -> SetTextFont(42);
        // latex_50ps -> SetTextSize(0.02);
        // latex_50ps -> Draw("same");
        
        TF1* f_model = new TF1(Form("f_model%07.3GHz",DCR),"[0]/(x)^[1]",0.,100000);
        f_model -> SetParameters(100.,1.);
        f_model -> SetLineStyle(2);
        f_model -> SetLineWidth(1);
        f_model -> SetLineColor(51+5*DCRIt);
        g_tRes_LE -> Fit(f_model,"QNRS+");
        f_model -> Draw("same");

        TLatex* latex_model = new TLatex(10.,10.,Form("f(N_{pe}) = %.0f ps / x^{%.2f}",f_model->GetParameter(0),f_model->GetParameter(1)));
        latex_model -> SetTextFont(42);
        latex_model -> SetTextSize(0.04);
        latex_model -> SetTextColor(51+5*DCRIt);
        latex_model -> Draw("same");
      }
      
      // TF1* f_model = new TF1(Form("f_model%07.3GHz",DCR),"1000.*sqrt( ([0]*sqrt([1]*[2])*[3]/x)^2 + (sqrt([4])*[3]/x)^2 )",0.,100000);
      // f_model -> SetParameters(kFactor,DCR,tau_LE,tau_S,thr);
      // f_model -> SetLineStyle(2);
      // f_model -> SetLineColor(51+5*DCRIt);
      // // f_model -> Draw("same");
      
      gPad -> Update();

      c -> cd(2);
      g_tResDCR_LE -> SetMarkerSize(0.7);
      g_tResDCR_LE -> SetMarkerColor(51+5*DCRIt);
      g_tResDCR_LE -> SetLineColor(51+5*DCRIt);
      g_tResDCR_LE -> Draw("PL");

      if( DCR == 0 )
      {
        latexL -> Draw("same");
        latexR -> Draw("same");
      }

      if( DCR > 0 )
      {
        TF1* f_model = new TF1(Form("f_DCR_model%07.3GHz",DCR),"[0]/(x)^[1]",0.,100000);
        f_model -> SetParameters(100.,1.);
        f_model -> SetLineStyle(2);
        f_model -> SetLineWidth(1);
        f_model -> SetLineColor(51+5*DCRIt);
        g_tResDCR_LE -> Fit(f_model,"QNRS+","",99,100000.);
        f_model -> Draw("same");
        
        TLatex* latex_model = new TLatex(10.,pow(10,0.3+(DCRIt-1)/(DCRs.size()-1.)),Form("f(N_{pe}) = %.0f ps / x^{%.2f}",f_model->GetParameter(0),f_model->GetParameter(1)));
        latex_model -> SetTextFont(42);
        latex_model -> SetTextSize(0.03);
        latex_model -> SetTextColor(51+5*DCRIt);
        latex_model -> Draw("same");
      }
      
      gPad -> Update();
      
      ++DCRIt;
    }

    c -> cd(1);
    legend -> Draw("same");
    c -> cd(2);
    legend -> Draw("same");
    
    c -> Print(Form("%s/c1_tRes_vs_nPhE_CFD_thr%06.1f_allDCR.png",plotDir.c_str(),thr));
    
  } //--- plots vs. nPhE  
  */

  /*
  //--- plots vs. thr
  xMin1 = 0.01;
  xMax1 = 500.;
  yMin1 = 1.;
  yMax1 = 1000.;
  xMin2 = 0.01;
  xMax2 = 500.;
  yMin2 = 1.;
  yMax2 = 1000.;
  title1 = ";threshold [# p.e.];#sigma_{t} [ps]";
  title2 = ";threshold [# p.e.];#sigma_{DCR} [ps]";
  
  for(auto nPhE : nPhEs)
  {
    TCanvas* c = new TCanvas(Form("c1_tRes_vs_thr_nPhE%04d_allDCR",nPhE),Form("c1_tRes_vs_thr_nPhE%04d_allDCR",nPhE),2400,1200);
    c -> Divide(2,1);
    c -> cd(1);
    gPad -> SetLogx();
    gPad -> SetLogy();
    hPad1 = (TH1F*)( gPad->DrawFrame(xMin1,yMin1,xMax1,yMax1) );
    hPad1 -> SetTitle(title1.c_str());
    hPad1 -> Draw();
    gPad -> SetGridx();
    gPad -> SetGridy();
    c -> cd(2);
    gPad -> SetLogx();
    gPad -> SetLogy();
    hPad2 = (TH1F*)( gPad->DrawFrame(xMin2,yMin2,xMax2,yMax2) );
    hPad2 -> SetTitle(title2.c_str());
    hPad2 -> Draw();
    gPad -> SetGridx();
    gPad -> SetGridy();
    
    legend = new TLegend(0.75,0.90-0.04*DCRs.size(),0.99,0.90);
    legend -> SetFillColor(0);
    legend -> SetFillStyle(1000);  
    legend -> SetTextFont(42);
    legend -> SetTextSize(0.03);
    
    std::map<float,float> RMS_0DCR;
    int DCRIt = 0;
    for(auto DCR : DCRs)
    {
      TGraphErrors* g_tRes_LE = new TGraphErrors();
      TGraphErrors* g_tResDCR_LE = new TGraphErrors();

      TGraphErrors* g_tRes_NthPhE = new TGraphErrors();
      TGraphErrors* g_tResDCR_NthPhE = new TGraphErrors();
      
      TGraphErrors* g_tRes_avgNPhE = new TGraphErrors();
      TGraphErrors* g_tResDCR_avgNPhE = new TGraphErrors();
      
      for(auto thr : thrs)
      {
        if( thr >= nPhE ) continue;
        
        std::pair<int,float> dummy(nPhE,DCR);
        inFile = TFile::Open(inFileNames[dummy].c_str(),"READ");
        
        histo = (TH1F*)( inFile->Get(Form("h1_timeLE_thr%06.1fPhE",thr)) );
        histo -> GetXaxis() -> SetRangeUser(21.01,100.);
        if( histo->Integral() > 100 )
        {
          g_tRes_LE -> SetPoint(g_tRes_LE->GetN(),thr,1000.*histo->GetRMS());
          g_tRes_LE -> SetPointError(g_tRes_LE->GetN()-1,0.,1000.*histo->GetRMSError());
        }
        
        if( DCR == 0 ) RMS_0DCR[thr] = 1000.*histo->GetRMS();
        else if( histo->Integral() > 100 )
        {
          g_tResDCR_LE -> SetPoint(g_tResDCR_LE->GetN(),thr,sqrt(pow(1000.*histo->GetRMS(),2)-pow(RMS_0DCR[thr],2)));
          g_tResDCR_LE -> SetPointError(g_tResDCR_LE->GetN()-1,0.,sqrt(1000.*histo->GetRMSError()));
        }
        
        histo = (TH1F*)( inFile->Get(Form("h1_timeNthPhE_thr%06.1fPhE",thr)) );
        if( histo->Integral() > 100 )
        {
          g_tRes_NthPhE -> SetPoint(g_tRes_NthPhE->GetN(),thr,1000.*histo->GetRMS());
          g_tRes_NthPhE -> SetPointError(g_tRes_NthPhE->GetN()-1,0.,1000.*histo->GetRMSError());
        }
        
        histo = (TH1F*)( inFile->Get(Form("h1_timeAvgNPhE_thr%06.1fPhE",thr)) );
        if( histo->Integral() > 100 )
        {
          g_tRes_avgNPhE -> SetPoint(g_tRes_avgNPhE->GetN(),thr,1000.*histo->GetRMS());
          g_tRes_avgNPhE -> SetPointError(g_tRes_avgNPhE->GetN()-1,0.,1000.*histo->GetRMSError());
        }
        
        inFile -> Close();
      }

      c -> cd(1);
      g_tRes_LE -> SetMarkerSize(1.);
      g_tRes_LE -> SetMarkerColor(51+5*DCRIt);
      g_tRes_LE -> SetLineColor(51+5*DCRIt);
      if( DCR == 0 ) g_tRes_LE -> Draw("P,same");
      else           g_tRes_LE -> Draw("PL,same");
      legend -> AddEntry(g_tRes_LE,Form("DCR: %.2f GHz",DCR),"PL");

      TLatex* latexL;
      TLatex* latexR;
      
      if( DCR == 0 )
      {
        g_tRes_NthPhE -> SetLineColor(51+5*DCRIt);
        g_tRes_NthPhE -> SetLineStyle(7);
        g_tRes_NthPhE -> SetLineWidth(2);
        // g_tRes_NthPhE -> Draw("E0,L,same");
        
        g_tRes_avgNPhE -> SetLineColor(51+5*DCRIt);
        g_tRes_avgNPhE -> SetLineStyle(3);
        g_tRes_avgNPhE -> SetLineWidth(2);
        // g_tRes_avgNPhE -> Draw("E0,L,same");
        
        latexL = new TLatex(0.14,0.90,Form("#%d p.e.",nPhE));
        latexL -> SetNDC();
        latexL -> SetTextFont(42);
        latexL -> SetTextSize(0.04);
        latexL ->SetTextAlign(11);
        latexL -> Draw("same");
      
        latexR = new TLatex(0.80,0.96,Form("1 p.e. pulse shape: %s",inFileName_1pe.c_str()));
        latexR -> SetNDC();
        latexR -> SetTextFont(42);
        latexR -> SetTextSize(0.03);
        latexR ->SetTextAlign(31);
        latexR -> Draw("same");
        
        // f_30ps -> Draw("same");
        // TLatex* latex_30ps = new TLatex(xMin1,30.,"30 ps");
        // latex_30ps -> SetTextFont(42);
        // latex_30ps -> SetTextSize(0.02);
        // latex_30ps -> Draw("same");
        
        // f_50ps -> Draw("same");
        // TLatex* latex_50ps = new TLatex(xMin1,50.,"50 ps");
        // latex_50ps -> SetTextFont(42);
        // latex_50ps -> SetTextSize(0.02);
        // latex_50ps -> Draw("same");
        
        TF1* f_model = new TF1(Form("f_model%07.3GHz",DCR),"[0]/(x)^[1]",0.,100000);
        f_model -> SetParameters(100.,1.);
        f_model -> SetLineStyle(2);
        f_model -> SetLineWidth(1);
        f_model -> SetLineColor(51+5*DCRIt);
        g_tRes_LE -> Fit(f_model,"QNRS+");
        f_model -> Draw("same");
      }
      
      // TF1* f_model = new TF1(Form("f_model%07.3GHz",DCR),"1000.*sqrt( ([0]*sqrt([1]*[2])*[3]/x)^2 + (sqrt([4])*[3]/x)^2 )",0.,100000);
      // f_model -> SetParameters(kFactor,DCR,tau_LE,tau_S,thr);
      // f_model -> SetLineStyle(2);
      // f_model -> SetLineColor(51+5*DCRIt);
      // // f_model -> Draw("same");
      
      gPad -> Update();

      c -> cd(2);
      g_tResDCR_LE -> SetMarkerSize(0.7);
      g_tResDCR_LE -> SetMarkerColor(51+5*DCRIt);
      g_tResDCR_LE -> SetLineColor(51+5*DCRIt);
      g_tResDCR_LE -> Draw("PL");

      if( DCR == 0 )
      {
        latexL -> Draw("same");
        latexR -> Draw("same");
        
        // f_20ps -> Draw("same");
        // TLatex* latex_20ps = new TLatex(xMin1,20.,"20 ps");
        // latex_20ps -> SetTextFont(42);
        // latex_20ps -> SetTextSize(0.02);
        // latex_20ps -> Draw("same");
        
        TF1* f_model_DCR = new TF1(Form("f_model_DCR_DCR%07.3GHz",DCR),"1000.*[0]*sqrt([1]*[2])*[3]/x",0.,100000);
        f_model_DCR -> SetParameters(kFactor,DCR,tau_LE,tau_S);
        f_model_DCR -> SetLineStyle(2);
        f_model_DCR -> SetLineColor(51+5*DCRIt);
        // f_model_DCR -> Draw("same");
      }
      
      gPad -> Update();
      
      ++DCRIt;
    }

    c -> cd(1);
    legend -> Draw("same");
    c -> cd(2);
    legend -> Draw("same");
    
    c -> Print(Form("%s/c1_tRes_vs_thr_nPhE%04d_allDCR.png",plotDir.c_str(),nPhE));
  }
  */
}
