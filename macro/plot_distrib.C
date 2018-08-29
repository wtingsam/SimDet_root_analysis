void plot(TString filename){
  TFile *_file0 = TFile::Open(filename);
  TCanvas *canvas = new TCanvas("c","CDC",1024,720);
  canvas->Divide(4,2);
  TTree *mc  = (TTree*)_file0->Get("mc");
  canvas->cd(1);
  mc->Draw("log(cdc_edep*1e9)","!trig && nTurns>0");
  canvas->cd(2);
  mc->Draw("cdc_driftD;Name","!trig && cdc_driftD>-100 && cdc_driftD<5000 && nTurns>0");
  canvas->cd(3);
  mc->Draw("cdc_nHits","!trig && cdc_driftD>-100 && cdc_driftD<5000 && nTurns>0");
  canvas->cd(4);
  mc->Draw("first_px","!trig && nTurns>0");
  canvas->cd(5);  
  mc->Draw("first_py","!trig && nTurns>0");
  canvas->cd(6);
  mc->Draw("first_pz","!trig && nTurns>0");
  canvas->cd(7);
  mc->Draw("sqrt(first_px**2+first_py**2):first_pz","!trig && nTurns>0 ","colz");
  canvas->cd(8);
  mc->Draw("first_z:first_pz","!trig&& nTurns>0","colz");
  
}
