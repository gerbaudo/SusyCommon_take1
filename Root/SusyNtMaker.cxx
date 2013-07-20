#include "egammaAnalysisUtils/CaloIsoCorrection.h"
#include "TauCorrections/TauCorrections.h"
//#include "SUSYTools/MV1.h"

#include "MultiLep/MuonTools.h"
#include "MultiLep/ElectronTools.h"
#include "MultiLep/SusyGridCrossSectionTools.h"
#include "MultiLep/TruthTools.h"

#include "SusyCommon/SusyNtMaker.h"
#include "SusyNtuple/SusyNtTools.h"
#include "SusyNtuple/WhTruthExtractor.h"

using namespace std;

#define GeV 1000.

/*--------------------------------------------------------------------------------*/
// SusyNtMaker Constructor
/*--------------------------------------------------------------------------------*/
SusyNtMaker::SusyNtMaker() : m_fillNt(true),
			     m_filter(true),
                             m_nLepFilter(0),
                             m_nLepTauFilter(2),
                             m_filterTrigger(false),
                             m_saveContTaus(false),
                             m_isWhSample(false),
                             m_hDecay(-1),
                             m_hasSusyProp(false)
{
  n_base_ele=0;
  n_base_muo=0;
  n_base_tau=0;
  n_base_jet=0;
  n_sig_ele=0;
  n_sig_muo=0;
  n_sig_tau=0;
  n_sig_jet=0;
  n_evt_initial=0;
  n_evt_susyProp=0;
  n_evt_grl=0;
  n_evt_ttcVeto=0;
  n_evt_WwSherpa=0;
  n_evt_tileTrip=0;
  n_evt_larErr=0;
  n_evt_tileErr=0;
  n_evt_larHole=0;
  n_evt_hotSpot=0;
  n_evt_badJet=0;
  n_evt_goodVtx=0;
  n_evt_badMu=0;
  n_evt_cosmic=0;
  n_evt_1Lep=0;
  n_evt_2Lep=0;
  n_evt_3Lep=0;
  n_evt_saved=0;
}
/*--------------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------------*/
SusyNtMaker::~SusyNtMaker()
{
}
/*--------------------------------------------------------------------------------*/
// The Begin() function is called at the start of the query.
// When running with PROOF Begin() is only called on the client.
// The tree argument is deprecated (on PROOF 0 is passed).
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::Begin(TTree* /*tree*/)
{
  SusyD3PDAna::Begin(0);
  if(m_dbg) cout << "SusyNtMaker::Begin" << endl;

  if(m_fillNt){

    // Open the output tree
    m_outTreeFile = new TFile("susyNt.root", "recreate");
    m_outTree = new TTree("susyNt", "susyNt");

    // Set autosave size (determines how often tree writes to disk)
    m_outTree->SetAutoSave(10000000);
    // Max tree size determines when a new file and tree are written
    m_outTree->SetMaxTreeSize(3000000000u);
    // Set all branches active for writing, for now.
    // Later, add switch for systematics
    m_susyNt.SetActive();
    m_susyNt.WriteTo(m_outTree);

  }

  m_isSusySample = m_sample.Contains("DGemt") || m_sample.Contains("DGstau") || m_sample.Contains("RPV") ||
                   m_sample.Contains("simplifiedModel") || m_sample.Contains("pMSSM");
  m_isWhSample = m_sample.Contains("simplifiedModel_wA_noslep_WH");

  // create histograms for cutflow
  // Raw event weights
  h_rawCutFlow = makeCutFlow("rawCutFlow", "rawCutFlow;Cuts;Events");
  // Generator event weights
  h_genCutFlow = makeCutFlow("genCutFlow", "genCutFlow;Cuts;Events");

  // Start the timer
  m_timer.Start();
}
/*--------------------------------------------------------------------------------*/
TH1F* SusyNtMaker::makeCutFlow(const char* name, const char* title)
{
  //TH1F* h = new TH1F(name, title, 9, -0.5, 3.5);
  TH1F* h = new TH1F(name, title, 15, 0., 15.);
  h->GetXaxis()->SetBinLabel(1, "Initial");
  h->GetXaxis()->SetBinLabel(2, "SusyProp Veto");
  h->GetXaxis()->SetBinLabel(3, "GRL");
  h->GetXaxis()->SetBinLabel(4, "LAr Error");
  h->GetXaxis()->SetBinLabel(5, "Tile Error");
  h->GetXaxis()->SetBinLabel(6, "TTC Veto");
  h->GetXaxis()->SetBinLabel(7, "Good Vertex");
  h->GetXaxis()->SetBinLabel(8, "Buggy WWSherpa");
  h->GetXaxis()->SetBinLabel(9, "Hot Spot");
  h->GetXaxis()->SetBinLabel(10, "Bad Jet");
  h->GetXaxis()->SetBinLabel(11, "Bad Muon");
  h->GetXaxis()->SetBinLabel(12, "Cosmic");
  h->GetXaxis()->SetBinLabel(13, ">=1 lep");
  h->GetXaxis()->SetBinLabel(14, ">=2 lep");
  h->GetXaxis()->SetBinLabel(15, "==3 lep");
  return h;
}
/*--------------------------------------------------------------------------------*/
TH1F* SusyNtMaker::getProcCutFlow(int signalProcess)
{
  // Look for it on the map
  map<int,TH1F*>::const_iterator it = m_procCutFlows.find(signalProcess);
  // New process
  if(it == m_procCutFlows.end()){
    stringstream stream;
    stream << signalProcess;
    string name = "procCutFlow" + stream.str();
    return m_procCutFlows[signalProcess] = makeCutFlow(name.c_str(), 
                                                       (name+";Cuts;Events").c_str());
  }
  // Already saved process
  else{
    return it->second;
  }
}

/*--------------------------------------------------------------------------------*/
// Main process loop function - This is just an example for testing
/*--------------------------------------------------------------------------------*/
Bool_t SusyNtMaker::Process(Long64_t entry)
{
  // Communicate the entry number to the interface objects
  GetEntry(entry);

  static Long64_t chainEntry = -1;
  chainEntry++;
  if(m_dbg || chainEntry%5000==0)
  {
    cout << "**** Processing entry " << setw(6) << chainEntry
         << " run " << setw(6) << d3pd.evt.RunNumber()
         << " event " << setw(7) << d3pd.evt.EventNumber() << " ****" << endl;
  }

  if(selectEvent() && m_fillNt){
    int bytes = m_outTree->Fill(); 
    if(bytes==-1){
      cout << "SusyNtMaker ERROR filling tree!  Abort!" << endl;
      abort();
    }
    n_evt_saved++;
  }

  return kTRUE;
}

/*--------------------------------------------------------------------------------*/
// The Terminate() function is the last function to be called during
// a query. It always runs on the client, it can be used to present
// the results graphically or save the results to file.
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::Terminate()
{
  // Stop the timer
  m_timer.Stop();

  SusyD3PDAna::Terminate();
  if(m_dbg) cout << "SusyNtMaker::Terminate" << endl;

  cout << endl;
  cout << "Object counter" << endl;
  cout << "  BaseEle   " << n_base_ele    << endl;
  cout << "  BaseMuo   " << n_base_muo    << endl;
  cout << "  BaseTau   " << n_base_tau    << endl;
  cout << "  BaseJet   " << n_base_jet    << endl;
  cout << "  SigEle    " << n_sig_ele     << endl;
  cout << "  SigMuo    " << n_sig_muo     << endl;
  cout << "  SigTau    " << n_sig_tau     << endl;
  cout << "  SigJet    " << n_sig_jet     << endl;
  cout << endl;
  cout << "Event counter" << endl;
  cout << "  Initial   " << n_evt_initial << endl;
  cout << "  SusyProp  " << n_evt_susyProp<< endl;
  cout << "  GRL       " << n_evt_grl     << endl;
  cout << "  LarErr    " << n_evt_larErr  << endl;
  cout << "  TileErr   " << n_evt_tileErr  << endl;
  cout << "  TTC Veto  " << n_evt_ttcVeto << endl;
  cout << "  GoodVtx   " << n_evt_goodVtx << endl;
  cout << "  WW Sherpa " << n_evt_WwSherpa<< endl;
  cout << "  TileTrip  " << n_evt_tileTrip<< endl;
  //cout << "  LarHole   " << n_evt_larHole << endl;
  cout << "  HotSpot   " << n_evt_hotSpot << endl;
  cout << "  BadJet    " << n_evt_badJet  << endl;
  cout << "  BadMuon   " << n_evt_badMu   << endl;
  cout << "  Cosmic    " << n_evt_cosmic  << endl;
  cout << "  >=1 Lep   " << n_evt_1Lep    << endl;
  cout << "  >=2 Lep   " << n_evt_2Lep    << endl;
  cout << "  ==3 Lep   " << n_evt_3Lep    << endl;
  cout << endl;

  if(m_fillNt){

    // Save the output tree
    m_outTreeFile = m_outTree->GetCurrentFile();
    m_outTreeFile->Write(0, TObject::kOverwrite);
    cout << "susyNt tree saved to " << m_outTreeFile->GetName() << endl;
    m_outTreeFile->Close();

  }

  // Report timer
  double realTime = m_timer.RealTime();
  double cpuTime  = m_timer.CpuTime();
  int hours = int(realTime / 3600);
  realTime -= hours * 3600;
  int min   = int(realTime / 60);
  realTime -= min * 60;
  int sec   = int(realTime);

  float speed = n_evt_initial/m_timer.RealTime()/1000;

  printf("---------------------------------------------------\n");
  printf(" Number of events processed: %d \n",n_evt_initial);
  printf(" Number of events saved:     %d \n",n_evt_saved);
  printf("\t Analysis time: Real %d:%02d:%02d, CPU %.3f      \n", hours, min, sec, cpuTime);
  printf("\t Analysis speed [kHz]: %2.3f                     \n",speed);
  printf("---------------------------------------------------\n\n");
}

