void plot(TString filename, int single_evnt=0){
  // Read files
  TFile *_file0 = TFile::Open(filename);
  TTree *mc = (TTree*)_file0->Get("mc");
  std::vector<int> *v_cdc_layerId = 0;
  std::vector<int> *v_cdc_cellId = 0;
  std::vector<int> *v_cdc_edep = 0;
  std::vector<int> *v_cdc_trackId = 0;
  int trig = 0;
  // Branch setting
  mc->SetBranchAddress("trig", &trig);
  mc->SetBranchAddress("cdc_layerId", &v_cdc_layerId);
  mc->SetBranchAddress("cdc_cellId", &v_cdc_cellId);
  mc->SetBranchAddress("cdc_edep", &v_cdc_edep);
  mc->SetBranchAddress("cdc_trackId", &v_cdc_trackId);
  int nEntries = mc->GetEntries();
  int n_trigged_evnts = 0;
  double charge[20][400] = {0};
  TH2D* hitMapn = new TH2D("hitMap","hitMap         ",400,0,400,20,0,20);
  TH2D* hitMaps = new TH2D("hitMap","hitMap(shifted)",400,0,400,20,0,20);
  TH1D* cell_diff = new TH1D("cell_diff","cell_diff",800,-400,400);
  TH1D* cell_150_min_max = new TH1D("cell_150_min_max","cell_150_min_max",800,-400,400);
  TH1D* cell_0_min_max = new TH1D("cell_0_min_max","cell_0_min_max",800,-400,400);
  
  int n_read_evnt = 1;
  if(single_evnt==0){
    n_read_evnt=nEntries;
  }
  const int bound_cut = 140;
  
  for(int iev=single_evnt;iev<single_evnt+n_read_evnt;iev++){
    mc->GetEntry(iev);
    if(trig){
      bool hasBoundHit = false;
      // Pre processing
      int min_cell = 1e5;
      int max_cell = -1e5;
      for(int idx=0;idx<(int)v_cdc_layerId->size();idx++){
	int trackId = v_cdc_trackId->at(idx);
	if(trackId!=1) continue;
	double edep = v_cdc_edep->at(idx);
	int lay = v_cdc_layerId->at(idx);
	int cel = v_cdc_cellId->at(idx);
	if(cel<min_cell) min_cell = cel;
	if(cel>max_cell) max_cell = cel;
      }
      if(max_cell-min_cell>bound_cut) hasBoundHit = true;

      int min_cell_0 = 1e5;
      int max_cell_0 = -1e5;
      int min_cell_150 = 1e5;
      int max_cell_150 = -1e5;
      // Get sections min and max
      if(hasBoundHit){
	for(int idx=0;idx<(int)v_cdc_layerId->size();idx++){
	  int trackId = v_cdc_trackId->at(idx);
	  if(trackId!=1) continue;
	  double edep = v_cdc_edep->at(idx);
	  int lay = v_cdc_layerId->at(idx);
	  int cel = v_cdc_cellId->at(idx);
	  if(cel>bound_cut && cel<min_cell_150) min_cell_150 = cel;
	  if(cel>bound_cut && cel>max_cell_150) max_cell_150 = cel;
	  if(cel<=bound_cut && cel<min_cell_0) min_cell_0 = cel;
	  if(cel<=bound_cut && cel>max_cell_0) max_cell_0 = cel;
	}
      }
      // printf("raw : %d %d \n",min_cell,max_cell);
      // printf("(0,150)   : %d %d \n",min_cell_0,max_cell_0);
      // printf("(150,300) : %d %d \n",min_cell_150,max_cell_150);
      cell_diff -> Fill(max_cell-min_cell);
      
      //if(hasBoundHit) printf("iev %d \n",iev);;
      for(int idx=0;idx<(int)v_cdc_layerId->size();idx++){
	int trackId = v_cdc_trackId->at(idx);
	if(trackId!=1) continue;
	double edep = v_cdc_edep->at(idx);
	int lay = v_cdc_layerId->at(idx);
	int cel = v_cdc_cellId->at(idx);
	//if(n_trigged_evnts<){
	//printf("%d %d \n",lay,cel);
	int new_cell = cel-min_cell;
	if(hasBoundHit && cel>bound_cut)  new_cell = cel-min_cell_150;
	if(hasBoundHit && cel<=bound_cut) new_cell = cel+max_cell_150-min_cell_150;
	if(new_cell>150)
	  printf("%d\n",iev);

	hitMapn -> Fill(cel,lay);
	hitMaps -> Fill(new_cell,lay);
	if(hasBoundHit){
	  cell_150_min_max->Fill(min_cell_150);
	  cell_150_min_max->Fill(max_cell_150);
	  cell_0_min_max->Fill(min_cell_0);
	  cell_0_min_max->Fill(max_cell_0);
	}
      }
      n_trigged_evnts++;
    }
  }
  TCanvas * c0 = new TCanvas("c0","c0",512,512);
  cell_diff->Draw();
  TCanvas * c1 = new TCanvas("c1","c1",1024,512);
  c1->Divide(2,1);
  c1->cd(1);
  hitMapn->Draw("colz");
  c1->cd(2);
  hitMaps->Draw("colz");
  printf("%% number triggered events %d \n",n_trigged_evnts);
  TCanvas * c2 = new TCanvas("c2","c2",1024,512);
  c2->Divide(2,1);
  c2->cd(1);
  cell_0_min_max->Draw();
  c2->cd(2);
  cell_150_min_max->Draw();
}
