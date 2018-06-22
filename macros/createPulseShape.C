double SiPM(double* x, double* par)
{
   double xx = x[0];
   double tau_r = par[0];
   double tau_d = par[1];
   double t0 = par[2];
   double norm = 0.00555 * (tau_d - tau_r) / ( pow(tau_d/tau_r,tau_r/(tau_r-tau_d)) - pow(tau_d/tau_r,tau_d/(tau_r-tau_d)) );
   if( (xx-t0) > 0. ) return( norm * ( exp(-1.*(xx-t0)/tau_d) - exp(-1.*(xx-t0)/tau_r) ) / (tau_d-tau_r) );
   else return 0.;
  
//   double xx = x[0];
//   double dur = par[0];
//   double t0 = par[1];
//   double norm = 0.00555;
//   if( (xx-t0) > 0. && (xx-t0) < dur ) return( norm );
//   else return 0.;
}



void createPulseShape()
{
  double xBinWidth = 0.005;
  
  
  //--- dummySiPM
  double SiPM_tau_r = 0.2;
  double SiPM_tau_d = 10.;
  double SiPM_t0 = 22.;
  
  TF1* f_SiPM = new TF1("f_SiPM",SiPM,0.,200.,3);
  f_SiPM -> SetParameters(SiPM_tau_r,SiPM_tau_d,SiPM_t0);
  f_SiPM -> SetNpx(10000);
  f_SiPM -> Draw();
  
  std::ofstream outFile("data/dummySiPM_1pe.csv");
  
  
//--- dummyStep
//    double SiPM_d = 40.;
//    double SiPM_t0 = 22.;
  
//   TF1* f_SiPM = new TF1("f_SiPM",SiPM,0.,200.,3);
//   f_SiPM -> SetParameters(SiPM_d,SiPM_t0);
//   f_SiPM -> SetNpx(10000);
//   f_SiPM -> Draw();
   
//   std::ofstream outFile("data/dummyStep_1pe.csv");
   
  
  double RT_low = -999.;
  double RT_hig = -999.;
  for(double x = 0.; x < 200.; x+=0.001)
  {
    if( f_SiPM->Eval(x) >= 0.10*0.00555 && RT_low < 0 ) RT_low = x;
    if( f_SiPM->Eval(x) >= 0.90*0.00555 && RT_hig < 0 ) RT_hig = x;
  }
  std::cout << "Rise time (10%-90%) = " << RT_hig - RT_low << " ns " << std::endl;
  
  
  
  for(double x = 0.; x < 200.; x+=xBinWidth)
  {
    outFile << std::scientific << x*1E-09 << "," << std::fixed << f_SiPM->Eval(x) << std::endl;
  }
}
