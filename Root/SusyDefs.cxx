#include "SusyCommon/SusyDefs.h"

using namespace std;

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

