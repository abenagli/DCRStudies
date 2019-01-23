void drawPulseShape(const std::string& inFileName1, const float& shift1,
                    const std::string& inFileName2="NULL", const float& shift2 = 0.,
                    const std::string& inFileName3="NULL",
                    const std::string& inFileName4="NULL")
{
  TGraph* g_ps1 = new TGraph(inFileName1.c_str(),"%le,%lf");
  
  double x,y;
  double y0 = 0;
  double yMax = -999.;
  for(int point = 0; point < g_ps1->GetN(); ++point)
  {
    g_ps1 -> GetPoint(point,x,y);
    if( point == 0 ) y0 = y;
    if( y > yMax ) yMax = y;
  }
  for(int point = 0; point < g_ps1->GetN(); ++point)
  {
    g_ps1 -> GetPoint(point,x,y);
    g_ps1 -> SetPoint(point,1.E09*x-shift1,(y-y0)/(yMax-y0));
  }
  g_ps1 -> Draw("APL");
  


  if( inFileName2 != "NULL" )
  {
    TGraph* g_ps2 = new TGraph(inFileName2.c_str(),"%le,%lf");
    
    y0 = 0;
    yMax = -999.;
    for(int point = 0; point < g_ps2->GetN(); ++point)
    {
      g_ps2 -> GetPoint(point,x,y);
      if( point == 0 ) y0 = y;
      if( y > yMax ) yMax = y;
    }
    for(int point = 0; point < g_ps2->GetN(); ++point)
    {
      g_ps2 -> GetPoint(point,x,y);
      g_ps2 -> SetPoint(point,1.E09*x-shift2,(y-y0)/(yMax-y0));
    }

    g_ps2 -> SetMarkerColor(kRed);
    g_ps2 -> SetLineColor(kRed);
    g_ps2 -> Draw("PL,same");
  }
  
  /*
  float xBinWidth = 0.002;
  float xMin = -100.;
  float xMax = +200.;
  int overSampling = 1;
  int nPoints = (xMax-xMin) / xBinWidth;
  int nFinePoints = nPoints;
  int globalShift = int( ( (0.-xMin) / xBinWidth ) + 0.1*xBinWidth );
  float* xAxis = new float[nPoints];
  for(int point = 0; point < nPoints; ++point)
  {
    xAxis[point] = xMin + point*xBinWidth;
  }
  
  
  //--------------
  // get 1pe shape
  std::string inFileName_1pe = "tofhir_1pe";
  TGraph* g_ps_1pe = NULL;
  TGraph* g_ps_1pe_baseSub = NULL;

  double x0,y0;
  double x,y;
  
  g_ps_1pe = new TGraph(("data/"+inFileName_1pe+".csv").c_str(),"%le,%lf");
  g_ps_1pe_baseSub = new TGraph();
  g_ps_1pe -> GetPoint(0,x0,y0);
  for(int point = 0; point < g_ps_1pe->GetN(); ++point)
  {
    g_ps_1pe -> GetPoint(point,x,y);
    g_ps_1pe_baseSub -> SetPoint(point,1.E09*x,y-y0);
  }
  
  float* yAxis_1PhE = new float[nPoints];
  for(int point = 0; point < nPoints; ++point)
  {
    yAxis_1PhE[point] = 0.;
  }
  
  
  float max_1pe = -999.;
  int shift = 1.*overSampling*globalShift;
  for(int jj = 0; jj < nPoints; ++jj)
  {
    if( (overSampling*jj-shift) >= 0 && (overSampling*jj-shift) < nFinePoints )
    {
      yAxis_1PhE[jj] = ( g_ps_1pe->Eval(overSampling*jj-shift) );
      
      if( yAxis_1PhE[jj] > max_1pe ) max_1pe = yAxis_1PhE[jj];
    }
  }
  std::cout << "max_1pe: " << max_1pe << std::endl;
  
  

  TGraph* g_ps_1PhE = new TGraph(nPoints,xAxis,yAxis_1PhE);
  g_ps_1PhE -> SetMarkerSize(0.2);
  g_ps_1PhE -> SetMarkerColor(kBlack);
  g_ps_1PhE -> SetLineColor(kBlack);
  g_ps_1PhE -> Draw("APL");
  /*
  //---------
  // run toys
  float* yAxis_sumNPhE = new float[nPoints];
  
  for(int point = 0; point < nPoints; ++point)
  {
    yAxis_sumNPhE[point] = 0.;
  }
  
  
  //--- signal ph.e.
  std::vector<double> times;
  for(int ii = 0; ii < int(nPhE); ++ii)
  {
    float time = f_scintillation->GetRandom() + r.Gaus(0.,SPTR);
    times.push_back(time);
    
    int shift = 1.*overSampling*globalShift + (time / (1.*xBinWidth/overSampling) );
    for(int jj = 0; jj < nPoints; ++jj)
    {
      if( (overSampling*jj-shift) >= 0 && (overSampling*jj-shift) < nFinePoints )
      {
        yAxis_sumNPhE[jj]         += ( hf_psFine_1pe_baseSub[overSampling*jj-shift] );
      }
    }
  }
  */
}
