#ifndef SUSYAnalysis_SusyD3PDInterface_h
#define SUSYAnalysis_SusyD3PDInterface_h


#include <iostream>

#include "TSelector.h"
#include "TTree.h"


/*

    SusyD3PDInterface

    A class for reading SUSY D3PDs using the interfaces defined in the SUSY MultiLep common code package

*/

class SusyD3PDInterface : public TSelector
{

  public:

    // Constructor and destructor
    SusyD3PDInterface(TTree * /*tree*/ = 0);
    virtual ~SusyD3PDInterface();

    //
    // TSelector methods
    //

    // Init is called every time a new TTree is attached
    virtual void    Init(TTree *tree);
    // Begin is called before looping on entries
    virtual void    Begin(TTree *tree);
    // Called at the first entry of a new file in a chain
    virtual Bool_t  Notify() { return kTRUE; }
    // Terminate is called after looping is finished
    virtual void    Terminate();

    // Main event loop function
    virtual Bool_t  Process(Long64_t entry);

    // Get entry simply communicates the entry number from TSelector to this class
    // D3PD interface objects hold pointers to m_entry;
    virtual Int_t   GetEntry(Long64_t e, Int_t getall = 0) {
        m_entry=e;
        return kTRUE;
    }

    //
    // Other methods
    //

    // Debug level
    void setDebug(int dbg) { m_dbg = dbg; }
    int dbg() { return m_dbg; }

    // Access tree
    TTree* getTree() { return m_tree; }

    ClassDef(SusyD3PDInterface, 0);

  protected:

    TTree* m_tree;              // Current tree

    Long64_t m_entry;           // Current entry in the current tree (not chain index!)

    int m_dbg;                  // debug level

};

#endif
