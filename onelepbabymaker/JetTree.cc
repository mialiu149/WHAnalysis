#include "JetTree.h"
#include "CMS3.h"
#include "JetSelections.h"
#include "btagsf/BTagCalibrationStandalone.h"
#include "../analysisutils/histTools.h"

using namespace tas;
 
JetTree::JetTree (){}

JetTree::JetTree (const std::string &prefix) : prefix_(prefix) {}

inline bool sortIndexbyCSV( pair<int, float> &vec1, pair<int, float> &vec2 ) {
    return vec1.second > vec2.second;
}

float getmbb(LorentzVector jet1,LorentzVector jet2) {
    float mbb = (jet1+jet2).mass(); 
    return mbb;
}

float getmct(LorentzVector jet1,LorentzVector jet2) {
    float ptb1 = jet1.pt();
    float ptb2 = jet2.pt();
    float phib1 = jet1.phi();
    float phib2 = jet2.phi();
    float dPhibb = getdphi(phib1,phib2);
    float mct = sqrt(2*ptb1*ptb2*(1+cos(dPhibb)));
    return mct;
}

void JetTree::InitBtagSFTool(bool isFastsim_) {
    //From Nick Amin:
    //and the latest btag SFs (inclusive + era dependent) in [2] https://github.com/cmstas/CORE/commit/3caca1e2d351125a4ac1fa779736b836c1db4a43

    char* jecpath;
    jecpath = getenv ("TOOLSPATH");
    cout<<Form("%s/btagsf/data/run2_25ns/DeepCSV_94XSF_V2_B_F.csv", jecpath)<<endl;
    calib = new BTagCalibration("deepCSV", Form("%s/btagsf/data/run2_25ns/DeepCSV_94XSF_V2_B_F.csv", jecpath));

    reader_heavy = new BTagCalibrationReader(calib, BTagEntry::OP_MEDIUM, "comb", "central"); // central
    reader_heavy_UP = new BTagCalibrationReader(calib, BTagEntry::OP_MEDIUM, "comb", "up");  // sys up
    reader_heavy_DN = new BTagCalibrationReader(calib, BTagEntry::OP_MEDIUM, "comb", "down");  // sys down
    reader_light = new BTagCalibrationReader(calib, BTagEntry::OP_MEDIUM, "incl", "central");  // central
    reader_light_UP = new BTagCalibrationReader(calib, BTagEntry::OP_MEDIUM, "incl", "up");  // sys up
    reader_light_DN = new BTagCalibrationReader(calib, BTagEntry::OP_MEDIUM, "incl", "down");  // sys down

    reader_heavy_loose = new BTagCalibrationReader(calib, BTagEntry::OP_LOOSE, "comb", "central"); // central
    reader_heavy_loose_UP = new BTagCalibrationReader(calib, BTagEntry::OP_LOOSE, "comb", "up");  // sys up
    reader_heavy_loose_DN = new BTagCalibrationReader(calib, BTagEntry::OP_LOOSE, "comb", "down");  // sys down
    reader_light_loose = new BTagCalibrationReader(calib, BTagEntry::OP_LOOSE, "incl", "central");  // central
    reader_light_loose_UP = new BTagCalibrationReader(calib, BTagEntry::OP_LOOSE, "incl", "up");  // sys up
    reader_light_loose_DN = new BTagCalibrationReader(calib, BTagEntry::OP_LOOSE, "incl", "down");  // sys down

    std::cout << "loaded fullsim btag SFs" << std::endl;
    //Sicong: If we are not using fast sim, hence not changing the csv file for now... (Currently don't have the SF's yet. 
    calib_fastsim = new BTagCalibration("CSV", "btagsf/fastsim_csvv2_ttbar_26_1_2017.csv"); // 25ns fastsim version of SFs
    reader_fastsim = new BTagCalibrationReader(calib_fastsim, BTagEntry::OP_MEDIUM, "fastsim", "central"); // central
    reader_fastsim_UP = new BTagCalibrationReader(calib_fastsim, BTagEntry::OP_MEDIUM, "fastsim", "up");  // sys up
    reader_fastsim_DN = new BTagCalibrationReader(calib_fastsim, BTagEntry::OP_MEDIUM, "fastsim", "down");  // sys down
    reader_fastsim_loose = new BTagCalibrationReader(calib_fastsim, BTagEntry::OP_LOOSE, "fastsim", "central"); // central
    reader_fastsim_loose_UP = new BTagCalibrationReader(calib_fastsim, BTagEntry::OP_LOOSE, "fastsim", "up");  // sys up
    reader_fastsim_loose_DN = new BTagCalibrationReader(calib_fastsim, BTagEntry::OP_LOOSE, "fastsim", "down");  // sys down
    if(!isFastsim_){
      char* eff_file_name;
      eff_file_name = "$COREPATH/Tools/btagsf/data/run2_25ns/btageff__ttbar_amc_94X_deepCSV.root";// "btagsf/btageff__ttbar_amc_94X_deepCSV.root";//
      getHist(h_btag_eff_b,eff_file_name,"h2_BTaggingEff_csv_med_Eff_b");
      getHist(h_btag_eff_c,eff_file_name,"h2_BTaggingEff_csv_med_Eff_c");
      getHist(h_btag_eff_udsg,eff_file_name,"h2_BTaggingEff_csv_med_Eff_udsg");
      getHist(h_btag_eff_b_loose,eff_file_name,"h2_BTaggingEff_csv_loose_Eff_b");
      getHist(h_btag_eff_c_loose,eff_file_name,"h2_BTaggingEff_csv_loose_Eff_c");
      getHist(h_btag_eff_udsg_loose,eff_file_name,"h2_BTaggingEff_csv_loose_Eff_udsg");
    }
    else{
      char* eff_file_name;
      eff_file_name = "$COREPATH/Tools/btagsf/data/run2_fastsim/btageff__SMS-T1bbbb-T1qqqq_25ns_Moriond17.root";
      getHist(h_btag_eff_b,eff_file_name,"h2_BTaggingEff_csv_med_Eff_b");
      getHist(h_btag_eff_c,eff_file_name,"h2_BTaggingEff_csv_med_Eff_c");
      getHist(h_btag_eff_udsg,eff_file_name,"h2_BTaggingEff_csv_med_Eff_udsg");
      getHist(h_btag_eff_b_loose,eff_file_name,"h2_BTaggingEff_csv_loose_Eff_b");
      getHist(h_btag_eff_c_loose,eff_file_name,"h2_BTaggingEff_csv_loose_Eff_c");
      getHist(h_btag_eff_udsg_loose,eff_file_name,"h2_BTaggingEff_csv_loose_Eff_udsg");
    }
    std::cout << "loaded fastsim btag SFs" << std::endl;
    return;
}

float JetTree::getBtagEffFromFile(float pt, float eta, int mcFlavour, bool isFastsim, bool isloose){
    if(!h_btag_eff_b || !h_btag_eff_c || !h_btag_eff_udsg ||!h_btag_eff_b_loose || !h_btag_eff_c_loose || !h_btag_eff_udsg_loose) {
      std::cout << "babyMaker::getBtagEffFromFile: ERROR: missing input hists" << std::endl;
      return 1.;
    }
    // only use pt bins up to 400 GeV for charm and udsg
    float pt_cutoff = std::max(20.,std::min(399.,double(pt)));
    TH2D* h(0);
    
    if (abs(mcFlavour) == 5) {
      h = isloose ? h_btag_eff_b_loose : h_btag_eff_b;
      // use pt bins up to 600 GeV for b
      pt_cutoff = std::max(20.,std::min(599.,double(pt)));
    }
    else if (abs(mcFlavour) == 4) {
      h = isloose? h_btag_eff_c_loose : h_btag_eff_c;
    }
    else {
      h = isloose ? h_btag_eff_udsg_loose: h_btag_eff_udsg;
    }

    int binx = h->GetXaxis()->FindBin(pt_cutoff);
    int biny = h->GetYaxis()->FindBin(fabs(eta));
    return h->GetBinContent(binx,biny);
}