/*--------------------------------------------------------------------------------*/
// Select event
/*--------------------------------------------------------------------------------*/
bool SusyNtMaker::selectEvent()
{
  if(m_dbg>=5) cout << "selectEvent" << endl;


  m_susyObj.Reset();
  clearObjects();
  m_susyNt.clear();

  // Susy final state
  //m_susyFinalState = m_isSusySample? get_finalState(d3pd.truth) : -1;
  m_susyFinalState = m_isSusySample? get_finalState(d3pd.evt.SUSY_Spart1_pdgId(), 
                                                    d3pd.evt.SUSY_Spart2_pdgId()) : -1;
  m_hDecay = m_isWhSample ? WhTruthExtractor().update(d3pd.truth.pdgId(),
						      d3pd.truth.child_index(),
						      d3pd.truth.parent_index()) : -1;
  m_hasSusyProp = (m_isSusySample ?
                   SusyNtTools::eventHasSusyPropagators(*d3pd.truth.pdgId(), *d3pd.truth.parent_index()) :
                   false);
  TH1F* h_procCutFlow = m_isSusySample? getProcCutFlow(m_susyFinalState) : 0;
  float w = m_isMC? d3pd.truth.event_weight() : 1;

  // Cut index
  int cut = 0;

  // Use a preprocessor macro to assist in filling the cutflow hists
  #define FillCutFlow()             \
    do{                             \
      h_rawCutFlow->Fill(cut);      \
      h_genCutFlow->Fill(cut, w);   \
      if(m_isSusySample) h_procCutFlow->Fill(cut, w);  \
      cut++;                        \
    } while(0)

  n_evt_initial++;
  FillCutFlow();
 
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
  // Obj Independent checks

  checkEventCleaning();

  // susyProp (just counts, doesn't drop)
  if(!m_hasSusyProp) { FillCutFlow(); n_evt_susyProp++; }

  // grl
  if(m_filter && (m_cutFlags & ECut_GRL) == 0) return false;
  FillCutFlow();
  n_evt_grl++;

  // larErr
  if(m_filter && (m_cutFlags & ECut_LarErr) == 0) return false;
  FillCutFlow();
  n_evt_larErr++;

  // tileErr
  if(m_filter && (m_cutFlags & ECut_TileErr) == 0) return false;
  FillCutFlow();
  n_evt_tileErr++;

  // Incomplete TTC event veto
  if(m_filter && (m_cutFlags & ECut_TTC) == 0) return false;
  FillCutFlow();
  n_evt_ttcVeto++;

  // primary vertex cut is actually filtered
  if(m_filter && (m_cutFlags & ECut_GoodVtx) == 0) return false;
  FillCutFlow();
  n_evt_goodVtx++;

  // Sherpa WW fix, remove radiative b-quark processes that overlap with single top
  if(m_filter && m_isMC && isBuggyWWSherpaSample(d3pd.truth.channel_number()) && 
     hasRadiativeBQuark(d3pd.truth.pdgId(), d3pd.truth.status())) return false;
  FillCutFlow();
  n_evt_WwSherpa++;

  // Tile trip cut
  if(m_filter && (m_cutFlags & ECut_TileTrip) == 0) return false;
  FillCutFlow();
  n_evt_tileTrip++;

  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//  
  // Get Nominal Objects
  
  selectObjects();
  buildMet();
  //evtCheck();
  checkObjectCleaning();


  // These next cuts are not used to filter the SusyNt because they depend on systematics.
  // Instead, they are simply used for the counters, for comparing the cutflow

  // Tile hot spot
  if(m_cutFlags & ECut_HotSpot)
  {
    FillCutFlow();
    n_evt_hotSpot++;

    // Bad jet cut
    if(m_cutFlags & ECut_BadJet)
    {
      FillCutFlow();
      n_evt_badJet++;

      // Bad muon veto
      if(m_cutFlags & ECut_BadMuon)
      {
	FillCutFlow();
        n_evt_badMu++;

        // Cosmic muon veto
        if(m_cutFlags & ECut_Cosmic)
        {
          FillCutFlow();
          n_evt_cosmic++;
  
          n_base_ele += m_baseElectrons.size();
          n_base_muo += m_baseMuons.size();
          n_base_tau += m_baseTaus.size();
          n_base_jet += m_baseJets.size();
          n_sig_ele += m_sigElectrons.size();
          n_sig_muo += m_sigMuons.size();
          n_sig_tau += m_sigTaus.size();
          n_sig_jet += m_sigJets.size();

          // Lepton multiplicity
          uint nSigLep = m_sigElectrons.size() + m_sigMuons.size();
          //cout << "nSigLep " << nSigLep << endl;
          if(nSigLep >= 1){
            FillCutFlow();
            n_evt_1Lep++;
            if(nSigLep >= 2){
              FillCutFlow();
              n_evt_2Lep++;
              if(nSigLep == 3){
                FillCutFlow();
                n_evt_3Lep++;
                //cout << "Event " << d3pd.evt.EventNumber() << endl;
              }
            }
          }
        }
      }
    }
  }

  // Match the triggers
  // Will this work for systematic leptons?
  // I think so.
  matchTriggers();

  // Setup reco truth matching
  if(m_isMC){
    m_recoTruthMatch = RecoTauMatch(0.1, d3pd.truth.channel_number(),
                                    d3pd.truth.n(), d3pd.truth.barcode(), d3pd.truth.status(), d3pd.truth.pdgId(),
                                    d3pd.truth.parents(), d3pd.truth.children(),
                                    d3pd.truth.pt(), d3pd.truth.eta(), d3pd.truth.phi(), d3pd.truth.m(),
                                    d3pd.jet.pt(), d3pd.jet.eta(), d3pd.jet.phi(), d3pd.jet.m(),
                                    d3pd.jet.flavor_truth_label(),
                                    d3pd.ele.pt(), d3pd.ele.eta(), d3pd.ele.phi(), d3pd.ele.m(),
                                    d3pd.ele.type(), d3pd.ele.origin(),
                                    d3pd.truthMu.pt(), d3pd.truthMu.eta(), d3pd.truthMu.phi(), d3pd.truthMu.m(),
                                    d3pd.truthMu.type(), d3pd.truthMu.origin(),
                                    d3pd.trk.pt(), d3pd.trk.eta(), d3pd.trk.phi_wrtPV(), d3pd.trk.mc_barcode());                    
  }

  if(m_fillNt){

    // This will fill the pre selected
    // objects prior to overlap removal
    fillNtVars();

    // If it is mc and option for sys is set
    if(m_isMC && m_sys) doSystematic(); 

    // TODO: add a command line option for controlling this filtering, 
    // so that we don't keep committing conflicting changes...

    // For filling the output tree, filter on number of saved light leptons and taus
    if(m_filter){ 
      uint nLepSaved = m_susyNt.ele()->size() + m_susyNt.muo()->size();
      uint nTauSaved = m_susyNt.tau()->size();
      if(nLepSaved < m_nLepFilter) return false;
      if((nLepSaved + nTauSaved) < m_nLepTauFilter) return false;

    }

    // Trigger filtering, only save events for which one of our triggers has fired
    if(m_filterTrigger && m_evtTrigFlags == 0) return false;
  }
  
  return true;
}

