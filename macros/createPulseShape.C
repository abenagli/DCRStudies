double SiPM(double* x, double* par)
{
  double xx = x[0];
  double tau_r = par[0];
  double tau_d = par[1];
  double t0 = par[2];
  double norm = 0.00555 * (tau_d - tau_r) / ( pow(tau_d/tau_r,tau_r/(tau_r-tau_d)) - pow(tau_d/tau_r,tau_d/(tau_r-tau_d)) );
  if( (xx-t0) > 0. ) return( norm * ( exp(-1.*(xx-t0)/tau_d) - exp(-1.*(xx-t0)/tau_r) ) / (tau_d-tau_r) );
  else return 0.;
}



void createPulseShape()
{
  double xBinWidth = 0.020;
  
  double SiPM_tau_r = 0.2;
  double SiPM_tau_d = 10.;
  double SiPM_t0 = 22.;
  
  TF1* f_SiPM = new TF1("f_SiPM",SiPM,0.,200.,3);
  f_SiPM -> SetParameters(SiPM_tau_r,SiPM_tau_d,SiPM_t0);
  f_SiPM -> SetNpx(10000);
  f_SiPM -> Draw();

  std::ofstream outFile("data/dummySiPM_1pe.csv");
  
  for(double x = 0.; x < 200.; x+=xBinWidth)
  {
    outFile << std::scientific << x*1E-09 << "," << std::fixed << f_SiPM->Eval(x) << std::endl;
  }
}
