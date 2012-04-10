#ifndef SusyCommon_SusyDefs_h
#define SusyCommon_SusyDefs_h

#include <iostream>
#include <fstream>
#include <string>

#include "TChain.h"

#include "MultiLep/ElectronD3PDObject.h"
#include "MultiLep/MuonD3PDObject.h"
#include "MultiLep/JetD3PDObject.h"
#include "MultiLep/TruthMuonD3PDObject.h"


//-----------------------------------------------------------------------------------
//  SusyDefs
//
//  common definitions for SUSY analysis code
//-----------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------
// Global constants
//-----------------------------------------------------------------------------------
const float GeV = 1000.;

//-----------------------------------------------------------------------------------
// Convenience typedefs
//-----------------------------------------------------------------------------------
typedef unsigned int uint;
typedef std::vector<std::string> stringvector;
typedef D3PDReader::ElectronD3PDObjectElement ElectronElement;
typedef D3PDReader::MuonD3PDObjectElement MuonElement;
typedef D3PDReader::JetD3PDObjectElement JetElement;
typedef D3PDReader::TruthMuonD3PDObjectElement TruthMuonElement;

// Truth particle proxy class was not generated.  TODO
// -> It might not work because the structure is slightly different
//    There is a channel_number variable which isn't a vector
//typedef D3PDReader::TruthParticleD3PDObjectElement TruthElement;


//-----------------------------------------------------------------------------------
// Trigger flags
//-----------------------------------------------------------------------------------

// Trigger enums - try to respect backwards compatibility by adding to the end
enum ElecTrigBit
{
  BIT_e10_medium = 0,
  BIT_e12_medium,
  BIT_e20_medium,
  BIT_e22_medium,
  BIT_e22vh_medium1,
  N_EL_TRIG
};
enum MuonTrigBit
{
  BIT_mu6 = 0,
  BIT_mu10_loose,
  BIT_mu18,
  BIT_mu18_medium,
  N_MU_TRIG
};

// Trigger bit masks - can represent multiple chains at once
// electron
const uint TRIG_e20_medium      = 1<<BIT_e20_medium;
const uint TRIG_e22_medium      = 1<<BIT_e22_medium;
const uint TRIG_e22vh_medium1   = 1<<BIT_e22vh_medium1;
// muon
const uint TRIG_mu18            = 1<<BIT_mu18;
const uint TRIG_mu18_medium     = 1<<BIT_mu18_medium;

// Trigger chain names, for convenience
stringvector getEleTrigChains();
stringvector getMuTrigChains();


//-----------------------------------------------------------------------------------
// Global functions
//-----------------------------------------------------------------------------------

// Build a TChain out of a fileList - returns nonzero if error
int buildChain(TChain* chain, std::string fileListName);


#endif
