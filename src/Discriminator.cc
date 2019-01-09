#include "interface/Discriminator.h"



std::pair<float,float> SubtractBaseline(const float& xMin, const float& xMax,
                                        const int& nBins, float* xAxis, float* yAxis)
{
  float baseline = 0.;
  float baselineRMS = 0.;
  float sum = 0.;
  float sum2 = 0.;
  
  int nBaseline = 0;
  
  for(int ii = 0; ii < nBins; ++ii)
  {
    if( xAxis[ii] < xMin ) continue;
    if( xAxis[ii] > xMax ) break;

    baseline += yAxis[ii];
    sum += yAxis[ii];
    sum2 += yAxis[ii]*yAxis[ii];
    ++nBaseline;
  }

  if( nBaseline > 0 )
  {
    baseline /= nBaseline;
    baselineRMS = sqrt( sum2/nBaseline - pow(sum/nBaseline,2) );
    
    for(int ii = 0; ii < nBins; ++ii)
    {
      yAxis[ii] -= baseline;
    }
    
    return std::make_pair(baseline,baselineRMS);
  }
  
  else return std::make_pair(0.,0.);
}



std::vector<float> GetTimeLE(const std::vector<float>& thrs,
                             const int& nBins, float* xAxis, float* yAxis,
                             const int& binStart)
{
  std::vector<float> result(thrs.size(),-999.);
  
  for(int ii = binStart; ii < nBins; ++ii)
  {
    if( result.at(thrs.size()-1) > -100. ) break;
    
    for(unsigned int thrIt = 0; thrIt < thrs.size(); ++thrIt)
    {
      if( result.at(thrIt) > -100. ) continue;
      
      float thr = thrs.at(thrIt);
      if( yAxis[ii] > thr ) result.at(thrIt) = xAxis[ii];
    }
  }
  
  return result;
}



void ImplementCFD(const int& delay,
                  const int& nBins, float* yAxis, float* yAxis_ref)
{
  for(int ii = 0; ii < nBins; ++ii)
  {
    if( (ii-delay) >= 0 && (ii-delay) < nBins )
      yAxis[ii] = yAxis[ii]-yAxis_ref[ii-delay];
  }

  return;
}
