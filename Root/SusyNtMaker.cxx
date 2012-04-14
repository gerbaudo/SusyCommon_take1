#include "MultiLep/MuonTools.h"
#include "SusyCommon/SusyNtMaker.h"

using namespace std;

/*--------------------------------------------------------------------------------*/
// SusyNtMaker Constructor
/*--------------------------------------------------------------------------------*/
SusyNtMaker::SusyNtMaker(TTree* tree) : SusyD3PDAna(tree),
                                        m_susyNt()
{
  n_base_ele=0;
  n_base_muo=0;
  n_base_jet=0;
  n_evt_initial=0;
  n_evt_grl=0;
  n_evt_larErr=0;
  n_evt_larHole=0;
  n_evt_badJet=0;
  n_evt_goodVtx=0;
  n_evt_badMu=0;
  n_evt_cosmic=0;
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

  // Open the output tree
  m_outTreeFile = new TFile("susyNt.root", "recreate");
  m_outTree = new TTree("susyNt", "susyNt");

  // Set autosave size (determines how often tree writes to disk)
  m_outTree->SetAutoSave(10000000);
  // Max tree size determines when a new file and tree are written
  m_outTree->SetMaxTreeSize(3000000000u);
  m_susyNt.WriteTo(m_outTree);
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
  if(m_dbg || chainEntry%1000==0)
  {
    cout << "**** Processing entry " << setw(6) << chainEntry
         << " run " << setw(6) << d3pd.evt.RunNumber()
         << " event " << setw(7) << d3pd.evt.EventNumber() << " ****" << endl;
  }

  if(selectEvent()){
    fillNtVars();
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
  SusyD3PDAna::Terminate();
  if(m_dbg) cout << "SusyNtMaker::Terminate" << endl;

  cout << endl;
  cout << "Object counter" << endl;
  cout << "  BaseEle  " << n_base_ele    << endl;
  cout << "  BaseMuo  " << n_base_muo    << endl;
  cout << "  BaseJet  " << n_base_jet    << endl;
  cout << endl;
  cout << "Event counter" << endl;
  cout << "  Initial  " << n_evt_initial << endl;
  cout << "  GRL      " << n_evt_grl     << endl;
  cout << "  LarErr   " << n_evt_larErr  << endl;
  cout << "  LarHole  " << n_evt_larHole << endl;
  cout << "  BadJet   " << n_evt_badJet  << endl;
  cout << "  GoodVtx  " << n_evt_goodVtx << endl;
  cout << "  BadMu    " << n_evt_badMu   << endl;
  cout << "  Cosmic   " << n_evt_cosmic  << endl;
  cout << endl;

  // Save the output tree
  m_outTreeFile = m_outTree->GetCurrentFile();
  m_outTreeFile->Write(0, TObject::kOverwrite);
  cout << "susyNt tree saved to " << m_outTreeFile->GetName() << endl;
  m_outTreeFile->Close();
}

/*--------------------------------------------------------------------------------*/
// Select event
/*--------------------------------------------------------------------------------*/
bool SusyNtMaker::selectEvent()
{
  if(m_dbg) cout << "selectEvent" << endl;

  n_evt_initial++;

  m_susyObj.Reset();
  clearObjects();
  m_susyNt.clear();
  selectObjects();
  buildMet();

  n_base_ele += m_baseElectrons.size();
  n_base_muo += m_baseMuons.size();
  n_base_jet += m_baseJets.size();

  // grl
  if(!passGRL()) return false;
  n_evt_grl++;
  // larErr
  if(!passLarErr()) return false;
  n_evt_larErr++;

  // lar hole veto
  if(!passLarHoleVeto()) return false;
  n_evt_larHole++;
  // bad jet veto
  if(!passBadJet()) return false;
  n_evt_badJet++;
  // primary vertex cut
  if(!passGoodVtx()) return false;
  n_evt_goodVtx++;
  // bad muon veto
  if(!passBadMuon()) return false;
  n_evt_badMu++;
  // cosmic veto
  if(!passCosmic()) return false;
  n_evt_cosmic++;

  matchTriggers();

  // For filling the output tree, require at least one baseline lepton
  if(m_baseLeptons.size()<1) return false;

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
  fillMetVars();
  m_outTree->Fill();
}

/*--------------------------------------------------------------------------------*/
// Fill Event variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillEventVars()
{
  Susy::Event* evt = m_susyNt.evt();

  evt->run              = d3pd.evt.RunNumber();
  evt->event            = d3pd.evt.EventNumber();
  evt->lb               = d3pd.evt.lbn();

  evt->isMC             = m_isMC;
  evt->mcChannel        = m_isMC? d3pd.truth.channel_number() : 0;
  evt->w                = m_isMC? d3pd.truth.event_weight()   : 1;
}

/*--------------------------------------------------------------------------------*/
// Fill lepton variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillLeptonVars()
{
  // loop over preselected leptons and fill the output tree
  for(uint iLep=0; iLep < m_baseLeptons.size(); iLep++){
    const LeptonInfo* lep = & m_baseLeptons[iLep];
    if(lep->isElectron()) fillElectronVars(lep);
    else fillMuonVars(lep);
  }
}
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillElectronVars(const LeptonInfo* lepIn)
{
  if(m_dbg) cout << "fillElectronVars" << endl;
  m_susyNt.ele()->push_back( Susy::Electron() );
  Susy::Electron* eleOut = & m_susyNt.ele()->back();
  const ElectronElement* element = lepIn->getElectronElement();

  // LorentzVector
  eleOut->SetPtEtaPhiM(lepIn->lv()->Pt()/GeV, lepIn->lv()->Eta(), lepIn->lv()->Phi(), lepIn->lv()->M()/GeV);

  eleOut->ptcone20      = element->ptcone20()/GeV;
  eleOut->q             = element->charge();
  // add d0 later, not sure if d0_exPV is available in the d3pds
  eleOut->mcType        = m_isMC? element->type() : 0;
  eleOut->mcOrigin      = m_isMC? element->origin() : 0;
  eleOut->clusEta       = element->cl_eta();
  eleOut->mediumPP      = element->mediumPP();
  eleOut->tightPP       = element->tightPP();

  eleOut->trigFlags     = m_eleTrigFlags[ lepIn->idx() ];
}
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillMuonVars(const LeptonInfo* lepIn)
{
  if(m_dbg) cout << "fillMuonVars" << endl;
  m_susyNt.muo()->push_back( Susy::Muon() );
  Susy::Muon* muOut = & m_susyNt.muo()->back();
  const MuonElement* element = lepIn->getMuonElement();

  // need truthMuon for type and origin
  const TruthMuonElement* trueMuon = m_isMC? getMuonTruth( &d3pd.muo, lepIn->idx(), &d3pd.truthMu ) : 0;

  // LorentzVector
  muOut->SetPtEtaPhiM(lepIn->lv()->Pt()/GeV, lepIn->lv()->Eta(), lepIn->lv()->Phi(), lepIn->lv()->M()/GeV);

  muOut->q              = element->charge();
  muOut->ptcone20       = element->ptcone20()/GeV;
  muOut->d0             = element->d0_exPV();
  muOut->errD0          = fabs(element->cov_d0_exPV());
  muOut->isCombined     = element->isCombinedMuon();

  muOut->mcType         = trueMuon? trueMuon->type()   : 0;
  muOut->mcOrigin       = trueMuon? trueMuon->origin() : 0;

  muOut->trigFlags      = m_muoTrigFlags[ lepIn->idx() ];
}

/*--------------------------------------------------------------------------------*/
// Fill jet variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillJetVars()
{
  if(m_dbg) cout << "fillJetVars" << endl;
  // Loop over selected jets and fill output tree
  for(uint iJet=0; iJet<m_sigJets.size(); iJet++){
    m_susyNt.jet()->push_back( Susy::Jet() );
    Susy::Jet* jetOut = & m_susyNt.jet()->back();
    const JetElement* element = & d3pd.jet[iJet];

    const TLorentzVector* lv = & m_susyObj.GetJetTLV(iJet);
    jetOut->SetPtEtaPhiM(lv->Pt()/GeV, lv->Eta(), lv->Phi(), lv->M()/GeV);

    jetOut->jvf         = element->jvtxf();
    jetOut->combNN      = element->flavor_weight_JetFitterCOMBNN();
  }
}
/*--------------------------------------------------------------------------------*/
// Fill MET variables
/*--------------------------------------------------------------------------------*/
void SusyNtMaker::fillMetVars()
{
  if(m_dbg) cout << "fillMetVars" << endl;
  // Just fill the lv for now
  m_susyNt.met()->SetPxPyPzE( m_met.Px()/GeV, m_met.Py()/GeV, m_met.Pz()/GeV, m_met.E()/GeV );
}
