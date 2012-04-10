#ifndef SusyCommon_SusyNtInterface_h
#define SusyCommon_SusyNtInterface_h

#include "TTree.h"

#include "MultiLep/VarHandle.h"
#include "SusyCommon/SusyNt.h"

namespace Susy
{


  /*

      SusyNtObject - An interface class for accessing SusNt objects

  */

  class SusyNtObject : public TObject
  {
    public:
  
      // Constructor for writing
      SusyNtObject();
      // Constructor for reading
      SusyNtObject(const Long64_t& entry);
  
      // Connect the objects to an output tree
      void WriteTo( TTree* tree );
      // Connect the objects to an input tree
      void ReadFrom( TTree* tree );
      // Clear variables when in read mode
      void clear();

      //
      // SusyNt variables
      // This may change to a map based usage later for systematics
      //

      // Event variables
      D3PDReader::VarHandle< Event* >                   evt;
      // Electron variables
      D3PDReader::VarHandle< std::vector<Electron>* >   ele;
      // Muon variables
      D3PDReader::VarHandle< std::vector<Muon>* >       muo;
      // Jet variables
      D3PDReader::VarHandle< std::vector<Jet>* >        jet;
      // Met variables
      D3PDReader::VarHandle< Met* >                     met;

      ClassDef(SusyNtObject,1);

    protected:

  
  };


};


#endif