/*--------------------------------------------------------------------------------*/
// Fill SusyNt variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillNtVars()
{
  fillEventVars();
  fillLeptonVars();
  fillJetVars();
  fillTauVars();
  fillMetVars();
  fillPhotonVars();
  if(m_isMC && getSelectTruthObjects() ) {
    fillTruthParticleVars();
    fillTruthJetVars();
    fillTruthMetVars();
  }
}

/*--------------------------------------------------------------------------------*/
// Fill Event variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillEventVars()
{
  if(m_dbg>=5) cout << "fillEventVars" << endl;
  Susy::Event* evt = m_susyNt.evt();

  evt->run              = d3pd.evt.RunNumber();
  evt->event            = d3pd.evt.EventNumber();
  evt->lb               = d3pd.evt.lbn();
  evt->stream           = m_stream;

  evt->isMC             = m_isMC;
  evt->mcChannel        = m_isMC? d3pd.truth.channel_number() : 0;
  evt->w                = m_isMC? d3pd.truth.event_weight()   : 1;

  evt->larError         = d3pd.evt.larError();

  evt->nVtx             = getNumGoodVtx();
  evt->avgMu            = d3pd.evt.averageIntPerXing();

  evt->hfor             = m_isMC? getHFORDecision() : -1;

  // SUSY final state
  evt->susyFinalState   = m_susyFinalState;

  float mZ = -1.0, mZtruthMax = 40.0;
  if(m_isMC){
    int dsid = d3pd.truth.channel_number();
    if(IsAlpgenLowMass(dsid) || IsAlpgenPythiaZll(dsid)) mZ = MllForAlpgen(&d3pd.truth);
    else if(IsSherpaZll(dsid)) mZ = MllForSherpa(&d3pd.truth);
  }
  evt->mllMcTruth       = mZ;
  evt->passMllForAlpgen = m_isMC ? (mZ < mZtruthMax) : true;
  evt->hDecay           = m_hDecay;
  evt->eventWithSusyProp= m_hasSusyProp;

  evt->trigFlags        = m_evtTrigFlags;

  evt->wPileup          = m_isMC? getPileupWeight() : 1;
  evt->wPileup_up       = m_isMC? getPileupWeightUp() : 1;
  evt->wPileup_dn       = m_isMC? getPileupWeightDown() : 1;
  evt->wPileupAB3       = m_isMC? getPileupWeightAB3() : 1;
  evt->wPileupAB        = m_isMC? getPileupWeightAB() : 1;
  //evt->wPileupIL        = m_isMC? getPileupWeightIL() : 1;
  evt->wPileupAE        = m_isMC? getPileupWeightAE() : 1;
  evt->xsec             = m_isMC? getXsecWeight() : 1;
  evt->errXsec          = m_isMC? m_errXsec : 1;
  evt->sumw             = m_isMC? m_sumw : 1;

  evt->pdf_id1          = m_isMC? d3pd.gen.pdf_id1()->at(0)    : 0;
  evt->pdf_id2          = m_isMC? d3pd.gen.pdf_id2()->at(0)    : 0;
  evt->pdf_x1           = m_isMC? d3pd.gen.pdf_x1()->at(0)     : 0;
  evt->pdf_x2           = m_isMC? d3pd.gen.pdf_x2()->at(0)     : 0;
  evt->pdf_scale        = m_isMC? d3pd.gen.pdf_scale()->at(0)  : 0;

  evt->pdfSF            = m_isMC? getPDFWeight8TeV() : 1;

  m_susyNt.evt()->cutFlags[NtSys_NOM] = m_cutFlags;
}

/*--------------------------------------------------------------------------------*/
// Fill lepton variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillLeptonVars()
{
  // loop over preselected leptons and fill the output tree
  for(uint iLep=0; iLep < m_preLeptons.size(); iLep++){
    const LeptonInfo* lep = & m_preLeptons[iLep];
    if(lep->isElectron()) fillElectronVars(lep);
    else fillMuonVars(lep);
  }
}
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillElectronVars(const LeptonInfo* lepIn)
{
  if(m_dbg>=5) cout << "fillElectronVars" << endl;
  m_susyNt.ele()->push_back( Susy::Electron() );
  Susy::Electron* eleOut = & m_susyNt.ele()->back();
  const ElectronElement* element = lepIn->getElectronElement();

  // LorentzVector
  const TLorentzVector* lv = lepIn->lv();
  float pt  = lv->Pt() / GeV;
  float eta = lv->Eta();
  float phi = lv->Phi();
  float m   = lv->M() / GeV;

  eleOut->SetPtEtaPhiM(pt, eta, phi, m);
  eleOut->pt            = pt;
  eleOut->eta           = eta;
  eleOut->phi           = phi;
  eleOut->m             = m;

  // TODO: clean this up, group things together, etc.

  eleOut->ptcone20      = element->ptcone20()/GeV;
  eleOut->ptcone30      = element->ptcone30()/GeV;
  eleOut->q             = element->charge();
  eleOut->mcType        = m_isMC? element->type() : 0;
  eleOut->mcOrigin      = m_isMC? element->origin() : 0;
  eleOut->clusE         = element->cl_E()/GeV;
  eleOut->clusEta       = element->cl_eta();
  eleOut->clusPhi       = element->cl_phi();
  eleOut->trackPt       = element->trackpt()/GeV;

  // Check for charge flip
  eleOut->isChargeFlip          = m_isMC? m_recoTruthMatch.isChargeFlip(*lv, element->charge()) : false;
  eleOut->matched2TruthLepton   = m_isMC? m_recoTruthMatch.Matched2TruthLepton(*lv) : false;
  //eleOut->truthMatchType      = m_isMC? m_recoTruthMatch.fakeType(*lv, element->origin(), element->type()) : -1;
  eleOut->truthType             = m_isMC? m_recoTruthMatch.fakeType(*lv, element->origin(), element->type()) : -1;

  // IsEM quality flags - no need to recalculate them
  eleOut->mediumPP    = element->mediumPP();
  eleOut->tightPP     = element->tightPP();

  eleOut->d0            = element->trackd0pv();
  eleOut->errD0         = element->tracksigd0pv();
  eleOut->z0            = element->trackz0pv();
  eleOut->errZ0         = element->tracksigz0pv();

  eleOut->d0Unbiased    = element->trackIPEstimate_d0_unbiasedpvunbiased();
  eleOut->errD0Unbiased = element->trackIPEstimate_sigd0_unbiasedpvunbiased();
  eleOut->z0Unbiased    = element->trackIPEstimate_z0_unbiasedpvunbiased();
  eleOut->errZ0Unbiased = element->trackIPEstimate_sigz0_unbiasedpvunbiased();

  // New iso variables!! 
  // Corrected topo iso is available in the susy d3pd, apparently calculated using the cluster E.
  // However, the CaloIsoCorrection header says to use the energy after scaling/smearing...
  // So, which should we use?
  // TODO: come back to this, open a discussion somewhere.
  // For now, I will just use the cluster E, which I suspect people will use anyway (even if mistaken)

  // Corrected etcone has Pt and ED corrections
  eleOut->etcone30Corr  = CaloIsoCorrection::GetPtEDCorrectedIsolation(element->Etcone40(), 
                                                                       element->Etcone40_ED_corrected(), 
                                                                       element->cl_E(), element->etas2(), 
                                                                       element->etap(), element->cl_eta(), 0.3, 
                                                                       m_isMC, element->Etcone30())/GeV;

  // Corrected topoEtcone has Pt and ED corrections.  Use D3PD branch for now
  //float topo            = CaloIsoCorrection::GetPtEDCorrectedTopoIsolation(element->ED_median(), element->cl_E(), 
  //                                                                         element->etas2(), element->etap(), 
  //                                                                         element->cl_eta(), 0.3, m_isMC, 
  //                                                                         element->topoEtcone30())/GeV;
  eleOut->topoEtcone30Corr      = element->topoEtcone30_corrected()/GeV;
  
  // Trigger flags
  eleOut->trigFlags     = m_eleTrigFlags[ lepIn->idx() ];

  // Efficiency scale factor.  For now, use tightPP if electrons is tightPP, otherwise mediumPP
  //int set               = eleOut->tightPP? 7 : 6;
  //eleOut->effSF         = m_isMC? m_susyObj.GetSignalElecSF   ( element->cl_eta(), lepIn->lv()->Pt(), set ) : 1;
  //eleOut->errEffSF      = m_isMC? m_susyObj.GetSignalElecSFUnc( element->cl_eta(), lepIn->lv()->Pt(), set ) : 1;

  // Tight electron SFs can come directly from SUSYTools
  // To get the SF uncert using GetSignalElecSF, we must get the shifted value and take the difference
  if(eleOut->tightPP){
    eleOut->effSF       = m_isMC? 
                          m_susyObj.GetSignalElecSF(element->cl_eta(), lepIn->lv()->Pt(), true, true, false) : 1;
    eleOut->errEffSF    = m_isMC?
                          m_susyObj.GetSignalElecSF(element->cl_eta(), lepIn->lv()->Pt(), true, true, false, 
                                                    200841, SystErr::EEFFUP) - eleOut->effSF : 0;
  }

  // For the medium SF, need to use our own function
  else{
    float sf = 1, uncert = 0;
    if (m_isMC) get_electron_eff_sf(sf, uncert, element->cl_eta(), lepIn->lv()->Pt(), true, true, false, m_isAF2,
                                    m_susyObj.GetElectron_recoSF_Class(), m_eleMediumSFTool, 0);
    eleOut->effSF       = sf;
    eleOut->errEffSF    = uncert;
  }

  // Do we need this??
  eleOut->idx           = lepIn->idx();
}

