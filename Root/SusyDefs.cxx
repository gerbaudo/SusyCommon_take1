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
// Flavor functions
/*--------------------------------------------------------------------------------*/
inline bool isSameFlav(const Lepton* l1, const Lepton* l2)
{ return l1->isEle()==l2->isEle() && l1->isMu()==l2->isMu(); }

inline bool isSFOS(const Lepton* l1, const Lepton* l2)
{ return isSameFlav(l1,l2) && (l1->q*l2->q < 0); }

inline bool isSFSS(const Lepton* l1, const Lepton* l2)
{ return isSameFlav(l1,l2) && (l1->q*l2->q > 0); }

/*--------------------------------------------------------------------------------*/
// Mass functions
/*--------------------------------------------------------------------------------*/
inline float Mll(const Lepton* l1, const Lepton* l2)
{ return (*l1 + *l2).M(); }

inline float Mlll(const Lepton* l1, const Lepton* l2, const Lepton* l3)
{ return (*l1 + *l2 + *l3).M(); }

inline float Mt(const Lepton* lep, const Met* met)
{ return sqrt( 2.*lep->Pt()*met->Pt()*(1 - cos(lep->DeltaPhi(*met))) ); }

inline bool isZ(const Lepton* l1, const Lepton* l2, float massWindow)
{ return fabs( Mll(l1,l2)-MZ ) < massWindow; }

inline bool hasZ(const LeptonVector& leps, float massWindow)
{
  for(uint i=0; i<leps.size(); i++){
    for(uint j=i+1; j<leps.size(); j++){
      if( isZ(leps[i],leps[j],massWindow) ) return true;
    }
  }
  return false;
}

/*--------------------------------------------------------------------------------*/
// helper function for sorting particle pointers by pt
/*--------------------------------------------------------------------------------*/
bool comparePt(const TLorentzVector* p1, const TLorentzVector* p2) 
{ return p1->Pt() > p2->Pt(); }

/*--------------------------------------------------------------------------------*/
// Trigger chain names
/*--------------------------------------------------------------------------------*/
stringvector getEleTrigChains()
{
  stringvector elTriggers;
  elTriggers.resize(N_EL_TRIG);
  elTriggers[BIT_e10_medium]    = "EF_e10_medium";
  elTriggers[BIT_e12_medium]    = "EF_e12_medium";
  elTriggers[BIT_e20_medium]    = "EF_e20_medium";
  elTriggers[BIT_e22_medium]    = "EF_e22_medium";
  elTriggers[BIT_e22vh_medium1] = "EF_e22vh_medium1";
  return elTriggers;
}
/*--------------------------------------------------------------------------------*/
stringvector getMuTrigChains()
{
  stringvector muTriggers;
  muTriggers.resize(N_MU_TRIG);
  muTriggers[BIT_mu18]          = "EF_mu18";
  muTriggers[BIT_mu18_medium]   = "EF_mu18_medium";
  return muTriggers;
}

