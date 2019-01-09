#include <iostream>
#include <string>
#include <map>

#include "TH1F.h"
#include "TF1.h"
#include "TPad.h"
#include "TLatex.h"



/*** find effective sigma ***/
void FindSmallestInterval(float* ret, TH1F* histo, const float& fraction,
                          const float& xMin = -999999.);

/*** draw time resolution plot ***/
void drawCTRPlot(TH1F* histo, const std::string& label, const int& rebin, const int& isMCP0, const int& isMCP1, const float& MCPIntrinsic,
                 const std::string& title, TLatex* latexLabel,
                 TH1F* histo_center, const std::string& center_label, TH1F* histo_border, const std::string& border_label);
