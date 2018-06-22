float SPTR = 0.001;
int nToys = 1000;
//std::string pulseShape1pe = "dummySiPM";
//std::string pulseShape1pe = "joao";
std::string pulseShape1pe = "bipolar";

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

  

void drawDCRPlots()
{
  std::vector<float> DCRs;
  DCRs.push_back(0.05);
  DCRs.push_back(0.10);
  DCRs.push_back(0.30);
  DCRs.push_back(0.50);
  DCRs.push_back(1.00);
  DCRs.push_back(3.00);
  DCRs.push_back(5.00);
  DCRs.push_back(10.00);
  DCRs.push_back(30.00);
  DCRs.push_back(50.00);

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
  thrs.push_back(200);
  // thrs.push_back(300);
  // thrs.push_back(500);
  // thrs.push_back(1000);
  
  std::vector<int> nPhEs;
  nPhEs.push_back(4000);
  nPhEs.push_back(4500);
  nPhEs.push_back(5000);
  nPhEs.push_back(6000);
  nPhEs.push_back(7000);
  nPhEs.push_back(9000);
  nPhEs.push_back(18000);
  

  //--- common graphics
  TF1* f_20ps = new TF1("f_20ps","20.",-1.,100000.);
  f_20ps -> SetLineStyle(2);
  f_20ps -> SetLineColor(kBlack);
  
  TF1* f_30ps = new TF1("f_30ps","30.",-1.,100000.);
  f_30ps -> SetLineStyle(2);
  f_30ps -> SetLineColor(kBlack);
  
  TF1* f_50ps = new TF1("f_50ps","50.",-1.,100000.);
  f_50ps -> SetLineStyle(2);
  f_50ps -> SetLineColor(kBlack);

  
  //--- define inFile names
  std::map<std::pair<int,float>,std::string> inFileNames;
  for(auto nPhE : nPhEs)
  {
    for(auto DCR : DCRs)
    {
      std::pair<int,float> dummy(nPhE,DCR);
      // inFileNames[dummy] = Form("plots/studyDCR_nPhE%05d_DCR%07.3fGHz_SPTR%.3fns_%s_1pe_baselineTracking21.780-21.980ns_nToys%d.root",nPhE,DCR,SPTR,pulseShape1pe.c_str(),nToys);
      inFileNames[dummy] = Form("plots/studyDCR_nPhE%05d_DCR%07.3fGHz_SPTR%.3fns_%s_1pe_baselineTracking21.200-21.220ns_nToys%d.root",nPhE,DCR,SPTR,pulseShape1pe.c_str(),nToys);
    }
  }


  //--- define plot dir
  std::string plotDir = Form("plots/DCRPlots_SPTR%.3fns_%s_1pe",SPTR,pulseShape1pe.c_str());
  system(Form("mkdir %s",plotDir.c_str()));

  
  //--- plots vs. nPhE
  xMin1 = 3000.;
  xMax1 = 30000.;
  yMin1 = 10.;
  yMax1 = 1000.;
  xMin2 = 3000.;
  xMax2 = 30000.;
  yMin2 = 5.;
  yMax2 = 500.;
  title1 = ";LY#timesLCE#timesPDE [# p.e.];#sigma_{t} [ps]";
  title2 = ";LY#timesLCE#timesPDE [# p.e.];#sigma_{DCR} [ps]";
  
  for(auto thr : thrs)
  {
    TCanvas* c = new TCanvas(Form("c1_tRes_vs_nPhE_thr%04d_allDCR",thr),Form("c1_tRes_vs_nPhE_thr%04d_allDCR",thr),1200,600);
    c -> Divide(2,1);
    c -> cd(1);
    gPad -> SetLogx();
    gPad -> SetLogy();
    hPad1 = (TH1F*)( gPad->DrawFrame(xMin1,yMin1,xMax1,yMax1) );
    hPad1 -> SetTitle(title1.c_str());
    hPad1 -> Draw();
    c -> cd(2);
    gPad -> SetLogx();
    gPad -> SetLogy();
    hPad2 = (TH1F*)( gPad->DrawFrame(xMin2,yMin2,xMax2,yMax2) );
    hPad2 -> SetTitle(title2.c_str());
    hPad2 -> Draw();
    
    legend = new TLegend(0.81,0.95-0.04*DCRs.size(),0.99,0.95);
    legend -> SetFillColor(0);
    legend -> SetFillStyle(1000);  
    legend -> SetTextFont(42);
    legend -> SetTextSize(0.02);
    
    std::map<int,float> RMS_0DCR;
    int DCRIt = 0;
    for(auto DCR : DCRs)
    {
      TGraphErrors* g_tRes_LESig = new TGraphErrors();
      TGraphErrors* g_tResDCR_LESig = new TGraphErrors();
      
      for(auto nPhE : nPhEs)
      {
        std::pair<int,float> dummy(nPhE,DCR);
        inFile = TFile::Open(inFileNames[dummy].c_str(),"READ");
        
        histo = (TH1F*)( inFile->Get(Form("h1_timeLESig_thr%04dPhE",thr)) );
        histo -> GetXaxis() -> SetRangeUser(22.01,100.);
        g_tRes_LESig -> SetPoint(g_tRes_LESig->GetN(),nPhE,1000.*histo->GetRMS());
        g_tRes_LESig -> SetPointError(g_tRes_LESig->GetN()-1,0.,1000.*histo->GetRMSError());

        if( DCRIt == 0 ) RMS_0DCR[nPhE] = 1000.*histo->GetRMS();
        else
        {
          g_tResDCR_LESig -> SetPoint(g_tResDCR_LESig->GetN(),nPhE,sqrt(pow(1000.*histo->GetRMS(),2)-pow(RMS_0DCR[nPhE],2)));
          g_tResDCR_LESig -> SetPointError(g_tResDCR_LESig->GetN()-1,0.,sqrt(1000.*histo->GetRMSError()));
        }

        inFile -> Close();
      }

      c -> cd(1);
      g_tRes_LESig -> SetMarkerSize(0.7);
      g_tRes_LESig -> SetMarkerColor(51+5*DCRIt);
      g_tRes_LESig -> SetLineColor(51+5*DCRIt);
      g_tRes_LESig -> Draw("PL");
      legend -> AddEntry(g_tRes_LESig,Form("DCR: %.2f GHz",DCR),"PL");

      TLatex* latexL = new TLatex(0.14,0.96,Form("thr: %d p.e.",thr));
      latexL -> SetNDC();
      latexL -> SetTextFont(42);
      latexL -> SetTextSize(0.03);
      latexL ->SetTextAlign(11);
      latexL -> Draw("same");
      
      TLatex* latexR = new TLatex(0.80,0.96,Form("1 p.e. pulse shape: %s",pulseShape1pe.c_str()));
      latexR -> SetNDC();
      latexR -> SetTextFont(42);
      latexR -> SetTextSize(0.03);
      latexR ->SetTextAlign(31);
      latexR -> Draw("same");
      
      f_30ps -> Draw("same");
      TLatex* latex_30ps = new TLatex(xMin1,30.,"30 ps");
      latex_30ps -> SetTextFont(42);
      latex_30ps -> SetTextSize(0.02);
      latex_30ps -> Draw("same");
      
      f_50ps -> Draw("same");
      TLatex* latex_50ps = new TLatex(xMin1,50.,"50 ps");
      latex_50ps -> SetTextFont(42);
      latex_50ps -> SetTextSize(0.02);
      latex_50ps -> Draw("same");
      
      TF1* f_model = new TF1(Form("f_model%07.3GHz",DCR),"1000.*sqrt( ([0]*sqrt([1]*[2])*[3]/x)^2 + (sqrt([4])*[3]/x)^2 )",0.,100000);
      f_model -> SetParameters(kFactor,DCR,tau_LE,tau_S,thr);
      f_model -> SetLineStyle(2);
      f_model -> SetLineColor(51+5*DCRIt);
      // f_model -> Draw("same");
      
      gPad -> Update();

      c -> cd(2);
      g_tResDCR_LESig -> SetMarkerSize(0.7);
      g_tResDCR_LESig -> SetMarkerColor(51+5*DCRIt);
      g_tResDCR_LESig -> SetLineColor(51+5*DCRIt);
      g_tResDCR_LESig -> Draw("PL");
      
      latexL -> Draw("same");
      latexR -> Draw("same");
      
      f_20ps -> Draw("same");
      TLatex* latex_20ps = new TLatex(xMin1,20.,"20 ps");
      latex_20ps -> SetTextFont(42);
      latex_20ps -> SetTextSize(0.02);
      latex_20ps -> Draw("same");
      
      TF1* f_model_DCR = new TF1(Form("f_model_DCR_DCR%07.3GHz",DCR),"1000.*[0]*sqrt([1]*[2])*[3]/x",0.,100000);
      f_model_DCR -> SetParameters(kFactor,DCR,tau_LE,tau_S);
      f_model_DCR -> SetLineStyle(2);
      f_model_DCR -> SetLineColor(51+5*DCRIt);
      // f_model_DCR -> Draw("same");
      
      gPad -> Update();
      
      ++DCRIt;
    }

    c -> cd(1);
    legend -> Draw("same");
    c -> cd(2);
    legend -> Draw("same");
    
    c -> Print(Form("%s/c1_tRes_vs_nPhE_thr%04d_allDCR.png",plotDir.c_str(),thr));
  }
}






