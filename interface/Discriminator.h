#ifndef Discriminator_h
#define Discriminator_h

#include <iostream>
#include <vector>
#include <cmath>
#include <utility>



std::pair<float,float> SubtractBaseline(const float& xMin, const float& xMax,
                                        const int& nBins, float* xAxis, float* yAxis);

std::vector<float> GetTimeLE(const std::vector<float>& thrs,
                             const int& nBins, float* xAxis, float* yAxis,
                             const int& binStart = 1);

void ImplementCFD(const int& delay,
                  const int& nBins, float* yAxis, float* yAxis_ref);

#endif
