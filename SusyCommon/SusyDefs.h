#ifndef SUSYAnalysis_SusyDefs_h
#define SUSYAnalysis_SusyDefs_h

#include <iostream>
#include <fstream>
#include <string>

#include "TChain.h"

#include "MultiLep/ElectronD3PDObject.h"
#include "MultiLep/MuonD3PDObject.h"
#include "MultiLep/JetD3PDObject.h"


//-----------------------------------------------------------------------------------
//  SusyDefs
//
//  common definitions for SUSY analysis code
//-----------------------------------------------------------------------------------


// typedefs
typedef unsigned int uint;
typedef D3PDReader::ElectronD3PDObjectElement ElectronElement;
typedef D3PDReader::MuonD3PDObjectElement MuonElement;
typedef D3PDReader::JetD3PDObjectElement JetElement;
// Truth particle proxy class was not generated.  TODO
//typedef D3PDReader::TruthParticleD3PDObjectElement TruthElement;


// preprocessor magic
#define GeV 1000.




//
// Helper functions
//

// Build a TChain out of a fileList - returns nonzero if error
int buildChain(TChain* chain, std::string fileListName);


#endif
