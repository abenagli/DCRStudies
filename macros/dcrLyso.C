void dcrLyso()
{
  TF1 *fb = new TF1("fb","27*sqrt(x)",0.1,100); // best fit
  fb->SetLineColor(kRed+2);
  TF1 *ff = new TF1("ff","19*sqrt(x)",0.1,100); // predicted
  ff->SetLineColor(kGreen+2);
  TF1 *fg = new TF1("ff","4*sqrt(x)",0.1,100); // predicted
  fg->SetLineStyle(3);

//   TGraphErrors *gg = new TGraphErrors();
//   gg->SetPoint(0,0.3,14.); gg->SetPointError(0,0,14./10);
//   gg->SetPoint(1,1,26); gg->SetPointError(1,0,26./10);
//   gg->SetPoint(2,3,47); gg->SetPointError(2,0,47./10.);
//   gg->SetPoint(3,10,76); gg->SetPointError(3,0,76/10.);
//   gg->SetPoint(4,30,130); gg->SetPointError(4,0,130/10.);
//   gg ->SetMarkerStyle(20);
//   gg -> SetMarkerColor(kGreen+1);
  
  TGraphErrors *gg2 = new TGraphErrors();
  gg2->SetPoint(0,0.3,14.66); gg2->SetPointError(0,0,13.66/32.);
  gg2->SetPoint(1,1,29.65); gg2->SetPointError(1,0,29.65/32.);
  gg2->SetPoint(2,3,46.05); gg2->SetPointError(2,0,46.05/32.);
  gg2->SetPoint(3,10,86.32); gg2->SetPointError(3,0,86.32/32.);
  gg2->SetPoint(4,30,149.8); gg2->SetPointError(4,0,149.8/32.);
  gg2->SetMarkerStyle(20);
  gg2->SetMarkerColor(kRed+1);
  
  TGraphErrors *gg3 = new TGraphErrors();
  gg3->SetPoint(0,0.3,11.38); gg3->SetPointError(0,0,11.38/32.);
  gg3->SetPoint(1,1,18.99); gg3->SetPointError(1,0,18.99/32.);
  gg3->SetPoint(2,3,31.32); gg3->SetPointError(2,0,31.32/32.);
  gg3->SetPoint(3,10,61.02); gg3->SetPointError(3,0,61.02/32.);
  gg3->SetPoint(4,30,91.38); gg3->SetPointError(4,0,91.38/32.);
  gg3->SetMarkerStyle(20);
  gg3->SetMarkerColor(kGreen+1);

  fb->Draw("");
  ff->Draw("same");
//   gg->Draw("P");
  gg2->Draw("P");
  gg3->Draw("P");
  //  ff->Draw("same");
  //  fg->Draw("same");

  TGraphErrors *g08 = new TGraphErrors();
  g08->SetPoint(0,30,130./3); g08->SetPointError(0,0,130/10./3.);
  TGraphErrors *g12 = new TGraphErrors();
  g12->SetPoint(0,30,130./6); g12->SetPointError(0,0,130/10./6.);
  TGraphErrors *g16 = new TGraphErrors();
  g16->SetPoint(0,30,130./8); g16->SetPointError(0,0,130/10./8.);
  g08->SetMarkerStyle(20); g08->SetMarkerColor(kYellow+1); 
  g12->SetMarkerStyle(20); g12->SetMarkerColor(kOrange+1); 
  g16->SetMarkerStyle(20); g16->SetMarkerColor(kOrange+1); 
  // g08->Draw("P");
  // g12->Draw("P");
  //  g16->Draw("P");
  
  TLatex *tl= new TLatex(0.2,130,"N_{pe} = 4000"); 
  tl->Draw("same");

  TLatex *tg3 = new TLatex (40,130./3,"x3");
  TLatex *tg6 = new TLatex (40,130./6,"x6");
  tg3 -> SetTextFont(41); 
  //  tg3->Draw();
  tg6 -> SetTextFont(41); 
  // tg6->Draw();

  /*
  TGraphErrors *g80 = new TGraphErrors();
  g80 -> SetPoint(0, 4000,82); g80->SetPointError(0, 0,86./10);   
  g80 -> SetPoint(1, 8000,41); g80->SetPointError(1, 0,41./10); 
  g80 -> SetPoint(2,12000,32); g80->SetPointError(2,0,32./10); 
  g80 -> SetPoint(3,16000,23); g80->SetPointError(3,0,23./10); 
  g80 -> SetPoint(4,24000,17); g80->SetPointError(4,0,17./10);   
  g80 -> SetPoint(5,32000,13); g80->SetPointError(5,0,13./10);
  
  g80 -> SetMarkerStyle(24); g80 -> SetMarkerColor(kBlue+1); 
  g80->Draw("same");
  */ 
  
  gPad -> SetLogx();
  gPad -> SetLogy();
}