void JetTree::FillCommon(std::vector<unsigned int> alloverlapjets_idx,  FactorizedJetCorrector* corrector, float& btagprob_data, float &btagprob_mc, float &btagprob_heavy_UP, float & btagprob_heavy_DN,float & btagprob_light_UP, float & btagprob_light_DN, float & btagprob_FS_UP, float & btagprob_FS_DN, unsigned int overlep1_idx, unsigned int overlep2_idx, bool applynewcorr, JetCorrectionUncertainty* jetcorr_uncertainty, int JES_type, bool applyBtagSFs, bool isFastsim)
{
    // fill info for ak4pfjets
    int nGoodJets=0.;
    int nFailJets=0.;
    unsigned int jindex=0;
    float HT = 0.;
    float JET_PT = 0.;
    int nbtags_med = 0;
    static const float BTAG_MED = 0.8484;
    static const float BTAG_LOOSE = 0.5426;
    float dPhiM = 0.;
    float btagdisc = 0.;   
    unsigned int leadbtag_idx = 0;

    float htssm = 0.;
    float htosm = 0.;
    float htratiom = 0.;
    float mbb = 0.;
    float mct = 0.;
   
    // Figure out which convention is in use for DeepCSV discriminator names
    // and store it statically so we don't have to re-find it for every event
    static TString deepCSV_prefix = "NULL";
    if( deepCSV_prefix == "NULL" ) {
	    for( TString discName : pfjets_bDiscriminatorNames() ) {
		    if( discName.Contains("pfDeepCSV") ) { // 2017 convention
			    deepCSV_prefix = "pfDeepCSV";
			    break;
		    }
		    else if( discName.Contains("deepFlavour") ) { // 2016 convention
			    deepCSV_prefix = "deepFlavour";
			    break;
		    }
	    } // end loop over b discriminator names
	    if( deepCSV_prefix == "NULL" ) {
		    cout << "Error in JetTree.cc: Can't find DeepCSV discriminator names!" << endl;
		    exit(1);
	    }
    } // end if prefix == "NULL"
 
    //apply JEC
    vector<float> newjecorr;
    newjecorr.clear();
    vector<pair <int, LorentzVector> > sortedJets_pt;
    vector <pair<int, float>> jet_csv_pairs;

    vector<LorentzVector> p4sCorrJets; // store corrected p4 for ALL jets, so indices match CMS3 ntuple
    vector<LorentzVector> p4sUCorrJets;
    p4sCorrJets.clear();
    p4sUCorrJets.clear();
    for(unsigned int iJet = 0; iJet < cms3.pfjets_p4().size(); iJet++){
      LorentzVector jetp4_cor = cms3.pfjets_p4().at(iJet);
        // get uncorrected jet p4 to use as input for corrections
      LorentzVector jetp4_uncor = pfjets_p4().at(iJet) * cms3.pfjets_undoJEC().at(iJet);

      // get L1FastL2L3Residual total correction
      corrector->setRho   ( cms3.evt_fixgridfastjet_all_rho() );
      corrector->setJetA  ( cms3.pfjets_area().at(iJet)       );
      corrector->setJetPt ( jetp4_uncor.pt()               );
      corrector->setJetEta( jetp4_uncor.eta()              );
      double corr = corrector->getCorrection();

      // check for negative correction
      if (corr < 0. && fabs(jetp4_uncor.eta()) < 4.7) {
        std::cout << "ScanChain::Looper: WARNING: negative jet correction: " << corr
                  << ", raw jet pt: " << jetp4_uncor.pt() << ", eta: " << jetp4_uncor.eta() << std::endl;
      }

      // include protections here on jet kinematics to prevent rare warnings/crashes
      double var = 1.;
      if (!evt_isRealData() && JES_type != 0 && jetp4_uncor.pt()*corr > 0. && fabs(jetp4_uncor.eta()) < 5.4) {
        jetcorr_uncertainty->setJetEta(jetp4_uncor.eta());
        jetcorr_uncertainty->setJetPt(jetp4_uncor.pt() * corr); // must use CORRECTED pt
        double unc = jetcorr_uncertainty->getUncertainty(true);
        var = (1. + JES_type * unc);
      }

      // apply new JEC to p4
      jetp4_cor = jetp4_uncor * corr*var;
      newjecorr.push_back(corr);
      if(applynewcorr) p4sCorrJets.push_back(jetp4_cor);
      else p4sCorrJets.push_back(pfjets_p4().at(iJet));
      p4sUCorrJets.push_back(jetp4_uncor);
    }
    sortedJets_pt =  sort_pt(p4sCorrJets,JET_PT);

    for (size_t idx = 0; idx < pfjets_p4().size(); ++idx) {
      jindex = sortedJets_pt.at(idx).first;
      //deal with the overlaps
      if(jindex == overlep1_idx){
	      ak4pfjet_overlep1_p4  = p4sCorrJets.at(jindex);
        ak4pfjet_overlep1_CSV = pfjets_pfCombinedInclusiveSecondaryVertexV2BJetTag().at(jindex);
        ak4pfjet_overlep1_deepCSV = getbtagvalue(deepCSV_prefix+"JetTags:probb",jindex) + getbtagvalue(deepCSV_prefix+"JetTags:probbb",jindex);
        //ak4pfjet_overlep1_pu_id = pfjets_pileupJetId().at(jindex);
        ak4pfjet_overlep1_chf = pfjets_chargedHadronE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy());
        ak4pfjet_overlep1_nhf = pfjets_neutralHadronE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy());
        ak4pfjet_overlep1_cef = pfjets_chargedEmE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy());
        ak4pfjet_overlep1_nef = pfjets_neutralEmE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy());
        ak4pfjet_overlep1_muf = pfjets_muonE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy());
        ak4pfjet_overlep1_cm  = pfjets_chargedMultiplicity().at(jindex);
        ak4pfjet_overlep1_nm  = cms3.pfjets_neutralMultiplicity().at(jindex);
      }
      if(jindex == overlep2_idx){
        ak4pfjet_overlep2_p4  = p4sCorrJets.at(jindex);
        ak4pfjet_overlep2_CSV = pfjets_pfCombinedInclusiveSecondaryVertexV2BJetTag().at(jindex);
        ak4pfjet_overlep2_deepCSV = getbtagvalue(deepCSV_prefix+"JetTags:probb",jindex) + getbtagvalue(deepCSV_prefix+"JetTags:probbb",jindex);
        //ak4pfjet_overlep2_pu_id = pfjets_pileupJetId().at(jindex);
        ak4pfjet_overlep2_chf = pfjets_chargedHadronE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy());
        ak4pfjet_overlep2_nhf = pfjets_neutralHadronE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy());
        ak4pfjet_overlep2_cef = pfjets_chargedEmE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy());
        ak4pfjet_overlep2_nef = pfjets_neutralEmE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy());
        ak4pfjet_overlep2_muf = pfjets_muonE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy());
        ak4pfjet_overlep2_cm  = pfjets_chargedMultiplicity().at(jindex);
        ak4pfjet_overlep2_nm  = cms3.pfjets_neutralMultiplicity().at(jindex);
      }
      //remove overlaps & apply preselections
      if(jindex == overlep1_idx || jindex == overlep2_idx) continue; //remove ovelaps from your jet collection

    	bool skipthis = false;
    	for(size_t jdx = 0; jdx < alloverlapjets_idx.size(); ++jdx){
    	  if(alloverlapjets_idx.at(jdx)==jindex) {
    	    skipthis = true;
    	    break;
    	  }
    	}
	    if(skipthis) continue; //remove all overlaps from jet collection

        //Jet selections
      if(p4sCorrJets.at(jindex).pt() < m_ak4_pt_cut) continue;
      if(fabs(p4sCorrJets.at(jindex).eta()) > m_ak4_eta_cut) continue;
      if(!isLoosePFJetV2(jindex)) ++nFailJets;
      if(!isFastsim && m_ak4_passid && !isLoosePFJetV2(jindex)) continue;
      nGoodJets++;
      float btagvalue = getbtagvalue(deepCSV_prefix+"JetTags:probb",jindex) + getbtagvalue(deepCSV_prefix+"JetTags:probbb",jindex);
      jet_csv_pairs.push_back(make_pair(jindex,btagvalue));
      ak4pfjets_p4.push_back(p4sCorrJets.at(jindex));
      ak4pfjets_pt.push_back(p4sCorrJets.at(jindex).pt());
      ak4pfjets_eta.push_back(p4sCorrJets.at(jindex).eta());
      ak4pfjets_phi.push_back(p4sCorrJets.at(jindex).phi());
      ak4pfjets_mass.push_back(p4sCorrJets.at(jindex).mass());

      ak4pfjets_CSV.push_back(pfjets_pfCombinedInclusiveSecondaryVertexV2BJetTag().at(jindex));
      ak4pfjets_mva.push_back(getbtagvalue("pfCombinedMVAV2BJetTags", jindex));
      ak4pfjets_deepCSV.push_back(getbtagvalue(deepCSV_prefix+"JetTags:probb",jindex) + getbtagvalue(deepCSV_prefix+"JetTags:probbb",jindex));
      ak4pfjets_puid.push_back(loosePileupJetId(jindex));
      ak4pfjets_parton_flavor.push_back(pfjets_partonFlavour().at(jindex));
      ak4pfjets_hadron_flavor.push_back(pfjets_hadronFlavour().at(jindex));
      ak4pfjets_loose_puid.push_back(loosePileupJetId(jindex));
      ak4pfjets_loose_pfid.push_back(isLoosePFJetV2(jindex));
      ak4pfjets_medium_pfid.push_back(isMediumPFJet(jindex));
      ak4pfjets_tight_pfid.push_back(isTightPFJet(jindex));

      ak4pfjets_chf.push_back(pfjets_chargedHadronE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy()) );
      ak4pfjets_nhf.push_back(pfjets_neutralHadronE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy()) );
      ak4pfjets_cef.push_back(pfjets_chargedEmE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy()) );
      ak4pfjets_nef.push_back(pfjets_neutralEmE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy()) );
      ak4pfjets_muf.push_back(pfjets_muonE().at(jindex)/ (pfjets_undoJEC().at(jindex)*p4sCorrJets[jindex].energy()) );
      ak4pfjets_cm.push_back(pfjets_chargedMultiplicity().at(jindex));
      ak4pfjets_nm.push_back(cms3.pfjets_neutralMultiplicity().at(jindex));
	
    	if (!evt_isRealData()) {
        ak4pfjets_mc3dr.push_back(pfjets_mc3dr().at(jindex));
        ak4pfjets_mc3id.push_back(pfjets_mc3_id().at(jindex));
        ak4pfjets_mc3idx.push_back(pfjets_mc3idx().at(jindex));
        ak4pfjets_mcmotherid.push_back(pfjets_mc_motherid().at(jindex));
    	}

    	//HTRatio
    	dPhiM = getdphi(evt_pfmetPhi(), p4sCorrJets.at(jindex).phi() );
    	if ( dPhiM  < (TMath::Pi()/2) ) htssm = htssm + p4sCorrJets.at(jindex).pt();
      else htosm = htosm + p4sCorrJets.at(jindex).pt();	

      HT = HT + p4sCorrJets.at(jindex).pt();
	
	    //medium btag
      //pfjets_pfCombinedInclusiveSecondaryVertexV2BJetTag().at(jindex)
	    if( btagvalue > BTAG_MED ) {
//        if(pfjets_pfCombinedInclusiveSecondaryVertexV2BJetTag().at(jindex) > BTAG_MED){
        ak4pfjets_passMEDbtag.push_back(true);
        nbtags_med++;
        if(nbtags_med == 1){
          ak4pfjets_leadMEDbjet_pt = p4sCorrJets.at(jindex).pt();  //plot leading bjet pT
          ak4pfjets_leadMEDbjet_p4 = p4sCorrJets.at(jindex);
        }
        ak4pfjets_MEDbjet_pt.push_back(p4sCorrJets.at(jindex).pt());
        // btag SF - not final yet

        if (!evt_isRealData()&&applyBtagSFs) {
          float eff = getBtagEffFromFile(p4sCorrJets[jindex].pt(),p4sCorrJets[jindex].eta(), pfjets_hadronFlavour().at(jindex), isFastsim,false);
		      BTagEntry::JetFlavor flavor = BTagEntry::FLAV_UDSG;

      		if (abs(pfjets_hadronFlavour().at(jindex)) == 5) flavor = BTagEntry::FLAV_B;
      		else if (abs(pfjets_hadronFlavour().at(jindex)) == 4) flavor = BTagEntry::FLAV_C;
      
      	        float pt_cutoff = std::max(20.,std::min(669.,double(p4sCorrJets[jindex].pt()))); //Sicong: lower it to 20GeV
      	        float eta_cutoff = std::min(2.39,fabs(double(p4sCorrJets[jindex].eta())));
      		float weight_cent(1.), weight_UP(1.), weight_DN(1.), weight_FS_UP(1.), weight_FS_DN(1.);
      
      //                cout<<"read uncertainty from btagsf reader:"<<endl;
      		if (flavor == BTagEntry::FLAV_UDSG) {
      		  weight_cent = reader_light->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_UP = reader_light_UP->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_DN = reader_light_DN->eval(flavor, eta_cutoff, pt_cutoff);
      		} else {
      		  weight_cent = reader_heavy->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_UP = reader_heavy_UP->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_DN = reader_heavy_DN->eval(flavor, eta_cutoff, pt_cutoff);
      		}
      		if (isFastsim) {
      		  weight_FS_UP = weight_cent * reader_fastsim_UP->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_FS_DN = weight_cent * reader_fastsim_DN->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_cent *= reader_fastsim->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_UP *= reader_fastsim->eval(flavor, eta_cutoff, pt_cutoff);//this is still just btagSF
      		  weight_DN *= reader_fastsim->eval(flavor, eta_cutoff, pt_cutoff);//this is still just btagSF
      		}
        //              cout<<"got uncertainty from btagsf reader:"<<endl;
                      btagprob_data *= weight_cent * eff;
                      btagprob_mc *= eff;
      		if (flavor == BTagEntry::FLAV_UDSG) {
      		  btagprob_light_UP *= weight_UP * eff;
      		  btagprob_light_DN *= weight_DN * eff;
      		  btagprob_heavy_UP *= weight_cent * eff;
      		  btagprob_heavy_DN *= weight_cent * eff;
      		} else {
      		  btagprob_light_UP *= weight_cent * eff;
      		  btagprob_light_DN *= weight_cent * eff;
      		  btagprob_heavy_UP *= weight_UP * eff;
      		  btagprob_heavy_DN *= weight_DN * eff;
                      }
      		if(isFastsim){
      		  btagprob_FS_UP *= weight_FS_UP * eff;
      		  btagprob_FS_DN *= weight_FS_DN * eff;
      		}
        }// end of if it's not real data and apply btaggign scale factor
      }// end of if it's medium
      else if(btagvalue < BTAG_MED && btagvalue > BTAG_LOOSE){
        ak4pfjets_passMEDbtag.push_back(false);
        // these fail medium but pass loose
        if(!evt_isRealData()&&applyBtagSFs){
          float eff = getBtagEffFromFile(p4sCorrJets[jindex].pt(),p4sCorrJets[jindex].eta(), pfjets_hadronFlavour().at(jindex), isFastsim,false);
          float eff_loose = getBtagEffFromFile(p4sCorrJets[jindex].pt(),p4sCorrJets[jindex].eta(), pfjets_hadronFlavour().at(jindex), isFastsim,true);
		      BTagEntry::JetFlavor flavor = BTagEntry::FLAV_UDSG;

      		if (abs(pfjets_hadronFlavour().at(jindex)) == 5) flavor = BTagEntry::FLAV_B;
      		else if (abs(pfjets_hadronFlavour().at(jindex)) == 4) flavor = BTagEntry::FLAV_C;

	        float pt_cutoff = std::max(20.,std::min(669.,double(p4sCorrJets[jindex].pt())));
	        float eta_cutoff = std::min(2.39,fabs(double(p4sCorrJets[jindex].eta())));
      		float weight_cent(1.), weight_UP(1.), weight_DN(1.), weight_FS_UP(1.), weight_FS_DN(1.);
      		float weight_cent_loose(1.), weight_UP_loose(1.), weight_DN_loose(1.), weight_FS_UP_loose(1.), weight_FS_DN_loose(1.);
          //                cout<<"read uncertainty from btagsf reader:"<<endl;
      		if (flavor == BTagEntry::FLAV_UDSG) {
      		  weight_cent = reader_light->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_UP = reader_light_UP->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_DN = reader_light_DN->eval(flavor, eta_cutoff, pt_cutoff);
      
      		  weight_cent_loose = reader_light_loose->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_UP_loose = reader_light_loose_UP->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_DN_loose = reader_light_loose_DN->eval(flavor, eta_cutoff, pt_cutoff);
      		} else {
      		  weight_cent = reader_heavy->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_UP = reader_heavy_UP->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_DN = reader_heavy_DN->eval(flavor, eta_cutoff, pt_cutoff);
      
      		  weight_cent_loose = reader_heavy_loose->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_UP_loose = reader_heavy_loose_UP->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_DN_loose = reader_heavy_loose_DN->eval(flavor, eta_cutoff, pt_cutoff);
      		}
      		if (isFastsim) {
      		  weight_FS_UP = weight_cent * reader_fastsim_loose_UP->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_FS_DN = weight_cent * reader_fastsim_loose_DN->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_cent *= reader_fastsim_loose->eval(flavor, eta_cutoff, pt_cutoff);
      		  weight_UP *= reader_fastsim_loose->eval(flavor, eta_cutoff, pt_cutoff);//this is still just btagSF
      		  weight_DN *= reader_fastsim_loose->eval(flavor, eta_cutoff, pt_cutoff);//this is still just btagSF
      		}
        //              cout<<"got uncertainty from btagsf reader:"<<endl;
          btagprob_data *= eff_loose*weight_cent_loose-eff*weight_cent;
          btagprob_mc *= eff_loose-eff;
      		if (flavor == BTagEntry::FLAV_UDSG) {
      		  btagprob_light_UP *= weight_UP_loose * eff_loose- weight_UP*eff;
      		  btagprob_light_DN *= weight_DN_loose * eff_loose- weight_DN*eff;
      		  btagprob_heavy_UP *= weight_cent_loose * eff_loose-weight_cent*eff;
      		  btagprob_heavy_DN *= weight_cent_loose * eff_loose-weight_cent*eff;}
          else {
      		  btagprob_light_UP *= weight_cent_loose * eff_loose-weight_cent*eff;
      		  btagprob_light_DN *= weight_cent_loose * eff_loose-weight_cent*eff;
      		  btagprob_heavy_UP *= weight_UP_loose * eff_loose- weight_UP*eff;
      		  btagprob_heavy_DN *= weight_DN_loose * eff_loose- weight_DN*eff;
          }
      		if(isFastsim){
      		  btagprob_FS_UP *= weight_FS_UP_loose * eff_loose -weight_FS_UP * eff;
      		  btagprob_FS_DN *= weight_FS_DN_loose * eff_loose -weight_FS_DN * eff;
      		}
        } 
      } // end of jets failing medium but pass loose
      else { //these fail loose 
        ak4pfjets_passMEDbtag.push_back(false);
        if (!evt_isRealData()&&applyBtagSFs){
          float eff = getBtagEffFromFile(p4sCorrJets[jindex].pt(),p4sCorrJets[jindex].eta(), pfjets_hadronFlavour().at(jindex), isFastsim,true);
  	      BTagEntry::JetFlavor flavor = BTagEntry::FLAV_UDSG;
  	      if (abs(pfjets_hadronFlavour().at(jindex)) == 5) flavor = BTagEntry::FLAV_B;
  	      else if (abs(pfjets_hadronFlavour().at(jindex)) == 4) flavor = BTagEntry::FLAV_C;
  	      float pt_cutoff = std::max(20.,std::min(669.,double(p4sCorrJets[jindex].pt())));
  	      float eta_cutoff = std::min(2.39,fabs(double(p4sCorrJets[jindex].eta())));
  	      float weight_cent(1.), weight_UP(1.), weight_DN(1.), weight_FS_UP(1.), weight_FS_DN(1.);
  	      if (flavor == BTagEntry::FLAV_UDSG) {
      		weight_cent = reader_light_loose->eval(flavor, eta_cutoff, pt_cutoff);
      		weight_UP = reader_light_loose_UP->eval(flavor, eta_cutoff, pt_cutoff);
      		weight_DN = reader_light_loose_DN->eval(flavor, eta_cutoff, pt_cutoff);
  	      } else {
      		weight_cent = reader_heavy_loose->eval(flavor, eta_cutoff, pt_cutoff);
      		weight_UP = reader_heavy_loose_UP->eval(flavor, eta_cutoff, pt_cutoff);
      		weight_DN = reader_heavy_loose_DN->eval(flavor, eta_cutoff, pt_cutoff);
          }
	        if (isFastsim) {
        		weight_FS_UP = weight_cent * reader_fastsim_loose_UP->eval(flavor, eta_cutoff, pt_cutoff);//this is pure fastsimSF
        		weight_FS_DN = weight_cent * reader_fastsim_loose_DN->eval(flavor, eta_cutoff, pt_cutoff);//this is pure fastsimSF
        		weight_cent *= reader_fastsim_loose->eval(flavor, eta_cutoff, pt_cutoff);
        		weight_UP *= reader_fastsim_loose_UP->eval(flavor, eta_cutoff, pt_cutoff);//this is still just btagSF
        		weight_DN *= reader_fastsim_loose_DN->eval(flavor, eta_cutoff, pt_cutoff);//this is still just btagSF
	        }
            btagprob_data *= (1. - weight_cent * eff);
            btagprob_mc *= (1. - eff);
	        if (flavor == BTagEntry::FLAV_UDSG) {
        		btagprob_light_UP *= (1. - weight_UP * eff);
        		btagprob_light_DN *= (1. - weight_DN * eff);
        		btagprob_heavy_UP *= (1. - weight_cent * eff);
        		btagprob_heavy_DN *= (1. - weight_cent * eff);
          } else {
        		btagprob_light_UP *= (1. - weight_cent * eff);
        		btagprob_light_DN *= (1. - weight_cent * eff);
        		btagprob_heavy_UP *= (1. - weight_UP * eff);
        		btagprob_heavy_DN *= (1. - weight_DN * eff);
          }
  	      if(isFastsim){
        		btagprob_FS_UP *= (1. - weight_FS_UP * eff);
        		btagprob_FS_DN *= (1. - weight_FS_DN * eff);
  	      }
        }
      }// end of if fail loose
  
    	if(pfjets_pfCombinedInclusiveSecondaryVertexV2BJetTag().at(jindex)> btagdisc){
    	  btagdisc = pfjets_pfCombinedInclusiveSecondaryVertexV2BJetTag().at(jindex);
    	  leadbtag_idx = jindex;
    	}
      ak4pfjets_leadbtag_p4 = p4sCorrJets.at(leadbtag_idx);//highest CSV jet
      sort( jet_csv_pairs.begin(), jet_csv_pairs.end(), sortIndexbyCSV);
      if(nGoodJets>1){
        if(nbtags_med>0 && p4sCorrJets.size()>1) {
          mbb = getmbb(p4sCorrJets.at(jet_csv_pairs.at(0).first),p4sCorrJets.at(jet_csv_pairs.at(1).first))  ; // at least one b-tagged, use CSV sorted
          mct = getmct(p4sCorrJets.at(jet_csv_pairs.at(0).first),p4sCorrJets.at(jet_csv_pairs.at(1).first))  ; // at least one b-tagged, use CSV sorted
        }
        else {
          mbb = getmbb(ak4pfjets_p4.at(0), ak4pfjets_p4.at(1)) ;  // no btagged use pt sorted.
          mct = getmct(ak4pfjets_p4.at(0), ak4pfjets_p4.at(1)) ;  // no btagged use pt sorted.
        } 
      }
      ngoodjets = nGoodJets;
      nfailjets = nFailJets;
      ak4_HT = HT;
      ak4_mbb = mbb;
      ak4_mct = mct;
      HT=0;
      ngoodbtags = nbtags_med;
      
      ak4_htssm = htssm;
      ak4_htosm = htosm;
      htratiom   = htssm / (htosm + htssm);
      ak4_htratiom = htratiom; 
    }// end of loop for ak4 jets
    nGoodJets = 0;
    // fill info for ak8pfjets
    for (size_t idx = 0; idx < ak8jets_p4().size(); ++idx)
    {
      if(pfjets_p4().at(idx).pt() < m_ak8_pt_cut) continue;
      if(fabs(pfjets_p4().at(idx).eta()) > m_ak8_eta_cut) continue;
      if(!isFastsim && m_ak8_passid && !isLoosePFJetV2(idx)) continue;

      ak8pfjets_p4.push_back(ak8jets_p4().at(idx));
      ak8pfjets_tau1.push_back(ak8jets_nJettinessTau1().at(idx));
      ak8pfjets_tau2.push_back(ak8jets_nJettinessTau2().at(idx));
      ak8pfjets_tau3.push_back(ak8jets_nJettinessTau3().at(idx));
//      ak8pfjets_pruned_mass.push_back(ak8jets_prunedMass().at(idx));
      ak8pfjets_puppi_softdropMass.push_back(ak8jets_puppi_softdropMass().at(idx));
//      ak8pfjets_top_mass.push_back(ak8jets_topJetMass().at(idx));
//      ak8pfjets_trimmed_mass.push_back(ak8jets_trimmedMass().at(idx));
//      ak8pfjets_filtered_mass.push_back(ak8jets_filteredMass().at(idx));
//      ak8pfjets_pu_id.push_back(ak8jets_pileupJetId().at(idx));    
      ak8pfjets_parton_flavor.push_back(ak8jets_partonFlavour().at(idx));
      ak8pfjets_deep_rawdisc_qcd.push_back(ak8jets_deep_rawdisc_qcd().at(idx));
      ak8pfjets_deep_rawdisc_top.push_back(ak8jets_deep_rawdisc_top().at(idx));
      ak8pfjets_deep_rawdisc_w.push_back(ak8jets_deep_rawdisc_w().at(idx));
      ak8pfjets_deep_rawdisc_z.push_back(ak8jets_deep_rawdisc_z().at(idx));
      ak8pfjets_deep_rawdisc_zbb.push_back(ak8jets_deep_rawdisc_zbb().at(idx));
      ak8pfjets_deep_rawdisc_hbb.push_back(ak8jets_deep_rawdisc_hbb().at(idx));
      ak8pfjets_deep_rawdisc_h4q.push_back(ak8jets_deep_rawdisc_h4q().at(idx));
      
      for ( auto name : ak8jets_bDiscriminatorNames() )
        ak8pfjets_bDiscriminatorNames.push_back(name.Data());
      for ( auto b_vec : ak8jets_bDiscriminators() )
        ak8pfjets_bDiscriminators.push_back(b_vec);
      
//      ak8pfjets_decorr_binscore_top.push_back(ak8jets_decorr_binscore_top().at(idx));
//      ak8pfjets_decorr_binscore_w.push_back(ak8jets_decorr_binscore_w().at(idx));
//      ak8pfjets_decorr_binscore_z.push_back(ak8jets_decorr_binscore_z().at(idx));
//      ak8pfjets_decorr_binscore_zbb.push_back(ak8jets_decorr_binscore_zbb().at(idx));
//      ak8pfjets_decorr_binscore_hbb.push_back(ak8jets_decorr_binscore_hbb().at(idx));
//      ak8pfjets_decorr_binscore_h4q.push_back(ak8jets_decorr_binscore_h4q().at(idx));
//      ak8pfjets_decorr_rawscore_top.push_back(ak8jets_decorr_rawscore_top().at(idx));
//      ak8pfjets_decorr_rawscore_w.push_back(ak8jets_decorr_rawscore_w().at(idx));
//      ak8pfjets_decorr_rawscore_z.push_back(ak8jets_decorr_rawscore_z().at(idx));
//      ak8pfjets_decorr_rawscore_zbb.push_back(ak8jets_decorr_rawscore_zbb().at(idx));
//      ak8pfjets_decorr_rawscore_hbb.push_back(ak8jets_decorr_rawscore_hbb().at(idx));
//      ak8pfjets_decorr_rawscore_h4q.push_back(ak8jets_decorr_rawscore_h4q().at(idx));
//      ak8pfjets_decorr_rawscore_qcd.push_back(ak8jets_decorr_rawscore_qcd().at(idx));
//      ak8pfjets_decorr_flavscore_bb.push_back(ak8jets_decorr_flavscore_bb().at(idx));
//      ak8pfjets_decorr_flavscore_cc.push_back(ak8jets_decorr_flavscore_cc().at(idx));
//      ak8pfjets_decorr_flavscore_bb_no_gluon.push_back(ak8jets_decorr_flavscore_bb_no_gluon().at(idx));
//      ak8pfjets_decorr_flavscore_cc_no_gluon.push_back(ak8jets_decorr_flavscore_cc_no_gluon().at(idx));

      nGoodJets++;
    }
    ak8GoodPFJets = nGoodJets;
    nGoodJets=0;

    // fill info for genjets
    if (!evt_isRealData()){
      for (size_t idx = 0; idx < genjets_p4NoMuNoNu().size(); ++idx){
        if (genjets_p4NoMuNoNu().at(idx).pt() < m_ak4_pt_cut) continue;
 
        ak4genjets_p4.push_back(genjets_p4NoMuNoNu().at(idx));
        if(genjets_p4NoMuNoNu().at(idx).pt() > 20.)  nGoodJets++;
      }
    }
    nGoodGenJets = nGoodJets;
}
        