/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillMuonVars(const LeptonInfo* lepIn)
{
  if(m_dbg>=5) cout << "fillMuonVars" << endl;
  m_susyNt.muo()->push_back( Susy::Muon() );
  Susy::Muon* muOut = & m_susyNt.muo()->back();
  const MuonElement* element = lepIn->getMuonElement();

  // LorentzVector
  const TLorentzVector* lv = lepIn->lv();
  float pt  = lv->Pt() / GeV;
  float eta = lv->Eta();
  float phi = lv->Phi();
  float m   = lv->M() / GeV;
  muOut->SetPtEtaPhiM(pt, eta, phi, m);
  muOut->pt  = pt;
  muOut->eta = eta;
  muOut->phi = phi;
  muOut->m   = m;

  muOut->q              = element->charge();
  muOut->ptcone20       = element->ptcone20()/GeV;
  muOut->ptcone30       = element->ptcone30()/GeV;
  muOut->etcone30       = element->etcone30()/GeV;

  muOut->ptcone30ElStyle= m_d3pdTag>=D3PD_p1181? element->ptcone30_trkelstyle()/GeV : 0;

  // ID coordinates
  muOut->idTrackPt      = element->id_qoverp_exPV() != 0.? 
                          fabs(sin(element->id_theta_exPV())/element->id_qoverp_exPV())/GeV : 0.;
  muOut->idTrackEta     = -1.*log(tan(element->id_theta_exPV()/2.));
  muOut->idTrackPhi     = element->id_phi_exPV();
  muOut->idTrackQ       = element->id_qoverp_exPV() < 0 ? -1 : 1;

  // MS coordinates
  muOut->msTrackPt      = element->ms_qoverp() != 0.? 
                          fabs(sin(element->ms_theta())/element->ms_qoverp())/GeV : 0.;
  muOut->msTrackEta     = -1.*log(tan(element->ms_theta()/2.));
  muOut->msTrackPhi     = element->ms_phi();
  muOut->msTrackQ       = element->ms_qoverp() < 0 ? -1 : 1;

  muOut->d0             = element->d0_exPV();
  muOut->errD0          = sqrt(element->cov_d0_exPV());
  muOut->z0             = element->z0_exPV();
  muOut->errZ0          = sqrt(element->cov_z0_exPV());

  muOut->d0Unbiased     = element->trackIPEstimate_d0_unbiasedpvunbiased();
  muOut->errD0Unbiased  = element->trackIPEstimate_sigd0_unbiasedpvunbiased();
  muOut->z0Unbiased     = element->trackIPEstimate_z0_unbiasedpvunbiased();
  muOut->errZ0Unbiased  = element->trackIPEstimate_sigz0_unbiasedpvunbiased();

  muOut->isCombined     = element->isCombinedMuon();
  
  // theta_exPV. Not sure if necessary.
  muOut->thetaPV        = element->theta_exPV();

  // Cleaning flags
  muOut->isBadMuon      = m_susyObj.IsBadMuon(element->qoverp_exPV(), element->cov_qoverp_exPV(), 0.2);
  muOut->isCosmic       = m_susyObj.IsCosmicMuon(element->z0_exPV(), element->d0_exPV(), 1., 0.2);

  // Truth flags
  if(m_isMC){
    muOut->mcType       = element->type();
    muOut->mcOrigin     = element->origin();
    // If type and origin are zero, try matching to the muons in the truthMuon block
    // This might not actually do anything
    if(element->type()==0 && element->origin()==0 && element->truth_barcode()!=0){
      const TruthMuonElement* trueMuon = getMuonTruth(&d3pd.muo, lepIn->idx(), &d3pd.truthMu);
      muOut->mcType     = trueMuon? trueMuon->type()   : 0;
      muOut->mcOrigin   = trueMuon? trueMuon->origin() : 0;
    }
    muOut->matched2TruthLepton  = m_recoTruthMatch.Matched2TruthLepton(*lv);
    muOut->truthType            = m_recoTruthMatch.fakeType(*lv, muOut->mcOrigin, muOut->mcType);
  }

  muOut->trigFlags      = m_muoTrigFlags[ lepIn->idx() ];

  // Syntax of the GetSignalMuonSF has changed.  Now, the same method is used to get the nominal and shifted value.
  // So, in order to store the uncert, I take the shifted value minus the nominal, and save that.
  muOut->effSF          = m_isMC? m_susyObj.GetSignalMuonSF(lepIn->idx()) : 1;
  muOut->errEffSF       = m_isMC? m_susyObj.GetSignalMuonSF(lepIn->idx(), SystErr::MEFFUP) - muOut->effSF : 0;

  // Do we need this??
  muOut->idx            = lepIn->idx();
}

/*--------------------------------------------------------------------------------*/
// Fill jet variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillJetVars()
{
  if(m_dbg>=5) cout << "fillJetVars" << endl;
  // Loop over selected jets and fill output tree
  for(uint iJet=0; iJet<m_preJets.size(); iJet++){
    int jetIndex = m_preJets[iJet];  

    fillJetVar(jetIndex);
  }
}
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillJetVar(int jetIdx)
{
  const JetElement* element = & d3pd.jet[jetIdx];
  m_susyNt.jet()->push_back( Susy::Jet() );
  Susy::Jet* jetOut = & m_susyNt.jet()->back();
  
  const TLorentzVector* lv = & m_susyObj.GetJetTLV(jetIdx);
  float pt  = lv->Pt() / GeV;
  float eta = lv->Eta();
  float phi = lv->Phi();
  float m   = lv->M() / GeV;
  jetOut->SetPtEtaPhiM(pt, eta, phi, m);
  jetOut->pt  = pt;
  jetOut->eta = eta;
  jetOut->phi = phi;
  jetOut->m   = m;

  jetOut->detEta        = element->constscale_eta();
  jetOut->idx           = jetIdx;
  jetOut->jvf           = element->jvtxf();
  jetOut->truthLabel    = m_isMC? element->flavor_truth_label() : 0;

  // truth jet matching
  jetOut->matchTruth    = m_isMC? matchTruthJet(jetIdx) : false;

  // btag weights
  jetOut->sv0           = element->flavor_weight_SV0();
  jetOut->combNN        = element->flavor_weight_JetFitterCOMBNN();
  jetOut->mv1           = element->flavor_weight_MV1();

  jetOut->bch_corr_jet  = element->BCH_CORR_JET();
  jetOut->isBadVeryLoose= JetID::isBadJet(JetID::VeryLooseBad,
                                          element->emfrac(), 
                                          element->hecf(), 
                                          element->LArQuality(), 
                                          element->HECQuality(),
                                          element->Timing(), 
                                          element->sumPtTrk_pv0_500MeV()/GeV, 
                                          element->emscale_eta(), pt,
                                          element->fracSamplingMax(), 
                                          element->NegativeE(), 
                                          element->AverageLArQF());
  jetOut->isHotTile     = m_susyObj.isHotTile(d3pd.evt.RunNumber(), element->fracSamplingMax(), 
                                              element->SamplingMax(), eta, phi);

  // Save the met weights for the jets
  // by checking status word similar to
  // what is done in met utility
 int sWord = element->MET_Egamma10NoTau_statusWord().at(0);
  bool passSWord = (MissingETTags::DEFAULT == sWord);       // Note assuming default met..
  
  // 0th element is what we care about
  jetOut->met_wpx = passSWord ? element->MET_Egamma10NoTau_wpx().at(0) : 0;
  jetOut->met_wpy = passSWord ? element->MET_Egamma10NoTau_wpy().at(0) : 0;
  

}

