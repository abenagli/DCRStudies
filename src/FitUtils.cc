#include "interface/FitUtils.h"



/*** find effective sigma ***/
void FindSmallestInterval(float* ret, TH1F* histo, const float& fraction,
                          const float& xMin)
{
  float integralMax = fraction * histo->Integral();
  
  // find first and last bin with non-null content
  int N = histo -> GetNbinsX();
  int M1 = 1;
  int M2 = 1;
  if( xMin != -999999. )
  {
    int M1 = histo->FindBin(xMin);
    M2 = M1;
  }
  for(int bin1 = 1; bin1 <= N; ++bin1)
  {
    if( histo->GetBinContent(bin1) > 0. && M1 == 1 ) M1 = bin1;
    if( histo->GetBinContent(bin1) > 0. )            M2 = bin1;
  }

  std::map<int,float> binCenters;
  std::map<int,float> binContents;
  std::map<int,float> binIntegrals;
  for(int bin1 = M1; bin1 <= M2; ++bin1)
  {
    if( histo->GetBinContent(bin1) == 0 ) continue;

    binCenters[bin1] = histo->GetBinCenter(bin1);
    binContents[bin1] = histo->GetBinContent(bin1);

    for(int bin2 = M1; bin2 <= bin1; ++bin2)
      binIntegrals[bin1] += histo->GetBinContent(bin2);
  }
  
  float min = 0.;
  float max = 0.;
  float delta = 999999.;
  for(std::map<int,float>::const_iterator mapIt1 = binIntegrals.begin(); mapIt1 != binIntegrals.end(); ++mapIt1)
  {
    for(std::map<int,float>::const_iterator mapIt2 = ++binIntegrals.begin(); mapIt2 != binIntegrals.end(); ++mapIt2)
    {
      if( (mapIt2->second-mapIt1->second) < integralMax ) continue;

      float tmpMin = binCenters[mapIt1->first];
      float tmpMax = binCenters[mapIt2->first];

      if( (tmpMax-tmpMin) < delta )
      {
        delta = (tmpMax - tmpMin);
        min = tmpMin;
        max = tmpMax;
      }

      break;
    }
  }
  
  TH1F* smallHisto = (TH1F*)( histo->Clone("smallHisto") );
  for(int bin = 1; bin <= smallHisto->GetNbinsX(); ++bin)
  {
    if( smallHisto->GetBinCenter(bin) < min )
      smallHisto -> SetBinContent(bin,0);

    if( smallHisto->GetBinCenter(bin) > max )
      smallHisto -> SetBinContent(bin,0);
  }
  smallHisto -> SetFillColor(kYellow);

  float mean = smallHisto -> GetMean();
  float meanErr = smallHisto -> GetMeanError();
  float RMS = smallHisto -> GetRMS();
  float RMSErr = smallHisto -> GetRMSError();

  ret[0] = mean;
  ret[1] = meanErr;
  ret[2] = RMS;
  ret[3] = RMSErr;
  ret[4] = min;
  ret[5] = max;
}



