#include <iomanip>
#include "TFile.h"
#include "SusyCommon/SusyNtAna.h"

using namespace std;
using namespace Susy;

/*--------------------------------------------------------------------------------*/
// SusyNtAna Constructor
/*--------------------------------------------------------------------------------*/
SusyNtAna::SusyNtAna() : 
        nt(m_entry),
        m_entry(0),
        m_dbg(0)
{
}

/*--------------------------------------------------------------------------------*/
// Attach tree (actually a TChain)
/*--------------------------------------------------------------------------------*/
void SusyNtAna::Init(TTree* tree)
{
  if(m_dbg) cout << "SusyNtAna::Init" << endl;
  m_tree = tree;
  nt.ReadFrom(tree);
}

/*--------------------------------------------------------------------------------*/
// The Begin() function is called at the start of the query.
// When running with PROOF Begin() is only called on the client.
// The tree argument is deprecated (on PROOF 0 is passed).
/*--------------------------------------------------------------------------------*/
void SusyNtAna::Begin(TTree* /*tree*/)
{
  if(m_dbg) cout << "SusyNtAna::Begin" << endl;
}

/*--------------------------------------------------------------------------------*/
// Main process loop function - This is just an example for testing
/*--------------------------------------------------------------------------------*/
Bool_t SusyNtAna::Process(Long64_t entry)
{
  // Communicate tree entry number to SusyNtObject
  GetEntry(entry);
  clearObjects();

  // Chain entry not the same as tree entry
  static Long64_t chainEntry = -1;
  chainEntry++;
  if(m_dbg || chainEntry%50000==0)
  {
    cout << "**** Processing entry " << setw(6) << chainEntry
         << " run " << setw(6) << nt.evt()->run
         << " event " << setw(7) << nt.evt()->event << " ****" << endl;
  }

  // Dump variables from the tree for testing
  if(m_dbg){
    cout << "num base ele: " << nt.ele()->size() << endl;
    cout << "num base muo: " << nt.muo()->size() << endl;
    cout << "num base jet: " << nt.jet()->size() << endl;
    //cout << "Met:          " << nt.met()->Pt()   << endl;
  }

  // select signal objects
  selectLeptons();  
  selectJets();

  if(m_signalLeptons.size() >= 3){

    cout << endl;
    nt.evt()->print();

    cout << "Signal electrons" << endl;
    for(uint iEl=0; iEl<m_signalElectrons.size(); iEl++){
      cout << "  " << iEl << " "; m_signalElectrons[iEl]->print();
    }
    cout << "Signal muons" << endl;
    for(uint iMu=0; iMu<m_signalMuons.size(); iMu++){
      cout << "  " << iMu << " "; m_signalMuons[iMu]->print();
    }
    cout << "Signal jets" << endl;
    for(uint iJ=0; iJ<m_signalJets.size(); iJ++){
      cout << "  " << iJ << " "; m_signalJets[iJ]->print();
    }
    nt.met()->print();

  }

  return kTRUE;
}

/*--------------------------------------------------------------------------------*/
// The Terminate() function is the last function to be called during
// a query. It always runs on the client, it can be used to present
// the results graphically or save the results to file.
/*--------------------------------------------------------------------------------*/
void SusyNtAna::Terminate()
{
  if(m_dbg) cout << "SusyNtAna::Terminate" << endl;
}

/*--------------------------------------------------------------------------------*/
// Get event weight, combine gen, pileup, xsec, and lumi weights
/*--------------------------------------------------------------------------------*/
float SusyNtAna::getEventWeight()
{
  // If data, these should all be one
  const Event* evt = nt.evt();
  return evt->w * evt->wPileup * evt->xsec * evt->lumiSF;
}