/*--------------------------------------------------------------------------------*/
// Fill Photon variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillPhotonVars()
{
  if(m_dbg>=5) cout << "fillPhotonVars" << endl;

  // Loop over photons
  for(uint iPh=0; iPh<m_sigPhotons.size(); iPh++){
    int phIndex = m_sigPhotons[iPh];  

    fillPhotonVar(phIndex);
  }
}
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillPhotonVar(int phIdx)
{
  if(m_dbg>=5) cout << "fillPhotonVar" << endl;
  m_susyNt.pho()->push_back( Susy::Photon() );
  Susy::Photon* phoOut = & m_susyNt.pho()->back();
  const PhotonElement* element = & d3pd.pho[phIdx];


  // Set TLV
  const TLorentzVector* phTLV = & m_susyObj.GetPhotonTLV(phIdx);
  float pt  = phTLV->Pt() / GeV;
  float E   = phTLV->E()  / GeV;
  float eta = phTLV->Eta();
  float phi = phTLV->Phi();
  
  phoOut->SetPtEtaPhiE(pt, eta, phi, E);
  phoOut->pt  = pt;
  phoOut->eta = eta;
  phoOut->phi = phi;
  phoOut->m   = 0.;
  
  // Save conversion info
  phoOut->isConv = element->isConv();

  // Miscellaneous 
  phoOut->idx    = phIdx;
}

/*--------------------------------------------------------------------------------*/
// Fill Tau variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillTauVars()
{
  if(m_dbg>=5) cout << "fillTauVars" << endl;

  // Loop over selected taus
  vector<int>& saveTaus = m_saveContTaus? m_contTaus : m_preTaus;
  for(uint iTau=0; iTau < saveTaus.size(); iTau++){
    int tauIdx = saveTaus[iTau];  

    fillTauVar(tauIdx);
  }
}
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillTauVar(int tauIdx)
{
  if(m_dbg>=5) cout << "fillTauVar" << endl;
  m_susyNt.tau()->push_back( Susy::Tau() );
  Susy::Tau* tauOut = & m_susyNt.tau()->back();
  const TauElement* element = & d3pd.tau[tauIdx];

  // Set TLV
  //const TLorentzVector* tauLV = & m_tauLVs.at(tauIdx);
  const TLorentzVector* tauLV = & m_susyObj.GetTauTLV(tauIdx);
  float pt  = tauLV->Pt() / GeV;
  float eta = tauLV->Eta();
  float phi = tauLV->Phi();
  float m   = tauLV->M() / GeV;
  
  tauOut->SetPtEtaPhiM(pt, eta, phi, m);
  tauOut->pt    = pt;
  tauOut->eta   = eta;
  tauOut->phi   = phi;
  tauOut->m     = m;

  tauOut->q                     = element->charge();
  tauOut->author                = element->author();
  tauOut->nTrack                = element->numTrack();
  tauOut->eleBDT                = element->BDTEleScore();
  tauOut->jetBDT                = element->BDTJetScore();

  tauOut->jetBDTSigLoose        = element->JetBDTSigLoose();
  tauOut->jetBDTSigMedium       = element->JetBDTSigMedium();
  tauOut->jetBDTSigTight        = element->JetBDTSigTight();

  // New ele BDT corrections 
  //tauOut->eleBDTLoose           = element->EleBDTLoose();
  //tauOut->eleBDTMedium          = element->EleBDTMedium();
  //tauOut->eleBDTTight           = element->EleBDTTight();
  tauOut->eleBDTLoose           = m_susyObj.GetCorrectedEleBDTFlag(SUSYTau::TauLoose, element->EleBDTLoose(), 
                                                                   element->BDTEleScore(), element->numTrack(), 
                                                                   tauLV->Pt(), element->leadTrack_eta());
  tauOut->eleBDTMedium          = m_susyObj.GetCorrectedEleBDTFlag(SUSYTau::TauMedium, element->EleBDTMedium(), 
                                                                   element->BDTEleScore(), element->numTrack(), 
                                                                   tauLV->Pt(), element->leadTrack_eta());
  tauOut->eleBDTTight           = m_susyObj.GetCorrectedEleBDTFlag(SUSYTau::TauTight, element->EleBDTTight(), 
                                                                   element->BDTEleScore(), element->numTrack(), 
                                                                   tauLV->Pt(), element->leadTrack_eta());

  tauOut->muonVeto              = element->muonVeto();

  tauOut->trueTau               = m_isMC? element->trueTauAssocSmall_matched() : false;

  tauOut->matched2TruthLepton   = m_isMC? m_recoTruthMatch.Matched2TruthLepton(*tauLV, true) : false;
  tauOut->detailedTruthType     = m_isMC? m_recoTruthMatch.TauDetailedFakeType(*tauLV) : -1;
  tauOut->truthType             = m_isMC? m_recoTruthMatch.TauFakeType(tauOut->detailedTruthType) : -1;

  // New efficiency scale factors
  // TODO: FINISH ME
  // ID efficiency scale factors
  if(m_isMC){
    TauCorrections* tauSF       = m_susyObj.GetTauCorrectionsProvider();
    tauOut->looseEffSF          = tauSF->GetIDSF(TauCorrections::BDTLOOSE, tauLV->Eta(), element->numTrack());
    tauOut->mediumEffSF         = tauSF->GetIDSF(TauCorrections::BDTMEDIUM, tauLV->Eta(), element->numTrack());
    tauOut->tightEffSF          = tauSF->GetIDSF(TauCorrections::BDTTIGHT, tauLV->Eta(), element->numTrack());
    tauOut->errLooseEffSF       = tauSF->GetIDSFUnc(TauCorrections::BDTLOOSE, tauLV->Eta(), element->numTrack());
    tauOut->errMediumEffSF      = tauSF->GetIDSFUnc(TauCorrections::BDTMEDIUM, tauLV->Eta(), element->numTrack());
    tauOut->errTightEffSF       = tauSF->GetIDSFUnc(TauCorrections::BDTTIGHT, tauLV->Eta(), element->numTrack());

    if(element->numTrack()==1){
      float eta = element->leadTrack_eta();
      tauOut->looseEVetoSF      = tauSF->GetEVetoSF(eta, TauCorrections::BDTLOOSE, TauCorrections::LOOSE, TauCorrections::LOOSE);
      tauOut->mediumEVetoSF     = tauSF->GetEVetoSF(eta, TauCorrections::BDTMEDIUM, TauCorrections::MEDIUM, TauCorrections::LOOSE);
      tauOut->tightEVetoSF      = tauSF->GetEVetoSF(eta, TauCorrections::BDTTIGHT, TauCorrections::TIGHT, TauCorrections::LOOSE);
      tauOut->errLooseEVetoSF   = tauSF->GetEVetoSFUnc(eta, TauCorrections::BDTLOOSE, TauCorrections::LOOSE, TauCorrections::LOOSE, 1);
      tauOut->errMediumEVetoSF  = tauSF->GetEVetoSFUnc(eta, TauCorrections::BDTMEDIUM, TauCorrections::MEDIUM, TauCorrections::LOOSE, 1);
      tauOut->errTightEVetoSF   = tauSF->GetEVetoSFUnc(eta, TauCorrections::BDTTIGHT, TauCorrections::TIGHT, TauCorrections::LOOSE, 1);
    }
  }

  tauOut->trigFlags             = m_tauTrigFlags[tauIdx];
  
  tauOut->idx   = tauIdx;
}