void JetTree::SetJetSelection (std::string cone_size, float pt_cut,float eta, bool id)
{
    if (cone_size == "ak4") { m_ak4_pt_cut = pt_cut; m_ak4_eta_cut = eta; m_ak4_passid = id; }
    else if (cone_size == "ak8") { m_ak8_pt_cut = pt_cut; m_ak8_eta_cut = eta; m_ak8_passid = id; }
    else {std::cout << "Invalid cone size." << std::endl;}
  
    return;
}
 
void JetTree::GetJetSelections (std::string cone_size)
{
    if (cone_size == "ak4") std::cout << "ak4 jet pt > " << m_ak4_pt_cut << std::endl;
    else if (cone_size == "ak8") std::cout << "ak8 jet pt > " << m_ak8_pt_cut << std::endl;
    else
    {
        std::cout << "ak4 jet pt > " << m_ak4_pt_cut << std::endl;        
        std::cout << "ak8 jet pt > " << m_ak8_pt_cut << std::endl;
    }
}

void JetTree::deleteBtagSFTool()
{
   
    delete calib;
    delete reader_heavy;
    delete reader_heavy_UP;
    delete reader_heavy_DN;
    delete reader_light;
    delete reader_light_UP;
    delete reader_light_DN;
    delete calib_fastsim;
    delete reader_fastsim;
    delete reader_fastsim_UP;
    delete reader_fastsim_DN;
    delete reader_heavy_loose;
    delete reader_heavy_loose_UP;
    delete reader_heavy_loose_DN;
    delete reader_light_loose;
    delete reader_light_loose_UP;
    delete reader_light_loose_DN;
    delete reader_fastsim_loose;
    delete reader_fastsim_loose_UP;
    delete reader_fastsim_loose_DN;

    return;
} 
void JetTree::Reset ()
{
    ak4pfjets_p4.clear();
    ak4pfjets_pt.clear();
    ak4pfjets_eta.clear();
    ak4pfjets_phi.clear();
    ak4pfjets_mass.clear();

    dphi_ak4pfjet_met.clear();
    ak4pfjets_qg_disc.clear();    
    ak4pfjets_CSV.clear();
    ak4pfjets_deepCSV.clear();
    ak4pfjets_mva.clear();
    ak4pfjets_puid.clear();
    ak4pfjets_parton_flavor.clear();
    ak4pfjets_hadron_flavor.clear();
    ak4pfjets_loose_puid.clear();
    ak4pfjets_loose_pfid.clear();
    ak4pfjets_medium_pfid.clear();
    ak4pfjets_tight_pfid.clear();

    ak4pfjets_chf.clear();
    ak4pfjets_nhf.clear();
    ak4pfjets_cef.clear();
    ak4pfjets_nef.clear();
    ak4pfjets_muf.clear();
    ak4pfjets_cm.clear();
    ak4pfjets_nm.clear();

    ak4pfjets_mc3dr.clear();
    ak4pfjets_mc3id.clear();
    ak4pfjets_mc3idx.clear();
    ak4pfjets_mcmotherid.clear();
 
    ak4_htssm    = -9999.;
    ak4_htosm    = -9999.;
    ak4_htratiom = -9999.; 

   //overlaps
    ak4pfjet_overlep1_p4 = LorentzVector(0,0, 0,0);
    ak4pfjet_overlep1_deepCSV = -9999;
    ak4pfjet_overlep1_CSV   = -9999;
    ak4pfjet_overlep1_pu_id = -9999;
    ak4pfjet_overlep1_chf   = -9999;
    ak4pfjet_overlep1_nhf   = -9999;
    ak4pfjet_overlep1_cef   = -9999;
    ak4pfjet_overlep1_nef   = -9999;
    ak4pfjet_overlep1_muf   = -9999;
    ak4pfjet_overlep1_cm    = -9999;
    ak4pfjet_overlep1_nm    = -9999;

    ak4pfjet_overlep2_p4 = LorentzVector(0,0, 0,0);
    ak4pfjet_overlep2_deepCSV = -9999;
    ak4pfjet_overlep2_CSV   = -9999;
    ak4pfjet_overlep2_pu_id = -9999;
    ak4pfjet_overlep2_chf   = -9999;
    ak4pfjet_overlep2_nhf   = -9999;
    ak4pfjet_overlep2_cef   = -9999;
    ak4pfjet_overlep2_nef   = -9999;
    ak4pfjet_overlep2_muf   = -9999;
    ak4pfjet_overlep2_cm    = -9999;
    ak4pfjet_overlep2_nm    = -9999;
  
    //ak8 
    ak8pfjets_p4.clear();
    ak8pfjets_tau1.clear();
    ak8pfjets_tau2.clear();
    ak8pfjets_tau3.clear();
    //ak8pfjets_top_mass.clear();
    //ak8pfjets_pruned_mass.clear();
    ak8pfjets_puppi_softdropMass.clear();
    //ak8pfjets_trimmed_mass.clear();
    //ak8pfjets_filtered_mass.clear();
    //ak8pfjets_pu_id.clear();    
    ak8pfjets_parton_flavor.clear();
    ak8pfjets_deep_rawdisc_qcd.clear();
    ak8pfjets_deep_rawdisc_top.clear();
    ak8pfjets_deep_rawdisc_w.clear();
    ak8pfjets_deep_rawdisc_z.clear();
    ak8pfjets_deep_rawdisc_zbb.clear();
    ak8pfjets_deep_rawdisc_hbb.clear();
    ak8pfjets_deep_rawdisc_h4q.clear();
    
    ak8pfjets_bDiscriminatorNames.clear();
    ak8pfjets_bDiscriminators.clear();
    
//    ak8pfjets_decorr_binscore_top.clear();
//    ak8pfjets_decorr_binscore_w.clear();
//    ak8pfjets_decorr_binscore_z.clear();
//    ak8pfjets_decorr_binscore_zbb.clear();
//    ak8pfjets_decorr_binscore_hbb.clear();
//    ak8pfjets_decorr_binscore_h4q.clear();
//    ak8pfjets_decorr_rawscore_top.clear();
//    ak8pfjets_decorr_rawscore_w.clear();
//    ak8pfjets_decorr_rawscore_z.clear();
//    ak8pfjets_decorr_rawscore_zbb.clear();
//    ak8pfjets_decorr_rawscore_hbb.clear();
//    ak8pfjets_decorr_rawscore_h4q.clear();
//    ak8pfjets_decorr_rawscore_qcd.clear();
//    ak8pfjets_decorr_flavscore_bb.clear();
//    ak8pfjets_decorr_flavscore_cc.clear();
//    ak8pfjets_decorr_flavscore_bb_no_gluon.clear();
//    ak8pfjets_decorr_flavscore_cc_no_gluon.clear();

 
    ak4genjets_p4.clear();
 
    ak4pfjets_MEDbjet_pt.clear();
    ak4pfjets_passMEDbtag.clear();
    ak4pfjets_leadMEDbjet_pt = -9999; 
    ak4pfjets_leadMEDbjet_p4 = LorentzVector(0,0, 0,0);
    ak4pfjets_leadbtag_p4 = LorentzVector(0,0, 0,0);
  
    ngoodjets     = -9999;  
    nfailjets     = -9999;  
    ak4_HT 	  = -9999.; 
    ak4_mbb 	  = -9999.; 
    ak4_mct 	  = -9999.; 
    ak8GoodPFJets = -9999;
    nGoodGenJets  = -9999;
    ngoodbtags    = -9999;
}
 
