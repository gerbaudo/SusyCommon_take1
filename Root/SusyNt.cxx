#include <iostream>
#include <iomanip>
#include "SusyCommon/SusyNt.h"

using namespace std;
using namespace Susy;

/*--------------------------------------------------------------------------------*/
// Electron print
/*--------------------------------------------------------------------------------*/
void Electron::print() const
{
  cout.precision(2);
  cout << fixed << "El : q " << setw(2) << q << " pt " << setw(6) << Pt() << " eta " << setw(5) << Eta()
       << " tight " << tightPP << " type " 
       << mcType << " origin " << mcOrigin 
       << endl;
  cout.precision(6);
  cout.unsetf(ios_base::fixed);
}
/*--------------------------------------------------------------------------------*/
// Muon print
/*--------------------------------------------------------------------------------*/
void Muon::print() const
{
  cout.precision(2);
  cout << fixed << "Mu : q " << setw(2) << q << " pt " << setw(6) << Pt() << " eta " << setw(5) << Eta()
       << " cb " << isCombined
       << " type " << mcType << " origin " << mcOrigin 
       << endl;
  cout.precision(6);
  cout.unsetf(ios_base::fixed);
}
/*--------------------------------------------------------------------------------*/
// Jet print
/*--------------------------------------------------------------------------------*/
void Jet::print() const
{
  cout.precision(2);
  cout << fixed << "Jet : pt " << setw(6) << Pt() << " eta " << setw(5) << Eta()
       << " jvf " << setw(4) << jvf
       << " btag " << setw(5) << combNN
       << endl;
  cout.precision(6);
  cout.unsetf(ios_base::fixed);
}
/*--------------------------------------------------------------------------------*/
// Met print
/*--------------------------------------------------------------------------------*/
void Met::print() const
{
  cout.precision(2);
  cout << fixed << "Met : pt " << setw(6) << Pt() << " phi " << setw(4) << Phi()
       << endl;
  cout.precision(6);
  cout.unsetf(ios_base::fixed);
}
/*--------------------------------------------------------------------------------*/
// Event print
/*--------------------------------------------------------------------------------*/
void Event::print() const
{
  cout << "Run " << run << " Event " << event << " Stream " << streamName(stream) << endl;
}
