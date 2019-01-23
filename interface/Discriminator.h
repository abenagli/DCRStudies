#ifndef Discriminator_h
#define Discriminator_h

#include <iostream>
#include <vector>
#include <cmath>
#include <utility>

#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TF1.h"



std::pair<float,float> SubtractBaseline(const float& xMin, const float& xMax,
                                        const int& nBins, float* xAxis, float* yAxis);

std::vector<float> GetTimeLE(const std::vector<float>& thrs,
                             const int& nBins, float* xAxis, float* yAxis,
                             const int& binStart = 1);

std::pair<float,float> GetTimeLEFit(const float& fraction, const int& nPointsL, const int& nPointsR,
                                    const float& xMin, const float& xMax, const float& noiseRMS,
                                    const int& nBins, float* xAxis, float* yAxis,
                                    const int& binStart = 1);

void ImplementCFD(const int& delay,
                  const int& nPoints, float* yAxis, float* yAxis_ref);

#endif
