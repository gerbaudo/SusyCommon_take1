#ifndef SusyCommon_SusyNtAna_h
#define SusyCommon_SusyNtAna_h

#include "TSelector.h"
#include "TTree.h"

#include "SusyCommon/SusyNtObject.h"

/*

    SusyNtAna - base class for analyzing SusyNt

*/

class SusyNtAna : public TSelector
{

  public:

    // Constructor and destructor
    SusyNtAna();
    virtual ~SusyNtAna(){};

    // SusyNt object, access to the SusyNt variables
    Susy::SusyNtObject nt;

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
    // Due to ROOT's stupid design, need to specify version >= 2 or the tree will not connect automatically
    virtual Int_t   Version() const {
      return 2;
    }

    // Main event loop function
    virtual Bool_t  Process(Long64_t entry);

    // Get entry simply communicates the entry number from TSelector 
    // to this class and hence to all of the VarHandles
    virtual Int_t   GetEntry(Long64_t e, Int_t getall = 0) {
      m_entry=e;
      return kTRUE;
    }

    // Get event weight - currently only MC weight
    float getEventWeight();

    // Object selection
    void clearObjects();
    void selectLeptons();
    void selectJets();
    bool isSignalElectron(const Susy::Electron*);
    bool isSignalMuon(const Susy::Muon*);
    bool isSignalJet(const Susy::Jet*);

    // Run dependent trigger chains
    uint getEleTrigger();
    uint getMuoTrigger();
    uint get2EleTrigger();
    uint get2MuoTrigger();

    // Event and object dumps
    void dumpEvent();
    void dumpBaselineObjects();
    void dumpSignalObjects();

    // Debug level
    void setDebug(int dbg) { m_dbg = dbg; }
    int dbg() { return m_dbg; }

    // Sample name - can be used however you like
    std::string sampleName() { return m_sample; }
    void setSampleName(std::string s) { m_sample = s; }

    // Access tree
    TTree* getTree() { return m_tree; }

    ClassDef(SusyNtAna, 1);

  protected:

    TTree* m_tree;              // Input tree (I think it actually points to a TChain)

    Long64_t m_entry;           // Current entry in the current tree (not chain index!)

    int m_dbg;                  // debug level

    std::string m_sample;       // sample name string

    //
    // Object collections
    //

    ElectronVector      m_baseElectrons;        // baseline electrons
    ElectronVector      m_signalElectrons;      // signal electrons
    MuonVector          m_baseMuons;            // baseline muons
    MuonVector          m_signalMuons;          // signal muons
    LeptonVector        m_baseLeptons;          // baseline leptons
    LeptonVector        m_signalLeptons;        // signal leptons
    JetVector           m_signalJets;           // signal jets

};



#endif