void JetTree::SetAK4Branches (TTree* tree)
{
    tree->Branch(Form("%sngoodjets", prefix_.c_str()) , &ngoodjets);
    tree->Branch(Form("%sngoodbtags", prefix_.c_str()) , &ngoodbtags);
    tree->Branch(Form("%sak4_HT", prefix_.c_str()) , &ak4_HT);
    tree->Branch(Form("%sak4_mbb", prefix_.c_str()) , &ak4_mbb);
    tree->Branch(Form("%sak4_mct", prefix_.c_str()) , &ak4_mct);
    tree->Branch(Form("%sak4_htratiom", prefix_.c_str()) , &ak4_htratiom);
    tree->Branch(Form("%sdphi_ak4pfjet_met", prefix_.c_str()) , &dphi_ak4pfjet_met);
    tree->Branch(Form("%sak4pfjets_deepCSV", prefix_.c_str()) , &ak4pfjets_deepCSV);

    tree->Branch(Form("%sak4pfjets_p4", prefix_.c_str()) , &ak4pfjets_p4);

    tree->Branch(Form("%sak4pfjets_passMEDbtag", prefix_.c_str()) , &ak4pfjets_passMEDbtag);
    tree->Branch(Form("%sak4pfjets_CSV", prefix_.c_str()) , &ak4pfjets_CSV);
    tree->Branch(Form("%sak4pfjets_mva", prefix_.c_str()) , &ak4pfjets_mva);
    tree->Branch(Form("%sak4pfjets_parton_flavor", prefix_.c_str()) , &ak4pfjets_parton_flavor);
    tree->Branch(Form("%sak4pfjets_hadron_flavor", prefix_.c_str()) , &ak4pfjets_hadron_flavor);
    tree->Branch(Form("%sak4pfjets_loose_puid", prefix_.c_str()) , &ak4pfjets_loose_puid);
    tree->Branch(Form("%sak4pfjets_loose_pfid", prefix_.c_str()) , &ak4pfjets_loose_pfid);

    tree->Branch(Form("%sak4pfjets_leadMEDbjet_p4", prefix_.c_str()) , &ak4pfjets_leadMEDbjet_p4);
    tree->Branch(Form("%sak4pfjets_leadbtag_p4", prefix_.c_str()) , &ak4pfjets_leadbtag_p4);
    tree->Branch(Form("%sak4genjets_p4", prefix_.c_str()) , &ak4genjets_p4); 
}