/*--------------------------------------------------------------------------------*/
// Fill MET variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillMetVars(SusyNtSys sys)
{
  if(m_dbg>=5) cout << "fillMetVars" << endl;

  // Just fill the lv for now
  double Et  = m_met.Et()/GeV;
  double phi = m_met.Phi(); 

  //double px = m_met.Px()/GeV;
  //double py = m_met.Py()/GeV;
  //double pz = m_met.Pz()/GeV;
  //double E  = m_met.E()/GeV;
  
  // Need to get the metUtility in order to 
  // get all the sumet terms.  In the future,
  // we could use the metUtility to get all the
  // comonents instead of the SUSYTools method
  // computeMetComponent, but that is up to Steve,
  // Lord of the Ntuples.
  METUtility* metUtil = m_susyObj.GetMETUtility();
  

  m_susyNt.met()->push_back( Susy::Met() );
  Susy::Met* metOut = & m_susyNt.met()->back();
  metOut->Et    = Et;
  metOut->phi   = phi;
  metOut->sys   = sys;  
  metOut->sumet = metUtil->getMissingET(METUtil::RefFinal, METUtil::None).sumet()/GeV;

  // MET comp terms 
  // Need to save these for the MET systematics as well.
  // Use the sys enum to determine which argument to pass to SUSYTools
  METUtil::Systematics metSys = METUtil::None;

  // I guess these are the only ones we need to specify, the ones specified in SUSYTools...
  // All the rest should be automatic (I think), e.g. JES
  if(sys == NtSys_SCALEST_UP) metSys = METUtil::ScaleSoftTermsUp;
  else if(sys == NtSys_SCALEST_DN) metSys = METUtil::ScaleSoftTermsDown;
  else if(sys == NtSys_RESOST) metSys = METUtil::ResoSoftTermsUp;

  // Save the MET terms
  TVector2 refEleV   = m_susyObj.computeMETComponent(METUtil::RefEle, metSys);
  TVector2 refMuoV   = m_susyObj.computeMETComponent(METUtil::MuonTotal, metSys);
  TVector2 refJetV   = m_susyObj.computeMETComponent(METUtil::RefJet, metSys);
  TVector2 refGammaV = m_susyObj.computeMETComponent(METUtil::RefGamma, metSys);
  //TVector2 softJetV  = m_susyObj.computeMETComponent(METUtil::SoftJets, metSys);
  //TVector2 refCellV  = m_susyObj.computeMETComponent(METUtil::CellOutEflow, metSys);
  TVector2 softTermV = m_susyObj.computeMETComponent(METUtil::SoftTerms, metSys);
  //float sumet = m_susyObj._metUtility->getMissingET(METUtil::SoftTerms).sumet();

  metOut->refEle     = refEleV.Mod()/GeV;
  metOut->refEle_etx = refEleV.Px()/GeV;
  metOut->refEle_ety = refEleV.Py()/GeV;
  metOut->refEle_sumet = metUtil->getMissingET(METUtil::RefEle, metSys).sumet()/GeV;

  metOut->refMuo     = refMuoV.Mod()/GeV;
  metOut->refMuo_etx = refMuoV.Px()/GeV;
  metOut->refMuo_ety = refMuoV.Py()/GeV;
  metOut->refMuo_sumet = metUtil->getMissingET(METUtil::MuonTotal, metSys).sumet()/GeV;

  metOut->refJet     = refJetV.Mod()/GeV;
  metOut->refJet_etx = refJetV.Px()/GeV;
  metOut->refJet_ety = refJetV.Py()/GeV;
  metOut->refJet_sumet = metUtil->getMissingET(METUtil::RefJet, metSys).sumet()/GeV;

  metOut->refGamma     = refGammaV.Mod()/GeV;
  metOut->refGamma_etx = refGammaV.Px()/GeV;
  metOut->refGamma_ety = refGammaV.Py()/GeV;
  metOut->refGamma_sumet = metUtil->getMissingET(METUtil::RefGamma, metSys).sumet()/GeV;

  //metOut->softJet     = softJetV.Mod()/GeV;
  //metOut->softJet_etx = softJetV.Px()/GeV;
  //metOut->softJet_ety = softJetV.Py()/GeV;

  //metOut->refCell     = refCellV.Mod()/GeV;
  //metOut->refCell_etx = refCellV.Px()/GeV;
  //metOut->refCell_ety = refCellV.Py()/GeV;

  metOut->softTerm     = softTermV.Mod()/GeV;
  metOut->softTerm_etx = softTermV.Px()/GeV;
  metOut->softTerm_ety = softTermV.Py()/GeV;
  metOut->softTerm_sumet = metUtil->getMissingET(METUtil::SoftTerms, metSys).sumet()/GeV;

  //metOut->refEle        = m_susyObj.computeMETComponent(METUtil::RefEle, metSys).Mod()/GeV;
  //metOut->refMuo        = m_susyObj.computeMETComponent(METUtil::MuonTotal, metSys).Mod()/GeV;
  //metOut->refJet        = m_susyObj.computeMETComponent(METUtil::RefJet, metSys).Mod()/GeV;
  //metOut->refGamma      = m_susyObj.computeMETComponent(METUtil::RefGamma, metSys).Mod()/GeV;
  //metOut->softJet       = m_susyObj.computeMETComponent(METUtil::SoftJets, metSys).Mod()/GeV;
  //metOut->refCell       = m_susyObj.computeMETComponent(METUtil::CellOutEflow, metSys).Mod()/GeV;
}
/*--------------------------------------------------------------------------------*/
// Fill Truth Particle variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillTruthParticleVars()
{
  if(m_dbg>=5) cout << "fillTruthParticleVars" << endl;

  // Retrieve indicies
  m_truParticles        = m_recoTruthMatch.LepFromHS_McIdx();
  vector<int> truthTaus = m_recoTruthMatch.TauFromHS_McIdx();
  m_truParticles.insert( m_truParticles.end(), truthTaus.begin(), truthTaus.end() );

  // Loop over selected truth particles
  for(uint iTruPar=0; iTruPar<m_truParticles.size(); iTruPar++){
    int truParIdx = m_truParticles[iTruPar];  

    m_susyNt.tpr()->push_back( Susy::TruthParticle() );
    Susy::TruthParticle* tprOut         = & m_susyNt.tpr()->back();

    // Set TLV
    float pt  = d3pd.truth.pt() ->at(truParIdx) / GeV;
    float eta = d3pd.truth.eta()->at(truParIdx);
    float phi = d3pd.truth.phi()->at(truParIdx);
    float m   = d3pd.truth.m()  ->at(truParIdx) / GeV;
 
    tprOut->SetPtEtaPhiM(pt, eta, phi, m);
    tprOut->pt          = pt;
    tprOut->eta         = eta;
    tprOut->phi         = phi;
    tprOut->m           = m;

    tprOut->charge      = d3pd.truth.charge()->at(truParIdx);
    tprOut->pdgId       = d3pd.truth.pdgId() ->at(truParIdx);
    tprOut->status      = d3pd.truth.status()->at(truParIdx);
    tprOut->motherPdgId = -999; // TO BE IMPLEMENTED
  }
}
/*--------------------------------------------------------------------------------*/
// Fill Truth Jet variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillTruthJetVars()
{
  if(m_dbg>=5) cout << "fillTruthJetVars" << endl;

  for(uint iTruJet=0; iTruJet<m_truJets.size(); iTruJet++){
    int truJetIdx = m_truJets[iTruJet];  
  
    m_susyNt.tjt()->push_back( Susy::TruthJet() );
    Susy::TruthJet* truJetOut = & m_susyNt.tjt()->back();
    const TruthJetElement* element = & d3pd.truthJet[truJetIdx];

    // Set TLV
    float pt  = element->pt() / GeV;
    float eta = element->eta();
    float phi = element->phi();
    float m   = element->m()  / GeV;
  
    truJetOut->SetPtEtaPhiM(pt, eta, phi, m);
    truJetOut->pt     = pt;
    truJetOut->eta    = eta;
    truJetOut->phi    = phi;
    truJetOut->m      = m;

    truJetOut->flavor = element->flavor_truth_label();
  }
}
/*--------------------------------------------------------------------------------*/
// Fill Truth Met variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillTruthMetVars()
{
  if(m_dbg>=5) cout << "fillTruthMetVars" << endl;

  // Just fill the lv for now
  double Et  = m_truMet.Et()/GeV;
  double phi = m_truMet.Phi(); 

  m_susyNt.tmt()->push_back( Susy::TruthMet() );
  Susy::TruthMet* truMetOut = & m_susyNt.tmt()->back();
  truMetOut->Et  = Et;
  truMetOut->phi = phi;
}

