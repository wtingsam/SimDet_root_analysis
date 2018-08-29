#include <vector>
#include <stdio.h>
#include <stdint.h>

#include "TChain.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
const bool IS_GEANTINO = true;

void printusage(char * name){
     printf("%s [InputFile]\n",name);
}
int main(int argc, char** argv){
     if (argc<2){
	  printusage(argv[0]);
	  return -1;
     }
     TString options = argv[1];
     TString outputFileName = argv[2];
     TString inputFileName = argv[3];
  
     // get input file
     TChain * iChain = new TChain("mc","mc");
     iChain->Add(inputFileName);
  
     std::vector<int>		*v_cdc_layerId = 0;
     std::vector<int>		*v_cdc_cellId  = 0;
     std::vector<double>	*v_cdc_edep    = 0;
     std::vector<double>	*v_cdc_DOCA    = 0;
     double	first_px = 0;
     double	first_py = 0;
     double	first_pz = 0;
     double	first_ix = 0;
     double	first_iy = 0;
     double	first_iz = 0;
     int trig = 0;
     int nTurns = 0;
     // Branch setting
     iChain->SetBranchAddress("trig", &trig);
     iChain->SetBranchAddress("nTurns", &nTurns);
     iChain->SetBranchAddress("cdc_layerId", &v_cdc_layerId);
     iChain->SetBranchAddress("cdc_cellId", &v_cdc_cellId);
     iChain->SetBranchAddress("cdc_edep", &v_cdc_edep);
     iChain->SetBranchAddress("cdc_DOCA", &v_cdc_DOCA);
     iChain->SetBranchAddress("first_px", &first_px);
     iChain->SetBranchAddress("first_py", &first_py);
     iChain->SetBranchAddress("first_pz", &first_pz);
     iChain->SetBranchAddress("first_x", &first_ix);
     iChain->SetBranchAddress("first_y", &first_iy);
     iChain->SetBranchAddress("first_z", &first_iz);
     //iChain->SetBranchAddress("cdc_trackId", &v_cdc_trackId);
     
     int nEntries = iChain->GetEntries();

     std::vector<int>		o_v_cdc_layerId ;
     std::vector<int>		o_v_cdc_cellId  ;
     std::vector<double>	o_v_cdc_edep    ;
     std::vector<double>	o_v_cdc_DOCA    ;
     int o_nturn;
     double o_pt;
     double o_pz;
     double o_ix;
     double o_iy;
     double o_iz;
     int o_iev;
     TFile *f_out(NULL);
     TTree *t_out(NULL);
     if(options.Contains("tree")){
	  f_out = new TFile("out.root","RECREATE");
	  t_out = new TTree("mc","combined hits");
	  t_out->Branch("iev",&o_iev);
	  t_out->Branch("pt",&o_pt);
	  t_out->Branch("pz",&o_pz);       
	  t_out->Branch("ix",&o_ix);
	  t_out->Branch("iy",&o_iy);
	  t_out->Branch("iz",&o_iz);
	  t_out->Branch("layer",&o_v_cdc_layerId);
	  t_out->Branch("cell",&o_v_cdc_cellId);
	  t_out->Branch("edep",&o_v_cdc_edep);
	  t_out->Branch("doca",&o_v_cdc_DOCA);
	  t_out->Branch("nturn",&o_nturn);
     }
     // Prepare output binary file
     FILE * bin_out(NULL);
     if(options.Contains("bin"))
	  bin_out = fopen("output.bin","wb");
     int16_t oint16;
     float   ofloat32;

     for(int  iev=0;iev<nEntries/10;iev++){
	  iChain->GetEntry(iev);
	  if(iev%10000==0) printf("%d/%d = %.4f \n",iev,nEntries, iev*1./nEntries);
	  if(!IS_GEANTINO && !trig) continue;
	  //if(nTurns==0)continue;
	  if(nTurns!=1)continue;

	  // Fill labels
	  o_pt = sqrt(first_px*first_px+first_py*first_py)*1000;
	  o_pz = first_pz*1000;
	  o_ix = first_ix;
	  o_iy = first_iy;
	  o_iz = first_iz;
	  o_nturn = nTurns;
	  
	  const int nhits = (int)v_cdc_layerId->size();
	  double min_r[20][500][2];
	  double total_edep[20][500] = {0}; 
	  for(int i=0;i<20;i++)
	       for(int j=0;j<500;j++){ 
		    min_r[i][j][0] = 1e5; // index 
		    min_r[i][j][1] = 1e5; // min R
	       }

	  for(int i=0;i<nhits;i++){
	       int lay = v_cdc_layerId->at(i);
	       int cel = v_cdc_cellId->at(i);
	       double dca = v_cdc_DOCA->at(i);
	       double edep = v_cdc_edep->at(i);
	       total_edep[lay][cel] += edep;
	       if(dca<min_r[lay][cel][1]){
		    min_r[lay][cel][1] = dca;
		    min_r[lay][cel][0] = i;
	       }	    
	  }
	  int ncombined = 0;
	  for(int i=0;i<nhits;i++){
	       int lay = v_cdc_layerId->at(i);
	       int cel = v_cdc_cellId->at(i);
	       int index = min_r[lay][cel][0] ;
	       if(index !=i) continue;
	       double dca = v_cdc_DOCA->at(i);
	       double edep = v_cdc_edep->at(i);
	       o_v_cdc_layerId.push_back(lay);
	       o_v_cdc_cellId.push_back(cel);
	       o_v_cdc_edep.push_back(edep);
	       o_v_cdc_DOCA.push_back(dca);
	       ncombined++;
	  }
	  if(options.Contains("tree"))
	       t_out->Fill();

	  // Output binary file
	  if(options.Contains("bin")){
	       oint16 = iev; fwrite(&oint16,sizeof(int16_t),1,bin_out);
	       oint16 = ncombined; fwrite(&oint16,sizeof(int16_t),1,bin_out);
	       ofloat32 = o_pt;fwrite(&ofloat32,sizeof(float),1,bin_out);
	       ofloat32 = o_pz;fwrite(&ofloat32,sizeof(float),1,bin_out);
	       ofloat32 = o_ix;fwrite(&ofloat32,sizeof(float),1,bin_out);
	       ofloat32 = o_iy;fwrite(&ofloat32,sizeof(float),1,bin_out);
	       ofloat32 = o_iz;fwrite(&ofloat32,sizeof(float),1,bin_out);
	       for(int i=0;i<(int)o_v_cdc_layerId.size();i++){
		    oint16 = o_v_cdc_layerId[i]; fwrite(&oint16,sizeof(int16_t),1,bin_out);
		    oint16 = o_v_cdc_cellId[i]; fwrite(&oint16,sizeof(int16_t),1,bin_out);
		    ofloat32 = o_v_cdc_edep[i];fwrite(&ofloat32,sizeof(float),1,bin_out);
		    ofloat32 = o_v_cdc_DOCA[i];fwrite(&ofloat32,sizeof(float),1,bin_out);
	       }
	  }
	  
	  // clear events
	  o_v_cdc_layerId.clear();
	  o_v_cdc_cellId.clear();
	  o_v_cdc_edep.clear();
	  o_v_cdc_DOCA.clear();
     }
     if(options.Contains("tree")){
	  t_out->Write();
	  f_out->Close();
     }
}