void JetTree::SetAK8Branches (TTree* tree)
{
    tree->Branch(Form("%sak8pfjets_p4", prefix_.c_str()) , &ak8pfjets_p4);
    tree->Branch(Form("%sak8pfjets_tau1", prefix_.c_str()) , &ak8pfjets_tau1);
    tree->Branch(Form("%sak8pfjets_tau2", prefix_.c_str()) , &ak8pfjets_tau2);
    tree->Branch(Form("%sak8pfjets_tau3", prefix_.c_str()) , &ak8pfjets_tau3);
    //tree->Branch(Form("%sak8pfjets_top_mass", prefix_.c_str()) , &ak8pfjets_top_mass);
    //tree->Branch(Form("%sak8pfjets_pruned_mass", prefix_.c_str()) , &ak8pfjets_pruned_mass);
    tree->Branch(Form("%sak8pfjets_puppi_softdropMass", prefix_.c_str()) , &ak8pfjets_puppi_softdropMass);
    //tree->Branch(Form("%sak8pfjets_trimmed_mass", prefix_.c_str()) , &ak8pfjets_trimmed_mass);
    //tree->Branch(Form("%sak8pfjets_filtered_mass", prefix_.c_str()) , &ak8pfjets_filtered_mass);
    //tree->Branch(Form("%sak8pfjets_pu_id", prefix_.c_str()) , &ak8pfjets_pu_id);    
    tree->Branch(Form("%sak8pfjets_parton_flavor", prefix_.c_str()) , &ak8pfjets_parton_flavor);
    tree->Branch(Form("%sak8GoodPFJets", prefix_.c_str()) , &ak8GoodPFJets);
    tree->Branch(Form("%sak8pfjets_deep_rawdisc_qcd", prefix_.c_str()) , &ak8pfjets_deep_rawdisc_qcd);
    tree->Branch(Form("%sak8pfjets_deep_rawdisc_top", prefix_.c_str()) , &ak8pfjets_deep_rawdisc_top);
    tree->Branch(Form("%sak8pfjets_deep_rawdisc_w", prefix_.c_str()) , &ak8pfjets_deep_rawdisc_w  );
    tree->Branch(Form("%sak8pfjets_deep_rawdisc_z", prefix_.c_str()) , &ak8pfjets_deep_rawdisc_z  );
    tree->Branch(Form("%sak8pfjets_deep_rawdisc_zbb", prefix_.c_str()) , &ak8pfjets_deep_rawdisc_zbb);
    tree->Branch(Form("%sak8pfjets_deep_rawdisc_hbb", prefix_.c_str()) , &ak8pfjets_deep_rawdisc_hbb);
    tree->Branch(Form("%sak8pfjets_deep_rawdisc_h4q", prefix_.c_str()) , &ak8pfjets_deep_rawdisc_h4q);
    
    tree->Branch(Form("%sak8pfjets_bDiscriminatorNames", prefix_.c_str()) , &ak8pfjets_bDiscriminatorNames);
    tree->Branch(Form("%sak8pfjets_bDiscriminators", prefix_.c_str()) , &ak8pfjets_bDiscriminators);

//    tree->Branch(Form("%sak8pfjets_decorr_binscore_top", prefix_.c_str()) , &ak8pfjets_decorr_binscore_top);
//    tree->Branch(Form("%sak8pfjets_decorr_binscore_w", prefix_.c_str()) , &ak8pfjets_decorr_binscore_w);
//    tree->Branch(Form("%sak8pfjets_decorr_binscore_z", prefix_.c_str()) , &ak8pfjets_decorr_binscore_z);
//    tree->Branch(Form("%sak8pfjets_decorr_binscore_zbb", prefix_.c_str()) , &ak8pfjets_decorr_binscore_zbb);
//    tree->Branch(Form("%sak8pfjets_decorr_binscore_hbb", prefix_.c_str()) , &ak8pfjets_decorr_binscore_hbb);
//    tree->Branch(Form("%sak8pfjets_decorr_binscore_h4q", prefix_.c_str()) , &ak8pfjets_decorr_binscore_h4q);
//    tree->Branch(Form("%sak8pfjets_decorr_rawscore_top", prefix_.c_str()) , &ak8pfjets_decorr_rawscore_top);
//    tree->Branch(Form("%sak8pfjets_decorr_rawscore_w", prefix_.c_str()) , &ak8pfjets_decorr_rawscore_w);
//    tree->Branch(Form("%sak8pfjets_decorr_rawscore_z", prefix_.c_str()) , &ak8pfjets_decorr_rawscore_z);
//    tree->Branch(Form("%sak8pfjets_decorr_rawscore_zbb", prefix_.c_str()) , &ak8pfjets_decorr_rawscore_zbb);
//    tree->Branch(Form("%sak8pfjets_decorr_rawscore_hbb", prefix_.c_str()) , &ak8pfjets_decorr_rawscore_hbb);
//    tree->Branch(Form("%sak8pfjets_decorr_rawscore_h4q", prefix_.c_str()) , &ak8pfjets_decorr_rawscore_h4q);
//    tree->Branch(Form("%sak8pfjets_decorr_rawscore_qcd", prefix_.c_str()) , &ak8pfjets_decorr_rawscore_qcd);
//    tree->Branch(Form("%sak8pfjets_decorr_flavscore_bb", prefix_.c_str()) , &ak8pfjets_decorr_flavscore_bb);
//    tree->Branch(Form("%sak8pfjets_decorr_flavscore_cc", prefix_.c_str()) , &ak8pfjets_decorr_flavscore_cc);
//    tree->Branch(Form("%sak8pfjets_decorr_flavscore_bb_no_gluon", prefix_.c_str()) , &ak8pfjets_decorr_flavscore_bb_no_gluon);            
//    tree->Branch(Form("%sak8pfjets_decorr_flavscore_cc_no_gluon", prefix_.c_str()) , &ak8pfjets_decorr_flavscore_cc_no_gluon);    
    
    
    
    
    
}

