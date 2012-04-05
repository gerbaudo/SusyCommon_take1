#include "SusyCommon/SusyD3PDInterface.h"


using namespace std;


/*--------------------------------------------------------------------------------*/
// SusyD3PDContainer constructor
/*--------------------------------------------------------------------------------*/
SusyD3PDContainer::SusyD3PDContainer(const Long64_t& entry) :
        evt(entry),
        ele(entry),
        muo(entry),
        jet(entry),
        met(entry),
        vtx(entry),
        trig(entry),
        gen(entry),
        truth(entry)
{}
/*--------------------------------------------------------------------------------*/
// Connect tree to the D3PD objects in container
/*--------------------------------------------------------------------------------*/
void SusyD3PDContainer::ReadFrom(TTree* tree)
{
  evt.ReadFrom(tree);
  ele.ReadFrom(tree);
  muo.ReadFrom(tree);
  jet.ReadFrom(tree);
  met.ReadFrom(tree);
  vtx.ReadFrom(tree);
  trig.ReadFrom(tree);
  gen.ReadFrom(tree);
  truth.ReadFrom(tree);
}

/*--------------------------------------------------------------------------------*/
// SusyD3PDInterface Constructor
/*--------------------------------------------------------------------------------*/
SusyD3PDInterface::SusyD3PDInterface(TTree* tree) :
        d3pd(m_entry),
        m_entry(0),
        m_dbg(0)
{}
/*--------------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------------*/
SusyD3PDInterface::~SusyD3PDInterface()
{}

/*--------------------------------------------------------------------------------*/
// Attach tree (or is it a chain???)
/*--------------------------------------------------------------------------------*/
void SusyD3PDInterface::Init(TTree* tree)
{
  if(m_dbg) cout << "SusyD3PDInterface::Init" << endl;
  m_tree = tree;
  d3pd.ReadFrom(tree);
}

/*--------------------------------------------------------------------------------*/
// The Begin() function is called at the start of the query.
// When running with PROOF Begin() is only called on the client.
// The tree argument is deprecated (on PROOF 0 is passed).
/*--------------------------------------------------------------------------------*/
void SusyD3PDInterface::Begin(TTree* /*tree*/)
{
  if(m_dbg) cout << "SusyD3PDInterface::Begin" << endl;
}

/*--------------------------------------------------------------------------------*/
// Main process loop function
/*--------------------------------------------------------------------------------*/
Bool_t SusyD3PDInterface::Process(Long64_t entry)
{
  // Communicate the entry number to the interface objects
  GetEntry(entry);

  return kTRUE;
}

/*--------------------------------------------------------------------------------*/
// The Terminate() function is the last function to be called during
// a query. It always runs on the client, it can be used to present
// the results graphically or save the results to file.
/*--------------------------------------------------------------------------------*/
void SusyD3PDInterface::Terminate()
{
  if(m_dbg) cout << "SusyD3PDInterface::Terminate" << endl;
}

