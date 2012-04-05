#include "SusyCommon/SusyD3PDInterface.h"


using namespace std;

/*--------------------------------------------------------------------------------*/
// Constructor
/*--------------------------------------------------------------------------------*/
SusyD3PDInterface::SusyD3PDInterface(TTree* tree) :
        m_entry(0),
        m_dbg(1)
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