void JetTree::SetAK4Branches_Overleps (TTree* tree)
{
    tree->Branch(Form("%sak4pfjet_overlep1_p4", prefix_.c_str()) , &ak4pfjet_overlep1_p4);                                                                                    
    tree->Branch(Form("%sak4pfjet_overlep1_deepCSV", prefix_.c_str()) , &ak4pfjet_overlep1_deepCSV);
    tree->Branch(Form("%sak4pfjet_overlep1_CSV", prefix_.c_str()) , &ak4pfjet_overlep1_CSV);
    tree->Branch(Form("%sak4pfjet_overlep1_pu_id",prefix_.c_str()) , &ak4pfjet_overlep1_pu_id);
    tree->Branch(Form("%sak4pfjet_overlep1_chf", prefix_.c_str()) , &ak4pfjet_overlep1_chf);
    tree->Branch(Form("%sak4pfjet_overlep1_nhf", prefix_.c_str()) , &ak4pfjet_overlep1_nhf);
    tree->Branch(Form("%sak4pfjet_overlep1_cef", prefix_.c_str()) , &ak4pfjet_overlep1_cef);
    tree->Branch(Form("%sak4pfjet_overlep1_nef", prefix_.c_str()) , &ak4pfjet_overlep1_nef);
    tree->Branch(Form("%sak4pfjet_overlep1_muf", prefix_.c_str()) , &ak4pfjet_overlep1_muf);
    tree->Branch(Form("%sak4pfjet_overlep1_cm", prefix_.c_str()) , &ak4pfjet_overlep1_cm);
    tree->Branch(Form("%sak4pfjet_overlep1_nm", prefix_.c_str()) , &ak4pfjet_overlep1_nm);

    tree->Branch(Form("%sak4pfjet_overlep2_p4", prefix_.c_str()) , &ak4pfjet_overlep2_p4);
    tree->Branch(Form("%sak4pfjet_overlep2_CSV", prefix_.c_str()) , &ak4pfjet_overlep2_CSV);
    tree->Branch(Form("%sak4pfjet_overlep2_deepCSV", prefix_.c_str()) , &ak4pfjet_overlep2_deepCSV);
    tree->Branch(Form("%sak4pfjet_overlep2_pu_id",prefix_.c_str()) , &ak4pfjet_overlep2_pu_id);
    tree->Branch(Form("%sak4pfjet_overlep2_chf", prefix_.c_str()) , &ak4pfjet_overlep2_chf);
    tree->Branch(Form("%sak4pfjet_overlep2_nhf", prefix_.c_str()) , &ak4pfjet_overlep2_nhf);
    tree->Branch(Form("%sak4pfjet_overlep2_cef", prefix_.c_str()) , &ak4pfjet_overlep2_cef);
    tree->Branch(Form("%sak4pfjet_overlep2_nef", prefix_.c_str()) , &ak4pfjet_overlep2_nef);
    tree->Branch(Form("%sak4pfjet_overlep2_muf", prefix_.c_str()) , &ak4pfjet_overlep2_muf);
    tree->Branch(Form("%sak4pfjet_overlep2_cm", prefix_.c_str()) , &ak4pfjet_overlep2_cm);
    tree->Branch(Form("%sak4pfjet_overlep2_nm", prefix_.c_str()) , &ak4pfjet_overlep2_nm);
}

