#ifndef SUSYAnalysis_SusyNt_h
#define SUSYAnalysis_SusyNt_h


//-----------------------------------------------------------------------------------
//  SusyNt
//
//  This header file contains definitions for the SusyNt tree structure
//  The classes here were adapted from UCINtuple classes but designed
//  to be more streamlined for quick analysis.  
//  They should hold the minimal content for doing analysis.
//  If an advanced study requires many additional variables, it might be preferred 
//  to write a new derived class rather than clutter theses classes up.
//-----------------------------------------------------------------------------------


#include "TLorentzVector.h"

#include "SusyCommon/SusyDefs.h"


namespace Susy
{
  // Event class
  class Event : public TObject
  {
    public:
      Event(){};
      virtual ~Event(){};

      uint run;                 // run number (or MC ID number)
      uint event;               // event number
      uint lb;                  // lumi block number
      uint nVtx;                // number of good vertices

      ClassDef(Event, 0);
  };

  // Particle class, base class for other object types
  class Particle : public TLorentzVector
  {
    public:
      Particle(){};
      virtual ~Particle(){};

      // Comparison operators for sorting, etc.
      inline bool operator > (const Particle & other) const {
        return Pt() > other.Pt();
      }
      inline bool operator < (const Particle & other) const {
        return Pt() < other.Pt();
      }

      ClassDef(Particle, 0);
  };

  // Lepton class, common to electrons and muons 
  class Lepton : public Particle
  {
    public:
      Lepton(){};
      virtual ~Lepton(){};
      
      float q;                  // Charge
      float ptcone20;           // ptcone20 isolation
      float d0;                 // d0 extrapolated to PV 
      float errD0;              // Uncertainty on d0
      uint mcType;              // MCTruthClassifier particle type
      uint mcOrigin;            // MCTruthClassifier particle origin

      ClassDef(Lepton, 0);
  };

  // Electron class
  class Electron : public Lepton
  {
    public:
      Electron(){};
      virtual ~Electron(){};

      float clusEta;            // CaloCluster eta
      bool mediumPP;            // isEM medium++

      ClassDef(Electron, 0);
  };

  // Muon class
  class Muon : public Lepton
  {
    public:
      Muon(){};
      virtual ~Muon(){};

      bool isCombined;          // Is combined muon

      ClassDef(Muon, 0);
  };

  // Jet class
  class Jet : public Particle
  {
    public:
      Jet(){};
      virtual ~Jet(){};

      float jvf;                // Jet vertex fraction
      float combNN;             // JetFitterCombNN b-tag weight

      ClassDef(Jet, 0);
  };

  // Met class
  class Met : public Particle
  {
    public:
      Met(){};
      virtual ~Met(){};

      // MET Composition info - do we want TLorentzVectors, TVector2, or just floats?
      float refEle;             // Ref electron term
      float refMuo;             // Ref muon term
      float refJet;             // Ref jet term
      float refCell;            // Cellout term

      ClassDef(Met, 0);
  };

};


#endif
