#include "interface/Discriminator.h"



float SubtractBaseline(const float& xMin, const float& xMax,
                       const int& nBins, float* xAxis, float* yAxis)
{
  float baseline = 0.;
  int nBaseline = 0;
  
  for(int ii = 0; ii < nBins; ++ii)
  {
    if( xAxis[ii] < xMin ) continue;
    if( xAxis[ii] > xMax ) break;

    baseline += yAxis[ii];
    ++nBaseline;
  }

  if( nBaseline > 0 )
  {
    baseline /= nBaseline;

    for(int ii = 0; ii < nBins; ++ii)
    {
      yAxis[ii] -= baseline;
    }

    return baseline;
  }
  
  else return 0.;
}



std::vector<float> GetTimeLE(const std::vector<float>& thrs,
                             const int& nBins, float* xAxis, float* yAxis,
                             const float& xStart)
{
  std::vector<float> result(thrs.size(),-999.);
  
  for(int ii = 0; ii < nBins; ++ii)
  {
    if( xStart != -999. && xAxis[ii] < xStart ) continue;
    
    for(unsigned int thrIt = 0; thrIt < thrs.size(); ++thrIt)
    {
      if( result.at(thrIt) != -999. ) continue;
      
      float thr = thrs.at(thrIt);
      if( yAxis[ii] > thr ) result.at(thrIt) = xAxis[ii];
    }
  }
  
  return result;
}