void JetTree::SetAK4Branches_SynchTools (TTree* tree)
{
    tree->Branch(Form("%sak4pfjets_pt", prefix_.c_str()) , &ak4pfjets_pt);
    tree->Branch(Form("%sak4pfjets_eta", prefix_.c_str()) , &ak4pfjets_eta);
    tree->Branch(Form("%sak4pfjets_phi", prefix_.c_str()) , &ak4pfjets_phi);
    tree->Branch(Form("%sak4pfjets_mass", prefix_.c_str()) , &ak4pfjets_mass);
}

void JetTree::SetAK4Branches_EF(TTree* tree)
{
    tree->Branch(Form("%sak4pfjets_chf", prefix_.c_str()) , &ak4pfjets_chf);
    tree->Branch(Form("%sak4pfjets_nhf", prefix_.c_str()) , &ak4pfjets_nhf);
    tree->Branch(Form("%sak4pfjets_cef", prefix_.c_str()) , &ak4pfjets_cef);
    tree->Branch(Form("%sak4pfjets_nef", prefix_.c_str()) , &ak4pfjets_nef);
    tree->Branch(Form("%sak4pfjets_muf", prefix_.c_str()) , &ak4pfjets_muf);
    tree->Branch(Form("%sak4pfjets_cm", prefix_.c_str()) , &ak4pfjets_cm);
    tree->Branch(Form("%sak4pfjets_nm", prefix_.c_str()) , &ak4pfjets_nm);
}

