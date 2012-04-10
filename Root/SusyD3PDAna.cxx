#include "SusyCommon/SusyD3PDAna.h"
#include "MultiLep/ElectronTools.h"
#include "MultiLep/MuonTools.h"
#include "MultiLep/JetTools.h"
#include "MultiLep/CutflowTools.h"

using namespace std;

/*--------------------------------------------------------------------------------*/
// SusyD3PDAna Constructor
/*--------------------------------------------------------------------------------*/
SusyD3PDAna::SusyD3PDAna(TTree* tree) : SusyD3PDInterface(tree)
{
}
/*--------------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------------*/
SusyD3PDAna::~SusyD3PDAna()
{
}
/*--------------------------------------------------------------------------------*/
// The Begin() function is called at the start of the query.
// When running with PROOF Begin() is only called on the client.
// The tree argument is deprecated (on PROOF 0 is passed).
/*--------------------------------------------------------------------------------*/
void SusyD3PDAna::Begin(TTree* /*tree*/)
{
  if(m_dbg) cout << "SusyD3PDAna::Begin" << endl;

  // Switch off isMC flag if truth info is unavailable
  if(!d3pd.truth.channel_number.IsAvailable()) m_isMC = false;

  if(m_isMC) cout << "Processing as MC"   << endl;
  else       cout << "Processing as DATA" << endl;

  // Setup SUSYTools
  m_susyObj.initialize();
  m_fakeMetEst.initialize("$ROOTCOREDIR/data/MultiLep/fest_periodF_v1.root");

  // GRL
  if(!m_isMC){
    if(m_grlFileName=="") m_grlFileName = "$ROOTCOREDIR/data/MultiLep/data11_7TeV.periodAllYear_DetStatus-v36-pro10_CoolRunQuery-00-04-08_Susy.xml";
    Root::TGoodRunsListReader* grlReader = new Root::TGoodRunsListReader();
    grlReader->AddXMLFile(m_grlFileName);
    grlReader->Interpret();
    m_grl = grlReader->GetMergedGoodRunsList();
    delete grlReader;
  }
}

/*--------------------------------------------------------------------------------*/
// Main process loop function - This is just an example for testing
/*--------------------------------------------------------------------------------*/
Bool_t SusyD3PDAna::Process(Long64_t entry)
{
  // Communicate the entry number to the interface objects
  GetEntry(entry);

  static Long64_t chainEntry = -1;
  chainEntry++;
  if(m_dbg || chainEntry%10000==0)
  {
    cout << "**** Processing entry " << setw(6) << chainEntry
         << " run " << setw(6) << d3pd.evt.RunNumber()
         << " event " << setw(7) << d3pd.evt.EventNumber() << " ****" << endl;
  }

  return kTRUE;
}

/*--------------------------------------------------------------------------------*/
// The Terminate() function is the last function to be called during
// a query. It always runs on the client, it can be used to present
// the results graphically or save the results to file.
/*--------------------------------------------------------------------------------*/
void SusyD3PDAna::Terminate()
{
  if(m_dbg) cout << "SusyD3PDAna::Terminate" << endl;
  m_susyObj.finalize();
}