void drawDCRPlots_vsBaseline()
{
  std::vector<float> DCRs;
  // DCRs.push_back(0.05);
  // DCRs.push_back(0.10);
  DCRs.push_back(1.00);
  DCRs.push_back(3.00);
  DCRs.push_back(5.00);
  DCRs.push_back(10.00);
  DCRs.push_back(30.00);
  DCRs.push_back(50.00);
  
  std::vector<int> thrs;
  thrs.push_back(5);
  thrs.push_back(10);
  thrs.push_back(20);
  thrs.push_back(30);
  thrs.push_back(50);
  thrs.push_back(100);
  
  std::vector<int> nPhEs;
  nPhEs.push_back(9000);

  std::vector<std::pair<float,float> > baselines;
  baselines.push_back( std::make_pair(13.780,13.980) );
  baselines.push_back( std::make_pair(14.780,14.980) );
  baselines.push_back( std::make_pair(15.780,15.980) );
  baselines.push_back( std::make_pair(16.780,16.980) );
  baselines.push_back( std::make_pair(17.780,17.980) );
  baselines.push_back( std::make_pair(18.780,18.980) );
  baselines.push_back( std::make_pair(19.780,19.980) );
  baselines.push_back( std::make_pair(20.780,20.980) );
  baselines.push_back( std::make_pair(21.480,21.680) );
  baselines.push_back( std::make_pair(21.780,21.980) );

  
  //--- define inFile names
  std::map<std::pair<std::pair<int,float>,std::pair<float,float> >,std::string> inFileNames;
  for(auto nPhE : nPhEs)
  {
    for(auto DCR : DCRs)
    {
      for(auto baseline : baselines )
      {
        std::pair<int,float> dummy(nPhE,DCR);
        std::pair<std::pair<int,float>,std::pair<float,float> > dummy2(dummy,baseline);
        
        inFileNames[dummy2] = Form("plots/studyDCR_nPhE%05d_DCR%07.3fGHz_SPTR%.3fns_%s_1pe_baselineTracking%.3f-%.3fns_nToys%d.root",
                                   nPhE,DCR,SPTR,pulseShape1pe.c_str(),baseline.first,baseline.second,nToys);
      }
    }
  }
  

  //--- define plot dir
  std::string plotDir = Form("plots/DCRPlots_vsBaseline_SPTR%.3fns_%s_1pe",SPTR,pulseShape1pe.c_str());
  system(Form("mkdir %s",plotDir.c_str()));

  
  //--- plots vs. baseline
  xMin1 = -10.;
  xMax1 = 1.;
  yMin1 = 20.;
  yMax1 = 120.;
  xMin2 = -10.;
  xMax2 = 1.;
  yMin2 = -0.4;
  yMax2 = 0.3;
  title1 = ";baseline tracking start [ns];#sigma_{t} [ps]";
  title2 = ";baseline tracking start [ns];baseline [mV]";

  for(auto nPhE : nPhEs)
  {
    for(auto thr : thrs)
    {
      TCanvas* c = new TCanvas(Form("c1_tRes_vs_baseline_nPhE%04d_thr%04d",nPhE,thr),Form("c1_tRes_vs_baseline_nPhE%04d_thr%04d",nPhE,thr),1200,600);
      c -> Divide(2,1);
      c -> cd(1);
      // gPad -> SetLogx();
      // gPad -> SetLogy();
      gPad -> SetGridy();
      hPad1 = (TH1F*)( gPad->DrawFrame(xMin1,yMin1,xMax1,yMax1) );
      hPad1 -> SetTitle(title1.c_str());
      hPad1 -> Draw();
      c -> cd(2);
      // gPad -> SetLogx();
      // gPad -> SetLogy();
      hPad2 = (TH1F*)( gPad->DrawFrame(xMin2,yMin2,xMax2,yMax2) );
      hPad2 -> SetTitle(title2.c_str());
      hPad2 -> Draw();
      
      legend = new TLegend(0.81,0.95-0.04*DCRs.size(),0.99,0.95);
      legend -> SetFillColor(0);
      legend -> SetFillStyle(1000);  
      legend -> SetTextFont(42);
      legend -> SetTextSize(0.02);

      int DCRIt = 0;
      for(auto DCR : DCRs)
      {
        TGraphErrors* g_tRes_LESig = new TGraphErrors();
        TGraphErrors* g_tResDCR_LESig = new TGraphErrors();

        TGraphErrors* g_baseline = new TGraphErrors();
        
        for(auto baseline : baselines)
        {
          std::pair<int,float> dummy(nPhE,DCR);
          std::pair<std::pair<int,float>,std::pair<float,float> > dummy2(dummy,baseline);
          inFile = TFile::Open(inFileNames[dummy2].c_str(),"READ");
          
          histo = (TH1F*)( inFile->Get(Form("h1_timeLESig_thr%04dPhE",thr)) );
          histo -> GetXaxis() -> SetRangeUser(22.01,100.);
          g_tRes_LESig -> SetPoint(g_tRes_LESig->GetN(),baseline.second-22.,1000.*histo->GetRMS());
          g_tRes_LESig -> SetPointError(g_tRes_LESig->GetN()-1,0.,1000.*histo->GetRMSError());

          histo = (TH1F*)( inFile->Get("h_baseline") );
          g_baseline -> SetPoint(g_baseline->GetN(),baseline.second-22.,histo->GetMean());
          g_baseline -> SetPointError(g_baseline->GetN()-1,0.,histo->GetRMS());
          
          inFile -> Close();
        }
        
        c -> cd(1);
        g_tRes_LESig -> SetMarkerSize(0.7);
        g_tRes_LESig -> SetMarkerColor(51+8*DCRIt);
        g_tRes_LESig -> SetLineColor(51+8*DCRIt);
        g_tRes_LESig -> Draw("PL");
        legend -> AddEntry(g_tRes_LESig,Form("DCR: %.2f GHz",DCR),"PL");
        
        TLatex* latexL = new TLatex(0.14,0.96,Form("thr: %d p.e.",thr));
        latexL -> SetNDC();
        latexL -> SetTextFont(42);
        latexL -> SetTextSize(0.03);
        latexL ->SetTextAlign(11);
        latexL -> Draw("same");
        
        TLatex* latexR = new TLatex(0.80,0.96,Form("1 p.e. pulse shape: %s",pulseShape1pe.c_str()));
        latexR -> SetNDC();
        latexR -> SetTextFont(42);
        latexR -> SetTextSize(0.03);
        latexR ->SetTextAlign(31);
        latexR -> Draw("same");
        
        gPad -> Update();
        
        c -> cd(2);
        g_baseline -> SetMarkerSize(0.7);
        g_baseline -> SetMarkerColor(51+8*DCRIt);
        g_baseline -> SetLineColor(51+8*DCRIt);
        g_baseline -> Draw("PL");
        
        ++DCRIt;
      }
      
      c -> cd(1);
      legend -> Draw("same");
      c -> cd(2);
      legend -> Draw("same");
      gPad -> Update();
      
      c -> Print(Form("%s/c1_tRes_vs_baseline_nPhE%04d_thr%04d_allDCR.png",plotDir.c_str(),nPhE,thr));
    }
  }
}