void JetTree::SetAK4Branches_Other(TTree* tree)
{
//these are not completely useless but could go?
    tree->Branch(Form("%snGoodGenJets", prefix_.c_str()) , &nGoodGenJets); 
    tree->Branch(Form("%snfailjets", prefix_.c_str()) , &nfailjets);
    tree->Branch(Form("%sak4_htssm", prefix_.c_str()) , &ak4_htssm);                                                                 
    tree->Branch(Form("%sak4_htosm", prefix_.c_str()) , &ak4_htosm);
    tree->Branch(Form("%sak4pfjets_qg_disc", prefix_.c_str()) , &ak4pfjets_qg_disc);
    tree->Branch(Form("%sak4pfjets_MEDbjet_pt",prefix_.c_str()) , &ak4pfjets_MEDbjet_pt);                                            
    tree->Branch(Form("%sak4pfjets_leadMEDbjet_pt",prefix_.c_str()) , &ak4pfjets_leadMEDbjet_pt);   
    tree->Branch(Form("%sak4pfjets_medium_pfid", prefix_.c_str()) , &ak4pfjets_medium_pfid);
    tree->Branch(Form("%sak4pfjets_tight_pfid", prefix_.c_str()) , &ak4pfjets_tight_pfid);
    tree->Branch(Form("%sak4pfjets_puid", prefix_.c_str()) , &ak4pfjets_puid); 
    tree->Branch(Form("%sak4pfjets_mc3dr", prefix_.c_str()) , &ak4pfjets_mc3dr);
    tree->Branch(Form("%sak4pfjets_mc3id", prefix_.c_str()) , &ak4pfjets_mc3id);                                        
    tree->Branch(Form("%sak4pfjets_mc3idx", prefix_.c_str()) , &ak4pfjets_mc3idx);
    tree->Branch(Form("%sak4pfjets_mcmotherid", prefix_.c_str()) , &ak4pfjets_mcmotherid);
}
