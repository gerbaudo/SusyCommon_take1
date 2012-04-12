#ifndef SusyCommon_SusyNt_h
#define SusyCommon_SusyNt_h


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

#include <iostream>

#include "TLorentzVector.h"

#include "SusyCommon/SusyDefs.h"


namespace Susy
{

  // Particle class, base class for other object types
  class Particle : public TLorentzVector
  {
    public:
      Particle(){ clear(); }
      virtual ~Particle(){};

      void clear() { TLorentzVector::Clear(); }

      // print method
      virtual void print() const {};

      // Comparison operators for sorting, etc.
      inline bool operator > (const Particle & other) const {
        return Pt() > other.Pt();
      }
      inline bool operator < (const Particle & other) const {
        return Pt() < other.Pt();
      }

      ClassDef(Particle, 1);
  };

  // Lepton class, common to electrons and muons 
  class Lepton : public Particle
  {
    public:
      Lepton(){ clear(); }
      virtual ~Lepton(){};

      // public member vars
      int q;                    // Charge
      float ptcone20;           // ptcone20 isolation
      float d0;                 // d0 extrapolated to PV 
      float errD0;              // Uncertainty on d0
      uint mcType;              // MCTruthClassifier particle type
      uint mcOrigin;            // MCTruthClassifier particle origin

      uint trigFlags;           // Bit word representing matched trigger chains

      // trigger matching
      bool matchTrig(uint mask){
        return (trigFlags & mask) == mask;
      }

      // polymorphism, baby!!
      virtual bool isEle() const { return false; }
      virtual bool isMu()  const { return false; }

      // print method
      virtual void print() const {};

      // clear vars
      void clear(){
        q = ptcone20 = d0 = errD0 = mcType = mcOrigin = trigFlags = 0;
        Particle::clear();
      }
      
      ClassDef(Lepton, 2);
  };

  // Electron class
  class Electron : public Lepton
  {
    public:
      Electron(){ clear(); }
      virtual ~Electron(){};

      float clusEta;            // CaloCluster eta
      bool mediumPP;            // isEM medium++
      bool tightPP;             // isEM tight++

      // polymorphism, baby!!
      bool isEle() const { return true; }
      bool isMu()  const { return false; }

      // print method
      void print() const;

      // clear vars
      void clear(){
        clusEta = mediumPP = tightPP = 0;
        Lepton::clear();
      }

      ClassDef(Electron, 1);
  };

  // Muon class
  class Muon : public Lepton
  {
    public:
      Muon(){ clear(); }
      virtual ~Muon(){};

      bool isCombined;          // Is combined muon

      // polymorphism, baby!!
      bool isEle() const { return false; }
      bool isMu()  const { return true; }

      // print method
      void print() const;

      // clear vars
      void clear(){
        isCombined = 0;
        Lepton::clear();
      }

      ClassDef(Muon, 1);
  };

  // Jet class
  class Jet : public Particle
  {
    public:
      Jet(){ clear(); }
      virtual ~Jet(){};

      float jvf;                // Jet vertex fraction
      float combNN;             // JetFitterCombNN b-tag weight

      // print method
      void print() const;

      // clear vars
      void clear(){
        jvf = combNN = 0;
        Particle::clear();
      }

      ClassDef(Jet, 1);
  };

  // Met class
  class Met : public Particle
  {
    public:
      Met(){ clear(); }
      virtual ~Met(){};

      // MET Composition info - do we want TLorentzVectors, TVector2, or just floats?
      float refEle;             // Ref electron term
      float refMuo;             // Ref muon term
      float refJet;             // Ref jet term
      float refCell;            // Cellout term

      // print vars
      void print() const;

      // clear vars
      void clear(){
        refEle = refMuo = refJet = refCell = 0;
        Particle::clear();
      }

      ClassDef(Met, 1);
  };

  // Event class
  class Event: public TObject
  {
    public:
      Event(){ clear(); }
      virtual ~Event(){};

      uint run;                 // run number 
      uint event;               // event number
      uint lb;                  // lumi block number
      uint nVtx;                // number of good vertices

      bool isMC;                // is MC flag
      uint mcChannel;           // MC channel ID number (mc run number)
      float w;                  // MC generator weight

      // print event
      void print() const{
        std::cout << "run " << run << " event " << event << " isMC " << isMC << std::endl;
      }

      // clear vars
      void clear(){
        run = event = lb = nVtx = 0;
        isMC = false;
        mcChannel = w = 0;
      }

      ClassDef(Event, 2);
  };

};


#endif
