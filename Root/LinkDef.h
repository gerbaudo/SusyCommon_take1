#include <vector>

#include "SusyCommon/SusyNt.h"
#include "SusyCommon/SusyD3PDInterface.h"
#include "SusyCommon/SusyD3PDAna.h"
#include "SusyCommon/SusyNtObject.h"
#include "SusyCommon/SusyNtMaker.h"
#include "SusyCommon/SusyNtAna.h"


#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;


#pragma link C++ class SusyD3PDInterface;
#pragma link C++ class SusyD3PDAna;
#pragma link C++ class SusyNtMaker;
#pragma link C++ class SusyNtAna;


#pragma link C++ namespace Susy+;

#pragma link C++ class Susy::SusyNtObject;
#pragma link C++ class Susy::Particle+;
#pragma link C++ class Susy::Lepton+;
#pragma link C++ class Susy::Electron+;
#pragma link C++ class Susy::Muon+;
#pragma link C++ class Susy::Jet+;
#pragma link C++ class Susy::Met+;
#pragma link C++ class Susy::Event+;

// STL
#pragma link C++ class std::vector< Susy::Particle >+;
#pragma link C++ class std::vector< Susy::Lepton >+;
#pragma link C++ class std::vector< Susy::Electron >+;
#pragma link C++ class std::vector< Susy::Muon >+;
#pragma link C++ class std::vector< Susy::Jet >+;

#endif
