#include "SusyCommon/SusyDefs.h"

using namespace std;


/*--------------------------------------------------------------------------------*/
// Trigger chain names
/*--------------------------------------------------------------------------------*/
stringvector getEleTrigChains()
{
  stringvector elTriggers;
  elTriggers.resize(N_EL_TRIG);
  elTriggers[BIT_e10_medium]    = "EF_e10_medium";
  elTriggers[BIT_e12_medium]    = "EF_e12_medium";
  elTriggers[BIT_e20_medium]    = "EF_e20_medium";
  elTriggers[BIT_e22_medium]    = "EF_e22_medium";
  elTriggers[BIT_e22vh_medium1] = "EF_e22vh_medium1";
  return elTriggers;
}
/*--------------------------------------------------------------------------------*/
stringvector getMuTrigChains()
{
  stringvector muTriggers;
  muTriggers.resize(N_MU_TRIG);
  muTriggers[BIT_mu18]          = "EF_mu18";
  muTriggers[BIT_mu18_medium]   = "EF_mu18_medium";
  return muTriggers;
}

/*--------------------------------------------------------------------------------*/
// Build TChain from input fileList
/*--------------------------------------------------------------------------------*/
int buildChain(TChain* chain, string fileListName){
  ifstream fileList(fileListName.c_str());
  if(!fileList.is_open()){
    cout << "ERROR opening fileList " << fileListName << endl;
    return 1;
  }
  string fileName;
  while(fileList >> fileName){
    //cout << fileName << endl;
    chain->Add(fileName.c_str());
  }
  fileList.close();
  return 0;
}