/*--------------------------------------------------------------------------------*/
// Handle Systematic 
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::doSystematic()
{
  // Loop over the systematics:
  // Start at 1, nominal saved
  for(int i = 1; i < NtSys_N; i++){

    SusyNtSys sys = (SusyNtSys) i;
    if(m_dbg>=5) cout << "Doing sys " << SusyNtSystNames[sys] << endl;

    // Reset Objects 
    m_susyObj.Reset();
    clearObjects();

    selectObjects(sys);
    buildMet(sys);                   

    checkEventCleaning();
    checkObjectCleaning();

    // Lepton Specific sys    
    if( isElecSys(sys) )
      saveElectronSF(sys);
    else if( isMuonSys(sys) )
      saveMuonSF(sys);
    else if( isJetSys(sys) )
      saveJetSF(sys);
    else if( isTauSys(sys) )
      saveTauSF(sys);

    // Fill the Met for this sys
    fillMetVars(sys);

    // Add the event flag for this event
    //addEventFlag(sys, m_evtFlag);
    m_susyNt.evt()->cutFlags[sys] = m_cutFlags;

  } // end loop over systematic
}

/*--------------------------------------------------------------------------------*/
void SusyNtMaker::saveElectronSF(SusyNtSys sys)
{
  // loop over preselected leptons and fill the output tree
  // TODO: Change this to be like the tau case, so that 
  // scale factors are ONLY calculated in one place!!
  for(uint iLep=0; iLep < m_preLeptons.size(); iLep++){
    const LeptonInfo* lep = & m_preLeptons[iLep];
    if(!lep->isElectron()) continue;
    
    bool match = false;

    // Match Indices and save sf
    for(uint iE=0; iE<m_susyNt.ele()->size(); ++iE){
      Susy::Electron* ele = & m_susyNt.ele()->at(iE);
      if( ele->idx == lep->idx() ){
	match = true;
	
	float sf = lep->lv()->E() / GeV / ele->E();
	//if(sys == NtSys_EES_UP)      ele->ees_up = sf;
	//else if(sys == NtSys_EES_DN) ele->ees_dn = sf;
	if(sys == NtSys_EES_Z_UP)        ele->ees_z_up = sf;
	else if(sys == NtSys_EES_Z_DN)   ele->ees_z_dn = sf;
	else if(sys == NtSys_EES_MAT_UP) ele->ees_mat_up = sf;
	else if(sys == NtSys_EES_MAT_DN) ele->ees_mat_dn = sf;
	else if(sys == NtSys_EES_PS_UP)  ele->ees_ps_up = sf;
	else if(sys == NtSys_EES_PS_DN)  ele->ees_ps_dn = sf;
	else if(sys == NtSys_EES_LOW_UP) ele->ees_low_up = sf;
	else if(sys == NtSys_EES_LOW_DN) ele->ees_low_dn = sf;
	else if(sys == NtSys_EER_UP)     ele->eer_up = sf;
	else if(sys == NtSys_EER_DN)     ele->eer_dn = sf;
	
      } // end if electron matches
    } // end loop over electrons in susyNt
    
    // The dreaded case...
    if(!match) addMissingElectron(lep, sys);

  }// end loop over leptons
}

/*--------------------------------------------------------------------------------*/
void SusyNtMaker::saveMuonSF(SusyNtSys sys)
{
  // loop over preselected leptons and fill the output tree
  // TODO: Change this to be like the tau case, so that 
  // scale factors are ONLY calculated in one place!!
  for(uint iLep=0; iLep < m_preLeptons.size(); iLep++){
    const LeptonInfo* lep = & m_preLeptons[iLep];
    if(lep->isElectron()) continue;
    
    bool match = false;

    // Match Indices and save sf
    for(uint iM=0; iM<m_susyNt.muo()->size(); ++iM){
      Susy::Muon* mu = & m_susyNt.muo()->at(iM);
      if( mu->idx == lep->idx() ){
	match = true;
	
	float sf = lep->lv()->E() / GeV / mu->E();
	if(sys == NtSys_MS_UP)      mu->ms_up = sf;
	else if(sys == NtSys_MS_DN) mu->ms_dn = sf;
	else if(sys == NtSys_ID_UP) mu->id_up = sf;
	else if(sys == NtSys_ID_DN) mu->id_dn = sf;

      } // end if muon matches
    } // end loop over muons in SusyNt
    
    // The dreaded case...
    if(!match) addMissingMuon(lep, sys);

  }// end loop over leptons
      
}

/*--------------------------------------------------------------------------------*/
void SusyNtMaker::saveJetSF(SusyNtSys sys)
{
  // Loop over selected jets and fill output tree
  // TODO: Change this to be like the tau case, so that 
  // scale factors are ONLY calculated in one place!!
  for(uint iJet=0; iJet<m_preJets.size(); iJet++){
    uint jetIdx = m_preJets[iJet];
    //const JetElement* element = & d3pd.jet[jetIdx];
    //const TLorentzVector* jetTLV = & m_susyObj.GetJetTLV(jetIdx);
    float E_sys = m_susyObj.GetJetTLV(jetIdx).E();
    
    bool match = false;

    // Check Indices and save sf
    for(uint iJ=0; iJ<m_susyNt.jet()->size(); ++iJ){
      Susy::Jet* jet = & m_susyNt.jet()->at(iJ);
      if( jet->idx == jetIdx ){
	match = true;
	
	//float sf = jetTLV->E() / GeV / jet->E();
	float sf = E_sys / GeV / jet->E();
	if(sys == NtSys_JES_UP)      jet->jes_up = sf;
	else if(sys == NtSys_JES_DN) jet->jes_dn = sf;
	else if(sys == NtSys_JER)    jet->jer = sf;

      } // end if the leptons match	
    } // end loop over what we have saved
    
    // The dreaded case...
    if(!match) addMissingJet(jetIdx, sys);
    
  }// end loop over jets in pre-jets

}

/*--------------------------------------------------------------------------------*/
void SusyNtMaker::saveTauSF(SusyNtSys sys)
{
  // Loop over selected taus and fill output tree
  for(uint iTau=0; iTau<m_preTaus.size(); iTau++){
    uint tauIdx = m_preTaus[iTau];

    // Get the systematic shifted E, used to calculate a shift factor
    float E_sys = m_susyObj.GetTauTLV(tauIdx).E();

    // Try to find this tau in the list of SusyNt taus
    Susy::Tau* tauOut = 0;
    for(uint iT=0; iT<m_susyNt.tau()->size(); iT++){
      Susy::Tau* tau = & m_susyNt.tau()->at(iT);
      if(tau->idx == tauIdx){
        tauOut = tau;
        break;
      }
    }
    // If tau not found, then it was not nominally pre-selected and must be added now
    if(tauOut == 0){
      addMissingTau(tauIdx, sys);
      tauOut = & m_susyNt.tau()->back();
    }

    // Calculate systematic scale factor
    float sf = E_sys / GeV / tauOut->E();
    if(sys == NtSys_TES_UP) tauOut->tes_up = sf;
    if(sys == NtSys_TES_DN) tauOut->tes_dn = sf;
  }
}

/*--------------------------------------------------------------------------------*/
void SusyNtMaker::addMissingElectron(const LeptonInfo* lep, SusyNtSys sys)
{
  // This electron did not pass nominal cuts, and therefore
  // needs to be added, but with the correct TLV
  
  // Get the systematic shifted E, used to calculate a shift factor
  //TLorentzVector tlv_sys = m_susyObj.GetElecTLV(lep->idx());
  float E_sys = m_susyObj.GetElecTLV(lep->idx()).E();

  D3PDReader::ElectronD3PDObject* e = & d3pd.ele;
  int index      = lep->idx();
  float cl_eta   = e->cl_eta()->at(index);
  float cl_phi   = e->cl_phi()->at(index);
  float cl_E     = e->cl_E()->at(index);
  float trk_eta  = e->tracketa()->at(index);
  float trk_phi  = e->trackphi()->at(index);
  float nPixHits = e->nPixHits()->at(index);
  float nSCTHits = e->nSCTHits()->at(index);

  // Reset the Nominal TLV
  // NOTE: this overwrites the TLV in SUSYObjDef with the nominal variables, 
  // regardless of our current systematic.
  m_susyObj.SetElecTLV(index, cl_eta, cl_phi, cl_E, trk_eta, trk_phi, nPixHits, nSCTHits, SystErr::NONE);

  // Now push it back onto to susyNt
  fillElectronVars(lep);
  
  // Set the sf
  Susy::Electron* ele = & m_susyNt.ele()->back();
  //float sf = tlv_sys.E() / GeV / ele->E();
  float sf = E_sys / GeV / ele->E();
  if(sys == NtSys_EES_Z_UP)        ele->ees_z_up = sf;
  else if(sys == NtSys_EES_Z_DN)   ele->ees_z_dn = sf;
  else if(sys == NtSys_EES_MAT_UP) ele->ees_mat_up = sf;
  else if(sys == NtSys_EES_MAT_DN) ele->ees_mat_dn = sf;
  else if(sys == NtSys_EES_PS_UP)  ele->ees_ps_up = sf;
  else if(sys == NtSys_EES_PS_DN)  ele->ees_ps_dn = sf;
  else if(sys == NtSys_EES_LOW_UP) ele->ees_low_up = sf;
  else if(sys == NtSys_EES_LOW_DN) ele->ees_low_dn = sf;
  else if(sys == NtSys_EER_UP)     ele->eer_up = sf;
  else if(sys == NtSys_EER_DN)     ele->eer_dn = sf;
}

