#include <vector>
#include <stdio.h>
#include <stdint.h>

#include "TChain.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TMath.h"

#include "WireManager.hxx"
const bool DEBUG = false;
const bool IS_GEANTINO = true;
const double frame_min_X = -80; //cm
const double frame_min_Y = -80; //cm
const double frame_max_X = 80; //cm
const double frame_max_Y = 80; //cm
const double frame_range = 160; //cm
const int pixel_bin = 512;
const int pixel_bin_size = frame_range/pixel_bin;
void printusage(char * name){
     printf("%s [InputFile]\n",name);
}

void Rotation(double x1, double y1, double& x2, double& y2, double ang){
     x2 =  x1*TMath::Cos(ang)+y1*TMath::Sin(ang);
     y2 = -x1*TMath::Sin(ang)+y1*TMath::Cos(ang);
}

int main(int argc, char** argv){
     // Read wire mapping
     WireManager::Get().Initialize();
     
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
     std::vector<double>        *v_cdc_mc_x = 0; 
     std::vector<double>        *v_cdc_mc_y = 0; 
     std::vector<double>        *v_cdc_mc_z = 0; 

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
     iChain->SetBranchAddress("cdc_x", &v_cdc_mc_x);
     iChain->SetBranchAddress("cdc_y", &v_cdc_mc_y);
     iChain->SetBranchAddress("cdc_z", &v_cdc_mc_z);
     //iChain->SetBranchAddress("cdc_trackId", &v_cdc_trackId);
     
     int nEntries = iChain->GetEntries();

     std::vector<int>		o_v_cdc_layerId ;
     std::vector<int>		o_v_cdc_cellId  ;
     std::vector<double>	o_v_cdc_edep    ;
     std::vector<double>	o_v_cdc_DOCA    ;
     std::vector<double>	o_v_cdc_mc_x    ;
     std::vector<double>	o_v_cdc_mc_y    ;
     std::vector<double>	o_v_cdc_mc_z    ;
     std::vector<double>        o_v_cdc_mc_x0;
     std::vector<double>        o_v_cdc_mc_y0;
     std::vector<double>        o_v_cdc_mc_x1;
     std::vector<double>        o_v_cdc_mc_y1;
     std::vector<double>        o_v_cdc_mc_x2;
     std::vector<double>        o_v_cdc_mc_y2;

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
	  t_out->Branch("x",&o_v_cdc_mc_x);
	  t_out->Branch("y",&o_v_cdc_mc_y);
	  t_out->Branch("z",&o_v_cdc_mc_z);
	  t_out->Branch("wx0",&o_v_cdc_mc_x0);
	  t_out->Branch("wy0",&o_v_cdc_mc_y0);
	  t_out->Branch("wx1",&o_v_cdc_mc_x1);
	  t_out->Branch("wy1",&o_v_cdc_mc_y1);
	  t_out->Branch("wx2",&o_v_cdc_mc_x2);
	  t_out->Branch("wy2",&o_v_cdc_mc_y2);
	  t_out->Branch("layer",&o_v_cdc_layerId);
	  t_out->Branch("cell",&o_v_cdc_cellId);
	  t_out->Branch("edep",&o_v_cdc_edep);
	  t_out->Branch("doca",&o_v_cdc_DOCA);
	  t_out->Branch("nturn",&o_nturn);
     }

     TH2D *hEvntD = new TH2D("hEvntD","hEvntD",
			     pixel_bin,frame_min_X,frame_max_X,
			     pixel_bin,frame_min_Y,frame_max_Y);
     TGraph *gEvntD = new TGraph(1000);
     float odriftR32[pixel_bin][pixel_bin]={0};
     float oedep32[pixel_bin][pixel_bin]={0};
     
     FILE * bin_out(NULL);
     if(options.Contains("bin"))
	  bin_out = fopen("output.bin","wb");
     int16_t oint16;
     float   ofloat32;
     
     //for(int  iev=0;iev<3;iev++){
     for(int  iev=0;iev<nEntries/10;iev++){
	  for(int ipy=0;ipy<pixel_bin;ipy++)
	       for(int ipx=0;ipx<pixel_bin;ipx++){
		    odriftR32[ipx][ipy] = -99;
		    oedep32[ipx][ipy] = -99;
	       }
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
	       o_v_cdc_mc_x.push_back(v_cdc_mc_x->at(i));
	       o_v_cdc_mc_y.push_back(v_cdc_mc_y->at(i));
	       o_v_cdc_mc_z.push_back(v_cdc_mc_z->at(i));
	       double xe1 = WireManager::Get().GetSenseWireXPosHV(lay+1,cel);
	       double ye1 = WireManager::Get().GetSenseWireYPosHV(lay+1,cel);
	       double xe2 = WireManager::Get().GetSenseWireXPosRO(lay+1,cel);
	       double ye2 = WireManager::Get().GetSenseWireYPosRO(lay+1,cel);
	       double xe0 = (xe1+xe2)/2;
	       double ye0 = (ye1+ye2)/2;
	       if(DEBUG){
		    hEvntD->Fill(xe0,ye0);
		    gEvntD->SetPoint(ncombined,xe0,ye0);
	       }
	       for(int ipx=0;ipx<pixel_bin;ipx++){
		    for(int ipy=0;ipy<pixel_bin;ipy++){
			 double xx_min = frame_min_X + ipx*frame_range/(pixel_bin-1);
			 double xx_max = frame_min_X + (ipx+1)*frame_range/(pixel_bin-1);
			 double yy_min = frame_min_Y + ipy*frame_range/(pixel_bin-1);
			 double yy_max = frame_min_Y + (ipy+1)*frame_range/(pixel_bin-1);
			 //printf("(%f %f) %f (%f %f) %f ",xx_min,xx_max,xe0,yy_min,yy_max,ye0);
			 double rot_x=xe0;
			 double rot_y=ye0;
			 // const double pi=3.14159265359;
			 // if(ye0>=0 && xe0<=0) min_angle += pi; // region 2
			 // if(ye0<=0 && xe0<=0) min_angle -= pi; // region 3
			 // Rotation(xe0,ye0,rot_x,rot_y,0);// for now no rotation
			 //printf("(%f %f)->(%f %f) \n",xe0,ye0, rot_x,rot_y);
			 if(rot_x>=xx_min && rot_x<xx_max &&
			    rot_y>=yy_min && rot_y<yy_max){
			      odriftR32[ipx][ipy] = dca;
			      oedep32[ipx][ipy] = edep;
			 }else{
			      //printf("\n");
			 }
		    }
	       }
	       o_v_cdc_mc_x0.push_back(xe0);
	       o_v_cdc_mc_y0.push_back(ye0);
	       o_v_cdc_mc_x1.push_back(xe1);
	       o_v_cdc_mc_y1.push_back(ye1);
	       o_v_cdc_mc_x2.push_back(xe2);
	       o_v_cdc_mc_y2.push_back(ye2);
	       ncombined++;
	  }
	  if(options.Contains("tree"))
	       t_out->Fill();
	  
	  if(DEBUG){
	       for(int ipy=0;ipy<pixel_bin;ipy++){
		    for(int ipx=0;ipx<pixel_bin;ipx++){
			 if(fabs(odriftR32[ipx][ipy])<1e-3)
			      printf("xxx ",odriftR32[ipx][ipy]);
			 else
			      printf("%.1f ",odriftR32[ipx][ipy]);
		    }
		    printf("\n");
	       }          
	  }

	  // Output binary file
	  if(options.Contains("bin")){
	       oint16 = iev; fwrite(&oint16,sizeof(int16_t),1,bin_out);
	       oint16 = ncombined; fwrite(&oint16,sizeof(int16_t),1,bin_out);
	       ofloat32 = o_pt;fwrite(&ofloat32,sizeof(float),1,bin_out);
	       ofloat32 = o_pz;fwrite(&ofloat32,sizeof(float),1,bin_out);
	       ofloat32 = o_ix;fwrite(&ofloat32,sizeof(float),1,bin_out);
	       ofloat32 = o_iy;fwrite(&ofloat32,sizeof(float),1,bin_out);
	       ofloat32 = o_iz;fwrite(&ofloat32,sizeof(float),1,bin_out);
	       for(int ipy=0;ipy<pixel_bin;ipy++){
                    for(int ipx=0;ipx<pixel_bin;ipx++){
			 ofloat32 = odriftR32[ipx][ipy]; fwrite(&ofloat32,sizeof(float),1,bin_out);
		    }
	       }
	       for(int ipy=0;ipy<pixel_bin;ipy++){
                    for(int ipx=0;ipx<pixel_bin;ipx++){
			 ofloat32 = oedep32[ipx][ipy]; fwrite(&ofloat32,sizeof(float),1,bin_out);
		    }
	       }
	  }

	  // clear events
	  o_v_cdc_layerId.clear();
	  o_v_cdc_cellId.clear();
	  o_v_cdc_edep.clear();
	  o_v_cdc_DOCA.clear();
	  o_v_cdc_mc_x.clear();
	  o_v_cdc_mc_y.clear();
	  o_v_cdc_mc_z.clear();
	  o_v_cdc_mc_x0.clear();
	  o_v_cdc_mc_y0.clear();
	  o_v_cdc_mc_x1.clear();
	  o_v_cdc_mc_y1.clear();
	  o_v_cdc_mc_x2.clear();
	  o_v_cdc_mc_y2.clear();
     }
     
     if(DEBUG){
	  TCanvas *canvas = new TCanvas("evnt","evnt",512,512);
	  canvas->cd();
	  hEvntD->Draw();
	  gEvntD->Draw("same p");
	  gEvntD->SetMarkerStyle(8);
	  gEvntD->SetMarkerSize(0.1);
	  gEvntD->SetMarkerColor(2);
	  canvas->SaveAs("event.pdf");
     }
    if(options.Contains("tree")){
	  t_out->Write();
	  f_out->Close();
     }
}
