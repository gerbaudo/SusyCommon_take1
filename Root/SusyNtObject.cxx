#include "SusyCommon/SusyNtObject.h"

using namespace std;
using namespace Susy;


/*--------------------------------------------------------------------------------*/
// SusyNtObject constructor for writing
/*--------------------------------------------------------------------------------*/
SusyNtObject::SusyNtObject():
        evt(this, "event", 0),
        ele(this, "electrons", 0),
        muo(this, "muons", 0),
        jet(this, "jets", 0),
        met(this, "met", 0)
{
}

/*--------------------------------------------------------------------------------*/
// SusyNtObject constructor for reading
/*--------------------------------------------------------------------------------*/
SusyNtObject::SusyNtObject(const Long64_t& entry):
        evt(this, "event", &entry),
        ele(this, "electrons", &entry),
        muo(this, "muons", &entry),
        jet(this, "jets", &entry),
        met(this, "met", &entry)
{
}

/*--------------------------------------------------------------------------------*/
// Connect the objects to an output tree
/*--------------------------------------------------------------------------------*/
void SusyNtObject::WriteTo(TTree* tree)
{
  evt.SetActive(true);
  ele.SetActive(true);
  muo.SetActive(true);
  jet.SetActive(true);
  met.SetActive(true);

  evt.WriteTo(tree);
  ele.WriteTo(tree);
  muo.WriteTo(tree);
  jet.WriteTo(tree);
  met.WriteTo(tree);
}

/*--------------------------------------------------------------------------------*/
// Connect the objects to an input tree
/*--------------------------------------------------------------------------------*/
void SusyNtObject::ReadFrom(TTree* tree)
{
  evt.ReadFrom(tree);
  ele.ReadFrom(tree);
  muo.ReadFrom(tree);
  jet.ReadFrom(tree);
  met.ReadFrom(tree);
}

/*--------------------------------------------------------------------------------*/
// Clear the variables when in read mode
/*--------------------------------------------------------------------------------*/
void SusyNtObject::clear()
{
  evt()->clear();
  ele()->clear();
  muo()->clear();
  jet()->clear();
  met()->clear();
}