void drawDCRPlots_vsRT()
{
  std::vector<float> DCRs;
  DCRs.push_back(0.00);
  
  std::vector<int> thrs;
  thrs.push_back(5);
  thrs.push_back(10);
  thrs.push_back(20);
  thrs.push_back(30);
  thrs.push_back(50);
  thrs.push_back(100);
  
  std::vector<int> nPhEs;
  nPhEs.push_back(4500);

  std::vector<std::pair<float,float> > RTs_DTs;
  RTs_DTs.push_back( std::make_pair(0.01,10.00) );
  RTs_DTs.push_back( std::make_pair(0.02,10.00) );
  RTs_DTs.push_back( std::make_pair(0.03,10.00) );
  RTs_DTs.push_back( std::make_pair(0.05,10.00) );
  RTs_DTs.push_back( std::make_pair(0.10,10.00) );
  RTs_DTs.push_back( std::make_pair(0.20,10.00) );
  RTs_DTs.push_back( std::make_pair(0.30,10.00) );
  RTs_DTs.push_back( std::make_pair(0.50,10.00) );
  RTs_DTs.push_back( std::make_pair(1.00,10.00) );
  RTs_DTs.push_back( std::make_pair(2.00,10.00) );
  RTs_DTs.push_back( std::make_pair(3.00,10.00) );
  RTs_DTs.push_back( std::make_pair(5.00,10.00) );
  
  
  //--- define inFile names
  std::map<std::pair<std::pair<int,float>,std::pair<float,float> >,std::string> inFileNames;
  for(auto nPhE : nPhEs)
  {
    for(auto DCR : DCRs)
    {
      for(auto RT_DT : RTs_DTs )
      {
        std::pair<int,float> dummy(nPhE,DCR);
        std::pair<std::pair<int,float>,std::pair<float,float> > dummy2(dummy,RT_DT);
        
        inFileNames[dummy2] = Form("plots/studyDCR_nPhE%05d_DCR%07.3fGHz_SPTR%.3fns_%s_%.2fnsRT_%.2fnsDT_1pe_baselineTracking%.3f-%.3fns_nToys%d.root",
                                   nPhE,DCR,SPTR,pulseShape1pe.c_str(),RT_DT.first,RT_DT.second,21.780,21.980,nToys);
      }
    }
  }
  

  //--- define plot dir
  std::string plotDir = Form("plots/DCRPlots_vsRT_SPTR%.3fns_%s_1pe",SPTR,pulseShape1pe.c_str());
  system(Form("mkdir %s",plotDir.c_str()));

  
  //--- plots vs. baseline
  xMin1 = -1.;
  xMax1 = 6.;
  yMin1 = 20.;
  yMax1 = 120.;
  xMin2 = -1.;
  xMax2 = 6.;
  yMin2 = 0.;
  yMax2 = 3.;
  title1 = ";rise time [ns];#sigma_{t} [ps]";
  title2 = ";rise time [ns];#LT t #GT [ns]";

  for(auto nPhE : nPhEs)
  {
    for(auto DCR : DCRs)
    {
      TCanvas* c = new TCanvas(Form("c1_tRes_vs_RT_nPhE%04d_DCR%07.3fGHz",nPhE,DCR),Form("c1_tRes_vs_RT_nPhE%04d_DCR%07.3fGHz",nPhE,DCR),1200,600);
      c -> Divide(2,1);
      c -> cd(1);
      // gPad -> SetLogx();
      // gPad -> SetLogy();
      gPad -> SetGridx();
      gPad -> SetGridy();
      hPad1 = (TH1F*)( gPad->DrawFrame(xMin1,yMin1,xMax1,yMax1) );
      hPad1 -> SetTitle(title1.c_str());
      hPad1 -> Draw();
      c -> cd(2);
      // gPad -> SetLogx();
      // gPad -> SetLogy();
      gPad -> SetGridx();
      gPad -> SetGridy();
      hPad2 = (TH1F*)( gPad->DrawFrame(xMin2,yMin2,xMax2,yMax2) );
      hPad2 -> SetTitle(title2.c_str());
      hPad2 -> Draw();
      
      legend = new TLegend(0.81,0.95-0.04*thrs.size(),0.99,0.95);
      legend -> SetFillColor(0);
      legend -> SetFillStyle(1000);  
      legend -> SetTextFont(42);
      legend -> SetTextSize(0.02);

      int thrIt = 0;
      for(auto thr : thrs)
      {
        TGraphErrors* g_tRes_LESig = new TGraphErrors();
        TGraphErrors* g_tAvg_LESig = new TGraphErrors();
        
        for(auto RT_DT : RTs_DTs)
        {
          std::pair<int,float> dummy(nPhE,DCR);
          std::pair<std::pair<int,float>,std::pair<float,float> > dummy2(dummy,RT_DT);
          inFile = TFile::Open(inFileNames[dummy2].c_str(),"READ");
          
          histo = (TH1F*)( inFile->Get(Form("h1_timeLESig_thr%04dPhE",thr)) );
          histo -> GetXaxis() -> SetRangeUser(22.01,100.);
          g_tRes_LESig -> SetPoint(g_tRes_LESig->GetN(),RT_DT.first,1000.*histo->GetRMS());
          g_tRes_LESig -> SetPointError(g_tRes_LESig->GetN()-1,0.,1000.*histo->GetRMSError());
          
          g_tAvg_LESig -> SetPoint(g_tAvg_LESig->GetN(),RT_DT.first,histo->GetMean()-22.);
          g_tAvg_LESig -> SetPointError(g_tAvg_LESig->GetN()-1,0.,histo->GetMeanError());
          
          inFile -> Close();
        }
        
        c -> cd(1);
        g_tRes_LESig -> SetMarkerSize(0.7);
        g_tRes_LESig -> SetMarkerColor(51+8*thrIt);
        g_tRes_LESig -> SetLineColor(51+8*thrIt);
        g_tRes_LESig -> Draw("PL");
        legend -> AddEntry(g_tRes_LESig,Form("thr: %d p.e.",thr),"PL");
        
        TLatex* latexL = new TLatex(0.14,0.96,Form("DCR: %.3f GHz",DCR));
        latexL -> SetNDC();
        latexL -> SetTextFont(42);
        latexL -> SetTextSize(0.03);
        latexL ->SetTextAlign(11);
        latexL -> Draw("same");
        
        TLatex* latexR = new TLatex(0.80,0.96,Form("1 p.e. pulse shape: %s",pulseShape1pe.c_str()));
        latexR -> SetNDC();
        latexR -> SetTextFont(42);
        latexR -> SetTextSize(0.03);
        latexR ->SetTextAlign(31);
        latexR -> Draw("same");
        
        gPad -> Update();
        
        c -> cd(2);
        g_tAvg_LESig -> SetMarkerSize(0.7);
        g_tAvg_LESig -> SetMarkerColor(51+8*thrIt);
        g_tAvg_LESig -> SetLineColor(51+8*thrIt);
        g_tAvg_LESig -> Draw("PL");
        
        ++thrIt;
      }
      
      c -> cd(1);
      legend -> Draw("same");
      c -> cd(2);
      legend -> Draw("same");
      gPad -> Update();
      
      c -> Print(Form("%s/c1_tRes_vs_RT_nPhE%04d_DCR%07.3fGHz_allDCR.png",plotDir.c_str(),nPhE,DCR));
    }
  }
}




