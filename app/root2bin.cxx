#include <vector>
#include <stdio.h>
#include <stdint.h>

#include "TChain.h"
#include "TString.h"

void printusage(char * name);

int main(int argc, char** argv){

  if (argc<2){
    printusage(argv[0]);
    return -1;
  }
  TString inputFileName = argv[1];
    
  // get input file
  TChain * iChain = new TChain("mc","mc");
  iChain->Add(inputFileName);
  int i_cth;
  int i_nHits;
  double i_cth_time;
  //int i_it;
  double i_ix;
  double i_iy;
  double i_iz;
  double i_ipx;
  double i_ipy;
  double i_ipz;
  std::vector<int> * i_layerID = 0;
  std::vector<int> * i_cellID = 0;
  //std::vector<double> * i_tstart = 0;
  std::vector<double> * i_edep = 0;
  std::vector<int> * i_iturn = 0;
  std::vector<double> * i_driftD = 0;
  std::vector<double> * i_t = 0;
  std::vector<double> * i_x = 0;
  std::vector<double> * i_y = 0;
  std::vector<double> * i_z = 0;
  std::vector<double> * i_px = 0;
  std::vector<double> * i_py = 0;
  std::vector<double> * i_pz = 0;
  iChain->SetBranchAddress("trig",&i_cth);
  //iChain->SetBranchAddress("cth_time",&i_cth_time);
  //iChain->SetBranchAddress("it",&i_it);
  iChain->SetBranchAddress("first_x",&i_ix);
  iChain->SetBranchAddress("first_y",&i_iy);
  iChain->SetBranchAddress("first_z",&i_iz);
  iChain->SetBranchAddress("first_px",&i_ipx);
  iChain->SetBranchAddress("first_py",&i_ipy);
  iChain->SetBranchAddress("first_pz",&i_ipz);
  iChain->SetBranchAddress("cdc_nHits",&i_nHits);
  iChain->SetBranchAddress("cdc_layerId",&i_layerID);
  iChain->SetBranchAddress("cdc_cellId",&i_cellID);
  iChain->SetBranchAddress("cdc_edep",&i_edep);
  iChain->SetBranchAddress("cdc_turnId",&i_iturn);
  iChain->SetBranchAddress("cdc_driftD",&i_driftD);
  iChain->SetBranchAddress("cdc_t",&i_t);
  iChain->SetBranchAddress("cdc_x",&i_x);
  iChain->SetBranchAddress("cdc_y",&i_y);
  iChain->SetBranchAddress("cdc_z",&i_z);
  iChain->SetBranchAddress("cdc_px",&i_px);
  iChain->SetBranchAddress("cdc_py",&i_py);
  iChain->SetBranchAddress("cdc_pz",&i_pz);

  // Prepare output binary file
  FILE * outputFile = fopen("output.bin","wb");
  int16_t oint16;
  float   ofloat32;

  // Loop in events
  int nEntries = iChain->GetEntries();

  for (int iEntry = 0; iEntry<nEntries; iEntry++){
    iChain->GetEntry(iEntry);
    if (i_cth!=1) continue;
    oint16 = iEntry;fwrite(&oint16,sizeof(int16_t),1,outputFile); // output iEntry
    // get number of hits
    int nHitsG = 0;
    for (int ihit = 0; ihit<i_nHits; ihit++){
      //if ((*i_tstart)[ihit]-i_cth_time<-25) continue;
      nHitsG++;
    }
    oint16 = nHitsG;fwrite(&oint16,sizeof(int16_t),1,outputFile); // output nHits
    for (int ihit = 0; ihit<i_nHits; ihit++){
      //if ((*i_tstart)[ihit]-i_cth_time<-25) continue;
      oint16 = (*i_layerID)[ihit];fwrite(&oint16,sizeof(int16_t),1,outputFile); // output layerID 
      oint16 = (*i_cellID)[ihit];fwrite(&oint16,sizeof(int16_t),1,outputFile); // output cellID
      //oint16 = (*i_tstart)[ihit]-i_cth_time;fwrite(&oint16,sizeof(int16_t),1,outputFile); // output driftT
      oint16 = (*i_edep)[ihit]*1e7>32767?32767:(*i_edep)[ihit]*1e7;fwrite(&oint16,sizeof(int16_t),1,outputFile); // output edep
      oint16 = (*i_iturn)[ihit];fwrite(&oint16,sizeof(int16_t),1,outputFile); // output turn #
      ofloat32 = (*i_driftD)[ihit];fwrite(&ofloat32,sizeof(float),1,outputFile); // output driftD
      //ofloat32 = (*i_t)[ihit]-i_it;fwrite(&ofloat32,sizeof(float),1,outputFile); // output tof
      ofloat32 = (*i_x)[ihit];fwrite(&ofloat32,sizeof(float),1,outputFile); // output x
      ofloat32 = (*i_y)[ihit];fwrite(&ofloat32,sizeof(float),1,outputFile); // output y
      ofloat32 = (*i_z)[ihit];fwrite(&ofloat32,sizeof(float),1,outputFile); // output z
      ofloat32 = (*i_px)[ihit];fwrite(&ofloat32,sizeof(float),1,outputFile); // output px
      ofloat32 = (*i_py)[ihit];fwrite(&ofloat32,sizeof(float),1,outputFile); // output py
      ofloat32 = (*i_pz)[ihit];fwrite(&ofloat32,sizeof(float),1,outputFile); // output pz
    }
    ofloat32 = i_ix;fwrite(&ofloat32,sizeof(float),1,outputFile); // output ix
    ofloat32 = i_iy;fwrite(&ofloat32,sizeof(float),1,outputFile); // output iy
    ofloat32 = i_iz;fwrite(&ofloat32,sizeof(float),1,outputFile); // output iz
    ofloat32 = i_ipx;fwrite(&ofloat32,sizeof(float),1,outputFile); // output ipx
    ofloat32 = i_ipy;fwrite(&ofloat32,sizeof(float),1,outputFile); // output ipy
    ofloat32 = i_ipz;fwrite(&ofloat32,sizeof(float),1,outputFile); // output ipz
  }

  fclose(outputFile);

  return 0;
}

void printusage(char * name){
  printf("%s [InputFile]\n",name);
}
