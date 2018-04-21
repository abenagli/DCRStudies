#ifndef graphFunc_h
#define graphFunc_h

#include "TGraph.h"



class graphFunc
{
 public:
  
  
  //! ctor
  graphFunc(TGraph* graph, const float& fineBinWidth)
  {
    graph_p = graph;
    nPoints_p = graph_p -> GetN();
    graph_p -> GetPoint(0,x0_p,y0_p);
    graph_p -> GetPoint(nPoints_p-1,xn_p,yn_p);
    
    fineBinWidth_p = fineBinWidth;
    nFinePoints_p = int( (xn_p-x0_p)/fineBinWidth_p );
    fineXaxis_p = new float[nFinePoints_p];
    fineYaxis_p = new float[nFinePoints_p];
    for(int ii = 0; ii < nFinePoints_p; ++ii)
    {
      fineXaxis_p[ii] = x0_p + ii*fineBinWidth_p;
      fineYaxis_p[ii] = graph_p -> Eval(fineXaxis_p[ii]);
    }
    fineGraph_p = new TGraph(nFinePoints_p,fineXaxis_p,fineYaxis_p);
  };
  
  
  //! dtor
  ~graphFunc()
  {};
  
  //! methods
  TGraph* GetFineGraph() { return fineGraph_p; };
  float* GetFineXaxis() { return fineXaxis_p; };
  float* GetFineYaxis() { return fineYaxis_p; };  
  int GetNFinePoints() { return nFinePoints_p; };

  
  //! operator()
  double operator()(double* x, double* par)
  {
    double xx = par[1] * (x[0] - par[2]);

    if( (xx < x0_p) || (xx >= xn_p) )
      return 1.e-10;
    
    else
      return par[0] * par[1] * graph_p->Eval(xx);
  }
  
  
  
 private:
  
  TGraph* graph_p;
  int nPoints_p;
  double x0_p;
  double y0_p;
  double xn_p;
  double yn_p;

  TGraph* fineGraph_p;
  float fineBinWidth_p;
  int nFinePoints_p;
  float* fineXaxis_p;
  float* fineYaxis_p;
};

#endif