void drawDCRPlots_vsDT()
{
  std::vector<float> DCRs;
  DCRs.push_back(0.00);
  
  std::vector<int> thrs;
  thrs.push_back(5);
  thrs.push_back(10);
  thrs.push_back(20);
  thrs.push_back(30);
  thrs.push_back(50);
  thrs.push_back(100);
  
  std::vector<int> nPhEs;
  nPhEs.push_back(4500);

  std::vector<std::pair<float,float> > RTs_DTs;
  // RTs_DTs.push_back( std::make_pair(0.01,0.10) );
  // RTs_DTs.push_back( std::make_pair(0.01,0.20) );
  // RTs_DTs.push_back( std::make_pair(0.01,0.30) );
  // RTs_DTs.push_back( std::make_pair(0.01,0.50) );
  RTs_DTs.push_back( std::make_pair(0.01,1.00) );
  RTs_DTs.push_back( std::make_pair(0.01,2.00) );
  RTs_DTs.push_back( std::make_pair(0.01,3.00) );
  RTs_DTs.push_back( std::make_pair(0.01,5.00) );
  RTs_DTs.push_back( std::make_pair(0.01,10.00) );
  RTs_DTs.push_back( std::make_pair(0.01,20.00) );
  RTs_DTs.push_back( std::make_pair(0.01,30.00) );
  RTs_DTs.push_back( std::make_pair(0.01,50.00) );  
  RTs_DTs.push_back( std::make_pair(0.01,100.00) );
  
  
  //--- define inFile names
  std::map<std::pair<std::pair<int,float>,std::pair<float,float> >,std::string> inFileNames;
  for(auto nPhE : nPhEs)
  {
    for(auto DCR : DCRs)
    {
      for(auto RT_DT : RTs_DTs )
      {
        std::pair<int,float> dummy(nPhE,DCR);
        std::pair<std::pair<int,float>,std::pair<float,float> > dummy2(dummy,RT_DT);
        
        inFileNames[dummy2] = Form("plots/studyDCR_nPhE%05d_DCR%07.3fGHz_SPTR%.3fns_%s_%.2fnsRT_%.2fnsDT_1pe_baselineTracking%.3f-%.3fns_nToys%d.root",
                                   nPhE,DCR,SPTR,pulseShape1pe.c_str(),RT_DT.first,RT_DT.second,21.780,21.980,nToys);
      }
    }
  }
  

  //--- define plot dir
  std::string plotDir = Form("plots/DCRPlots_vsDT_SPTR%.3fns_%s_1pe",SPTR,pulseShape1pe.c_str());
  system(Form("mkdir %s",plotDir.c_str()));

  
  //--- plots vs. baseline
  xMin1 = 0.;
  xMax1 = 110.;
  yMin1 = 20.;
  yMax1 = 120.;
  xMin2 = 0.;
  xMax2 = 110.;
  yMin2 = 0.;
  yMax2 = 3.;
  title1 = ";decay time [ns];#sigma_{t} [ps]";
  title2 = ";decay time [ns];#LT t #GT [ns]";

  for(auto nPhE : nPhEs)
  {
    for(auto DCR : DCRs)
    {
      TCanvas* c = new TCanvas(Form("c1_tRes_vs_DT_nPhE%04d_DCR%07.3fGHz",nPhE,DCR),Form("c1_tRes_vs_DT_nPhE%04d_DCR%07.3fGHz",nPhE,DCR),1200,600);
      c -> Divide(2,1);
      c -> cd(1);
      // gPad -> SetLogx();
      // gPad -> SetLogy();
      gPad -> SetGridx();
      gPad -> SetGridy();
      hPad1 = (TH1F*)( gPad->DrawFrame(xMin1,yMin1,xMax1,yMax1) );
      hPad1 -> SetTitle(title1.c_str());
      hPad1 -> Draw();
      c -> cd(2);
      // gPad -> SetLogx();
      // gPad -> SetLogy();
      gPad -> SetGridx();
      gPad -> SetGridy();
      hPad2 = (TH1F*)( gPad->DrawFrame(xMin2,yMin2,xMax2,yMax2) );
      hPad2 -> SetTitle(title2.c_str());
      hPad2 -> Draw();
      
      legend = new TLegend(0.81,0.95-0.04*thrs.size(),0.99,0.95);
      legend -> SetFillColor(0);
      legend -> SetFillStyle(1000);  
      legend -> SetTextFont(42);
      legend -> SetTextSize(0.02);

      int thrIt = 0;
      for(auto thr : thrs)
      {
        TGraphErrors* g_tRes_LESig = new TGraphErrors();
        TGraphErrors* g_tAvg_LESig = new TGraphErrors();
        
        for(auto RT_DT : RTs_DTs)
        {
          std::pair<int,float> dummy(nPhE,DCR);
          std::pair<std::pair<int,float>,std::pair<float,float> > dummy2(dummy,RT_DT);
          inFile = TFile::Open(inFileNames[dummy2].c_str(),"READ");
          
          histo = (TH1F*)( inFile->Get(Form("h1_timeLESig_thr%04dPhE",thr)) );
          histo -> GetXaxis() -> SetRangeUser(22.01,100.);
          g_tRes_LESig -> SetPoint(g_tRes_LESig->GetN(),RT_DT.second,1000.*histo->GetRMS());
          g_tRes_LESig -> SetPointError(g_tRes_LESig->GetN()-1,0.,1000.*histo->GetRMSError());
          
          g_tAvg_LESig -> SetPoint(g_tAvg_LESig->GetN(),RT_DT.second,histo->GetMean()-22.);
          g_tAvg_LESig -> SetPointError(g_tAvg_LESig->GetN()-1,0.,histo->GetMeanError());
          
          inFile -> Close();
        }
        
        c -> cd(1);
        g_tRes_LESig -> SetMarkerSize(0.7);
        g_tRes_LESig -> SetMarkerColor(51+8*thrIt);
        g_tRes_LESig -> SetLineColor(51+8*thrIt);
        g_tRes_LESig -> Draw("PL");
        legend -> AddEntry(g_tRes_LESig,Form("thr: %d p.e.",thr),"PL");
        
        TLatex* latexL = new TLatex(0.14,0.96,Form("DCR: %.3f GHz",DCR));
        latexL -> SetNDC();
        latexL -> SetTextFont(42);
        latexL -> SetTextSize(0.03);
        latexL ->SetTextAlign(11);
        latexL -> Draw("same");
        
        TLatex* latexR = new TLatex(0.80,0.96,Form("1 p.e. pulse shape: %s",pulseShape1pe.c_str()));
        latexR -> SetNDC();
        latexR -> SetTextFont(42);
        latexR -> SetTextSize(0.03);
        latexR ->SetTextAlign(31);
        latexR -> Draw("same");
        
        gPad -> Update();
        
        c -> cd(2);
        g_tAvg_LESig -> SetMarkerSize(0.7);
        g_tAvg_LESig -> SetMarkerColor(51+8*thrIt);
        g_tAvg_LESig -> SetLineColor(51+8*thrIt);
        g_tAvg_LESig -> Draw("PL");
        
        ++thrIt;
      }
      
      c -> cd(1);
      legend -> Draw("same");
      c -> cd(2);
      legend -> Draw("same");
      gPad -> Update();
      
      c -> Print(Form("%s/c1_tRes_vs_DT_nPhE%04d_DCR%07.3fGHz_allDCR.png",plotDir.c_str(),nPhE,DCR));
    }
  }
}