void drawCTRPlot(TH1F* histo, const std::string& label, const int& rebin, const int& isMCP0, const int& isMCP1, const float& MCPIntrinsic,
                 const std::string& title, TLatex* latexLabel,
                 TH1F* histo_center, const std::string& center_label, TH1F* histo_border, const std::string& border_label)
{
  float* vals = new float[6];
  FindSmallestInterval(vals,histo,0.68);
  histo -> Rebin(rebin);
  histo -> SetMarkerStyle(20);
  histo -> SetMarkerColor(kBlack);
  histo -> SetLineColor(kBlack);
  histo -> SetMaximum(1.5*histo->GetMaximum());
  histo -> SetTitle(title.c_str());
  histo -> Draw("PE");
  
  float norm = histo -> GetMaximum();
  float mean = vals[0];
  float min = vals[4];
  float max = vals[5];
  float delta = max-min;
  float sigma = 0.5*delta;
  float effSigma = sigma;
  min = min - 2.*delta;
  max = max + 2.*delta;
  
  histo -> GetXaxis() -> SetRangeUser(min,max);  
  
  TLatex* latex;
  if( (isMCP0 && !isMCP1) || (!isMCP0 && isMCP1) )
    latex = new TLatex(0.16,0.85,Form("#splitline{%s:}{#sigma_{single}^{eff} = %.1f ps}",
                                      label.c_str(),sqrt(effSigma*effSigma - MCPIntrinsic*MCPIntrinsic)*1000.));
  else
    latex = new TLatex(0.16,0.85,Form("#splitline{%s:}{#sigma_{single}^{eff} = %.1f ps}",label.c_str(),fabs(effSigma*1000)/sqrt(2)));
  latex -> SetNDC();
  latex -> SetTextFont(42);
  latex -> SetTextSize(0.05);
  latex -> SetTextColor(kBlack);
  latex -> Draw("same");
  
  // gaus fit
  std::string gausName(Form("fitFunc_gaus_corr"));
  TF1* fitFunc_gaus_corr = new TF1(gausName.c_str(),"[0]*exp(-1.*(x-[1])*(x-[1])/(2.*[2]*[2]))",mean-2.*sigma,mean+2.*sigma);
  fitFunc_gaus_corr -> SetNpx(10000);
  fitFunc_gaus_corr -> SetParameters(norm,mean,sigma);
  fitFunc_gaus_corr -> SetLineColor(kBlack);
  histo -> Fit(gausName.c_str(),"QNRSL");
  fitFunc_gaus_corr -> Draw("same");
  
  norm = fitFunc_gaus_corr -> GetParameter(0);
  mean = fitFunc_gaus_corr -> GetParameter(1);
  sigma = fitFunc_gaus_corr -> GetParameter(2);
  float sigmaErr = fitFunc_gaus_corr -> GetParError(2);
  
  TLatex* latex_gaus;
  if( (isMCP0 && !isMCP1) || (!isMCP0 && isMCP1) )
    latex_gaus = new TLatex(0.16,0.75,Form("#sigma_{single}^{gaus} = (%.1f #pm %.1f) ps",
                                           sqrt(sigma*sigma - MCPIntrinsic*MCPIntrinsic)*1000.,
                                           fabs(sigmaErr*1000)));
  else
    latex_gaus = new TLatex(0.16,0.75,Form("#sigma_{single}^{gaus} = (%.1f #pm %.1f) ps",fabs(sigma*1000)/sqrt(2),fabs(sigmaErr*1000)/sqrt(2)));
  latex_gaus -> SetNDC();
  latex_gaus -> SetTextFont(42);
  latex_gaus -> SetTextSize(0.03);
  latex_gaus -> SetTextColor(kBlack);
  // latex_gaus -> Draw("same");  
  
  
  if( histo_center)
  {
    vals = new float[6];
    FindSmallestInterval(vals,histo_center,0.68);
    histo_center -> Rebin(rebin);
    histo_center -> SetMarkerStyle(22);
    histo_center -> SetMarkerSize(0.7);
    histo_center -> SetMarkerColor(kRed);
    histo_center -> SetLineColor(kRed);
    histo_center -> Draw("PE,same");
    
    norm = histo -> GetMaximum();
    mean = vals[0];
    min = vals[4];
    max = vals[5];
    delta = max-min;
    sigma = 0.5*delta;
    effSigma = sigma;
    min = min - 2.*delta;
    max = max + 2.*delta;
    
    TLatex* latex_center;
    if( (isMCP0 && !isMCP1) || (!isMCP0 && isMCP1) )
      latex_center = new TLatex(0.75,0.40,Form("#splitline{%s:}{#sigma_{single}^{eff} = %.1f ps}",
                                               center_label.c_str(),sqrt(effSigma*effSigma - MCPIntrinsic*MCPIntrinsic)*1000.));
    else
      latex_center = new TLatex(0.75,0.40,Form("#splitline{%s:}{#sigma_{single}^{eff} = %.1f ps}",center_label.c_str(),fabs(effSigma*1000)/sqrt(2)));
    latex_center -> SetNDC();
    latex_center -> SetTextFont(42);
    latex_center -> SetTextSize(0.03);
    latex_center -> SetTextColor(kRed);
    latex_center -> Draw("same");  
  }
  
  
  if( histo_border)
  {
    vals = new float[6];
    FindSmallestInterval(vals,histo_border,0.68);
    histo_border -> Rebin(rebin);
    histo_border -> SetMarkerStyle(23);
    histo_border -> SetMarkerSize(0.7);
    histo_border -> SetMarkerColor(kBlue);
    histo_border -> SetLineColor(kBlue);
    histo_border -> Draw("PE,same");
    
    norm = histo -> GetMaximum();
    mean = vals[0];
    min = vals[4];
    max = vals[5];
    delta = max-min;
    sigma = 0.5*delta;
    effSigma = sigma;
    min = min - 2.*delta;
    max = max + 2.*delta;
    
    TLatex* latex_border;
    if( (isMCP0 && !isMCP1) || (!isMCP0 && isMCP1) )
      latex_border = new TLatex(0.75,0.30,Form("#splitline{%s:}{#sigma_{single}^{eff} = %.1f ps}",
                                               border_label.c_str(),sqrt(effSigma*effSigma - MCPIntrinsic*MCPIntrinsic)*1000.));
    else
      latex_border = new TLatex(0.75,0.30,Form("#splitline{%s:}{#sigma_{single}^{eff} = %.1f ps}",border_label.c_str(),fabs(effSigma*1000)/sqrt(2)));
    latex_border -> SetNDC();
    latex_border -> SetTextFont(42);
    latex_border -> SetTextSize(0.03);
    latex_border -> SetTextColor(kBlue);
    latex_border -> Draw("same");  
  }
  
  
  latexLabel -> Draw("same");
  
  
  gPad -> Update();
}