/*--------------------------------------------------------------------------------*/
// Baseline object selection
/*--------------------------------------------------------------------------------*/
void SusyD3PDAna::selectBaselineObjects()
{
  if(m_dbg) cout << "selectBaselineObjects" << endl;
  vector<int> goodJets;  // What the hell is this??

  // Preselection - no systematics yet
  m_preElectrons = get_electrons_baseline( &d3pd.ele, !m_isMC, d3pd.evt.RunNumber(), m_susyObj, 10.*GeV, 2.47, 0, 0, false );
  m_preMuons     = get_muons_baseline( &d3pd.muo, !m_isMC, m_susyObj, 10.*GeV, 2.4, "" );
  m_preJets      = get_jet_baseline( &d3pd.jet, !m_isMC, m_susyObj, 20.*GeV, 4.9, JetErr::NONE, false, goodJets );
  
  // e-e overlap removal
  m_baseElectrons = overlap_removal(m_susyObj, &d3pd.ele, m_preElectrons, &d3pd.ele, m_preElectrons, 0.1, 1);
  // jet-e overlap removal
  m_baseJets      = overlap_removal(m_susyObj, &d3pd.jet, m_preJets, &d3pd.ele, m_baseElectrons, 0.2, 0);
  // e-jet overlap removal
  m_baseElectrons = overlap_removal(m_susyObj, &d3pd.ele, m_baseElectrons, &d3pd.jet, m_baseJets, 0.4, 0);
  // m-jet overlap removal
  m_baseMuons     = overlap_removal(m_susyObj, &d3pd.muo, m_preMuons, &d3pd.jet, m_baseJets, 0.4, 0);
  // e-m overlap removal
  m_baseElectrons = overlap_removal(m_susyObj, &d3pd.ele, m_baseElectrons, &d3pd.muo, m_baseMuons, 0.1, 0);
  m_baseMuons     = overlap_removal(m_susyObj, &d3pd.muo, m_baseMuons, &d3pd.ele, m_baseElectrons, 0.1, 0);
  // remove SFOS lepton pairs with Mll < 20 GeV
  m_baseElectrons = RemoveSFOSPair(m_susyObj, &d3pd.ele, m_baseElectrons, 20.*GeV);
  m_baseMuons     = RemoveSFOSPair(m_susyObj, &d3pd.muo, m_baseMuons,     20.*GeV);

  // combine leptons
  m_baseLeptons   = buildLeptonInfos(&d3pd.ele, m_baseElectrons, &d3pd.muo, m_baseMuons, m_susyObj);
}
/*--------------------------------------------------------------------------------*/
// Signal object selection - do baseline selection first!
/*--------------------------------------------------------------------------------*/
void SusyD3PDAna::selectSignalObjects()
{
  if(m_dbg) cout << "selectSignalObjects" << endl;
  // TODO: make these functions more symmetric
  m_sigElectrons = get_electrons_signal(&d3pd.ele, m_baseElectrons, m_susyObj, 10.*GeV);
  m_sigMuons     = get_muons_signal(&d3pd.muo, m_susyObj, m_baseMuons, 10.*GeV, 1.8*GeV);
  m_sigJets      = get_jet_signal(&d3pd.jet, m_susyObj, m_baseJets, 30.*GeV, 2.5, 0.75);

  // combine leptons
  m_sigLeptons   = buildLeptonInfos(&d3pd.ele, m_sigElectrons, &d3pd.muo, m_sigMuons, m_susyObj);
}

/*--------------------------------------------------------------------------------*/
// Build MissingEt
/*--------------------------------------------------------------------------------*/
void SusyD3PDAna::buildMet()
{
  if(m_dbg) cout << "buildMet" << endl;
  // Need ALL electrons in order to calculate the MET
  // Actually, I see common code uses all electrons that have lv.Pt() != 0
  // That's fine though because SUSYObjDef specifically fills for electrons that
  // should enter the RefEle term
  vector<int> allElectrons = get_electrons_all(&d3pd.ele, m_susyObj);
  TVector2 metVector = GetMetVector(&d3pd.jet, m_susyObj, &d3pd.muo, &d3pd.ele, &d3pd.met, m_baseMuons, m_baseElectrons, allElectrons, JetErr::NONE);
  m_met.SetPxPyPzE(metVector.X(), metVector.Y(), 0, metVector.Mod());
}

/*--------------------------------------------------------------------------------*/
// Clear selected objects
/*--------------------------------------------------------------------------------*/
void SusyD3PDAna::clearObjects()
{
  m_preElectrons.clear();
  m_preMuons.clear();
  m_preJets.clear();
  m_baseElectrons.clear();
  m_baseMuons.clear();
  m_baseLeptons.clear();
  m_baseJets.clear();
  m_sigElectrons.clear();
  m_sigMuons.clear();
  m_sigLeptons.clear();
  m_sigJets.clear();
}