/*--------------------------------------------------------------------------------*/
// Clear objects
/*--------------------------------------------------------------------------------*/
void SusyNtAna::clearObjects()
{
  m_baseElectrons.clear();
  m_signalElectrons.clear();
  m_baseMuons.clear();
  m_signalMuons.clear();
  m_baseLeptons.clear();
  m_signalLeptons.clear();
  m_signalJets.clear();
}
/*--------------------------------------------------------------------------------*/
// Select baseline and signal leptons
/*--------------------------------------------------------------------------------*/
void SusyNtAna::selectLeptons()
{
  // Currently, the objects stored in SusyNt are baseline objects
  const vector<Electron>* electrons = nt.ele();
  uint nEle = electrons->size();
  for(uint iEl=0; iEl<nEle; iEl++){
    const Electron* ele = & (*electrons)[iEl];
    m_baseElectrons.push_back(ele);
    m_baseLeptons.push_back(ele);
    if(isSignalElectron(ele)){
      m_signalElectrons.push_back(ele);
      m_signalLeptons.push_back(ele);
    }
  }
  const vector<Muon>* muons = nt.muo();
  uint nMu = muons->size();
  for(uint iMu=0; iMu<nMu; iMu++){
    const Muon* mu = & (*muons)[iMu];
    m_baseMuons.push_back(mu);
    m_baseLeptons.push_back(mu);
    if(isSignalMuon(mu)){
      m_signalMuons.push_back(mu);
      m_signalLeptons.push_back(mu);
    }
  }
  // sort leptons by pt
  std::sort(m_baseLeptons.begin(), m_baseLeptons.end(), comparePt);
  std::sort(m_signalLeptons.begin(), m_signalLeptons.end(), comparePt);
}
/*--------------------------------------------------------------------------------*/
bool SusyNtAna::isSignalElectron(const Electron* ele)
{
  // hardcode values for now, make it better later
  if(!ele->tightPP) return false;
  if(ele->ptcone20/ele->Pt() > 0.1) return false;
  return true;
}
/*--------------------------------------------------------------------------------*/
bool SusyNtAna::isSignalMuon(const Muon* mu)
{
  // hardcode values for now, make it better later
  if(mu->ptcone20 > 1.8) return false;
  return true;
}
/*--------------------------------------------------------------------------------*/
// Select signal jets
/*--------------------------------------------------------------------------------*/
void SusyNtAna::selectJets()
{
  const vector<Jet>* jets = nt.jet();
  uint nJet = jets->size();
  for(uint iJ=0; iJ<nJet; iJ++){
    const Jet* jet = & (*jets)[iJ];
    if(isSignalJet(jet)) m_signalJets.push_back(jet);
  }
}
/*--------------------------------------------------------------------------------*/
// Signal jet
/*--------------------------------------------------------------------------------*/
bool SusyNtAna::isSignalJet(const Jet* jet)
{
  if(jet->Pt() < 30) return false;
  if(jet->Eta() > 2.5) return false;
  if(jet->jvf < 0.75) return false;
  return true;
}

/*--------------------------------------------------------------------------------*/
// Run dependent trigger chains
/*--------------------------------------------------------------------------------*/
uint SusyNtAna::getEleTrigger()
{
  uint run = nt.evt()->run;
  if(run < 186873) return TRIG_e20_medium;
  else if(run < 188902) return TRIG_e22_medium;
  else return TRIG_e22vh_medium1;
}
/*--------------------------------------------------------------------------------*/
uint SusyNtAna::getMuoTrigger()
{
  uint run = nt.evt()->run;
  if(run < 186516) return TRIG_mu18;
  else return TRIG_mu18_medium;
}
/*--------------------------------------------------------------------------------*/
uint SusyNtAna::get2EleTrigger()
{
  return 0;
  //uint run = nt.evt()->run;
  //if(run < 186873) return TRIG_2e12_medium;
  //else if(run < 188902) return TRIG_2e12T_medium;
  //else return TRIG_2e12Tvh_medium1;
}
/*--------------------------------------------------------------------------------*/
uint SusyNtAna::get2MuoTrigger()
{
  return 0;
  //return TRIG_2mu10_loose;
}

