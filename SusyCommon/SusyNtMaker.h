#ifndef SusyCommon_SusyNtMaker_h
#define SusyCommon_SusyNtMaker_h


#include <iostream>

#include "SusyCommon/SusyD3PDAna.h"
#include "SusyCommon/SusyNtObject.h"


/*

    SusyNtMaker - a class for making SusyNt from Susy D3PDs

*/

class SusyNtMaker : public SusyD3PDAna
{

  public:

    // Constructor and destructor
    SusyNtMaker(TTree* /*tree*/ = 0);
    virtual ~SusyNtMaker();

    // Begin is called before looping on entries
    virtual void    Begin(TTree *tree);
    // Main event loop function
    virtual Bool_t  Process(Long64_t entry);
    // Terminate is called after looping is finished
    virtual void    Terminate();

    // Event selection - loose object/event cuts for filling tree
    virtual bool    selectEvent();

    //
    // SusyNt Fill methods
    //

    // all SusyNt variables
    void fillNtVars();
    // event variables
    void fillEventVars();
    // lepton variables
    void fillLeptonVars();
    void fillElectronVars(const LeptonInfo* lepIn);
    void fillMuonVars(const LeptonInfo* lepIn);
    // jet variables
    void fillJetVars();
    void fillMetVars();

  protected:

    TFile*      m_outTreeFile;          // output tree file
    TTree*      m_outTree;              // output tree

    Susy::SusyNtObject  m_susyNt;       // SusyNt interface

};

#endif
