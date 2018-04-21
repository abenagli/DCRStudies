double Scintillation(double* x, double* par)
{
  double xx = x[0];
  double tau_r = par[0];
  double tau_d = par[1];
  if( xx < 0. ) return 0.;
  return( ( exp(-1.*xx/tau_d) - exp(-1.*xx/tau_r) ) / (tau_d-tau_r) );
}


double SiPM(double* x, double* par)
{
  double xx = x[0];
  double tau_r = par[0];
  double tau_d = par[1];
  double t0 = par[2];
  double norm = (tau_d - tau_r) / ( pow(tau_d/tau_r,tau_r/(tau_r-tau_d)) - pow(tau_d/tau_r,tau_d/(tau_r-tau_d)) );
  if( (xx-t0) > 0. ) return( norm * ( exp(-1.*(xx-t0)/tau_d) - exp(-1.*(xx-t0)/tau_r) ) / (tau_d-tau_r) );
  else return 0.;
}



double Convolution(double* x, double* par)
{
  double xx = x[0];
  int nPhE = par[0];
  
  double val = 0.;
  for(int ii = 0; ii < nPhE; ++ii)
  {
    double* par_SiPM = new double[3];
    par_SiPM[0] = par[1+ii*3+0];
    par_SiPM[1] = par[1+ii*3+1];
    par_SiPM[2] = par[1+ii*3+2];
    val += SiPM(x,par_SiPM);
  }
  
  return val;
}



void dummyResolution()
{
  int nPhE = 4500;
  int nToys = 10000;
  double timeBin = 0.010;
  
  double tau_r = 0.080;
  double tau_d = 40.;
  
  double SiPM_tau_r = 1.;
  double SiPM_tau_d = 5.;
  
  
  //----
  TF1* f_scintillation = new TF1("f_scintillation",Scintillation,-50.,200.,2);
  f_scintillation -> SetParameters(tau_r,tau_d);
  f_scintillation -> SetNpx(10000);
  
  
  
  //----
  TF1* f_SiPM = new TF1("f_SiPM",SiPM,-50.,200.,2);
  f_SiPM -> SetParameters(SiPM_tau_r,SiPM_tau_d);
  f_SiPM -> SetNpx(10000);

  TCanvas* c = new TCanvas("c_pulses","c_pulses",1200,500);
  c -> Divide(2,1);
  c -> cd(1);
  f_scintillation -> Draw();
  c -> cd(2);
  f_SiPM -> Draw();
  
  
  
  //-----
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

  TFile* outFile = TFile::Open("dummyResolution.root","RECREATE");
  outFile -> cd();
  std::map<int,TH1F*> h_timeLE;
  for(int thrIt = 0; thrIt < thrs.size(); ++thrIt)
  {
    int thr = thrs.at(thrIt);
    h_timeLE[thr] = new TH1F(Form("h_timeLE_thr%d",thr),"",10000,0.,10.);
  }
  
  
  
  //----
  for(int iToy = 0; iToy < nToys; ++iToy)
  {
    std::cout << "processing iToy " << iToy << " / " << nToys << "\r" << std::flush;
    
    std::vector<double> times;
    for(int ii = 0; ii < nPhE; ++ii)
    {
      double time = f_scintillation->GetRandom();      
      times.push_back(time);
    }
    std::sort(times.begin(),times.end());
    
    
    for(int thrIt = 0; thrIt < thrs.size(); ++thrIt)
    {
      int thr = thrs.at(thrIt);
      h_timeLE[thr] -> Fill(times.at(thr));
    }
    
    // TF1* f_convolution = new TF1("f_convolution",Convolution,-50.,200.,1+3*times.size());
    // f_convolution -> SetParameter(0,nPhE);
    // for(int ii = 0; ii < nPhE; ++ii)
    // {
    //   f_convolution -> SetParameter(1+ii*3+0,SiPM_tau_r);
    //   f_convolution -> SetParameter(1+ii*3+1,SiPM_tau_d);
    //   f_convolution -> SetParameter(1+ii*3+2,times.at(ii));
    // }
    // f_convolution -> SetNpx(10000);

    // std::map<double,double> timesLE;
    // int step = 0;
    // while(1)
    // {
    //   double timeTemp = 0. + step*timeBin;
    //   if(timeTemp > 200. ) break;
      
    //   for(int thrIt = 0; thrIt < thrs.size(); ++thrIt)
    //   {
    //     double thr = thrs.at(thrIt);
    //     if( f_convolution->Eval(timeTemp) > thr && timesLE[thr] == 0. )
    //     {
    //       std::cout << "thr: " << thr << " time: " << timeTemp << "   ";
    //       timesLE[thr] = timeTemp;
    //       if( thrIt == thrs.size()-1 ) break;
    //     }
    //   }
      
    //   ++step;
    // }
    
    // delete f_convolution;
  }
  std::cout << std::endl;



  int bytes = outFile -> Write();
  std::cout << "============================================"  << std::endl;
  std::cout << "nr of  B written:  " << int(bytes)             << std::endl;
  std::cout << "nr of KB written:  " << int(bytes/1024.)       << std::endl;
  std::cout << "nr of MB written:  " << int(bytes/1024./1024.) << std::endl;
  std::cout << "============================================"  << std::endl;
}