/*--------------------------------------------------------------------------------*/
// Electron trigger matching
/*--------------------------------------------------------------------------------*/
void SusyD3PDAna::matchElectronTriggers()
{
  if(m_dbg) cout << "matchElectronTriggers" << endl;
  // loop over all baseline electrons
  for(uint i=0; i<m_baseElectrons.size(); i++){
    int iEl = m_baseElectrons[i];
    const TLorentzVector* lv = & m_susyObj.GetElecTLV(iEl);
    
    // trigger flags
    uint flags = 0;

    if(lv->Pt() > 25.*GeV){
      // e20_medium
      if( matchElectronTrigger(lv->Eta(), lv->Phi(), d3pd.trig.trig_EF_el_EF_e20_medium()) ){
        flags |= TRIG_e20_medium;
      }
      // e22_medium
      if( matchElectronTrigger(lv->Eta(), lv->Phi(), d3pd.trig.trig_EF_el_EF_e22_medium()) ){
        flags |= TRIG_e22_medium;
      }
      // e22vh_medium1
      if( matchElectronTrigger(lv->Eta(), lv->Phi(), d3pd.trig.trig_EF_el_EF_e22vh_medium1()) ){
        flags |= TRIG_e22vh_medium1;
      }
    }

    // assign the flags in the map
    m_eleTrigFlags[iEl] = flags;
  }
}
/*--------------------------------------------------------------------------------*/
// Electron trigger matching
/*--------------------------------------------------------------------------------*/
bool SusyD3PDAna::matchElectronTrigger(float eta, float phi, vector<int>* rois)
{
  if(rois->size()==0) return false;
  // matched trigger index - not used
  static int indexEF = -1;
  // Use function defined in egammaAnalysisUtils/egammaTriggerMatching.h
  return PassedTriggerEF(eta, phi, rois, indexEF, d3pd.trig.trig_EF_el_n(), 
                         d3pd.trig.trig_EF_el_eta(), d3pd.trig.trig_EF_el_phi());
}

/*--------------------------------------------------------------------------------*/
// Muon trigger matching
/*--------------------------------------------------------------------------------*/
void SusyD3PDAna::matchMuonTriggers()
{
  if(m_dbg) cout << "matchMuonTriggers" << endl;
  // This seems very complicated, and 2L seems to match muons differently than 3L.
  // For now, just use the common code 3L matching function.
  // TODO: come back to this and try to improve it.

  // Get trigger plateau muons - but use baseline muons instead of signal muons
  vector<int> plateau_muons = get_muons_signal_triggerplateau(&d3pd.muo, m_baseMuons, m_susyObj, 20.*GeV);
  // Get muons which matched to trigger
  vector<int> matched_muons = get_muons_triggered(&d3pd.muo, plateau_muons, &d3pd.trig, 0.15);

  // Now assign the flags
  // I guess I'll just flag them all until I have a different prescription
  for(uint i=0; i < matched_muons.size(); i++){
    uint flags = 0;
    flags |= TRIG_mu18;
    flags |= TRIG_mu18_medium;
    m_muoTrigFlags[ matched_muons[i] ] = flags;
  }
}

/*--------------------------------------------------------------------------------*/
// Pass Lar hole veto
// Prior to calling this, need jet and MET selection
/*--------------------------------------------------------------------------------*/
bool SusyD3PDAna::passLarHoleVeto()
{
  TVector2 metVector = m_met.Vect().XYvector();
  return !check_jet_larhole(&d3pd.jet, m_preJets, !m_isMC, m_susyObj, 180614, metVector, &m_fakeMetEst);
}
/*--------------------------------------------------------------------------------*/
// Pass bad jet cut
/*--------------------------------------------------------------------------------*/
bool SusyD3PDAna::passBadJet()
{
  return !IsBadJetEvent(&d3pd.jet, m_baseJets, 20.*GeV, m_susyObj);
}
/*--------------------------------------------------------------------------------*/
// Pass good vertex
/*--------------------------------------------------------------------------------*/
bool SusyD3PDAna::passGoodVtx()
{
  return PrimaryVertexCut(m_susyObj, &d3pd.vtx);
}
/*--------------------------------------------------------------------------------*/
// Pass bad muon veto
/*--------------------------------------------------------------------------------*/
bool SusyD3PDAna::passBadMuon()
{
  return !IsBadMuonEvent(m_susyObj, &d3pd.muo, m_preMuons, 0.2);
}
/*--------------------------------------------------------------------------------*/
// Pass cosmic veto
/*--------------------------------------------------------------------------------*/
bool SusyD3PDAna::passCosmic()
{
  return !IsCosmic(m_susyObj, &d3pd.muo, m_baseMuons, 1., 0.2);
}
