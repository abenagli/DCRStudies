double SiPM(double* x, double* par)
{
  double xx = x[0];
  
   //--- dummySiPM
   double tau_r = par[1];
   double tau_d = par[2];
   double t0 = par[3];
   if( (xx-t0) > 0. ) return( par[0] * ( exp(-1.*(xx-t0)/tau_d) - exp(-1.*(xx-t0)/tau_r) ) / (tau_d-tau_r) );
   else return 0.;
  
//   //--- step
//   double dur = par[0];
//   double t0 = par[1];
//   double norm = 0.00555;
//   if( (xx-t0) > 0. && (xx-t0) < dur ) return( norm );
//   else return 0.;
   
  // //--- bipolar 
  // float tau_r = par[1];
  // float tau_d = par[2];
  // float td = par[3];
  // float t0 = par[4];
  // if( (xx-t0) > 0.) return par[0] * (1./(tau_d-td)*(tau_d*exp(-(xx-t0)/td)-td*exp(-(xx-t0)/tau_d))-1/(tau_r-td)*(tau_r*exp(-(xx-t0)/td)-td*exp(-(xx-t0)/tau_r)));
  // else return 0.;
}



void createPulseShape()
{
  double xBinWidth = 0.005;
  
  //--- dummySiPM
  double SiPM_tau_r = 1.16;
  double SiPM_tau_d = 6.20;
  double SiPM_t0 = 22.;
  TF1* f_SiPM = new TF1("f_SiPM",SiPM,0.,200.,4);  
  f_SiPM -> SetParameters(1.,SiPM_tau_r,SiPM_tau_d,SiPM_t0);
  std::ofstream outFile(Form("data/dummySiPM_%.02fnsRT_%.02fnsnsDT_1pe.csv",SiPM_tau_r,SiPM_tau_d));
  
  // //--- step
  // double SiPM_tau_r = 0.2;
  // double SiPM_d = 40.;
  // double SiPM_t0 = 22.;
  // TF1* f_SiPM = new TF1("f_SiPM",SiPM,0.,200.,4);
  // f_SiPM -> SetParameters(1.,SiPM_d,SiPM_t0);
  // std::ofstream outFile(Form("data/step_1pe.csv"));
  
  //  //--- bipolar
  // double SiPM_tau_r =  1.1;
  // double SiPM_tau_d = 18.0;
  // double SiPM_td    = 1.0;
  // double SiPM_t0 = 22.;
  // TF1* f_SiPM = new TF1("f_SiPM",SiPM,0.,200.,5);
  // f_SiPM -> SetParameters(1.,SiPM_tau_r,SiPM_tau_d,SiPM_td,SiPM_t0);
  // std::ofstream outFile(Form("data/bipolar_1pe.csv"));

  f_SiPM -> SetNpx(10000);
  f_SiPM -> SetParameter(0,1./f_SiPM->GetMaximum());
  
  
  f_SiPM -> SetNpx(10000);
  f_SiPM -> Draw();
  
  double RT_low = -999.;
  double RT_hig = -999.;
  for(double x = 0.; x < 200.; x+=0.001)
  {
    if( f_SiPM->Eval(x) >= 0.10*1. && RT_low < 0 ) RT_low = x;
    if( f_SiPM->Eval(x) >= 0.90*1. && RT_hig < 0 ) RT_hig = x;
  }
  std::cout << "Rise time (10%-90%) = " << RT_hig - RT_low << " ns " << std::endl;
  
  double FT_low = -999.;
  double FT_hig = -999.;
  bool afterMax = false;
  for(double x = 0.; x < 200.; x+=0.001)
  {
    if( f_SiPM->Eval(x) >= 0.95*1. && !afterMax ) afterMax = true;
    if( !afterMax ) continue;
    
    if( f_SiPM->Eval(x) >= 0.90*1. && FT_low < 0 ) FT_low = x;
    if( f_SiPM->Eval(x) >= 0.10*1. ) FT_hig = x;
  }
  std::cout << "Fall time (90%-10%) = " << FT_hig - FT_low << " ns " << std::endl;
  
  for(double x = 0.; x < 200.; x+=xBinWidth)
    outFile << std::scientific << x*1E-09 << "," << std::fixed << f_SiPM->Eval(x) << std::endl;
}