/*--------------------------------------------------------------------------------*/
void SusyNtMaker::addMissingMuon(const LeptonInfo* lep, SusyNtSys sys)
{
  // This muon did not pass nominal cuts, and therefore
  // needs to be added, but with the correct TLV
  
  //TLorentzVector tlv_sys = m_susyObj.GetMuonTLV(lep->idx());
  float E_sys = m_susyObj.GetMuonTLV(lep->idx()).E();

  D3PDReader::MuonD3PDObject* m = & d3pd.muo;
  int index               = lep->idx();
  float pt                = m->pt()->at(index);
  float eta               = m->eta()->at(index);
  float phi               = m->phi()->at(index);
  float me_qoverp_exPV    = m->me_qoverp_exPV()->at(index);
  float id_qoverp_exPV    = m->id_qoverp_exPV()->at(index);
  float me_theta_exPV     = m->me_theta_exPV()->at(index);
  float id_theta_exPV     = m->id_theta_exPV()->at(index);
  float charge            = m->charge()->at(index);
  int isCombined          = m->isCombinedMuon()->at(index);
  int isSegTag            = m->isSegmentTaggedMuon()->at(index);

  // Reset the Nominal TLV
  // NOTE: this overwrites the TLV in SUSYObjDef with the nominal variables, 
  // regardless of our current systematic.
  m_susyObj.SetMuonTLV(index, pt, eta, phi, me_qoverp_exPV, id_qoverp_exPV, me_theta_exPV, 
                       id_theta_exPV, charge, isCombined, isSegTag, SystErr::NONE);
  
  // Now push it back onto to susyNt
  fillMuonVars(lep);
  
  // Set the sf
  Susy::Muon* mu = & m_susyNt.muo()->back();
  //float sf = tlv_sys.E() / GeV / mu->E();
  float sf = E_sys / GeV / mu->E();
  if(sys == NtSys_MS_UP) mu->ms_up = sf;
  else if(sys == NtSys_MS_DN) mu->ms_dn = sf;
  else if(sys == NtSys_ID_UP) mu->id_up = sf;
  else if(sys == NtSys_ID_DN) mu->id_dn = sf;
}

/*--------------------------------------------------------------------------------*/
void SusyNtMaker::addMissingJet(int index, SusyNtSys sys)
{
  // Get the systematic shifted E, used to calculate a shift factor
  //TLorentzVector tlv_sys = m_susyObj.GetJetTLV(index);
  float E_sys = m_susyObj.GetJetTLV(index).E();

  // Reset the Nominal TLV
  // NOTE: this overwrites the TLV in SUSYObjDef with the nominal variables, 
  // regardless of our current systematic.
  const D3PDReader::JetD3PDObjectElement* jet = & d3pd.jet[index];
  m_susyObj.FillJet(index, jet->pt(), jet->eta(), jet->phi(), jet->E(),
                    jet->constscale_eta(), jet->constscale_phi(), jet->constscale_E(), jet->constscale_m(),
                    jet->ActiveAreaPx(), jet->ActiveAreaPy(), jet->ActiveAreaPz(), jet->ActiveAreaE(),
                    d3pd.evt.Eventshape_rhoKt4LC(),
                    d3pd.evt.averageIntPerXing(),
                    d3pd.vtx.nTracks());

  // Need to save the calibrated TLV
  //TLorentzVector tlv_nom;
  //m_susyObj.RecalibrateJet(&tlv_nom, 
  //                         d3pd.jet.constscale_E()->at(index),
  //                         d3pd.jet.constscale_eta()->at(index),
  //                         d3pd.jet.constscale_phi()->at(index),
  //                         d3pd.jet.constscale_m()->at(index),
  //                         d3pd.jet.ActiveAreaPx()->at(index),
  //                         d3pd.jet.ActiveAreaPy()->at(index),
  //                         d3pd.jet.ActiveAreaPz()->at(index),
  //                         d3pd.jet.ActiveAreaE()->at(index),
  //                         d3pd.evt.Eventshape_rhoKt4LC(),
  //                         d3pd.evt.averageIntPerXing(),
  //                         d3pd.vtx.nTracks());
  
  //D3PDReader::JetD3PDObject* jets = & d3pd.jet;

  //float pt  = tlv_nom.Pt() / GeV;  //jets->pt()->at(index);
  //float eta = tlv_nom.Eta();       //jets->eta()->at(index);
  //float phi = tlv_nom.Phi();       //jets->phi()->at(index);
  //float E   = tlv_nom.E() / GeV;   //jets->E()->at(index);
  
  // Reset the Nominal TLV
  // NOTE: this overwrites the TLV in SUSYObjDef with the nominal variables, 
  // regardless of our current systematic.
  //m_susyObj.SetJetTLV(index, pt, eta, phi, E);

  // Fill the Jet vars for this guy, using the now nominal TLV
  fillJetVar(index);

  // Set SF
  Susy::Jet* j = & m_susyNt.jet()->back();
  //float sf = tlv_sys.E() / GeV / j->E();
  float sf = E_sys / GeV / j->E();
  if(sys == NtSys_JER)         j->jer = sf;
  else if(sys == NtSys_JES_UP) j->jes_up = sf;
  else if(sys == NtSys_JES_DN) j->jes_dn = sf;
}

/*--------------------------------------------------------------------------------*/
void SusyNtMaker::addMissingTau(int index, SusyNtSys sys)
{
  // This tau did not pass nominal cuts, and therefore
  // needs to be added, but with the correct TLV.

  // Get the systematic shifted E, used to calculate a shift factor
  //TLorentzVector tlv_sys = m_susyObj.GetTauTLV(index);
  //float E_sys = m_susyObj.GetTauTLV(index).E();

  // Grab the d3pd variables
  const TauElement* element = & d3pd.tau[index];
  float Et       = element->Et();
  float eta      = element->eta();
  float phi      = element->phi();
  float E        = cosh(eta)*Et;
  int   numTrack = element->numTrack();

  // Reset the Nominal TLV
  // NOTE: this overwrites the TLV in SUSYObjDef with the nominal variables, 
  // regardless of our current systematic.
  m_susyObj.SetTauTLV(index, Et, eta, phi, E, numTrack, SystErr::NONE);

  // Fill the tau vars for this guy
  fillTauVar(index);

  // Set SF
  // This should only be done in saveTauSF
  //Susy::Tau* tau = & m_susyNt.tau()->back();
  //float sf = E_sys / GeV / tau->E();
  //if(sys == NtSys_TES_UP) tau->tes_up = sf;
  //if(sys == NtSys_TES_DN) tau->tes_dn = sf;
}
/*--------------------------------------------------------------------------------*/
//bool SusyNtMaker::isBuggyWwSherpaSample(const int &dsid)
//{
//  return (dsid==126892 || dsid==157817 || dsid==157818 || dsid==157819);
//}
/*--------------------------------------------------------------------------------*/
//bool SusyNtMaker::hasRadiativeBquark(const vint_t *pdg, const vint_t *status)
//{
//  if(!pdg || !status || pdg->size()!=status->size()) return false;
//  const vint_t &p = *pdg;
//  const vint_t &s = *status;
//  const int pdgB(5), statRad(3); 
//  for(size_t i=0; i<p.size(); ++i) if(abs(p[i])==pdgB && s[i]==statRad) return true;
//  return false;
//}

#undef GeV