/*--------------------------------------------------------------------------------*/
// Trigger reweight maps
/*--------------------------------------------------------------------------------*/
void SusyNtAna::loadTriggerMaps()
{
  TFile* eleTrigFile = new TFile("$ROOTCOREDIR/data/DGTriggerReweight/electron_maps.root");
  TFile* muoTrigFile = new TFile("$ROOTCOREDIR/data/DGTriggerReweight/muon_triggermaps.root");

  // TODO: dilepton triggers
  m_elTrigWeightMap[TRIG_e20_medium]     = loadTrigWeighter(eleTrigFile, "e20_medium");
  m_elTrigWeightMap[TRIG_e22_medium]     = loadTrigWeighter(eleTrigFile, "e22_medium");
  m_elTrigWeightMap[TRIG_e22vh_medium1]  = loadTrigWeighter(eleTrigFile, "e22vh_medium1");
  //m_elTrigWeightMap[TRIG_2e12_medium]    = loadTrigWeighter(eleTrigFile, "e12_medium");
  //m_elTrigWeightMap[TRIG_2e12T_medium]   = loadTrigWeighter(eleTrigFile, "e12T_medium");
  //m_elTrigWeightMap[TRIG_2e12Tvh_medium] = loadTrigWeighter(eleTrigFile, "e12Tvh_medium");

  m_muTrigWeightMap[TRIG_mu18]           = loadTrigWeighter(muoTrigFile, "mu18");
  m_muTrigWeightMap[TRIG_mu18_medium]    = loadTrigWeighter(muoTrigFile, "mu18_medium");
}
/*--------------------------------------------------------------------------------*/
APReweightND* SusyNtAna::loadTrigWeighter(TFile* f, TString chain)
{
  TString numName = "ths_"+chain+"_num";
  TString denName = "ths_"+chain+"_den";
  // muon file currently contains a typo
  if (chain.Contains("mu")) numName = "ths_"+chain+"_nom";

  // Does this memory get cleaned up when the file closes?
  THnSparseD* num = (THnSparseD*) f->Get( numName );
  THnSparseD* den = (THnSparseD*) f->Get( denName );
  if(!num || !den){
    cout << "ERROR loading trig maps for chain " << chain << endl;
    abort();
  }
  return new APReweightND( den, num, true );
}
/*--------------------------------------------------------------------------------*/
APReweightND* SusyNtAna::getEleTrigWeighter(uint trigFlag)
{
  map<int, APReweightND*>::iterator itr = m_elTrigWeightMap.find(trigFlag);
  if(itr==m_elTrigWeightMap.end()){
    cout << "ERROR - Electron trigger reweighter " << trigFlag << " doesn't exist" << endl;
    return 0;
  }
  return itr->second;
}
/*--------------------------------------------------------------------------------*/
APReweightND* SusyNtAna::getMuoTrigWeighter(uint trigFlag)
{
  map<int, APReweightND*>::iterator itr = m_muTrigWeightMap.find(trigFlag);
  if(itr==m_muTrigWeightMap.end()){
    cout << "ERROR - Muon trigger reweighter " << trigFlag << " doesn't exist" << endl;
    return 0;
  }
  return itr->second;
}

/*--------------------------------------------------------------------------------*/
// Event and object dumps
/*--------------------------------------------------------------------------------*/
void SusyNtAna::dumpEvent()
{
  nt.evt()->print();
  nt.met()->print();
}
/*--------------------------------------------------------------------------------*/
void SusyNtAna::dumpBaselineObjects()
{
  cout << "Baseline electrons" << endl;
  for(uint iEl=0; iEl < m_baseElectrons.size(); iEl++){
    cout << "  ";
    m_baseElectrons[iEl]->print();
  }
  cout << "Baseline muons" << endl;
  for(uint iMu=0; iMu < m_baseMuons.size(); iMu++){
    cout << "  ";
    m_baseMuons[iMu]->print();
  }
  // baseline jets are already stored in the SusyNt
  cout << "Baseline jets" << endl;
  uint nJet = nt.jet()->size();
  for(uint iJ=0; iJ < nJet; iJ++){
    cout << "  ";
    nt.jet()->at(iJ).print();
  }
}
/*--------------------------------------------------------------------------------*/
void SusyNtAna::dumpSignalObjects()
{
  cout << "Signal electrons" << endl;
  for(uint iEl=0; iEl < m_signalElectrons.size(); iEl++){
    cout << "  ";
    m_signalElectrons[iEl]->print();
  }
  cout << "Signal muons" << endl;
  for(uint iMu=0; iMu < m_signalMuons.size(); iMu++){
    cout << "  ";
    m_signalMuons[iMu]->print();
  }
  cout << "Signal jets" << endl;
  for(uint iJ=0; iJ < m_signalJets.size(); iJ++){
    cout << "  ";
    m_signalJets[iJ]->print();
  }
}
