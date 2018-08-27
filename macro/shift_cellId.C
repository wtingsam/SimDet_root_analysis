
void plot(){
	// Read files
	TFile *_file0 = TFile::Open("sig_100K_ana.0801.root");
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
	nEntries = mc->GetEntries();
	int n_trigged_evnts = 0;
	double charge[20][400] = {0};
	TH2D* hitMap = new TH2D("hitMap","hitMap(shifted)",400,0,400,20,0,20);
	TH1D* cell_diff = new TH1D("cell_diff","cell_diff",800,-400,400);
	int single_evnt = 9781;
	int n_read_evnt = 1;

	const int bound_cut = 150;
				
	//for(int iev=single_evnt;iev<single_evnt+n_read_evnt;iev++){
		//for(int iev=17195;iev<17195+1;iev++){
	for(int iev=0;iev<nEntries;iev++){
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
			/* printf("raw : %d %d \n",min_cell,max_cell); */
			/* printf("(0,150)   : %d %d \n",min_cell_0,max_cell_0); */
			/* printf("(150,300) : %d %d \n",min_cell_150,max_cell_150); */
			cell_diff -> Fill(max_cell-min_cell);
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
				if(hasBoundHit)
					hitMap -> Fill(new_cell,lay);
			}
			n_trigged_evnts++;
		}
	}
	TCanvas * c0 = new TCanvas("c0","c0",512,512);
	cell_diff->Draw();
	TCanvas * c1 = new TCanvas("c1","c1",512,512);
	hitMap->Draw("colz");
	printf("%% number triggered events %d \n",n_trigged_evnts);
}
