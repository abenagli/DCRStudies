float SPTR = 0.001;
int nToys = 10000;
//std::string pulseShape1pe = "dummySiPM";
std::string pulseShape1pe = "joao";

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
      inFileNames[dummy] = Form("plots/studyDCR_nPhE%05d_DCR%07.3fGHz_SPTR%.3fns_%s_1pe_baselineTracking21.780-21.980ns_nToys%d.root",nPhE,DCR,SPTR,pulseShape1pe.c_str(),nToys);
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
      f_model -> Draw("same");
      
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
      f_model_DCR -> Draw("same");
      
      gPad -> Update();
      
      ++DCRIt;
    }

    legend -> Draw("same");
    
    c -> Print(Form("%s/c1_tRes_vs_nPhE_thr%04d_allDCR.png",plotDir.c_str(),thr));
  }
}
