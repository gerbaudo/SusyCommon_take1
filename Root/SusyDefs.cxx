#include <algorithm>
#include "SusyCommon/SusyDefs.h"
#include "SusyCommon/SusyNt.h"

using namespace std;
using namespace Susy;



/*--------------------------------------------------------------------------------*/
// LepInfo comparison operators
/*--------------------------------------------------------------------------------*/
inline bool LepInfo::operator > (const LepInfo & other) const
{
  return l->Pt() > other.l->Pt();
}
inline bool LepInfo::operator < (const LepInfo & other) const
{
  return l->Pt() < other.l->Pt();
}

/*--------------------------------------------------------------------------------*/
// Data stream
/*--------------------------------------------------------------------------------*/
string streamName(DataStream stream)
{
  if     (stream==Stream_MC) return "MC";
  else if(stream==Stream_Muons) return "Muons";
  else if(stream==Stream_Egamma) return "Egamma";
  else return "Unknown";
}

/*--------------------------------------------------------------------------------*/
// Flavor functions
/*--------------------------------------------------------------------------------*/
bool isSameFlav(const Lepton* l1, const Lepton* l2)
{ return l1->isEle()==l2->isEle() && l1->isMu()==l2->isMu(); }

bool isSFOS(const Lepton* l1, const Lepton* l2)
{ return isSameFlav(l1,l2) && (l1->q*l2->q < 0); }

bool isSFSS(const Lepton* l1, const Lepton* l2)
{ return isSameFlav(l1,l2) && (l1->q*l2->q > 0); }

bool hasSFOS(const LeptonVector& leps){
  uint nLep = leps.size();
  for(uint i=0; i<nLep; i++){
    for(uint j=i+1; j<nLep; j++){
      if(isSFOS(leps[i],leps[j])) return true;
    }
  }
  return false;
}

/*--------------------------------------------------------------------------------*/
// Mass functions
/*--------------------------------------------------------------------------------*/
float Mll(const Lepton* l1, const Lepton* l2)
{ return (*l1 + *l2).M(); }

float Mlll(const Lepton* l1, const Lepton* l2, const Lepton* l3)
{ return (*l1 + *l2 + *l3).M(); }

float Mt(const Lepton* lep, const Met* met)
{ return sqrt( 2.*lep->Pt()*met->Pt()*(1 - cos(lep->DeltaPhi(*met))) ); }

bool isZ(const Lepton* l1, const Lepton* l2, float massWindow)
{ return isSFOS(l1,l2) && fabs( Mll(l1,l2)-MZ ) < massWindow; }

bool hasZ(const LeptonVector& leps, float massWindow)
{
  for(uint i=0; i<leps.size(); i++){
    for(uint j=i+1; j<leps.size(); j++){
      if( isZ(leps[i],leps[j],massWindow) ) return true;
    }
  }
  return false;
}

/*--------------------------------------------------------------------------------*/
// Jet functions
/*--------------------------------------------------------------------------------*/
bool hasBJet(const JetVector& jets, float weight)
{
  uint nJet = jets.size();
  for(uint i=0; i<nJet; i++){
    if( jets[i]->combNN > weight ) return true;
  }
  return false;
}

/*--------------------------------------------------------------------------------*/
// helper function for sorting particle pointers by pt
/*--------------------------------------------------------------------------------*/
bool comparePt(const TLorentzVector* p1, const TLorentzVector* p2) 
{ return p1->Pt() > p2->Pt(); }

/*--------------------------------------------------------------------------------*/
// Find a lepton in a collection
/*--------------------------------------------------------------------------------*/
bool findLepton(const Lepton* lep, const LeptonVector& leptons) { 
  return std::find(leptons.begin(), leptons.end(), lep) != leptons.end(); 
}

/*--------------------------------------------------------------------------------*/
// Trigger chain names
/*--------------------------------------------------------------------------------*/
stringvector getEleTrigChains()
{
  stringvector elTriggers;
  elTriggers.resize(N_EL_TRIG);
  elTriggers[BIT_e10_medium]     = "EF_e10_medium";
  elTriggers[BIT_e12_medium]     = "EF_e12_medium";
  elTriggers[BIT_e20_medium]     = "EF_e20_medium";
  elTriggers[BIT_e22_medium]     = "EF_e22_medium";
  elTriggers[BIT_e22vh_medium1]  = "EF_e22vh_medium1";
  elTriggers[BIT_2e12_medium]    = "EF_2e12_medium";
  elTriggers[BIT_2e12T_medium]   = "EF_2e12T_medium";
  elTriggers[BIT_2e12Tvh_medium] = "EF_2e12Tvh_medium";
  return elTriggers;
}
/*--------------------------------------------------------------------------------*/
stringvector getMuTrigChains()
{
  stringvector muTriggers;
  muTriggers.resize(N_MU_TRIG);
  muTriggers[BIT_mu6]           = "EF_mu6";
  muTriggers[BIT_mu10_loose]    = "EF_mu10_loose";
  muTriggers[BIT_mu18]          = "EF_mu18";
  muTriggers[BIT_mu18_medium]   = "EF_mu18_medium";
  muTriggers[BIT_2mu10_loose]   = "EF_2mu10_loose";
  return muTriggers;
}

